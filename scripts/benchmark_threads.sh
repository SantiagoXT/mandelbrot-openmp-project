#!/usr/bin/env bash
set -euo pipefail

WIDTH=${1:-1920}
HEIGHT=${2:-1080}
ITERATIONS=${3:-1000}
RADIUS=${4:-8}
MAX_THREADS=${5:-8}
REPEATS=${6:-5}

mkdir -p results/raw

OUTPUT="results/raw/thread_scaling_${WIDTH}x${HEIGHT}_$(date +%Y%m%d_%H%M%S).csv"

echo "threads,repeat,width,height,iterations,blur_radius,mandelbrot_time,convolution_time,total_time" > "$OUTPUT"

make mandelbrot_simd >/dev/null

for threads in $(seq 1 "$MAX_THREADS"); do
    for repeat in $(seq 1 "$REPEATS"); do
        echo "Running threads=$threads repeat=$repeat"

        output=$(OMP_NUM_THREADS="$threads" ./mandelbrot_simd "$WIDTH" "$HEIGHT" "$ITERATIONS" "$RADIUS")

        mandelbrot_time=$(echo "$output" | grep "Tarea A - Mandelbrot" | awk '{print $(NF-1)}')
        convolution_time=$(echo "$output" | grep "Tarea B - Convolucion" | awk '{print $(NF-1)}')
        total_time=$(echo "$output" | grep "Tiempo total" | awk '{print $(NF-1)}')

        echo "$threads,$repeat,$WIDTH,$HEIGHT,$ITERATIONS,$RADIUS,$mandelbrot_time,$convolution_time,$total_time" >> "$OUTPUT"
    done
done

echo "Benchmark terminado."
echo "Archivo generado: $OUTPUT"
