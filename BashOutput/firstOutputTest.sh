#!/bin/bash

clear_screen() {
    tput reset
}

draw_drone() {
    local x=$1
    local y=$2

    tput cup $y $x
    echo "  ^  "
    tput cup $((y + 1)) $x
    echo " /X\\ "
    tput cup $((y + 2)) $x
    echo "|---|"
}

clear_screen

x=0
y=0

while true; do
    clear_screen
    draw_drone $x $y

    ((x += 2))
    ((y += 1))

    if [ $x -ge $(( $(tput cols) - 5 )) ] || [ $y -ge $(( $(tput lines) - 3 )) ]; then
        x=0
        y=0
    fi

    sleep 0.1
done
