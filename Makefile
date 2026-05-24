CXX = g++
CXXFLAGS = -std=c++17 -O3 -Wall -Wextra -pedantic
OMPFLAGS = -fopenmp

TARGET_SEQ = mandelbrot_sequential
TARGET_OMP = mandelbrot_openmp_ai

SRC_SEQ = src/main.cpp
SRC_OMP = src/main_openmp_ai.cpp

all: $(TARGET_SEQ) $(TARGET_OMP)

$(TARGET_SEQ): $(SRC_SEQ)
	$(CXX) $(CXXFLAGS) $(SRC_SEQ) -o $(TARGET_SEQ)

$(TARGET_OMP): $(SRC_OMP)
	$(CXX) $(CXXFLAGS) $(OMPFLAGS) $(SRC_OMP) -o $(TARGET_OMP)

run-seq-small: $(TARGET_SEQ)
	./$(TARGET_SEQ) 1000 1000 1000 5

run-omp-small: $(TARGET_OMP)
	OMP_NUM_THREADS=4 ./$(TARGET_OMP) 1000 1000 1000 5

run-seq-medium: $(TARGET_SEQ)
	./$(TARGET_SEQ) 1920 1080 1000 8

run-omp-medium: $(TARGET_OMP)
	OMP_NUM_THREADS=4 ./$(TARGET_OMP) 1920 1080 1000 8

run-seq-4k: $(TARGET_SEQ)
	./$(TARGET_SEQ) 3840 2160 1000 8

run-omp-4k: $(TARGET_OMP)
	OMP_NUM_THREADS=4 ./$(TARGET_OMP) 3840 2160 1000 8

run-seq-8k: $(TARGET_SEQ)
	./$(TARGET_SEQ) 7680 4320 1000 8

run-omp-8k: $(TARGET_OMP)
	OMP_NUM_THREADS=4 ./$(TARGET_OMP) 7680 4320 1000 8

clean:
	rm -f $(TARGET_SEQ)
	rm -f $(TARGET_OMP)
	rm -f results/*.ppm
	rm -f results/*.png

.PHONY: all clean run-seq-small run-omp-small run-seq-medium run-omp-medium run-seq-4k run-omp-4k run-seq-8k run-omp-8k
