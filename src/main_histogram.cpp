#include <chrono>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#include <omp.h>

struct Pixel {
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
};

class Timer {
public:
    Timer() : start_time(std::chrono::high_resolution_clock::now()) {}

    double elapsedSeconds() const {
        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end_time - start_time;
        return elapsed.count();
    }

private:
    std::chrono::high_resolution_clock::time_point start_time;
};

Pixel colorFromIteration(int iteration, int maxIterations) {
    if (iteration >= maxIterations) {
        return Pixel{0, 0, 0};
    }

    double t = static_cast<double>(iteration) / static_cast<double>(maxIterations);

    std::uint8_t r = static_cast<std::uint8_t>(9.0 * (1.0 - t) * t * t * t * 255.0);
    std::uint8_t g = static_cast<std::uint8_t>(15.0 * (1.0 - t) * (1.0 - t) * t * t * 255.0);
    std::uint8_t b = static_cast<std::uint8_t>(8.5 * (1.0 - t) * (1.0 - t) * (1.0 - t) * t * 255.0);

    return Pixel{r, g, b};
}

void savePPM(const std::string& filename, const std::vector<Pixel>& image, int width, int height) {
    std::ofstream file(filename, std::ios::binary);

    if (!file) {
        throw std::runtime_error("No se pudo abrir el archivo de salida: " + filename);
    }

    file << "P6\n";
    file << width << " " << height << "\n";
    file << "255\n";

    file.write(reinterpret_cast<const char*>(image.data()), static_cast<std::streamsize>(image.size() * sizeof(Pixel)));

    if (!file) {
        throw std::runtime_error("Error al escribir la imagen: " + filename);
    }
}

void generateMandelbrot(std::vector<Pixel>& image, int width, int height, int maxIterations) {
    const double minReal = -2.0;
    const double maxReal = 1.0;
    const double minImag = -1.5;
    const double maxImag = 1.5;

    #pragma omp parallel for schedule(static)
    for (int y = 0; y < height; ++y) {
        double cImag = minImag + (maxImag - minImag) * static_cast<double>(y) / static_cast<double>(height - 1);

        for (int x = 0; x < width; ++x) {
            double cReal = minReal + (maxReal - minReal) * static_cast<double>(x) / static_cast<double>(width - 1);

            double zReal = 0.0;
            double zImag = 0.0;

            int iteration = 0;

            while ((zReal * zReal + zImag * zImag <= 4.0) && (iteration < maxIterations)) {
                double zRealTemp = zReal * zReal - zImag * zImag + cReal;
                zImag = 2.0 * zReal * zImag + cImag;
                zReal = zRealTemp;
                ++iteration;
            }

            image[y * width + x] = colorFromIteration(iteration, maxIterations);
        }
    }
}

int clampInt(int value, int minValue, int maxValue) {
    if (value < minValue) {
        return minValue;
    }

    if (value > maxValue) {
        return maxValue;
    }

    return value;
}

void applyBoxBlur(
    const std::vector<Pixel>& input,
    std::vector<Pixel>& output,
    int width,
    int height,
    int radius
) {
    int kernelSize = 2 * radius + 1;
    int kernelArea = kernelSize * kernelSize;

    #pragma omp parallel for schedule(static)
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int sumR = 0;
            int sumG = 0;
            int sumB = 0;

            for (int ky = -radius; ky <= radius; ++ky) {
                int sampleY = clampInt(y + ky, 0, height - 1);

                for (int kx = -radius; kx <= radius; ++kx) {
                    int sampleX = clampInt(x + kx, 0, width - 1);
                    const Pixel& p = input[sampleY * width + sampleX];

                    sumR += p.r;
                    sumG += p.g;
                    sumB += p.b;
                }
            }

            output[y * width + x] = Pixel{
                static_cast<std::uint8_t>(sumR / kernelArea),
                static_cast<std::uint8_t>(sumG / kernelArea),
                static_cast<std::uint8_t>(sumB / kernelArea)
            };
        }
    }
}

std::uint32_t colorKey(const Pixel& p) {
    return (static_cast<std::uint32_t>(p.r) << 16)
         | (static_cast<std::uint32_t>(p.g) << 8)
         | static_cast<std::uint32_t>(p.b);
}

struct HistogramResult {
    double timeSeconds;
    std::uint64_t checksum;
    std::size_t uniqueColors;
};

HistogramResult histogramAtomic(const std::vector<Pixel>& image) {
    constexpr std::size_t HIST_SIZE = 1u << 24;
    std::vector<std::uint32_t> histogram(HIST_SIZE, 0);

    Timer timer;

    #pragma omp parallel for schedule(static)
    for (std::size_t i = 0; i < image.size(); ++i) {
        std::uint32_t key = colorKey(image[i]);

        #pragma omp atomic update
        histogram[key]++;
    }

    double elapsed = timer.elapsedSeconds();

    std::uint64_t checksum = 0;
    std::size_t uniqueColors = 0;

    for (std::uint32_t count : histogram) {
        checksum += count;
        if (count > 0) {
            ++uniqueColors;
        }
    }

    return HistogramResult{elapsed, checksum, uniqueColors};
}

HistogramResult histogramCritical(const std::vector<Pixel>& image) {
    std::unordered_map<std::uint32_t, std::uint64_t> histogram;
    histogram.reserve(image.size() / 4);

    Timer timer;

    #pragma omp parallel for schedule(static)
    for (std::size_t i = 0; i < image.size(); ++i) {
        std::uint32_t key = colorKey(image[i]);

        #pragma omp critical(histogram_update)
        {
            histogram[key]++;
        }
    }

    double elapsed = timer.elapsedSeconds();

    std::uint64_t checksum = 0;

    for (const auto& item : histogram) {
        checksum += item.second;
    }

    return HistogramResult{elapsed, checksum, histogram.size()};
}

HistogramResult histogramLocal(const std::vector<Pixel>& image) {
    int threadCount = omp_get_max_threads();
    std::vector<std::unordered_map<std::uint32_t, std::uint64_t>> localHistograms(threadCount);

    Timer timer;

    #pragma omp parallel
    {
        int threadId = omp_get_thread_num();
        auto& local = localHistograms[threadId];
        local.reserve(image.size() / static_cast<std::size_t>(threadCount * 4));

        #pragma omp for schedule(static)
        for (std::size_t i = 0; i < image.size(); ++i) {
            std::uint32_t key = colorKey(image[i]);
            local[key]++;
        }
    }

    std::unordered_map<std::uint32_t, std::uint64_t> finalHistogram;
    finalHistogram.reserve(image.size() / 4);

    for (const auto& local : localHistograms) {
        for (const auto& item : local) {
            finalHistogram[item.first] += item.second;
        }
    }

    double elapsed = timer.elapsedSeconds();

    std::uint64_t checksum = 0;

    for (const auto& item : finalHistogram) {
        checksum += item.second;
    }

    return HistogramResult{elapsed, checksum, finalHistogram.size()};
}

struct CounterNoPadding {
    volatile std::uint64_t value = 0;
};

struct alignas(64) CounterPadded {
    volatile std::uint64_t value = 0;
};

HistogramResult falseSharingCounter(const std::vector<Pixel>& image) {
    int threadCount = omp_get_max_threads();
    std::vector<CounterNoPadding> counters(threadCount);

    Timer timer;

    #pragma omp parallel
    {
        int threadId = omp_get_thread_num();

        #pragma omp for schedule(static)
        for (std::size_t i = 0; i < image.size(); ++i) {
            counters[threadId].value += static_cast<std::uint64_t>(image[i].r)
                                      + static_cast<std::uint64_t>(image[i].g)
                                      + static_cast<std::uint64_t>(image[i].b)
                                      + 1u;
        }
    }

    double elapsed = timer.elapsedSeconds();

    std::uint64_t checksum = 0;
    for (const auto& counter : counters) {
        checksum += counter.value;
    }

    return HistogramResult{elapsed, checksum, 0};
}

HistogramResult paddedCounter(const std::vector<Pixel>& image) {
    int threadCount = omp_get_max_threads();
    std::vector<CounterPadded> counters(threadCount);

    Timer timer;

    #pragma omp parallel
    {
        int threadId = omp_get_thread_num();

        #pragma omp for schedule(static)
        for (std::size_t i = 0; i < image.size(); ++i) {
            counters[threadId].value += static_cast<std::uint64_t>(image[i].r)
                                      + static_cast<std::uint64_t>(image[i].g)
                                      + static_cast<std::uint64_t>(image[i].b)
                                      + 1u;
        }
    }

    double elapsed = timer.elapsedSeconds();

    std::uint64_t checksum = 0;
    for (const auto& counter : counters) {
        checksum += counter.value;
    }

    return HistogramResult{elapsed, checksum, 0};
}

void printHistogramResult(const std::string& mode, const HistogramResult& result) {
    std::cout << "Histogram " << mode << ": "
              << result.timeSeconds << " s "
              << "checksum=" << result.checksum << " "
              << "unique_colors=" << result.uniqueColors
              << std::endl;
}

void runHistogramMode(const std::string& mode, const std::vector<Pixel>& image) {
    if (mode == "atomic") {
        printHistogramResult("atomic", histogramAtomic(image));
    } else if (mode == "critical") {
        printHistogramResult("critical", histogramCritical(image));
    } else if (mode == "local") {
        printHistogramResult("local", histogramLocal(image));
    } else if (mode == "false_shared") {
        printHistogramResult("false_shared", falseSharingCounter(image));
    } else if (mode == "padded") {
        printHistogramResult("padded", paddedCounter(image));
    } else if (mode == "all") {
        printHistogramResult("atomic", histogramAtomic(image));
        printHistogramResult("critical", histogramCritical(image));
        printHistogramResult("local", histogramLocal(image));
        printHistogramResult("false_shared", falseSharingCounter(image));
        printHistogramResult("padded", paddedCounter(image));
    } else {
        throw std::runtime_error("Modo de histograma invalido: " + mode);
    }
}

int main(int argc, char* argv[]) {
    int width = 1000;
    int height = 1000;
    int maxIterations = 1000;
    int blurRadius = 5;
    std::string histogramMode = "all";

    if (argc >= 2) {
        width = std::stoi(argv[1]);
    }

    if (argc >= 3) {
        height = std::stoi(argv[2]);
    }

    if (argc >= 4) {
        maxIterations = std::stoi(argv[3]);
    }

    if (argc >= 5) {
        blurRadius = std::stoi(argv[4]);
    }

    if (argc >= 6) {
        histogramMode = argv[5];
    }

    if (width <= 0 || height <= 0 || maxIterations <= 0 || blurRadius < 0) {
        std::cerr << "Error: parametros invalidos." << std::endl;
        return 1;
    }

    std::cout << "=== Mandelbrot OpenMP histogram benchmark ===" << std::endl;
    std::cout << "Resolucion: " << width << "x" << height << std::endl;
    std::cout << "Iteraciones maximas: " << maxIterations << std::endl;
    std::cout << "Radio del filtro: " << blurRadius << std::endl;
    std::cout << "Modo de histograma: " << histogramMode << std::endl;
    std::cout << "Hilos maximos OpenMP: " << omp_get_max_threads() << std::endl;

    try {
        std::vector<Pixel> mandelbrotImage(static_cast<std::size_t>(width) * static_cast<std::size_t>(height));
        std::vector<Pixel> filteredImage(static_cast<std::size_t>(width) * static_cast<std::size_t>(height));

        Timer totalTimer;

        Timer mandelbrotTimer;
        generateMandelbrot(mandelbrotImage, width, height, maxIterations);
        double mandelbrotTime = mandelbrotTimer.elapsedSeconds();

        Timer convolutionTimer;
        applyBoxBlur(mandelbrotImage, filteredImage, width, height, blurRadius);
        double convolutionTime = convolutionTimer.elapsedSeconds();

        Timer saveFilteredTimer;
        savePPM("results/mandelbrot_filtered.ppm", filteredImage, width, height);
        double saveFilteredTime = saveFilteredTimer.elapsedSeconds();

        std::cout << "\n=== Tiempos base ===" << std::endl;
        std::cout << "Tarea A - Mandelbrot: " << mandelbrotTime << " s" << std::endl;
        std::cout << "Tarea B - Convolucion: " << convolutionTime << " s" << std::endl;
        std::cout << "Guardado imagen filtrada: " << saveFilteredTime << " s" << std::endl;

        std::cout << "\n=== Histograma ===" << std::endl;
        runHistogramMode(histogramMode, filteredImage);

        double totalTime = totalTimer.elapsedSeconds();

        std::cout << "\nTiempo total: " << totalTime << " s" << std::endl;
        std::cout << "Imagen generada: results/mandelbrot_filtered.ppm" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error de ejecucion: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
