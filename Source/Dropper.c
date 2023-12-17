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


// Funktion zum Empfangen von Nachrichten aus der Warteschlange
void receiveDropMessage(int msg_queue_id, long msg_type, bool* drop) {
    struct DropMessage msg;
    if (msgrcv(msg_queue_id, &msg, sizeof(msg.DropPackage), msg_type, 0) == -1) {
        perror("[D] Error receiving message");
        exit(EXIT_FAILURE);
    }
    *drop = msg.DropPackage;
}

int main()
{    
    // get Shared Memory
    struct SharedData *sharedData = getShm();

    // Anschließen an die Nachrichtenwarteschlange
    int msg_queue_id = getMessageQueue();

    // Hauptlogik des Droppers
    while (1) {
        // Empfangen Sie Steuerbefehle von der Nachrichtenwarteschlange
        bool drop = false;
        receiveDropMessage(msg_queue_id, DROPMSGTYPE, &drop);

        // Hier interpretieren Sie die Steuerbefehle und führen Aktionen aus
        // Führe eine Motor basierend auf dem Steuerbefehl aus
        printf("[D] Dropprocess received control command: %d\n", drop);


        if (drop == true)
        {
            sharedData->MyPackageData.IsDropping = true;
            writeToLog(DROPPER_LOG_FILE, "Started dropping package");
            usleep(DROPTIME_DROPPERSENSOR * 1000);
            sharedData->PackageDropPosition.XPos = sharedData->GPSPosition.XPos;
            sharedData->PackageDropPosition.YPos = sharedData->GPSPosition.YPos;
            writeToLog(DROPPER_LOG_FILE, "Finished dropping package");
            sharedData->MyPackageData.IsDropping = false;
            sharedData->MyPackageData.HasPackage = false;
        }
    }
}