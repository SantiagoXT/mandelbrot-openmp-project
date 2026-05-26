#!/usr/bin/env bash
set -euo pipefail

WIDTH=${1:-1920}
HEIGHT=${2:-1080}
ITERATIONS=${3:-1000}
RADIUS=${4:-8}
THREADS=${5:-4}
REPEATS=${6:-5}

mkdir -p results/raw

OUTPUT="results/raw/affinity_benchmark_${WIDTH}x${HEIGHT}_t${THREADS}_$(date +%Y%m%d_%H%M%S).csv"

echo "config,proc_bind,places,threads,repeat,width,height,iterations,blur_radius,mandelbrot_time,convolution_time,total_time" > "$OUTPUT"

make mandelbrot_simd >/dev/null

CONFIGS=(
    "no_bind,false,unset"
    "close_cores,close,cores"
    "spread_cores,spread,cores"
    "close_threads,close,threads"
    "spread_threads,spread,threads"
)

for config_entry in "${CONFIGS[@]}"; do
    IFS=',' read -r config proc_bind places <<< "$config_entry"

    for repeat in $(seq 1 "$REPEATS"); do
        echo "Running config=$config proc_bind=$proc_bind places=$places threads=$THREADS repeat=$repeat"

        if [[ "$places" == "unset" ]]; then
            output=$(OMP_NUM_THREADS="$THREADS" OMP_PROC_BIND="$proc_bind" ./mandelbrot_simd "$WIDTH" "$HEIGHT" "$ITERATIONS" "$RADIUS")
        else
            output=$(OMP_NUM_THREADS="$THREADS" OMP_PROC_BIND="$proc_bind" OMP_PLACES="$places" ./mandelbrot_simd "$WIDTH" "$HEIGHT" "$ITERATIONS" "$RADIUS")
        fi

        mandelbrot_time=$(echo "$output" | grep "Tarea A - Mandelbrot" | awk '{print $(NF-1)}')
        convolution_time=$(echo "$output" | grep "Tarea B - Convolucion" | awk '{print $(NF-1)}')
        total_time=$(echo "$output" | grep "Tiempo total" | awk '{print $(NF-1)}')

        echo "$config,$proc_bind,$places,$THREADS,$repeat,$WIDTH,$HEIGHT,$ITERATIONS,$RADIUS,$mandelbrot_time,$convolution_time,$total_time" >> "$OUTPUT"
    done
done

echo "Benchmark terminado."
echo "Archivo generado: $OUTPUT"
