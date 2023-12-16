#!/bin/bash

# for file in *.c;do
# 	gcc "$file" -o "${file%.c}.out"
# done


# ersteinmal manuell alle dateien reinschreiben, da das bash output script mit -lncurses compiliert werden muss
gcc "GPSSensor.c" -o "GPSSensor.out" 
gcc "Controler.c" -o "Controler.out"
gcc "Motor.c" -o "Motor.out"
gcc "Droper.c" -o "Droper.out"
gcc "PackageSensor.c" -o "PackageSensor.out"
gcc "Surroundingsensor.c" -o "Surroundingsensor.out"

gcc "DroneAnimation.c" -o "DroneAnimation.out" -lncurses