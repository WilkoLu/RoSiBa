// actuators.c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "robotics_data.h" // Hinzugefügte Header-Datei

int main() {
    key_t key = ftok("/tmp", 's');
    int shmID = shmget(key, sizeof(struct ControlCommand), IPC_CREAT | 0644);
    struct ControlCommand *controlCommand = (struct ControlCommand *)shmat(shmID, 0, 0);

    // Beispiel für Aktuatorsteuerung basierend auf Steuerungsbefehlen
    while (1) {
        // Verarbeite Steuerungsbefehle und führe Aktionen aus
        if (controlCommand->motorControl == 1) {
            // Beispiel für Motorsteuerung
            printf("Motor wird gestartet!\n");
        } else {
            printf("Motor wird gestoppt!\n");
        }
        sleep(1); // Simulation der Aktuatoraktion
    }
    
    return 0;
}
