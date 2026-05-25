# Mandelbrot OpenMP Project

Proyecto de evaluacion de paralelizacion con OpenMP usando C++.

El programa realiza dos tareas principales:

1. Genera una imagen del conjunto de Mandelbrot.
2. Aplica un filtro de convolucion 2D pesado sobre la imagen generada.

## Versiones implementadas

Actualmente el proyecto incluye:

- mandelbrot_sequential: version secuencial base.
- mandelbrot_openmp_ai: linea base paralela generada con OpenMP.
- mandelbrot_scheduler: version para evaluar planificadores de OpenMP usando OMP_SCHEDULE.

## Compilacion

make

## Ejecuciones rapidas

Version secuencial:

make run-seq-small

Version OpenMP base:

make run-omp-small

Pruebas de scheduler:

make run-sched-static-small
make run-sched-dynamic-small
make run-sched-guided-small

## Benchmark de schedulers

El proyecto incluye un script para comparar static, dynamic y guided con diferentes tamanos de bloque.

Ejemplo formal:

./scripts/benchmark_schedulers.sh 1920 1080 1000 8 4 5

Argumentos:

width height iterations blur_radius threads repeats

Ejemplo rapido:

./scripts/benchmark_schedulers.sh 1000 1000 1000 5 4 2

Los resultados se guardan como archivos CSV en:

results/raw/

Para resumir el CSV mas reciente:

python3 scripts/summarize_scheduler.py

## Salidas

El programa genera imagenes en formato PPM:

results/mandelbrot_original.ppm
results/mandelbrot_filtered.ppm

Estas imagenes no se suben al repositorio porque son archivos generados.

## Prompts

Los prompts utilizados se encuentran en:

prompts.md

## Estado actual del proyecto

Completado:

1. Codigo secuencial base.
2. Linea base paralela con OpenMP.
3. Benchmark de schedulers de OpenMP.

Pendiente:

1. Histograma de colores y comparacion de sincronizacion.
2. Analisis de false sharing.
3. Vectorizacion/SPMD.
4. Graficas y reporte tecnico final.
