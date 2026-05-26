#!/usr/bin/env python3
import csv
import glob
import statistics
import sys
from collections import defaultdict

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

baseline = statistics.mean(groups[1])

print(f"Archivo analizado: {csv_file}")
print()
print("Escalamiento por numero de hilos:")
print()
print(f"{'threads':<10} {'avg_time':<12} {'stdev':<12} {'speedup':<12} {'n':<5}")
print("-" * 55)

for threads in sorted(groups):
    values = groups[threads]
    avg = statistics.mean(values)
    stdev = statistics.stdev(values) if len(values) > 1 else 0.0
    speedup = baseline / avg
    print(f"{threads:<10} {avg:<12.6f} {stdev:<12.6f} {speedup:<12.6f} {len(values):<5}")
