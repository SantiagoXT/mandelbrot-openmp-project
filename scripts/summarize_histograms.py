#!/usr/bin/env python3
import csv
import glob
import statistics
import sys
from collections import defaultdict

if len(sys.argv) >= 2:
    csv_file = sys.argv[1]
else:
    files = sorted(glob.glob("results/raw/histogram_benchmark_*.csv"))
    if not files:
        print("No se encontraron CSVs de histograma en results/raw/")
        sys.exit(1)
    csv_file = files[-1]

groups = defaultdict(list)

with open(csv_file, newline="") as f:
    reader = csv.DictReader(f)
    for row in reader:
        groups[row["mode"]].append(float(row["histogram_time"]))

summary = []

for mode, values in groups.items():
    avg = statistics.mean(values)
    stdev = statistics.stdev(values) if len(values) > 1 else 0.0
    summary.append((avg, stdev, mode, len(values)))

summary.sort()

print(f"Archivo analizado: {csv_file}")
print()
print("Comparacion de modos de histograma:")
print()
print(f"{'mode':<15} {'avg_time':<12} {'stdev':<12} {'n':<5}")
print("-" * 50)

for avg, stdev, mode, n in summary:
    print(f"{mode:<15} {avg:<12.6f} {stdev:<12.6f} {n:<5}")
