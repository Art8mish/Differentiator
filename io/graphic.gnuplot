reset
set terminal png size 400,300 enhanced font "Arial,10"
set output 'graphic.png'
plot log ((x) + (1))