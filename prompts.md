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
