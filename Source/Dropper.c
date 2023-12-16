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
    // Shared Memory ID abrufen
    int shmID = shmget(SHMKEY, sizeof(struct SharedMemory), 0644);
    if (shmID == -1) {
        perror("[M] shmget");
        exit(EXIT_FAILURE);
    }

    // Shared Memory anhängen
    struct SharedMemory *sharedData = shmat(shmID, NULL, 0);
    if (sharedData == (void *)-1) {
        perror("[D] shmat");
        exit(EXIT_FAILURE);
    }


    // Anschließen an die Nachrichtenwarteschlange
    int msg_queue_id = msgget(MSGKEY, 0666 | IPC_CREAT);
    if (msg_queue_id == -1) {
        perror("[D] Error connecting to message queue");
        exit(EXIT_FAILURE);
    }


    // Hauptlogik des Mootors
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
            sleep(10);
            writeToLog(DROPPER_LOG_FILE, "Finished dropping package");
            sharedData->MyPackageData.HasPackage = false;
            sharedData->MyPackageData.IsDropping = false;
        }
        
        //sleep(2); // Beispiel: Wartezeit zwischen Aktionen (in Sekunden)
    }

}