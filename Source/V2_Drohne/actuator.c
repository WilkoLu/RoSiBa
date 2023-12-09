#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include "robotics_system.h"

// Funktion zum Empfangen von Nachrichten aus der Warteschlange
void receive_message(int msg_queue_id, long msg_type, char* msg_text) {
    struct message msg;
    if (msgrcv(msg_queue_id, &msg, sizeof(msg.msgText), msg_type, 0) == -1) {
        perror("Fehler beim Empfangen der Nachricht");
        exit(EXIT_FAILURE);
    }
    strncpy(msg_text, msg.msgText, sizeof(msg.msgText));
}

int main() {
    int x = 0;
    int y = 0;
    // Schlüssel generieren (muss derselbe wie im anderen Programm sein)
    key_t key = ftok("/tmp", 's');
    if (key == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    // Shared Memory ID abrufen
    int shmID = shmget(key, sizeof(struct SharedData), 0644);
    if (shmID == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    // Shared Memory anhängen
    struct SharedData *sharedData = shmat(shmID, NULL, 0);
    if (sharedData == (void *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    // Beispiel: Zugriff auf den Shared Memory
    printf("Value at obstacles[5][20]: %d\n", sharedData->obstacles[5][20]);

    // Anschließen an die Nachrichtenwarteschlange
    int msg_queue_id = msgget(7681, 0666 | IPC_CREAT);
    if (msg_queue_id == -1) {
        perror("Fehler beim Anschließen an die Nachrichtenwarteschlange");
        exit(EXIT_FAILURE);
    }

    // Hauptlogik des Aktuatorprozesses
    while (1) {
        // Empfangen Sie Steuerbefehle von der Nachrichtenwarteschlange
        char control_command[MSGSIZE];
        receive_message(msg_queue_id, 2, control_command);

        // Hier interpretieren Sie die Steuerbefehle und führen hypothetische Aktionen aus
        // Beispiel: Führe eine Aktion basierend auf dem Steuerbefehl aus
        printf("Aktuatorprozess empfing Steuerbefehl: %s\n", control_command);

        if (strcmp(control_command, "up") == 0)
        {
            sharedData->obstacles[x][y] = 0;
            x = x - 1;
            sharedData->obstacles[x][y] = 2; // 2 als drown id
        }
        else if (strcmp(control_command, "down") == 0)
        {
            sharedData->obstacles[x][y] = 0;
            x = x + 1;
            sharedData->obstacles[x][y] = 2;
        }
        else if (strcmp(control_command, "right") == 0)
        {
            sharedData->obstacles[x][y] = 0;
            y = y + 1;
            sharedData->obstacles[x][y] = 2;
        }
        else if (strcmp(control_command, "left") == 0)
        {
            sharedData->obstacles[x][y] = 0;
            y = y - 1;
            sharedData->obstacles[x][y] = 2;
        }
        sharedData->posX = x;
        sharedData->posY = y;

        // sharedData->obstacles[x][y] = 0;
        // x = x + 1;
        // y = y + 1;
        // sharedData->obstacles[x][y] = 2;

        // Hier könnten Sie entsprechende Aktuatoraktionen ausführen

        sleep(2); // Beispiel: Wartezeit zwischen Aktionen (in Sekunden)
    }

    // Aufräumarbeiten
}