#!/bin/bash

# Funktion zum Starten eines Prozesses im Hintergrund
start_process() {    
    ./$1 &
    echo "[T] Started process $1 (PID: $!)."
}


# Funktion zum Aufräumen
cleanup() {
    echo "[T] Terminating all child processes and cleaning inter process communication ..."

    # Alle Kindprozesse terminieren und alle Ausgabefenster schließen
    pkill -TERM -P $$
    killall DroneAnimation.out
    # Message Queue löschen
    ipcrm -Q 7681
    # Shared Memory löschen
    ipcrm -M 7681

    exit 0
}

# Trap Ctrl+C (SIGINT) --> führe "cleanup" aus
trap cleanup SIGINT


# Überwache die Prozess-IDs in einer Endlosschleife
while true; do

    # Überwache Steuerungsprozess
    if ! pgrep -f "Controller.out" > /dev/null; then
        echo "[T] Controller-process stopped working. Restart..."
        start_process Controller.out
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

    # Überwache PackageSensor
    if ! pgrep -f "PackageSensor.out" > /dev/null; then #ToDo: chaecken warum es hier nicht mit -x funktioniert
        echo "[T] Packagesensor-process stopped working. Restart..."
        start_process PackageSensor.out
    fi

    # Überwache Aktuatorprozess Engine
    if ! pgrep -x "Engine.out" > /dev/null; then
        echo "[T] Engine-process stopped working. Restart..."
        start_process Engine.out
    fi

    # Überwache Aktuatorprozess Dropper
    if ! pgrep -x "Dropper.out" > /dev/null; then
        echo "[T] Dropper-process stopped working. Restart..."
        start_process Dropper.out
    fi

    # Überwache DroneAnimation UI-Prozess
    if ! pgrep -f "DroneAnimation.out" > /dev/null; then
        echo "[T] DroneAnimation-process stopped working. Restart..."
        current_dir=$(pwd)
        cmd.exe /C start wsl bash -c "cd $current_dir && ./DroneAnimation.out;"
    fi

    sleep 5 # Überwachungsintervall (in Sekunden)
done