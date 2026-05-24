#include <chrono>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

// ===============================
// Estructura basica para un pixel RGB
// ===============================
struct Pixel {
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
};

// ===============================
// Medidor de tiempo simple
// ===============================
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

// ===============================
// Convierte el numero de iteraciones a color
// ===============================
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

// ===============================
// Guarda una imagen en formato PPM binario P6
// No requiere librerias externas.
// ===============================
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

// ===============================
// Tarea A: Generacion secuencial del conjunto de Mandelbrot
// ===============================
void generateMandelbrot(std::vector<Pixel>& image, int width, int height, int maxIterations) {
    const double minReal = -2.0;
    const double maxReal = 1.0;
    const double minImag = -1.5;
    const double maxImag = 1.5;

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

// ===============================
// Funcion auxiliar para limitar indices en los bordes
// ===============================
int clampInt(int value, int minValue, int maxValue) {
    if (value < minValue) {
        return minValue;
    }

    if (value > maxValue) {
        return maxValue;
    }

    return value;
}

// ===============================
// Tarea B: Filtro de convolucion 2D secuencial
// Se usa un box blur pesado con radio configurable.
// Aunque es mas simple que un Gaussiano, sigue siendo una convolucion 2D pesada.
// ===============================
void applyBoxBlur(
    const std::vector<Pixel>& input,
    std::vector<Pixel>& output,
    int width,
    int height,
    int radius
) {
    int kernelSize = 2 * radius + 1;
    int kernelArea = kernelSize * kernelSize;

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

// ===============================
// Lectura simple de argumentos
// Uso:
// ./mandelbrot_sequential [width] [height] [iterations] [blur_radius]
// ===============================
int main(int argc, char* argv[]) {
    int width = 1000;
    int height = 1000;
    int maxIterations = 1000;
    int blurRadius = 5;

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

    if (width <= 0 || height <= 0 || maxIterations <= 0 || blurRadius < 0) {
        std::cerr << "Error: parametros invalidos." << std::endl;
        return 1;
    }

    std::cout << "=== Mandelbrot secuencial + convolucion 2D ===" << std::endl;
    std::cout << "Resolucion: " << width << "x" << height << std::endl;
    std::cout << "Iteraciones maximas: " << maxIterations << std::endl;
    std::cout << "Radio del filtro: " << blurRadius << std::endl;

    try {
        std::vector<Pixel> mandelbrotImage(static_cast<std::size_t>(width) * static_cast<std::size_t>(height));
        std::vector<Pixel> filteredImage(static_cast<std::size_t>(width) * static_cast<std::size_t>(height));

        Timer totalTimer;

        Timer mandelbrotTimer;
        generateMandelbrot(mandelbrotImage, width, height, maxIterations);
        double mandelbrotTime = mandelbrotTimer.elapsedSeconds();

        Timer saveOriginalTimer;
        savePPM("results/mandelbrot_original.ppm", mandelbrotImage, width, height);
        double saveOriginalTime = saveOriginalTimer.elapsedSeconds();

        Timer convolutionTimer;
        applyBoxBlur(mandelbrotImage, filteredImage, width, height, blurRadius);
        double convolutionTime = convolutionTimer.elapsedSeconds();

        Timer saveFilteredTimer;
        savePPM("results/mandelbrot_filtered.ppm", filteredImage, width, height);
        double saveFilteredTime = saveFilteredTimer.elapsedSeconds();

        double totalTime = totalTimer.elapsedSeconds();

        std::cout << "\n=== Tiempos ===" << std::endl;
        std::cout << "Tarea A - Mandelbrot: " << mandelbrotTime << " s" << std::endl;
        std::cout << "Guardado imagen original: " << saveOriginalTime << " s" << std::endl;
        std::cout << "Tarea B - Convolucion: " << convolutionTime << " s" << std::endl;
        std::cout << "Guardado imagen filtrada: " << saveFilteredTime << " s" << std::endl;
        std::cout << "Tiempo total: " << totalTime << " s" << std::endl;

        std::cout << "\nImagenes generadas:" << std::endl;
        std::cout << "results/mandelbrot_original.ppm" << std::endl;
        std::cout << "results/mandelbrot_filtered.ppm" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error de ejecucion: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
