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
#include "Logging.h"

void sendSurroundingmessage(int msg_queue_id, long msg_type, struct DroneSurrounding surrounding) {
    struct SurroundingMessage msg;
    msg.MsgType = msg_type;
    msg.Surrounding = surrounding;
    msgsnd(msg_queue_id, &msg, sizeof(msg.Surrounding), 0);

    // Logging
    char logMessage[100];
    sprintf(logMessage, "[S] Surroundingprocess sends : %d %d %d %d %d %d %d %d ", 
    surrounding.Top, surrounding.TopRight, surrounding.Right, surrounding.BottomRight,
    surrounding.Bottom, surrounding.BottomLeft, surrounding.Left, surrounding.TopLeft);
    writeToLog(SURROUNDING_LOG_FILE, logMessage);
}

// Helper function to check if the coordinates are within the grid
int isWithinGrid(int nx, int ny) {
        return nx >= 0 && nx < MAX_X && ny >= 0 && ny < MAX_Y;
}

void CalculateSurroundings(struct SharedData *sharedData, struct DroneSurrounding *surrounding) {
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
    // get Shared Memory
    struct SharedData *sharedData = getShm();
    
    // Erstellen oder Anschließen an die Nachrichtenwarteschlange
    int msg_queue_id = getMessageQueue();

    struct DroneSurrounding mySurrounding = { .Top = 1, .TopRight = 1, .Right = 1, .BottomRight = 1, .Bottom = 1, .BottomLeft = 1, .Left = 1, .TopLeft = 1};


    // Hauptlogik des Sensorprozesses
    while (1) {
        // Hier Simulieren Sie Sensoraktivität und generieren Sie Nachrichten

        CalculateSurroundings(sharedData, &mySurrounding);

        sendSurroundingmessage(msg_queue_id, SURROUNDINGMSGTYPE, mySurrounding);

        usleep(SAMPLERATE_SURROUNDINGSENSOR * 1000); // Simuliere einen Sensorleseintervall (in Sekunden)
    }

    return 0;
}