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

void send_package_message(int msg_queue_id, long msg_type, struct PackageData package) {
    struct PackageDataMessage msg;
    msg.MsgType = msg_type;
    msg.PackageInfo = package;
    msgsnd(msg_queue_id, &msg, sizeof(msg.PackageInfo), 0);

    // Logging
    char logMessage[] = "[P] Packageprocess sends Packagedata Sensordata HasPackage; IsDropping: ";
    strcat(logMessage, package.HasPackage ? "True ;" : "False ;");    
    strcat(logMessage, package.IsDropping ? " True" : " False");
    writeToLog(PACKAGE_LOG_FILE, logMessage);
}


int main()
{    
    // get Shared Memory 
    struct SharedData *sharedData = getShm();

    // Erstellen oder Anschließen an die Nachrichtenwarteschlange
    int msg_queue_id = getMessageQueue();

    struct PackageData myPackageStatus = { .HasPackage = true, .IsDropping = false};
    sharedData->MyPackageData.HasPackage = myPackageStatus.HasPackage;
    sharedData->MyPackageData.IsDropping = myPackageStatus.IsDropping;


    // Hauptlogik des Sensorprozesses
    while (1) {
        // Hier Simulieren Sie Sensoraktivität und generieren Sie Nachrichten

        myPackageStatus.HasPackage = sharedData->MyPackageData.HasPackage;
        myPackageStatus.IsDropping = sharedData->MyPackageData.IsDropping;

        // printf("[P] Packageprocess sends Packagedata Sensordata: %d %d\n", myPackageStatus.HasPackage, myPackageStatus.IsDropping);
        send_package_message(msg_queue_id, PACKAGEDATAMSGTYPE, myPackageStatus);

        usleep(SAMPLERATE_PACKAGESENSOR * 1000); // Simuliere einen Sensorleseintervall (in Sekunden)

    }

    return 0;
}