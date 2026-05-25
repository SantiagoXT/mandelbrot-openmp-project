#!/usr/bin/env bash
set -euo pipefail

WIDTH=${1:-1000}
HEIGHT=${2:-1000}
ITERATIONS=${3:-1000}
RADIUS=${4:-5}
THREADS=${5:-4}
REPEATS=${6:-3}

mkdir -p results/raw

OUTPUT="results/raw/histogram_benchmark_${WIDTH}x${HEIGHT}_t${THREADS}_$(date +%Y%m%d_%H%M%S).csv"

echo "mode,threads,repeat,width,height,iterations,blur_radius,histogram_time,checksum,unique_colors,total_time" > "$OUTPUT"

make mandelbrot_histogram >/dev/null

MODES=("atomic" "critical" "local" "false_shared" "padded")

for mode in "${MODES[@]}"; do
    for repeat in $(seq 1 "$REPEATS"); do
        echo "Running mode=$mode threads=$THREADS repeat=$repeat"

        output=$(OMP_NUM_THREADS="$THREADS" ./mandelbrot_histogram "$WIDTH" "$HEIGHT" "$ITERATIONS" "$RADIUS" "$mode")

        hist_line=$(echo "$output" | grep "Histogram ${mode}:")
        histogram_time=$(echo "$hist_line" | awk '{print $3}')
        checksum=$(echo "$hist_line" | sed -E 's/.*checksum=([0-9]+).*/\1/')
        unique_colors=$(echo "$hist_line" | sed -E 's/.*unique_colors=([0-9]+).*/\1/')
        total_time=$(echo "$output" | grep "Tiempo total" | awk '{print $(NF-1)}')

        echo "$mode,$THREADS,$repeat,$WIDTH,$HEIGHT,$ITERATIONS,$RADIUS,$histogram_time,$checksum,$unique_colors,$total_time" >> "$OUTPUT"
    done
done

echo "Benchmark terminado."
echo "Archivo generado: $OUTPUT"
