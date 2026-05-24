#!/usr/bin/env bash
set -euo pipefail

WIDTH=${1:-1920}
HEIGHT=${2:-1080}
ITERATIONS=${3:-1000}
RADIUS=${4:-8}
THREADS=${5:-4}
REPEATS=${6:-3}

mkdir -p results/raw

OUTPUT="results/raw/scheduler_benchmark_${WIDTH}x${HEIGHT}_t${THREADS}_$(date +%Y%m%d_%H%M%S).csv"

echo "schedule,chunk,threads,repeat,width,height,iterations,blur_radius,mandelbrot_time,convolution_time,total_time" > "$OUTPUT"

make mandelbrot_scheduler >/dev/null

SCHEDULES=("static" "dynamic" "guided")
CHUNKS=("default" "1" "2" "4" "8" "16" "32" "64" "128")

for schedule in "${SCHEDULES[@]}"; do
    for chunk in "${CHUNKS[@]}"; do
        if [[ "$chunk" == "default" ]]; then
            omp_schedule="$schedule"
        else
            omp_schedule="$schedule,$chunk"
        fi

        for repeat in $(seq 1 "$REPEATS"); do
            echo "Running schedule=$schedule chunk=$chunk threads=$THREADS repeat=$repeat"

            output=$(OMP_NUM_THREADS="$THREADS" OMP_SCHEDULE="$omp_schedule" ./mandelbrot_scheduler "$WIDTH" "$HEIGHT" "$ITERATIONS" "$RADIUS")

            mandelbrot_time=$(echo "$output" | grep "Tarea A - Mandelbrot" | awk '{print $(NF-1)}')
            convolution_time=$(echo "$output" | grep "Tarea B - Convolucion" | awk '{print $(NF-1)}')
            total_time=$(echo "$output" | grep "Tiempo total" | awk '{print $(NF-1)}')

            echo "$schedule,$chunk,$THREADS,$repeat,$WIDTH,$HEIGHT,$ITERATIONS,$RADIUS,$mandelbrot_time,$convolution_time,$total_time" >> "$OUTPUT"
        done
    done
done

echo "Benchmark terminado."
echo "Archivo generado: $OUTPUT"
