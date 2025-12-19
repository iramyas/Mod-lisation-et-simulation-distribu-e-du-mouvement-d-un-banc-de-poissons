# Usage: gnuplot -e "csv='experiments/bench.csv'; N=500; out='figures/bench_time_vs_radius.png'" experiments/plot_time_vs_radius.gp
# CSV columns: mode,N,radius,cellSize,steps,ms_total,ms_per_step

# Note: we filter with awk which outputs whitespace-separated columns.
# So we keep the default datafile separator (whitespace).
set term pngcairo size 1200,700
set output out

set title sprintf('Coût de calcul vs rayon (N=%d)', N)
set xlabel 'Rayon de voisinage (pixels)'
set ylabel 'Temps moyen par itération (ms)'
set key top left
set grid

plot \
    sprintf("< awk -F, '$1==\"grid\" && $2==%d {print $3, $7}' %s", N, csv) using 1:2 with linespoints lw 2 pt 7 title 'SpatialGrid', \
    sprintf("< awk -F, '$1==\"naive\" && $2==%d {print $3, $7}' %s", N, csv) using 1:2 with linespoints lw 2 pt 5 title 'Naïf (O(N^2))'
