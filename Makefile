CXX = g++
CXXFLAGS = -std=c++17 -O3 -Wall -Wextra -pedantic
TARGET = mandelbrot_sequential
SRC = src/main.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

run: all
	./$(TARGET)

run-small: all
	./$(TARGET) 1000 1000 1000 5

run-medium: all
	./$(TARGET) 1920 1080 1000 8

run-4k: all
	./$(TARGET) 3840 2160 1000 8

run-8k: all
	./$(TARGET) 7680 4320 1000 8

clean:
	rm -f $(TARGET)
	rm -f results/*.ppm

.PHONY: all run run-small run-medium run-4k run-8k clean
