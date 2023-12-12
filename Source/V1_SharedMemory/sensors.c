// sensors.c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "robotics_data.h" // Hinzugefügte Header-Datei

int main() {
    key_t key = ftok("/tmp", 's');
    int shmID = shmget(key, sizeof(struct SensorData), IPC_CREAT | 0644);
    struct SensorData *sensorData = (struct SensorData *)shmat(shmID, 0, 0);

    // Beispiel für Simulation von Sensordaten
    while (1) {
        sensorData->distance = (double)rand() / RAND_MAX * 100.0; // Beispiel für Distanzsensoren
        sensorData->temperature = (double)rand() / RAND_MAX * 50.0; // Beispiel für Temperatursensoren
        sleep(1); // Simulation der Sensordatenänderung
    }
    
    return 0;
}
