# Usage: gnuplot -e "csv='experiments/bench.csv'; radius=50; out='figures/bench_time_vs_N.png'" experiments/plot_time_vs_N.gp
# CSV columns: mode,N,radius,cellSize,steps,ms_total,ms_per_step


set term pngcairo size 1200,700
set output out

set title sprintf('Coût de calcul vs N (radius=%g)', radius)
set xlabel 'N (nombre de boids)'
set ylabel 'Temps moyen par itération (ms)'
set key top left
set grid

# Filter via awk to keep only desired radius
plot \
    sprintf("< awk -F, '$1==\"grid\" && $3==%g {print $2, $7}' %s", radius, csv) using 1:2 with linespoints lw 2 pt 7 title 'SpatialGrid', \
    sprintf("< awk -F, '$1==\"naive\" && $3==%g {print $2, $7}' %s", radius, csv) using 1:2 with linespoints lw 2 pt 5 title 'Naïf (O(N^2))'
