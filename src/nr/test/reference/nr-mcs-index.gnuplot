set terminal postscript eps enhanced color
set output 'nr-mcs-index.eps'
set xlabel 'SINR [dB]'
set ylabel 'MCS index'
set grid
set key bottom right
plot 'nr-mcs-index.dat' index 0 using 1:2 title 'QPSK' with linespoints lw 4 pt 8,\
     'nr-mcs-index.dat' index 1 using 1:2 title '16QAM' with linespoints lw 4 pt 4,\
     'nr-mcs-index.dat' index 2 using 1:2 title '64QAM' with linespoints lw 4 pt 10
