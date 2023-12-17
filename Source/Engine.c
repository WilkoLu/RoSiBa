#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <stdbool.h>
#include "RoboticSystem.h"
#include "Logging.h"

// Funktion, um ENUM-Werte als Text auszugeben
const char* enumToString(enum Direction flyDirection) {
    switch (flyDirection) {
        case UP:
            return "Up";
        case DOWN:
            return "Down";
        case RIGHT:
            return "Right";
        case LEFT:
            return "Left";
    }
}

// Funktion zum Empfangen von Nachrichten aus der Warteschlange
void receiveDirectionMessage(int msg_queue_id, long msg_type, enum Direction* dir) {
    struct DirectionMessage msg;
    if (msgrcv(msg_queue_id, &msg, sizeof(msg.FlyDirection), msg_type, 0) == -1) {
        perror("[E] Error receiving message");
        exit(EXIT_FAILURE);
    }
    *dir = msg.FlyDirection;
}

int main()
{
    // get Shared Memory
    struct SharedData *sharedData = getShm();

    // Anschließen an die Nachrichtenwarteschlange
    int msg_queue_id = getMessageQueue();

    // Hauptlogik des Motors
    while (1) {
        // Empfangen Sie Steuerbefehle von der Nachrichtenwarteschlange
        enum Direction flyDirection;
        receiveDirectionMessage(msg_queue_id, FLYDIRECTIONMSGTYPE, &flyDirection);

        // Hier interpretieren Sie die Steuerbefehle und führen Aktionen aus
        // Führe eine Motor basierend auf dem Steuerbefehl aus
        printf("[E] Engine process received control command: %s\n", enumToString(flyDirection));

        // Simulierte zeit des Motors um befehle auszuführen
        usleep(MOVETIME_ENGINE * 1000);


        int xPos = sharedData->GPSPosition.XPos;
        int yPos = sharedData->GPSPosition.YPos;
        if (flyDirection == UP)
        {
            sharedData->Grid[yPos][xPos] = 0;
            yPos = yPos - 1;
            sharedData->Grid[yPos][xPos] = 2; // 2 als drown id
            writeToLog(ENGINE_LOG_FILE, "Engine: UP");
        }
        else if (flyDirection == DOWN)
        {
            sharedData->Grid[yPos][xPos] = 0;
            yPos = yPos + 1;
            sharedData->Grid[yPos][xPos] = 2;
            writeToLog(ENGINE_LOG_FILE, "Engine: DOWN");
        }
        else if (flyDirection == RIGHT)
        {
            sharedData->Grid[yPos][xPos] = 0;
            xPos = xPos + 1;
            sharedData->Grid[yPos][xPos] = 2;
            writeToLog(ENGINE_LOG_FILE, "Engine: RIGHT");
        }
        else if (flyDirection == LEFT)
        {
            sharedData->Grid[yPos][xPos] = 0;
            xPos = xPos - 1;
            sharedData->Grid[yPos][xPos] = 2;
            writeToLog(ENGINE_LOG_FILE, "Engine: LEFT");
        }
        sharedData->GPSPosition.XPos = xPos;
        sharedData->GPSPosition.YPos = yPos;

    }

}