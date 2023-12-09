#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include "robotics_system.h"

// Funktionen zum Senden und Empfangen von Nachrichten
void send_message(int msg_queue_id, long msg_type, const char* msg_text) {
    struct message msg;
    msg.msgType = msg_type;
    strncpy(msg.msgText, msg_text, MSGSIZE - 1);
    msgsnd(msg_queue_id, &msg, sizeof(msg.msgText), 0);
}

void receive_message(int msg_queue_id, long msg_type, char* msg_text) {
    struct message msg;
    if (msgrcv(msg_queue_id, &msg, sizeof(msg.msgText), msg_type, 0) == -1) {
        perror("Fehler beim Empfangen der Nachricht");
        exit(EXIT_FAILURE);
    }
    strncpy(msg_text, msg.msgText, sizeof(msg.msgText));
}

int main() {
    int targetPointX = 11;
    int targetPointY = 70;


    key_t key = ftok("/tmp", 's');
    if (key == -1)
    {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    // Erstellen oder Zugriff auf den Shared Memory
    int shmID = shmget(key, sizeof(struct SharedData), IPC_CREAT | 0644);
    if (shmID == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    // Attach des Shared Memory
    struct SharedData *sharedData = shmat(shmID, NULL, 0);
    if (sharedData == (void *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    // Initialisierung des Shared Memory mit den Hindernisdaten
    for (int i = 0; i < MAX_Y; i++) {
        for (int j = 0; j < MAX_X; j++) {
            sharedData->obstacles[i][j] = 0;
        }
    }

    sharedData->obstacles[5][20] = 1;
    sharedData->obstacles[10][40] = 1;
    sharedData->obstacles[15][60] = 1;
    sharedData->obstacles[20][10] = 1;
    sharedData->obstacles[2][2] = 1;
    sharedData->obstacles[13][41] = 1;

    sharedData->posX = 0;
    sharedData->posX = 0;

    // Anschließen an die Nachrichtenwarteschlange
    int msg_queue_id = msgget(7681, 0666 | IPC_CREAT);
    if (msg_queue_id == -1) {
        perror("Fehler beim Anschließen an die Nachrichtenwarteschlange");
        exit(EXIT_FAILURE);
    }

    // Hauptlogik des Controllerprozesses
    while (1) {
        // Hier bewerten Sie Sensordaten und planen Pfade
        // Beispiel: Empfange Sensordaten von der Nachrichtenwarteschlange
        char sensor_data[MSGSIZE];
        receive_message(msg_queue_id, 1, sensor_data);

        // Hier könnten Sie die Pfadplanungslogik implementieren
        // Beispiel: Einfache Ausgabe von Sensordaten
        printf("Controllerprozess empfing Sensordaten: %s\n", sensor_data);

        // Hier generieren Sie Steuerbefehle basierend auf den bewerteten Daten
        // Beispiel: Einfache Steuerbefehlsgenerierung
        char control_command[MSGSIZE];
        if (sharedData->posX < targetPointX)//position muss von sensoren kommen
        {
            snprintf(control_command, sizeof(control_command), "down");//Move to waypoint
        }
        else if (sharedData->posX > targetPointX)
        {
            snprintf(control_command, sizeof(control_command), "up");//Move to waypoint
        }
        else if (sharedData->posY < targetPointY)
        {
            snprintf(control_command, sizeof(control_command), "right");//Move to waypoint
        }
        else if (sharedData->posY > targetPointY)
        {
            snprintf(control_command, sizeof(control_command), "left");//Move to waypoint
        }
        
        
        //snprintf(control_command, sizeof(control_command), "down");//Move to waypoint

        // Senden Sie die generierten Steuerbefehle an die Nachrichtenwarteschlange
        send_message(msg_queue_id, 2, control_command);

        sleep(2); // Beispiel: Intervall zwischen Pfadbewertungen und Steuerbefehlsgenerierung (in Sekunden)
    }

    // Aufräumarbeiten (normalerweise wird dies nicht erreicht)
    if (msgctl(msg_queue_id, IPC_RMID, NULL) == -1) {
        perror("Fehler beim Löschen der Nachrichtenwarteschlange");
        exit(EXIT_FAILURE);
    }

    return 0;
}
