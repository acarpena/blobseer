set terminal png
set output "graphic00.png"
plot "00.txt.dat" using 1:2 title 'writes' with lines, \
     "00.txt.dat" using 1:3 title 'reads' with lines

set output "graphic10.png"
plot "10.txt.dat" using 1:2 title 'writes' with lines, \
     "10.txt.dat" using 1:3 title 'reads' with lines

set output "graphic01.png"
plot "01.txt.dat" using 1:2 title 'writes' with lines, \
     "01.txt.dat" using 1:3 title 'reads' with lines

set output "graphic11.png"
plot "11.txt.dat" using 1:2 title 'writes' with lines, \
     "11.txt.dat" using 1:3 title 'reads' with lines
