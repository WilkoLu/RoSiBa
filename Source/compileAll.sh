#!/bin/bash

# for file in *.c;do
# 	gcc "$file" -o "${file%.c}.out"
# done


# ersteinmal manuell alle dateien reinschreiben, da das bash output script mit -lncurses compiliert werden muss
gcc GPSSensor.c Logging.c -o "GPSSensor.out" 
gcc Controller.c Logging.c -o "Controller.out"
gcc Engine.c Logging.c -o "Engine.out"
gcc Dropper.c Logging.c -o "Dropper.out"
gcc PackageSensor.c Logging.c -o "PackageSensor.out"
gcc Surroundingsensor.c Logging.c -o "Surroundingsensor.out"

gcc "DroneAnimation.c" -o "DroneAnimation.out" -lncursesw
# gcc "DroneAnimation.c" -o "DroneAnimation.out" -lncurses