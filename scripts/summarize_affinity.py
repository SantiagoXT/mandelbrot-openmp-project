#!/usr/bin/env python3
import csv
import glob
import statistics
import sys
from collections import defaultdict

if len(sys.argv) >= 2:
    csv_file = sys.argv[1]
else:
    files = sorted(glob.glob("results/raw/affinity_benchmark_*.csv"))
    if not files:
        print("No se encontraron CSVs de afinidad en results/raw/")
        sys.exit(1)
    csv_file = files[-1]

groups = defaultdict(lambda: {"mandelbrot": [], "convolution": [], "total": []})

with open(csv_file, newline="") as f:
    reader = csv.DictReader(f)
    for row in reader:
        key = row["config"]
        groups[key]["mandelbrot"].append(float(row["mandelbrot_time"]))
        groups[key]["convolution"].append(float(row["convolution_time"]))
        groups[key]["total"].append(float(row["total_time"]))

summary = []

for config, values in groups.items():
    mandelbrot_avg = statistics.mean(values["mandelbrot"])
    convolution_avg = statistics.mean(values["convolution"])
    total_avg = statistics.mean(values["total"])
    total_stdev = statistics.stdev(values["total"]) if len(values["total"]) > 1 else 0.0
    n = len(values["total"])

    summary.append((total_avg, total_stdev, config, mandelbrot_avg, convolution_avg, n))

summary.sort()

print(f"Archivo analizado: {csv_file}")
print()
print("Comparacion de afinidad OpenMP:")
print()
print(f"{'config':<16} {'mandelbrot':<12} {'convolution':<12} {'total_avg':<12} {'total_stdev':<12} {'n':<5}")
print("-" * 75)

for total_avg, total_stdev, config, mandelbrot_avg, convolution_avg, n in summary:
    print(f"{config:<16} {mandelbrot_avg:<12.6f} {convolution_avg:<12.6f} {total_avg:<12.6f} {total_stdev:<12.6f} {n:<5}")
