reset
set terminal png size 400,300 enhanced font "Arial,10"
set output 'graphic.png'
plot (cos ((x) ** (3))) + ((sin ((15) * (x))) ** (4))