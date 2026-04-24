#!/usr/bin/env python3
import csv
import subprocess

# Lire le CSV
data = {'sequential': {}, 'mpi': {}}
with open('scalability_results.csv', 'r') as f:
    reader = csv.DictReader(f)
    for row in reader:
        n = float(row['N'])
        ms = float(row['ms_per_step'])
        impl = row['implementation'].lower()
        if 'sequential' in impl:
            data['sequential'][n] = ms
        else:
            data['mpi'][n] = ms

# Préparer les données pour gnuplot
with open('plot_data.txt', 'w') as f:
    for n in sorted(data['sequential'].keys()):
        seq = data['sequential'][n]
        mpi = data['mpi'].get(n, seq)
        speedup = seq / mpi
        f.write(f"{n} {seq} {mpi} {speedup}\n")

# Générer le graphe avec gnuplot
gnuplot_script = """
set datafile separator ' '
set terminal png size 1200,700 font "Arial,12"
set output 'benchmark_comparison.png'
set xlabel 'Number of Boids (N)'
set ylabel 'Time per Step (milliseconds)'
set title 'MPI Benchmark: Sequential vs Distributed\\nPerformance Comparison'
set grid back
set key outside right top
set logscale x
plot 'plot_data.txt' using 1:2 with linespoints lw 2 pt 7 ps 1.5 title 'Sequential', \
     '' using 1:3 with linespoints lw 2 pt 5 ps 1.5 title 'MPI (1 processor)'

set output 'speedup_analysis.png'
set ylabel 'Speedup Factor'
set title 'Speedup: Sequential vs MPI\\n(higher is better)'
set logscale x
set logscale y
plot 'plot_data.txt' using 1:4 with linespoints lw 2 pt 9 ps 1.5 title 'Speedup (Sequential/MPI)'
"""

subprocess.run(['gnuplot'], input=gnuplot_script.encode(), check=True)
print("✓ Graphes générés:")
print("  - benchmark_comparison.png")
print("  - speedup_analysis.png")
