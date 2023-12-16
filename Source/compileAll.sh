#!/bin/bash

# for file in *.c;do
# 	gcc "$file" -o "${file%.c}.out"
# done


# ersteinmal manuell alle dateien reinschreiben, da das bash output script mit -lncurses compiliert werden muss
gcc "GPSSensor.c" -o "GPSSensor.out" 
gcc "Controller.c" -o "Controller.out"
gcc "Engine.c" -o "Engine.out"
gcc "Dropper.c" -o "Dropper.out"
gcc "PackageSensor.c" -o "PackageSensor.out"
gcc "Surroundingsensor.c" -o "Surroundingsensor.out"

gcc "DroneAnimation.c" -o "DroneAnimation.out" -lncurses