CXX = g++
CXXFLAGS = -std=c++17 -O3 -Wall -Wextra -pedantic
OMPFLAGS = -fopenmp

TARGET_SEQ = mandelbrot_sequential
TARGET_OMP = mandelbrot_openmp_ai
TARGET_SCHED = mandelbrot_scheduler
TARGET_HIST = mandelbrot_histogram

SRC_SEQ = src/main.cpp
SRC_OMP = src/main_openmp_ai.cpp
SRC_SCHED = src/main_scheduler.cpp
SRC_HIST = src/main_histogram.cpp

all: $(TARGET_SEQ) $(TARGET_OMP) $(TARGET_SCHED) $(TARGET_HIST)

$(TARGET_SEQ): $(SRC_SEQ)
	$(CXX) $(CXXFLAGS) $(SRC_SEQ) -o $(TARGET_SEQ)

$(TARGET_OMP): $(SRC_OMP)
	$(CXX) $(CXXFLAGS) $(OMPFLAGS) $(SRC_OMP) -o $(TARGET_OMP)

$(TARGET_SCHED): $(SRC_SCHED)
	$(CXX) $(CXXFLAGS) $(OMPFLAGS) $(SRC_SCHED) -o $(TARGET_SCHED)

$(TARGET_HIST): $(SRC_HIST)
	$(CXX) $(CXXFLAGS) $(OMPFLAGS) $(SRC_HIST) -o $(TARGET_HIST)

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

run-hist-atomic-small: $(TARGET_HIST)
	OMP_NUM_THREADS=4 ./$(TARGET_HIST) 1000 1000 1000 5 atomic

run-hist-critical-small: $(TARGET_HIST)
	OMP_NUM_THREADS=4 ./$(TARGET_HIST) 1000 1000 1000 5 critical

run-hist-local-small: $(TARGET_HIST)
	OMP_NUM_THREADS=4 ./$(TARGET_HIST) 1000 1000 1000 5 local

run-seq-medium: $(TARGET_SEQ)
	./$(TARGET_SEQ) 1920 1080 1000 8

run-omp-medium: $(TARGET_OMP)
	OMP_NUM_THREADS=4 ./$(TARGET_OMP) 1920 1080 1000 8

run-sched-medium: $(TARGET_SCHED)
	OMP_NUM_THREADS=4 OMP_SCHEDULE=dynamic,4 ./$(TARGET_SCHED) 1920 1080 1000 8

run-hist-medium: $(TARGET_HIST)
	OMP_NUM_THREADS=4 ./$(TARGET_HIST) 1920 1080 1000 8 all

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
	rm -f $(TARGET_SCHED)
	rm -f $(TARGET_HIST)
	rm -f results/*.ppm
	rm -f results/*.png

.PHONY: all clean run-seq-small run-omp-small run-sched-static-small run-sched-dynamic-small run-sched-guided-small run-hist-small run-hist-atomic-small run-hist-critical-small run-hist-local-small run-seq-medium run-omp-medium run-sched-medium run-hist-medium run-seq-4k run-omp-4k run-seq-8k run-omp-8k
