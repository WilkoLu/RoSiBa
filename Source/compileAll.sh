#!/bin/bash

# for file in *.c;do
# 	gcc "$file" -o "${file%.c}.out"
# done


# ersteinmal manuell alle dateien reinschreiben, da das bash output script mit -lncurses compiliert werden muss
gcc GPSSensor.c Logging.c RoboticSystem.c -o "GPSSensor.out" 
gcc Controller.c Logging.c RoboticSystem.c -o "Controller.out"
gcc Engine.c Logging.c RoboticSystem.c -o "Engine.out"
gcc Dropper.c Logging.c RoboticSystem.c -o "Dropper.out"
gcc PackageSensor.c Logging.c RoboticSystem.c -o "PackageSensor.out"
gcc Surroundingsensor.c Logging.c RoboticSystem.c -o "Surroundingsensor.out"

gcc DroneAnimation.c RoboticSystem.c -o "DroneAnimation.out" -lncursesw
# gcc "DroneAnimation.c" -o "DroneAnimation.out" -lncurses