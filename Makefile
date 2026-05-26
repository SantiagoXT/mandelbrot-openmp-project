CXX = g++
CXXFLAGS = -std=c++17 -O3 -Wall -Wextra -pedantic
OMPFLAGS = -fopenmp

TARGET_SEQ = mandelbrot_sequential
TARGET_OMP = mandelbrot_openmp_ai
TARGET_SCHED = mandelbrot_scheduler
TARGET_HIST = mandelbrot_histogram
TARGET_SIMD = mandelbrot_simd

SRC_SEQ = src/main.cpp
SRC_OMP = src/main_openmp_ai.cpp
SRC_SCHED = src/main_scheduler.cpp
SRC_HIST = src/main_histogram.cpp
SRC_SIMD = src/main_simd.cpp

all: $(TARGET_SEQ) $(TARGET_OMP) $(TARGET_SCHED) $(TARGET_HIST) $(TARGET_SIMD)

$(TARGET_SEQ): $(SRC_SEQ)
	$(CXX) $(CXXFLAGS) $(SRC_SEQ) -o $(TARGET_SEQ)

$(TARGET_OMP): $(SRC_OMP)
	$(CXX) $(CXXFLAGS) $(OMPFLAGS) $(SRC_OMP) -o $(TARGET_OMP)

$(TARGET_SCHED): $(SRC_SCHED)
	$(CXX) $(CXXFLAGS) $(OMPFLAGS) $(SRC_SCHED) -o $(TARGET_SCHED)

$(TARGET_HIST): $(SRC_HIST)
	$(CXX) $(CXXFLAGS) $(OMPFLAGS) $(SRC_HIST) -o $(TARGET_HIST)

$(TARGET_SIMD): $(SRC_SIMD)
	$(CXX) $(CXXFLAGS) $(OMPFLAGS) $(SRC_SIMD) -o $(TARGET_SIMD)

run-seq-small: $(TARGET_SEQ)
	./$(TARGET_SEQ) 1000 1000 1000 5

run-omp-small: $(TARGET_OMP)
	OMP_NUM_THREADS=4 ./$(TARGET_OMP) 1000 1000 1000 5

run-sched-static-small: $(TARGET_SCHED)
	OMP_NUM_THREADS=4 OMP_SCHEDULE=static ./$(TARGET_SCHED) 1000 1000 1000 5

run-sched-dynamic-small: $(TARGET_SCHED)
	OMP_NUM_THREADS=4 OMP_SCHEDULE=dynamic,8 ./$(TARGET_SCHED) 1000 1000 1000 5

run-sched-guided-small: $(TARGET_SCHED)
	OMP_NUM_THREADS=4 OMP_SCHEDULE=guided,8 ./$(TARGET_SCHED) 1000 1000 1000 5

run-hist-small: $(TARGET_HIST)
	OMP_NUM_THREADS=4 ./$(TARGET_HIST) 1000 1000 1000 5 all

run-simd-small: $(TARGET_SIMD)
	OMP_NUM_THREADS=4 ./$(TARGET_SIMD) 1000 1000 1000 5

run-seq-medium: $(TARGET_SEQ)
	./$(TARGET_SEQ) 1920 1080 1000 8

run-omp-medium: $(TARGET_OMP)
	OMP_NUM_THREADS=4 ./$(TARGET_OMP) 1920 1080 1000 8

run-sched-medium: $(TARGET_SCHED)
	OMP_NUM_THREADS=4 OMP_SCHEDULE=dynamic,4 ./$(TARGET_SCHED) 1920 1080 1000 8

run-hist-medium: $(TARGET_HIST)
	OMP_NUM_THREADS=4 ./$(TARGET_HIST) 1920 1080 1000 8 all

run-simd-medium: $(TARGET_SIMD)
	OMP_NUM_THREADS=4 ./$(TARGET_SIMD) 1920 1080 1000 8

vectorization-report: $(SRC_SIMD)
	mkdir -p results/raw
	$(CXX) $(CXXFLAGS) $(OMPFLAGS) -fopt-info-vec-all=results/raw/vectorization_report.txt $(SRC_SIMD) -o $(TARGET_SIMD)

clean:
	rm -f $(TARGET_SEQ)
	rm -f $(TARGET_OMP)
	rm -f $(TARGET_SCHED)
	rm -f $(TARGET_HIST)
	rm -f $(TARGET_SIMD)
	rm -f results/*.ppm
	rm -f results/*.png

.PHONY: all clean vectorization-report run-seq-small run-omp-small run-sched-static-small run-sched-dynamic-small run-sched-guided-small run-hist-small run-simd-small run-seq-medium run-omp-medium run-sched-medium run-hist-medium run-simd-medium
