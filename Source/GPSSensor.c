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


// Funktion zum Erstellen einer logischen Nachricht aus einer Position2D
void logMessageFromPosition(const struct Position2D *position, char *buffer, size_t bufferSize)
{
    snprintf(buffer, bufferSize, "Position: X=%d, Y=%d", position->XPos, position->YPos);
}

void send_message(int msg_queue_id, long msg_type, struct Position2D Pos2D)
{
    struct GPSPosMessage msg;
    msg.MsgType = msg_type;
    msg.GPSPosition = Pos2D;
    msgsnd(msg_queue_id, &msg, sizeof(msg.GPSPosition), 0);
}

int main()
{
    
    // get Shared Memory    
    struct SharedData *sharedData = getShm();
    
    // Anschließen an die Nachrichtenwarteschlange
    int msg_queue_id = getMessageQueue();

    struct Position2D myPos = {.XPos = 0, .YPos = 0};


    // Hauptlogik des Sensorprozesses
    while (1)
    {
        // Hier Simulieren Sie Sensoraktivität und generieren Sie Nachrichten

        myPos.XPos = sharedData->GPSPosition.XPos;
        myPos.YPos = sharedData->GPSPosition.YPos;
        
        send_message(msg_queue_id, GPSPOSMSGTYPE, myPos);

        //Erstellen Sie eine logische Nachricht aus der Position
        char logMessage[100]; // Passen Sie die Puffergröße nach Bedarf an
        logMessageFromPosition(&myPos, logMessage, sizeof(logMessage));

        // Schreiben Sie einen Log-Eintrag
        writeToLog(GPS_LOG_FILE , logMessage);

        usleep(SAMPLERATE_GPSSENSOR * 1000); // Simuliere einen Sensorleseintervall (in Sekunden)
    }

    return 0;
}