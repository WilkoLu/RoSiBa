#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <stdbool.h>
#include "RoboticSystem.h"



void ReceiveGPSPosMessage(int msg_queue_id, long msg_type, struct Position2D* gpsPos) {
    struct GPSPosMessage msg;
    if (msgrcv(msg_queue_id, &msg, sizeof(msg.GPSPosition), msg_type, 0) == -1) {
        perror("Fehler beim Empfangen der Nachricht");
        exit(EXIT_FAILURE);
    }
    gpsPos->XPos = msg.GPSPosition.XPos;
    gpsPos->YPos = msg.GPSPosition.YPos;
}

void sendDirectionMessage(int msg_queue_id, long msg_type, enum Direction direction) {
    struct DirectionMessage msg;
    msg.MsgType = msg_type;
    msg.FlyDirection = direction;
    msgsnd(msg_queue_id, &msg, sizeof(msg.FlyDirection), 0);
}

int main()
{
    int targetPointX = 11;
    int targetPointY = 70;

    key_t key = ftok("/tmp", 's');
    
    if (key == -1)
    {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    // Erstellen oder Zugriff auf den Shared Memory
    int shmID = shmget(key, sizeof(struct SharedMemory), IPC_CREAT | 0644);
    if (shmID == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    // Attach des Shared Memory
    struct SharedMemory *sharedData = shmat(shmID, NULL, 0);
    if (sharedData == (void *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    // Initialisierung des Shared Memory mit den Hindernisdaten
    for (int i = 0; i < MAX_Y; i++) {
        for (int j = 0; j < MAX_X; j++) {
            sharedData->Grid[i][j] = 0;
        }
    }

    sharedData->Grid[5][20] = 1;
    sharedData->Grid[10][40] = 1;
    sharedData->Grid[15][60] = 1;
    sharedData->Grid[20][10] = 1;
    sharedData->Grid[2][2] = 1;
    sharedData->Grid[13][41] = 1;

    sharedData->GPSPosition.XPos = 0;
    sharedData->GPSPosition.YPos = 0;


    // Anschließen an die Nachrichtenwarteschlange
    int msg_queue_id = msgget(7681, 0666 | IPC_CREAT);
    if (msg_queue_id == -1) {
        perror("[C] Fehler beim Anschließen an die Nachrichtenwarteschlange");
        exit(EXIT_FAILURE);
    }



    // Hauptlogik des Controllerprozesses
    while (1) {
        // Hier bewerten Sie Sensordaten und planen Pfade
        // Beispiel: Empfange Sensordaten von der Nachrichtenwarteschlange
        struct Position2D sensorGPSPos;
        
        ReceiveGPSPosMessage(msg_queue_id, 1, &sensorGPSPos);

        // Hier könnten Sie die Pfadplanungslogik implementieren
        printf("[C] Controllerprozess empfing GPS Sensordaten: %d %d\n", sensorGPSPos.XPos, sensorGPSPos.YPos);

        // Hier generieren Sie Steuerbefehle basierend auf den bewerteten Daten
        // Beispiel: Einfache Steuerbefehlsgenerierung
        enum Direction nextDirection;
        if (sensorGPSPos.XPos < targetPointX)
        {
            nextDirection = DOWN;
        }
        else if (sensorGPSPos.XPos > targetPointX)
        {
            nextDirection = UP;
        }
        else if (sensorGPSPos.YPos < targetPointY)
        {
            nextDirection = RIGHT;
        }
        else if (sensorGPSPos.YPos > targetPointY)
        {
            nextDirection = LEFT;
        }
        else 
        {
            /* code */
            // send drop befehl
            sleep(2);
            continue;
        }
        
        // Senden Sie die generierten Steuerbefehle an die Nachrichtenwarteschlange
        sendDirectionMessage(msg_queue_id, 8, nextDirection);

        sleep(2); // Beispiel: Intervall zwischen Pfadbewertungen und Steuerbefehlsgenerierung (in Sekunden)
    }



    // Aufräumarbeiten (normalerweise wird dies nicht erreicht)
    if (msgctl(msg_queue_id, IPC_RMID, NULL) == -1) {
        perror("[C] Fehler beim Löschen der Nachrichtenwarteschlange");
        exit(EXIT_FAILURE);
    }

    return 0;


}
