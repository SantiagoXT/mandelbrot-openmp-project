# Mandelbrot OpenMP Project

Proyecto de evaluacion de paralelizacion con OpenMP.

## Estado actual

Version secuencial base.

El programa realiza dos tareas principales:

1. Genera una imagen del conjunto de Mandelbrot.
2. Aplica un filtro de convolucion 2D pesado sobre la imagen generada.

## Compilacion

make

## Ejecucion rapida

make run-small

## Ejecucion manual

./mandelbrot_sequential [width] [height] [iterations] [blur_radius]

Ejemplo:

./mandelbrot_sequential 1000 1000 1000 5

## Salidas

El programa genera dos imagenes en formato PPM:

results/mandelbrot_original.ppm
results/mandelbrot_filtered.ppm

## Notas

Esta version no usa OpenMP. Sirve como linea base secuencial inicial.
