#!/usr/bin/env python3
import csv
import glob
import statistics
import sys
from collections import defaultdict
from pathlib import Path

import matplotlib.pyplot as plt

if len(sys.argv) >= 2:
    csv_file = sys.argv[1]
else:
    files = sorted(glob.glob("results/raw/thread_scaling_*.csv"))
    if not files:
        print("No se encontraron CSVs de escalamiento en results/raw/")
        sys.exit(1)
    csv_file = files[-1]

groups = defaultdict(list)

with open(csv_file, newline="") as f:
    reader = csv.DictReader(f)
    for row in reader:
        groups[int(row["threads"])].append(float(row["total_time"]))

threads = sorted(groups.keys())
avg_times = [statistics.mean(groups[t]) for t in threads]
stdev_times = [statistics.stdev(groups[t]) if len(groups[t]) > 1 else 0.0 for t in threads]

baseline = avg_times[0]
speedups = [baseline / t for t in avg_times]

Path("results/graphs").mkdir(parents=True, exist_ok=True)

time_graph = "results/graphs/thread_time.png"
speedup_graph = "results/graphs/thread_speedup.png"

plt.figure()
plt.errorbar(threads, avg_times, yerr=stdev_times, marker="o", capsize=4)
plt.xlabel("Numero de hilos")
plt.ylabel("Tiempo promedio total (s)")
plt.title("Tiempo de ejecucion vs numero de hilos")
plt.grid(True)
plt.savefig(time_graph, dpi=200, bbox_inches="tight")
plt.close()

plt.figure()
plt.plot(threads, speedups, marker="o", label="Speedup observado")
plt.plot(threads, threads, linestyle="--", label="Speedup ideal")
plt.xlabel("Numero de hilos")
plt.ylabel("Speedup")
plt.title("Speedup vs numero de hilos")
plt.grid(True)
plt.legend()
plt.savefig(speedup_graph, dpi=200, bbox_inches="tight")
plt.close()

print(f"Archivo analizado: {csv_file}")
print(f"Grafica de tiempo: {time_graph}")
print(f"Grafica de speedup: {speedup_graph}")
