set terminal png
set output "histogram.png"
set xlabel "Subintervals"
set ylabel "Frequency"
set xrange [-1:5]
set yrange [0:248998620]
set grid
unset key
set xtics rotate out
set style data histogram
set style fill solid border
set style histogram clustered
set boxwidth 0.5
plot for [COL=2:2] "data.dat" using COL:xticlabels(1)
