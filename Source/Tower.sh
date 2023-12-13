#!/bin/bash

# Funktion zum Starten eines Prozesses im Hintergrund
start_process() {
    ./$1 &
    echo "[T] Prozess $1 gestartet (PID: $!)."
}


# Function to cleanup
cleanup() {
    echo "[T] Terminating all child processes..."
    pkill -TERM -P $$
    exit 0
}

# Trap Ctrl+C (SIGINT) and call the cleanup function
trap cleanup SIGINT


# Überwache die Prozess-IDs in einer Endlosschleife
while true; do

    # Überwache Steuerungsprozess
    if ! pgrep -x "Controler.out" > /dev/null; then
        echo "[T] Steuerungsprozess wurde beendet. Starte neu..."
        start_process Controler.out
    fi

    # Überwache GPS Sensorprozess
    if ! pgrep -x "GPSSensor.out" > /dev/null; then
        echo "[T] GPSSensorprozess wurde beendet. Starte neu..."
        start_process GPSSensor.out
    fi

    # Überwache Aktuatorprozess
    if ! pgrep -x "Motor.out" > /dev/null; then
        echo "[T] Motorprozess wurde beendet. Starte neu..."
        start_process Motor.out
    fi

    # Überwache Aktuatorprozess
    if ! pgrep -x "Droper.out" > /dev/null; then
        echo "[T] Droperprozess wurde beendet. Starte neu..."
        start_process Droper.out
    fi

    sleep 5 # Überwachungsintervall (in Sekunden)
done
