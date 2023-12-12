#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <stdbool.h>
#include "RoboticSystem.h"


void send_message(int msg_queue_id, long msg_type, struct Position2D Pos2D) {
    struct GPSPosMessage msg;
    msg.MsgType = msg_type;
    msg.GPSPosition = Pos2D;
    msgsnd(msg_queue_id, &msg, sizeof(msg.GPSPosition), 0);
}


int main()
{
    // Schlüssel generieren (muss derselbe wie im anderen Programm sein)
    key_t key = ftok("/tmp", 's');
    if (key == -1) {
        perror("[G] ftok");
        exit(EXIT_FAILURE);
    }

    // Shared Memory ID abrufen
    int shmID = shmget(key, sizeof(struct SharedMemory), 0644);
    if (shmID == -1) {
        perror("[G] shmget");
        exit(EXIT_FAILURE);
    }

    // Shared Memory anhängen
    struct SharedMemory *sharedData = shmat(shmID, NULL, 0);
    if (sharedData == (void *)-1) {
        perror("[G] shmat");
        exit(EXIT_FAILURE);
    }

    // Erstellen oder Anschließen an die Nachrichtenwarteschlange
    int msg_queue_id = msgget(7681, 0666 | IPC_CREAT);
    if (msg_queue_id == -1)
    {
        perror("[G] Fehler beim Erstellen der Nachrichtenwarteschlange");
        exit(EXIT_FAILURE);
    }



    struct Position2D myPos = { .XPos = 0, .YPos = 0};
    // Hauptlogik des Sensorprozesses
    while (1) {
        // Hier Simulieren Sie Sensoraktivität und generieren Sie Nachrichten

        myPos.XPos = sharedData->GPSPosition.XPos;
        myPos.YPos = sharedData->GPSPosition.YPos;

        send_message(msg_queue_id, 1, myPos);

        sleep(2); // Simuliere einen Sensorleseintervall (in Sekunden)
    }


    // Aufräumarbeiten (normalerweise wird dies nicht erreicht)
    if (msgctl(msg_queue_id, IPC_RMID, NULL) == -1) {
        perror("[G] Fehler beim Löschen der Nachrichtenwarteschlange");
        exit(EXIT_FAILURE);
    }

    return 0;
}