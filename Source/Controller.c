#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <time.h>
#include <sys/ipc.h>
#include <stdbool.h>
#include <errno.h>
#include "RoboticSystem.h"
#include "Logging.h"

/// @brief Wenn msgrcv eine Message [msg] mit {Type 1} mit ID [msg_queue_id] und Type [msg_type] empfängt. Setze die empfangene GPS Position in X und Y
/// @param msg_queue_id
/// @param msg_type
/// @param gpsPos
void ReceiveGPSPosMessage(int msg_queue_id, long msg_type, struct Position2D *gpsPos)
{
    struct GPSPosMessage msg;

    while (1)
    {
        if (msgrcv(msg_queue_id, &msg, sizeof(msg.GPSPosition), msg_type, IPC_NOWAIT | MSG_NOERROR) == -1)
        {

            if (errno == ENOMSG)
            {
                // Keine Nachricht vorhanden
                // printf("[C] No message available.\n");
                break;
            }
            else
            {
                perror("[C] Error while receiving the GPSMessage.");
                exit(EXIT_FAILURE);
            }
        }
    }

    if (msgrcv(msg_queue_id, &msg, sizeof(msg.GPSPosition), msg_type, 0) == -1)
    {
        perror("[C] Error while receiving the GPSPosMessage.");
        exit(EXIT_FAILURE);
    }
    gpsPos->XPos = msg.GPSPosition.XPos;
    gpsPos->YPos = msg.GPSPosition.YPos;
}


/// @brief Entfernt alle alten Nachrichten und wartet auf nächste Nachricht.
/// Wenn msgrcv eine Message [msg] {Type 2} mit ID [msg_queue_id] und Type [msg_type] empfängt. 
/// Setze die empfangenen Umgebungswerte [0 - Kein Hindernis] oder [1 - Hindernis]
/// @param msg_queue_id
/// @param msg_type
/// @param surrounding
void ReceiveNewSurroundingMessage(int msg_queue_id, long msg_type, struct DroneSurrounding *surrounding)
{
    struct SurroundingMessage msg;

    while (1)
    {
        if (msgrcv(msg_queue_id, &msg, sizeof(msg.Surrounding), msg_type, IPC_NOWAIT | MSG_NOERROR) == -1)
        {

            if (errno == ENOMSG)
            {
                // Keine Nachricht vorhanden
                // printf("[C] No message available.\n");
                break;
            }
            else
            {
                perror("[C] Error while receiving the SurroundingMessage.");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            /* code */
        }

        // alle Nachrichten abarbeiten bis keine mehr da sind
        // da sie nicht aktuell sind und wir nur die aktuellste brauchen
    }
    if (msgrcv(msg_queue_id, &msg, sizeof(msg.Surrounding), msg_type, 0) == -1)
    {
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



/// @brief Entfernt alle alten Nachrichten und wartet auf nächste Nachricht.
/// Wenn msgrcv eine Message [msg] {Type 3} mit ID [msg_queue_id] und Type [msg_type] empfängt. 
/// Setze die empfangenen Paketdaten HasPackage [True | false] und IsDropping [True | false]
/// @param msg_queue_id
/// @param msg_type
/// @param packData
void ReceiveNewPackageDataMessage(int msg_queue_id, long msg_type, struct PackageData *packData)
{
    struct PackageDataMessage msg;

    while (1)
    {
        if (msgrcv(msg_queue_id, &msg, sizeof(msg.PackageInfo), msg_type, IPC_NOWAIT | MSG_NOERROR) == -1)
        {

            if (errno == ENOMSG)
            {
                // Keine Nachricht vorhanden
                // printf("[C] No message available.\n");
                break;
            }
            else
            {
                perror("[C] Error while receiving the PackageDataMessage.");
                exit(EXIT_FAILURE);
            }
        }
        // alle Nachrichten abarbeiten bis keine mehr da sind
        // da sie nicht aktuell sind und wir nur die aktuellste brauchen
    }
    if (msgrcv(msg_queue_id, &msg, sizeof(msg.PackageInfo), msg_type, 0) == -1)
    {
        perror("[C] Error while receiving the PackageDataMessage.");
        exit(EXIT_FAILURE);
    }
    packData->HasPackage = msg.PackageInfo.HasPackage;
    packData->IsDropping = msg.PackageInfo.IsDropping;
}

/// @brief Sendet Message an den Engine {Type 8} mit FlyDirection UP DOWN RIGHT LEFT
/// @param msg_queue_id
/// @param msg_type
/// @param
void SendDirectionMessage(int msg_queue_id, long msg_type, enum Direction direction)
{
    struct DirectionMessage msg;
    msg.MsgType = msg_type;
    msg.FlyDirection = direction;
    msgsnd(msg_queue_id, &msg, sizeof(msg.FlyDirection), 0);

    // Logging der gesendeten Nachricht in die Logdatei
    char directionString[] = "[C] Controllerprocess sent DirectionMessage: ";
    strcat(directionString, directionToString(direction));
    writeToLog(CONTROLLER_LOG_FILE, directionString);
}

/// @brief Sendet Message an den Dropper {Type 9} Dropbefehl ausführen. Drop = true
/// @param msg_queue_id
/// @param msg_type
/// @param drop
void SendDropMessage(int msg_queue_id, long msg_type, bool drop)
{
    struct DropMessage msg;
    msg.MsgType = msg_type;
    msg.DropPackage = drop;
    msgsnd(msg_queue_id, &msg, sizeof(msg.DropPackage), 0);

    // Logging der gesendeten Nachricht in die Logdatei
    char dropString[] = "[C] Controllerprocess sent DropMessage: ";
    strcat(dropString, boolToString(drop));
    writeToLog(CONTROLLER_LOG_FILE, dropString);
}

/// @brief Berechnet den nächsten Schritt anhand der GPS Position, der Ziel Position und der Umgebung
/// @param gpsPos
/// @param targetPos
/// @param surrounding
/// @return enum Direction
enum Direction calculateNextStep(struct Position2D gpsPos, struct Position2D targetPos, struct DroneSurrounding surrounding)
{
    // Entscheide zufällig, ob in X- oder Y-Richtung bewegt wird
    int randomDirection = rand() % 2;

    if (randomDirection == 0) // Bewege dich in X-Richtung
    {
        if (gpsPos.XPos < targetPos.XPos && surrounding.Right == 0)
        {
            return RIGHT;
        }
        else if (gpsPos.XPos > targetPos.XPos && surrounding.Left == 0)
        {
            return LEFT;
        }
    }
    else //Bewege dich in Y-Richtung
    {
        if (gpsPos.YPos < targetPos.YPos && surrounding.Bottom == 0)
        {
            return DOWN;
        }
        else if (gpsPos.YPos > targetPos.YPos && surrounding.Top == 0)
        {
            return UP;
        }
    }

    // Wenn die gewählte Richtung nicht möglich ist, dann probiere die andere Richtung
    if (randomDirection == 0) // Bewege dich in X-Richtung
    {
        if (gpsPos.YPos < targetPos.YPos && surrounding.Bottom == 0)
        {
            return DOWN;
        }
        else if (gpsPos.YPos > targetPos.YPos && surrounding.Top == 0)
        {
            return UP;
        }
    }
    else // Bewege dich in Y-Richtung
    {
        if (gpsPos.XPos < targetPos.XPos && surrounding.Right == 0)
        {
            return RIGHT;
        }
        else if (gpsPos.XPos > targetPos.XPos && surrounding.Left == 0)
        {
            return LEFT;
        }
    }

    // Wenn keine Richtung möglich ist, dann finde zumindest freien Weg
    if (surrounding.Bottom == 0)
    {
        return DOWN;
    }
    else if (surrounding.Top == 0)
    {
        return UP;
    }
    else if (surrounding.Right == 0)
    {
        return RIGHT;
    }
    else if (surrounding.Left == 0)
    {
        return LEFT;
    }
    
    // Dieser Fall sollte nicht eintreten
    printf("No direction found.\n");
    exit(EXIT_FAILURE);
}

enum Direction calculateNextStep2(struct Position2D gpsPos, struct Position2D targetPos, struct DroneSurrounding surrounding) {
    // Calculate differences in X and Y positions
    int diffX = targetPos.XPos - gpsPos.XPos;
    int diffY = targetPos.YPos - gpsPos.YPos;

    // Prioritize the direction with the largest difference
    if (abs(diffX) > abs(diffY)) {
        // Move in X direction if possible
        if (diffX > 0 && surrounding.Right == 0) {
            return RIGHT;
        } else if (diffX < 0 && surrounding.Left == 0) {
            return LEFT;
        }
    } else {
        // Move in Y direction if possible
        if (diffY > 0 && surrounding.Bottom == 0) {
            return DOWN;
        } else if (diffY < 0 && surrounding.Top == 0) {
            return UP;
        }
    }

    // If preferred direction is blocked, try the other direction
    if (diffY != 0) {
        if (diffY > 0 && surrounding.Bottom == 0) {
            return DOWN;
        } else if (diffY < 0 && surrounding.Top == 0) {
            return UP;
        }
    }
    if (diffX != 0) {
        if (diffX > 0 && surrounding.Right == 0) {
            return RIGHT;
        } else if (diffX < 0 && surrounding.Left == 0) {
            return LEFT;
        }
    }
    
    exit(EXIT_FAILURE);
    // If both directions are blocked, stay in place or implement a more complex strategy
    //return STAY; // STAY is a new enum value indicating no movement.
}


/// @brief Generiert [numberOfObstacles] Hindernisse und schreibt diese ins Grid im SharedData
/// @param sharedData
/// @param numberOfObstacles
void generateObstacles(struct SharedData *sharedData, int numberOfObstacles)
{
    srand(850); // Initialisiere den Zufallszahlengenerator

    for (int i = 0; i < numberOfObstacles; ++i)
    {
        int randomX = rand() % MAX_X; // Zufällige X-Position zwischen 0 und MAX_X-1
        int randomY = rand() % MAX_Y; // Zufällige Y-Position zwischen 0 und MAX_Y-1

        sharedData->Grid[randomY][randomX] = 1; // Setze die Position auf 1
    }
}

/// @brief Generiert [numberOfObstacles] Hindernisse und schreibt diese ins Grid im SharedData
/// @param sharedData
/// @param numberOfObstacles
void generateCustomObstacles(struct SharedData *sharedData)
{
    sharedData->Grid[5][5] = 1;
    sharedData->Grid[7][8] = 1;
    sharedData->Grid[8][4] = 1;
    sharedData->Grid[2][3] = 1;
    sharedData->Grid[6][1] = 1;
    sharedData->Grid[0][4] = 1;
    sharedData->Grid[1][7] = 1;
}

void makeNextStep(struct Position2D sensorGPSPos, struct Position2D targetPosition, struct DroneSurrounding sensorDroneSurrounding, int msg_queue_id)
{
    // Wenn am Ziel, mache Nichts
    if (sensorGPSPos.XPos == targetPosition.XPos &&
        sensorGPSPos.YPos == targetPosition.YPos)
    {
        return;
    }

    enum Direction nextDirection;
    nextDirection = calculateNextStep(sensorGPSPos, targetPosition, sensorDroneSurrounding);
    // Sende Steuerbefehl an die Nachrichtenwarteschlange
    SendDirectionMessage(msg_queue_id, FLYDIRECTIONMSGTYPE, nextDirection);
}

bool isSamePosition(struct Position2D pos1, struct Position2D pos2) {
    return pos1.XPos == pos2.XPos && pos1.YPos == pos2.YPos;
}

bool isAtTarget(struct Position2D currentPos, struct Position2D targetPos) {
    return isSamePosition(currentPos, targetPos);
}

bool shouldDropPackage(struct PackageData packageData) {
    return packageData.HasPackage == true && packageData.IsDropping == false;
}

void resetToOrigin(struct Position2D* targetPosition, struct Position2D* lastGPSPos, struct SharedData* sharedData) {
    targetPosition->XPos = 0;
    targetPosition->YPos = 0;
    sharedData->TargetPosition.XPos = targetPosition->XPos;
    sharedData->TargetPosition.YPos = targetPosition->YPos;
    lastGPSPos->XPos = 0;
    lastGPSPos->YPos = -1;
}

int main()
{
    srand(468);

    // Generiere Zufallszahlen für targetPointX und targetPointY
    struct Position2D targetPosition = {0, 0};
    targetPosition.XPos = rand() % (MAX_X); // Zufallszahl zwischen 0 und maxX
    targetPosition.YPos = rand() % (MAX_Y); // Zufallszahl zwischen 0 und maxY
    // targetPosition.XPos = 39; // Zufallszahl zwischen 0 und maxX
    // targetPosition.YPos = 19; // Zufallszahl zwischen 0 und maxY

    // Erstellen des Shared Memory
    struct SharedData *sharedData = createShm();

    // Erstellen an die Nachrichtenwarteschlange
    int msg_queue_id = createMessageQueue();

    // Initialisierung des Shared Memory mit den Hindernisdaten
    for (int i = 0; i < MAX_Y; i++)
    {
        for (int j = 0; j < MAX_X; j++)
        {
            sharedData->Grid[i][j] = 0;
        }
    }

    generateObstacles(sharedData, 35);

    // Setze die Startposition auf 0,0
    sharedData->GPSPosition.XPos = 0;
    sharedData->GPSPosition.YPos = 0;

    sharedData->TargetPosition.XPos = targetPosition.XPos;
    sharedData->TargetPosition.YPos = targetPosition.YPos;
    // Noch kein abgeworfenes Paket zu Anfang
    sharedData->PackageDropPosition.XPos = -1;
    sharedData->PackageDropPosition.YPos = -1;

    // Hauptlogik des Controllerprozesses
    struct Position2D lastGPSPos = {0, -1};
    while (1)
    {
        struct Position2D sensorGPSPos;
        struct PackageData sensorPackageData;
        struct DroneSurrounding sensorDroneSurrounding;

        ReceiveGPSPosMessage(msg_queue_id, GPSPOSMSGTYPE, &sensorGPSPos);

        // Gleiche Position wie letztes Mal
        if (isSamePosition(sensorGPSPos, lastGPSPos))
        {
            // Wenn am Ziel
            if (isAtTarget(sensorGPSPos, targetPosition))
            {
                ReceiveNewPackageDataMessage(msg_queue_id, PACKAGEDATAMSGTYPE, &sensorPackageData);
                printf("[C] Controllerprocess received Packagedata-sensordata: %d %d\n", sensorPackageData.HasPackage, sensorPackageData.IsDropping);

                if (shouldDropPackage(sensorPackageData))
                {
                    // Sende Drop-Befehl
                    SendDropMessage(msg_queue_id, DROPMSGTYPE, true);
                }
                else if (sensorPackageData.HasPackage == false)
                {
                    // Setze Ziel zum Ursprung
                    resetToOrigin(&targetPosition, &lastGPSPos, sharedData);
                }
            }
            else 
            {
                usleep(SAMPLERATE_CONTROLLER * 1000);
            }
            
            // usleep(SAMPLERATE_CONTROLLER * 1000);
            continue;
        }
        lastGPSPos.XPos = sensorGPSPos.XPos;
        lastGPSPos.YPos = sensorGPSPos.YPos;
        printf("[C] Controllerprocess received GPS-sensordata: %d %d\n", sensorGPSPos.XPos, sensorGPSPos.YPos);

        ReceiveNewSurroundingMessage(msg_queue_id, SURROUNDINGMSGTYPE, &sensorDroneSurrounding);
        printf("[C] Controllerprocess received Surroundingdata-sensordata: %d %d %d %d %d %d %d %d \n",
               sensorDroneSurrounding.Top, sensorDroneSurrounding.TopRight, sensorDroneSurrounding.Right, sensorDroneSurrounding.BottomRight,
               sensorDroneSurrounding.Bottom, sensorDroneSurrounding.BottomLeft, sensorDroneSurrounding.Left, sensorDroneSurrounding.TopLeft);

        makeNextStep(sensorGPSPos, targetPosition, sensorDroneSurrounding, msg_queue_id);
    }

    return 0;
}
