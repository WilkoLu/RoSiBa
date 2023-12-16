#!/bin/bash

# Funktion zum Starten eines Prozesses im Hintergrund
start_process() {
    ./$1 &
    echo "[T] Started process $1 (PID: $!)."
}


# Function to cleanup
cleanup() {
    echo "[T] Terminating all child processes and cleaning inter process communication ..."

    # Alle Kindprozesse terminieren
    pkill -TERM -P $$
    # Message Queue löschen
    ipcrm -Q 7681
    # Shared Memory löschen
    ipcrm -M 7681

    exit 0
}

# Trap Ctrl+C (SIGINT) and call the cleanup function
trap cleanup SIGINT


# Überwache die Prozess-IDs in einer Endlosschleife
while true; do

    # Überwache Steuerungsprozess
    if ! pgrep -x "Controler.out" > /dev/null; then
        echo "[T] Controller-process stopped working. Restart..."
        start_process Controler.out
    fi

    # Überwache GPS Sensorprozess
    if ! pgrep -x "GPSSensor.out" > /dev/null; then
        echo "[T] GPSSensor-process stopped working. Restart..."
        start_process GPSSensor.out
    fi

    # Überwache Surroundingsensor
    if ! pgrep -f "Surroundingsensor.out" > /dev/null; then
        echo "[T] Surroundingsensor-process stopped working. Restart..."
        start_process Surroundingsensor.out
    fi

    # Überwache Aktuatorprozess
    if ! pgrep -x "Motor.out" > /dev/null; then
        echo "[T] Engine-process stopped working. Restart..."
        start_process Motor.out
    fi

    # Überwache Aktuatorprozess
    if ! pgrep -x "Droper.out" > /dev/null; then
        echo "[T] Droper-process stopped working. Restart..."
        start_process Droper.out
    fi

    # Überwache Aktuatorprozess
    if ! pgrep -f "PackageSensor.out" > /dev/null; then #ToDo: chaecken warum es hier nicht mit -x funktioniert
        echo "[T] Packagesensor-process stopped working. Restart..."
        start_process PackageSensor.out
    fi

    sleep 5 # Überwachungsintervall (in Sekunden)
done
