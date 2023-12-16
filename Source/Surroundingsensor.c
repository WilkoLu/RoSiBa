#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <stdbool.h>
#include <time.h>
#include "RoboticSystem.h"

void sendSurroundingmessage(int msg_queue_id, long msg_type, struct DroneSurrounding surrounding) {
    struct SurroundingMessage msg;
    msg.MsgType = msg_type;
    msg.Surrounding = surrounding;
    msgsnd(msg_queue_id, &msg, sizeof(msg.Surrounding), 0);
}

// Helper function to check if the coordinates are within the grid
int isWithinGrid(int nx, int ny) {
        return nx >= 0 && nx < MAX_X && ny >= 0 && ny < MAX_Y;
}

void CalculateSurroundings(struct SharedMemory *sharedData, struct DroneSurrounding *surrounding) {
    int x = sharedData->GPSPosition.XPos;
    int y = sharedData->GPSPosition.YPos;
    
    // Assigning values to each direction
    surrounding->Top        = isWithinGrid(x, y - 1) ? sharedData->Grid[y - 1][x] : 1;
    surrounding->TopRight   = isWithinGrid(x + 1, y - 1) ? sharedData->Grid[y - 1][x + 1] : 1;
    surrounding->Right      = isWithinGrid(x + 1, y) ? sharedData->Grid[y][x + 1] : 1;
    surrounding->BottomRight= isWithinGrid(x + 1, y + 1) ? sharedData->Grid[y + 1][x + 1] : 1;
    surrounding->Bottom     = isWithinGrid(x, y + 1) ? sharedData->Grid[y + 1][x] : 1;
    surrounding->BottomLeft = isWithinGrid(x - 1, y + 1) ? sharedData->Grid[y + 1][x - 1] : 1;
    surrounding->Left       = isWithinGrid(x - 1, y) ? sharedData->Grid[y][x - 1] : 1;
    surrounding->TopLeft    = isWithinGrid(x - 1, y - 1) ? sharedData->Grid[y - 1][x - 1] : 1;
}


int main()
{
    // Schlüssel generieren (muss derselbe wie im anderen Programm sein)
    key_t key = ftok("/tmp", 's');
    if (key == -1) {
        perror("[S] ftok");
        exit(EXIT_FAILURE);
    }

    // Shared Memory ID abrufen
    int shmID = shmget(key, sizeof(struct SharedMemory), 0644);
    if (shmID == -1) {
        perror("[S] shmget");
        exit(EXIT_FAILURE);
    }

    // Shared Memory anhängen
    struct SharedMemory *sharedData = shmat(shmID, NULL, 0);
    if (sharedData == (void *)-1) {
        perror("[S] shmat");
        exit(EXIT_FAILURE);
    }

    // Erstellen oder Anschließen an die Nachrichtenwarteschlange
    int msg_queue_id = msgget(MSGKEY, 0666 | IPC_CREAT);
    if (msg_queue_id == -1)
    {
        perror("[S] Fehler beim Erstellen der Nachrichtenwarteschlange");
        exit(EXIT_FAILURE);
    }


    struct DroneSurrounding mySurrounding = { .Top = 1, .TopRight = 1, .Right = 1, .BottomRight = 1, .Bottom = 1, .BottomLeft = 1, .Left = 1, .TopLeft = 1};


    // Hauptlogik des Sensorprozesses
    while (1) {
        // Hier Simulieren Sie Sensoraktivität und generieren Sie Nachrichten

        CalculateSurroundings(sharedData, &mySurrounding);

        printf("[S] Surroundingprozess sendet : %d %d %d %d ....\n", mySurrounding.Top, mySurrounding.TopRight, mySurrounding.Right, mySurrounding.BottomRight);

        sendSurroundingmessage(msg_queue_id, SURROUNDINGMSGTYPE, mySurrounding);

        sleep(2); // Simuliere einen Sensorleseintervall (in Sekunden)
    }


    // Aufräumarbeiten (normalerweise wird dies nicht erreicht)
    if (msgctl(msg_queue_id, IPC_RMID, NULL) == -1) {
        perror("[S] Fehler beim Löschen der Nachrichtenwarteschlange");
        exit(EXIT_FAILURE);
    }

    return 0;
}