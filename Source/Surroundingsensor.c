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

void CalculateSuroundings(struct SharedMemory *sharedData, struct DroneSurrounding *surrounding) {

    int x = sharedData->GPSPosition.XPos;
    int y = sharedData->GPSPosition.YPos;

    printf("testX %d \n", x);
    printf("testY %d \n", y);

    int halfWindowSize = WINDOW_SIZE / 2;

    int surroundings[WINDOW_SIZE][WINDOW_SIZE];

    for (int i = -halfWindowSize; i <= halfWindowSize; ++i)
    {
        for (int j = -halfWindowSize; j <= halfWindowSize; ++j)
        {
            //surroundings[i + halfWindowSize][j + halfWindowSize] = 1;
            printf("%d",y + i);
            printf("%d",x + i);
            if ((y + i) < 0 || (y + i) > MAX_Y || (x + j) < 0 || (x + j) > MAX_X)
            {
                surroundings[i + halfWindowSize][j + halfWindowSize] = 1;
            }
            else
            {
                surroundings[i + halfWindowSize][j + halfWindowSize] = sharedData->Grid[y + i][x + j];
            }
        }
    }
    
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

        CalculateSuroundings(sharedData, &mySurrounding);

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