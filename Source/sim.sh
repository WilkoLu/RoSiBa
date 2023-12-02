#!/bin/bash

# Funktion zum Starten eines Prozesses im Hintergrund
start_process() {
    ./$1 &
    echo "Prozess $1 gestartet (PID: $!)."
}

# Überwache die Prozess-IDs in einer Endlosschleife
while true; do
    # Überwache Sensorprozess
    if ! pgrep -x "sensors" > /dev/null; then
        echo "Sensorprozess wurde beendet. Starte neu..."
        start_process sensors
    fi

    # Überwache Aktuatorprozess
    if ! pgrep -x "actuators" > /dev/null; then
        echo "Aktuatorprozess wurde beendet. Starte neu..."
        start_process actuators
    fi

    # Überwache Steuerungsprozess
    if ! pgrep -x "controller" > /dev/null; then
        echo "Steuerungsprozess wurde beendet. Starte neu..."
        start_process controller
    fi

    sleep 5 # Überwachungsintervall (in Sekunden)
done
