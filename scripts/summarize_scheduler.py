#!/usr/bin/env python3
import csv
import glob
import statistics
import sys
from collections import defaultdict

if len(sys.argv) >= 2:
    csv_file = sys.argv[1]
else:
    files = sorted(glob.glob("results/raw/scheduler_benchmark_*.csv"))
    if not files:
        print("No se encontraron CSVs en results/raw/")
        sys.exit(1)
    csv_file = files[-1]

groups = defaultdict(list)

with open(csv_file, newline="") as f:
    reader = csv.DictReader(f)
    for row in reader:
        key = (row["schedule"], row["chunk"])
        groups[key].append(float(row["mandelbrot_time"]))

summary = []

for (schedule, chunk), values in groups.items():
    avg = statistics.mean(values)
    stdev = statistics.stdev(values) if len(values) > 1 else 0.0
    summary.append((avg, stdev, schedule, chunk, len(values)))

summary.sort()

print(f"Archivo analizado: {csv_file}")
print()
print("Mejores configuraciones por tiempo promedio de Mandelbrot:")
print()
print(f"{'schedule':<10} {'chunk':<10} {'avg_time':<12} {'stdev':<12} {'n':<5}")
print("-" * 55)

for avg, stdev, schedule, chunk, n in summary:
    print(f"{schedule:<10} {chunk:<10} {avg:<12.6f} {stdev:<12.6f} {n:<5}")
