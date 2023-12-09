#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include "robotics_system.h"

#define WINDOW_SIZE 3  // Größe des Umfelds (ungerade für Symmetrie)

// Funktion zum Senden von Nachrichten an die Warteschlange
void send_message(int msg_queue_id, long msg_type, const char* msg_text) {
    struct message msg;
    msg.msgType = msg_type;
    strncpy(msg.msgText, msg_text, MSGSIZE - 1);
    msgsnd(msg_queue_id, &msg, sizeof(msg.msgText), 0);
}

// Funktion zum Senden des Umfelds an einen anderen Prozess über eine Message Queue
void sendSurroundingsToOtherProcess(struct SharedData *sharedData, int y, int x, int msg_queue_id) {
    // int halfWindowSize = WINDOW_SIZE / 2;

    // // Überprüfen, ob die gegebene Position im gültigen Bereich liegt
    // if (y >= halfWindowSize && y < MAX_Y - halfWindowSize && x >= halfWindowSize && x < MAX_X - halfWindowSize) {
    //     // Erstellen des Umfelds
    //     int surroundings[WINDOW_SIZE][WINDOW_SIZE];

    //     for (int i = -halfWindowSize; i <= halfWindowSize; ++i) {
    //         for (int j = -halfWindowSize; j <= halfWindowSize; ++j) {
    //             surroundings[i + halfWindowSize][j + halfWindowSize] = sharedData->obstacles[y + i][x + j];
    //         }
    //     }

    //     // Umwandeln des Umfelds in einen Text (könnte je nach Bedarf angepasst werden)
    //     char surroundingsText[MSGSIZE];
    //     sprintf(surroundingsText, "Surroundings: %d %d %d %d %d %d %d %d %d",
    //             surroundings[0][0], surroundings[0][1], surroundings[0][2],
    //             surroundings[1][0], surroundings[1][1], surroundings[1][2],
    //             surroundings[2][0], surroundings[2][1], surroundings[2][2]);

    //     // Senden der Nachricht an die Message Queue
    //     send_message(msg_queue_id, 1, surroundingsText);
    // } else {
    //     // Die Position liegt am Rand, und das Umfeld kann nicht vollständig erfasst werden
    //     printf("Invalid position for surroundings.\n");
    // }

    int halfWindowSize = WINDOW_SIZE / 2;

    int surroundings[WINDOW_SIZE][WINDOW_SIZE];

    for (int i = -halfWindowSize; i <= halfWindowSize; ++i)
    {
        for (int j = -halfWindowSize; j <= halfWindowSize; ++j)
        {
            //surroundings[i + halfWindowSize][j + halfWindowSize] = 1;
            //printf();
            if ((y + i) < 0 || (y + i) > MAX_Y || (x + j) < 0 || (x + j) > MAX_X)
            {
                surroundings[i + halfWindowSize][j + halfWindowSize] = 1;
            }
            else
            {
                surroundings[i + halfWindowSize][j + halfWindowSize] = sharedData->obstacles[y + i][x + j];
            }
        }
    }

    // Umwandeln des Umfelds in einen Text(könnte je nach Bedarf angepasst werden) 
    char surroundingsText[MSGSIZE];
    sprintf(surroundingsText, "Surroundings: %d %d %d %d %d %d %d %d %d",
            surroundings[0][0], surroundings[0][1], surroundings[0][2],
            surroundings[1][0], surroundings[1][1], surroundings[1][2],
            surroundings[2][0], surroundings[2][1], surroundings[2][2]);

    // Senden der Nachricht an die Message Queue
    send_message(msg_queue_id, 1, surroundingsText);
}

int findValueInSharedData(struct SharedData *sharedData, int *foundY, int *foundX, int value) {
    
    for (int i = 0; i < MAX_Y; ++i) {
        for (int j = 0; j < MAX_X; ++j) {
            if (sharedData->obstacles[i][j] == value) {
                *foundY = i;
                *foundX = j;
                return 1;  // Wert gefunden
            }
        }
    }
    return 0;  // Wert nicht gefunden
}

int main() {
    // Schlüssel generieren (muss derselbe wie im anderen Programm sein)
    key_t key = ftok("/tmp", 's');
    if (key == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    // Shared Memory ID abrufen
    int shmID = shmget(key, sizeof(struct SharedData), 0644);
    if (shmID == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    // Shared Memory anhängen
    struct SharedData *sharedData = shmat(shmID, NULL, 0);
    if (sharedData == (void *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    // Beispiel: Zugriff auf den Shared Memory
    printf("Value at obstacles[5][20]: %d\n", sharedData->obstacles[5][20]);

    // Erstellen oder Anschließen an die Nachrichtenwarteschlange
    int msg_queue_id = msgget(7681, 0666 | IPC_CREAT);
    if (msg_queue_id == -1) {
        perror("Fehler beim Erstellen der Nachrichtenwarteschlange");
        exit(EXIT_FAILURE);
    }

    // Hauptlogik des Sensorprozesses
    while (1) {
        // Hier Simulieren Sie Sensoraktivität und generieren Sie Nachrichten
        // Beispiel: Generiere Entfernungssensor-Daten
        int distance = rand() % 100; // Beispiel: Zufällige Entfernung im Bereich von 0 bis 99
        char sensor_data[MSGSIZE];
        snprintf(sensor_data, sizeof(sensor_data), "Distance: %d", distance);

        // Sende die generierten Daten an die Nachrichtenwarteschlange
        //send_message(msg_queue_id, 1, sensor_data);

        // Beispielaufruf von sendSurroundings für das Element in der Mitte
        int middleY = MAX_Y / 2;
        int middleX = MAX_X / 2;
        int foundY, foundX;
        int valueToFind = 2;
        // int hasFind = findValueInSharedData(sharedData, &foundY, &foundX, valueToFind);
        // //printf("Position: (%d, %d)\n", foundY, foundX);
        // //printf("(%d)\n", hasFind);
        // if (hasFind == 1) {
        //     // Gefundene Position verwenden
        //     printf("Gefundene Position: (%d, %d)\n", foundY, foundX);
        //     sendSurroundingsToOtherProcess(sharedData, foundY, foundX, msg_queue_id);
        //     //sendSurroundingsToOtherProcess(sharedData, middleY, middleX, msg_queue_id);
        // }
        if (sharedData->posX >= 0 && sharedData->posX <= MAX_X && sharedData->posY >= 0 && sharedData->posY <= MAX_Y)
        {
            printf("Pos.: %d %d \n",sharedData->posY, sharedData->posX);
            sendSurroundingsToOtherProcess(sharedData, sharedData->posX, sharedData->posY, msg_queue_id);
        }
        //sendSurroundingsToOtherProcess(sharedData, sharedData->posY, sharedData->posX, msg_queue_id);

        sleep(2); // Simuliere einen Sensorleseintervall (in Sekunden)
    }

    // Aufräumarbeiten (normalerweise wird dies nicht erreicht)
    if (msgctl(msg_queue_id, IPC_RMID, NULL) == -1) {
        perror("Fehler beim Löschen der Nachrichtenwarteschlange");
        exit(EXIT_FAILURE);
    }

    return 0;
}
