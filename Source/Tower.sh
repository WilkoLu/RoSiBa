#!/bin/bash

# Funktion zum Starten eines Prozesses im Hintergrund
start_process() {
    ./$1 &
    echo "Prozess $1 gestartet (PID: $!)."
}


# Function to cleanup
cleanup() {
    echo "Terminating all child processes..."
    pkill -TERM -P $$
    exit 0
}

# Trap Ctrl+C (SIGINT) and call the cleanup function
trap cleanup SIGINT


# Überwache die Prozess-IDs in einer Endlosschleife
while true; do

    # Überwache Steuerungsprozess
    if ! pgrep -x "Controler.out" > /dev/null; then
        echo "Steuerungsprozess wurde beendet. Starte neu..."
        start_process Controler.out
    fi

    # Überwache GPS Sensorprozess
    if ! pgrep -x "GPSSensor.out" > /dev/null; then
        echo "Sensorprozess wurde beendet. Starte neu..."
        start_process GPSSensor.out
    fi

    # # Überwache Aktuatorprozess
    # if ! pgrep -x "actuator.out" > /dev/null; then
    #     echo "Aktuatorprozess wurde beendet. Starte neu..."
    #     start_process actuator.out
    # fi

    sleep 5 # Überwachungsintervall (in Sekunden)
done
