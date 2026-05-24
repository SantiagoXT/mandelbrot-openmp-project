# Prompts utilizados

## Prompt 1: Codigo secuencial base

Genera un programa en C++ puramente secuencial que realice las siguientes tareas:

1. Generar una imagen de alta resolucion del conjunto de Mandelbrot.
2. Guardar la imagen generada en formato PPM sin usar librerias externas.
3. Aplicar un filtro de convolucion 2D pesado sobre la imagen generada.
4. Guardar la imagen filtrada en formato PPM.
5. Medir por separado el tiempo de generacion del fractal y el tiempo del filtro de convolucion.
6. Permitir configurar por argumentos la resolucion, el numero maximo de iteraciones y el radio del filtro.
7. Mantener el codigo sin OpenMP, sin hilos y sin paralelizacion, porque esta version sera la linea base secuencial del proyecto.

## Prompt 2: Linea base paralela con OpenMP

A partir del programa secuencial en C++ que genera una imagen del conjunto de Mandelbrot, guarda la imagen en formato PPM, aplica un filtro de convolucion 2D pesado y mide los tiempos de ambas tareas, genera una version paralela usando OpenMP.

La version paralela debe:
1. Mantener la misma funcionalidad que la version secuencial.
2. Paralelizar la generacion de Mandelbrot.
3. Paralelizar el filtro de convolucion 2D.
4. Usar directivas OpenMP simples.
5. Mantener mediciones separadas para la Tarea A y la Tarea B.
6. Servir como linea base paralela generada por IA, antes de aplicar optimizaciones manuales.
