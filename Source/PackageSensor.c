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

void send_package_message(int msg_queue_id, long msg_type, struct PackageData package) {
    struct PackageDataMessage msg;
    msg.MsgType = msg_type;
    msg.PackageInfo = package;
    msgsnd(msg_queue_id, &msg, sizeof(msg.PackageInfo), 0);
}


int main()
{
    // Schlüssel generieren (muss derselbe wie im anderen Programm sein)
    key_t key = ftok("/tmp", 's');
    if (key == -1) {
        perror("[P] ftok");
        exit(EXIT_FAILURE);
    }

    // Shared Memory ID abrufen
    int shmID = shmget(key, sizeof(struct SharedMemory), 0644);
    if (shmID == -1) {
        perror("[P] shmget");
        exit(EXIT_FAILURE);
    }

    // Shared Memory anhängen
    struct SharedMemory *sharedData = shmat(shmID, NULL, 0);
    if (sharedData == (void *)-1) {
        perror("[P] shmat");
        exit(EXIT_FAILURE);
    }

    // Erstellen oder Anschließen an die Nachrichtenwarteschlange
    int msg_queue_id = msgget(MSGKEY, 0666 | IPC_CREAT);
    if (msg_queue_id == -1)
    {
        perror("[P] Fehler beim Erstellen der Nachrichtenwarteschlange");
        exit(EXIT_FAILURE);
    }



    struct PackageData myPackageStatus = { .HasPackage = true, .IsDropping = false};
    sharedData->MyPackageData.HasPackage = myPackageStatus.HasPackage;
    sharedData->MyPackageData.IsDropping = myPackageStatus.IsDropping;


    // Hauptlogik des Sensorprozesses
    while (1) {
        // Hier Simulieren Sie Sensoraktivität und generieren Sie Nachrichten

        myPackageStatus.HasPackage = sharedData->MyPackageData.HasPackage;
        myPackageStatus.IsDropping = sharedData->MyPackageData.IsDropping;

        printf("[P] Packageprozess sendet Packagedata Sensordaten: %d %d\n", myPackageStatus.HasPackage, myPackageStatus.IsDropping);
        send_package_message(msg_queue_id, PACKAGEDATAMSGTYPE, myPackageStatus);

        sleep(2); // Simuliere einen Sensorleseintervall (in Sekunden)

    }


    // Aufräumarbeiten (normalerweise wird dies nicht erreicht)
    if (msgctl(msg_queue_id, IPC_RMID, NULL) == -1) {
        perror("[P] Fehler beim Löschen der Nachrichtenwarteschlange");
        exit(EXIT_FAILURE);
    }

    return 0;
}