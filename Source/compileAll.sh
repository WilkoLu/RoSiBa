#!/bin/bash

# for file in *.c;do
# 	gcc "$file" -o "${file%.c}.out"
# done


# ersteinmal manuell alle dateien reinschreiben, da das bash output script mit -lncurses compiliert werden muss
gcc "GPSSensor.c" -o "GPSSensor.out"