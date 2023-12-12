// controller.c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "robotics_data.h" // Hinzugefügte Header-Datei

int main() {
    key_t key = ftok("/tmp", 's');
    int shmID_sensor = shmget(key, sizeof(struct SensorData), IPC_CREAT | 0644);
    int shmID_actuator = shmget(key, sizeof(struct ControlCommand), IPC_CREAT | 0644);

    struct SensorData *sensorData = (struct SensorData *)shmat(shmID_sensor, 0, 0);
    struct ControlCommand *controlCommand = (struct ControlCommand *)shmat(shmID_actuator, 0, 0);

    // Beispiel für Auswertung von Sensordaten und Generierung von Steuerungsbefehlen
    while (1) {
        // Beispiel für Auswertung von Sensordaten und Generierung von Steuerungsbefehlen
        if (sensorData->distance > 50.0) {
            controlCommand->motorControl = 1; // Beispiel für Motor starten
        } else {
            controlCommand->motorControl = 0; // Beispiel für Motor stoppen
        }
        sleep(1); // Simulation der Steuerungsauswertung
    }
    
    return 0;
}
