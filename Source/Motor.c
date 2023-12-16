#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <stdbool.h>
#include "RoboticSystem.h"


// Funktion zum Empfangen von Nachrichten aus der Warteschlange
void receiveDirectionMessage(int msg_queue_id, long msg_type, enum Direction* dir) {
    struct DirectionMessage msg;
    if (msgrcv(msg_queue_id, &msg, sizeof(msg.FlyDirection), msg_type, 0) == -1) {
        perror("[M] Fehler beim Empfangen der Nachricht");
        exit(EXIT_FAILURE);
    }
    *dir = msg.FlyDirection;
}

int main()
{
    // Schlüssel generieren (muss derselbe wie im anderen Programm sein)
    key_t key = ftok("/tmp", 's');
    if (key == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    // Shared Memory ID abrufen
    int shmID = shmget(key, sizeof(struct SharedMemory), 0644);
    if (shmID == -1) {
        perror("[M] shmget");
        exit(EXIT_FAILURE);
    }

    // Shared Memory anhängen
    struct SharedMemory *sharedData = shmat(shmID, NULL, 0);
    if (sharedData == (void *)-1) {
        perror("[M] shmat");
        exit(EXIT_FAILURE);
    }


    // Anschließen an die Nachrichtenwarteschlange
    int msg_queue_id = msgget(MSGKEY, 0666 | IPC_CREAT);
    if (msg_queue_id == -1) {
        perror("[M] Fehler beim Anschließen an die Nachrichtenwarteschlange");
        exit(EXIT_FAILURE);
    }


    // Hauptlogik des Mootors
    while (1) {
        // Empfangen Sie Steuerbefehle von der Nachrichtenwarteschlange
        enum Direction flyDirection;
        receiveDirectionMessage(msg_queue_id, FLYDIRECTIONMSGTYPE, &flyDirection);

        // Hier interpretieren Sie die Steuerbefehle und führen Aktionen aus
        // Führe eine Motor basierend auf dem Steuerbefehl aus
        printf("[M] Motorprozess empfing Steuerbefehl: %d\n", flyDirection);

        int xPos = sharedData->GPSPosition.XPos;
        int yPos = sharedData->GPSPosition.YPos;
        if (flyDirection == UP)
        {
            sharedData->Grid[yPos][xPos] = 0;
            yPos = yPos - 1;
            sharedData->Grid[yPos][xPos] = 2; // 2 als drown id
        }
        else if (flyDirection == DOWN)
        {
            sharedData->Grid[yPos][xPos] = 0;
            yPos = yPos + 1;
            sharedData->Grid[yPos][xPos] = 2;
        }
        else if (flyDirection == RIGHT)
        {
            sharedData->Grid[yPos][xPos] = 0;
            xPos = xPos + 1;
            sharedData->Grid[yPos][xPos] = 2;
        }
        else if (flyDirection == LEFT)
        {
            sharedData->Grid[yPos][xPos] = 0;
            xPos = xPos - 1;
            sharedData->Grid[yPos][xPos] = 2;
        }
        sharedData->GPSPosition.XPos = xPos;
        sharedData->GPSPosition.YPos = yPos;

        //sleep(2); // Beispiel: Wartezeit zwischen Aktionen (in Sekunden)
    }

}