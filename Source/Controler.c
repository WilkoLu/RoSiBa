#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <stdbool.h>
#include "RoboticSystem.h"


/// @brief Wenn msgrcv eine Message [msg] mit {Type 1} mit ID [msg_queue_id] und Type [msg_type] empfängt. Setze die empfangene GPS Position in X und Y
/// @param msg_queue_id 
/// @param msg_type 
/// @param gpsPos 
void ReceiveGPSPosMessage(int msg_queue_id, long msg_type, struct Position2D* gpsPos) {
    struct GPSPosMessage msg;
    if (msgrcv(msg_queue_id, &msg, sizeof(msg.GPSPosition), msg_type, 0) == -1) {
        perror("[C] Error while receiving the GPSPosMessage.");
        exit(EXIT_FAILURE);
    }
    gpsPos->XPos = msg.GPSPosition.XPos;
    gpsPos->YPos = msg.GPSPosition.YPos;
}

/// @brief Wenn msgrcv eine Message [msg] {Type 2} mit ID [msg_queue_id] und Type [msg_type] empfängt. Setze die empfangenen Umgebungswerte [0 - Kein Hindernis] oder [1 - Hindernis]
/// @param msg_queue_id 
/// @param msg_type 
/// @param surrounding 
void ReceiveSurroundingMessage(int msg_queue_id, long msg_type, struct DroneSurrounding* surrounding) {
    struct SurroundingMessage msg;
    if (msgrcv(msg_queue_id, &msg, sizeof(msg.Surrounding), msg_type, 0) == -1) {
        perror("[C] Error while receiving the SurroundingMessage.");
        exit(EXIT_FAILURE);
    }
    surrounding->Top = msg.Surrounding.Top;
    surrounding->TopRight = msg.Surrounding.TopRight;
    surrounding->Right = msg.Surrounding.Right;
    surrounding->BottomRight = msg.Surrounding.BottomRight;
    surrounding->Bottom = msg.Surrounding.Bottom;
    surrounding->BottomLeft = msg.Surrounding.BottomLeft;
    surrounding->Left = msg.Surrounding.Left;
    surrounding->TopLeft = msg.Surrounding.TopLeft;
}

/// @brief Wenn msgrcv eine Message [msg] {Type 3} mit ID [msg_queue_id] und Type [msg_type] empfängt. Setze die empfangenen Paketdaten HasPackage [True | false] und IsDropping [True | false]
/// @param msg_queue_id 
/// @param msg_type 
/// @param packData 
void ReceivePackageDataMessage(int msg_queue_id, long msg_type, struct PackageData* packData) {
    struct PackageDataMessage msg;
    if (msgrcv(msg_queue_id, &msg, sizeof(msg.PackageInfo), msg_type, 0) == -1) {
        perror("[C] Error while receiving the PackageDataMessage.");
        exit(EXIT_FAILURE);
    }
    packData->HasPackage = msg.PackageInfo.HasPackage;
    packData->IsDropping = msg.PackageInfo.IsDropping;
}

/// @brief Sendet Message an den Motor {Type 8} mit FlyDirection UP DOWN RIGHT LEFT
/// @param msg_queue_id 
/// @param msg_type 
/// @param  
void SendDirectionMessage(int msg_queue_id, long msg_type, enum Direction direction) {
    struct DirectionMessage msg;
    msg.MsgType = msg_type;
    msg.FlyDirection = direction;
    msgsnd(msg_queue_id, &msg, sizeof(msg.FlyDirection), 0);
}

/// @brief Sendet Message an den Droper {Type 9} Dropbefehl ausführen. Drop = true
/// @param msg_queue_id 
/// @param msg_type 
/// @param drop 
void SendDropMessage(int msg_queue_id, long msg_type, bool drop) {
    struct DropMessage msg;
    msg.MsgType = msg_type;
    msg.DropPackage = drop;
    msgsnd(msg_queue_id, &msg, sizeof(msg.DropPackage), 0);
}

int main()
{
    int targetPointX = 11;
    int targetPointY = 7;

    key_t key = ftok("/tmp", 's');
    
    if (key == -1)
    {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    // Erstellen oder Zugriff auf den Shared Memory
    int shmID = shmget(SHMKEY, sizeof(struct SharedMemory), IPC_CREAT | 0644);
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
    sharedData->Grid[2][1] = 1;
    sharedData->Grid[13][41] = 1;

    sharedData->GPSPosition.XPos = 0;
    sharedData->GPSPosition.YPos = 0;


    // Anschließen an die Nachrichtenwarteschlange
    int msg_queue_id = msgget(MSGKEY, 0666 | IPC_CREAT);
    if (msg_queue_id == -1) {
        perror("[C] Error while adding to the message queue.");
        exit(EXIT_FAILURE);
    }



    // Hauptlogik des Controllerprozesses
    while (1) {
        // Hier bewerten Sie Sensordaten und planen Pfade
        // Beispiel: Empfange Sensordaten von der Nachrichtenwarteschlange
        struct Position2D sensorGPSPos;
        struct PackageData sensorPackageData;
        struct DroneSurrounding sensorDroneSurrounding;
        
        
        ReceiveGPSPosMessage(msg_queue_id, GPSPOSMSGTYPE, &sensorGPSPos);
        ReceiveSurroundingMessage(msg_queue_id, SURROUNDINGMSGTYPE, &sensorDroneSurrounding);
        ReceivePackageDataMessage(msg_queue_id, PACKAGEDATAMSGTYPE, &sensorPackageData);

        // Hier könnten Sie die Pfadplanungslogik implementieren
        printf("[C] Controllerprocess received GPS-sensordata: %d %d\n", sensorGPSPos.XPos, sensorGPSPos.YPos);
        printf("[C] Controllerprocess received Surroundingdata-sensordata: %d %d %d ....\n", sensorDroneSurrounding.Top, sensorDroneSurrounding.TopRight, sensorDroneSurrounding.Right);
        printf("[C] Controllerprocess received Packagedata-sensordata: %d %d\n", sensorPackageData.HasPackage, sensorPackageData.IsDropping);

        // Hier generieren Sie Steuerbefehle basierend auf den bewerteten Daten
        // Beispiel: Einfache Steuerbefehlsgenerierung
        enum Direction nextDirection;
        if (sensorGPSPos.XPos < targetPointX)
        {
            nextDirection = RIGHT;
        }
        else if (sensorGPSPos.XPos > targetPointX)
        {
            nextDirection = LEFT;
        }
        else if (sensorGPSPos.YPos < targetPointY)
        {
            nextDirection = DOWN;
        }
        else if (sensorGPSPos.YPos > targetPointY)
        {
            nextDirection = UP;
        }
        else 
        {
            if (sensorPackageData.HasPackage == true && sensorPackageData.IsDropping == false)
            {
                // send drop befehl                
                SendDropMessage(msg_queue_id, DROPMSGTYPE, true);
                sleep(2); 
                continue;
            }
            else if (sensorPackageData.IsDropping == true)
            {
                continue;
            }
            else if (sensorPackageData.HasPackage == false && sensorPackageData.IsDropping == false)
            {
                // ToDo: new target point
                targetPointX = 0;
                targetPointY = 0;
            }
            continue;
        }
        
        // Senden Sie die generierten Steuerbefehle an die Nachrichtenwarteschlange
        SendDirectionMessage(msg_queue_id, FLYDIRECTIONMSGTYPE, nextDirection);

        sleep(2); // Beispiel: Intervall zwischen Pfadbewertungen und Steuerbefehlsgenerierung (in Sekunden)
    }



    // Aufräumarbeiten (normalerweise wird dies nicht erreicht)
    if (msgctl(msg_queue_id, IPC_RMID, NULL) == -1) {
        perror("[C] Error while deleting the message queue.");
        exit(EXIT_FAILURE);
    }

    return 0;


}
