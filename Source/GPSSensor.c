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


// Funktion zum Speichern der Daten
void saveData(struct Position2D *data) {
    FILE *file = fopen(PERSISTENT_DATA_FILE, "wb");
    if (file == NULL) {
        perror("[G] Error while opening the save file.");
        exit(EXIT_FAILURE);
    }

    fwrite(data, sizeof(struct Position2D), 1, file);

    fclose(file);
}

// Funktion zum Laden der Daten
void loadData(struct Position2D *data) {
    FILE *file = fopen(PERSISTENT_DATA_FILE, "rb");
    if (file == NULL) {
        perror("[G] Error opening the load file.");
        return;
    }

    fread(data, sizeof(struct Position2D), 1, file);

    fclose(file);
}

// Funktion zum Schreiben von Log-Einträgen (kann vielleicht mehrfach verwendet werden?)
void writeToLog(const char *message) {
    FILE *logFile = fopen(LOG_FILE, "a"); // 'a' für Anhängen an die Datei
    if (logFile == NULL) {
        perror("[G] Fehler beim Öffnen der Log-Datei");
        return;
    }

    // Zeitstempel hinzufügen
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    fprintf(logFile, "[%04d-%02d-%02d %02d:%02d:%02d] %s\n",
            timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
            timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, message);

    fclose(logFile);
}

// Funktion zum Erstellen einer logischen Nachricht aus einer Position2D
void logMessageFromPosition(const struct Position2D *position, char *buffer, size_t bufferSize) {
    snprintf(buffer, bufferSize, "Position: X=%d, Y=%d", position->XPos, position->YPos);
}

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
    int shmID = shmget(SHMKEY, sizeof(struct SharedMemory), 0644);
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
    int msg_queue_id = msgget(MSGKEY, 0666 | IPC_CREAT);
    if (msg_queue_id == -1)
    {
        perror("[G] Error while creating the message queue.");
        exit(EXIT_FAILURE);
    }



    struct Position2D myPos = { .XPos = 0, .YPos = 0};

    // Lade vorhandene Daten, wenn verfügbar
    loadData(&myPos);

    // Hauptlogik des Sensorprozesses
    while (1) {
        // Hier Simulieren Sie Sensoraktivität und generieren Sie Nachrichten

        myPos.XPos = sharedData->GPSPosition.XPos;
        myPos.YPos = sharedData->GPSPosition.YPos;

        send_message(msg_queue_id, GPSPOSMSGTYPE, myPos);

        // Speichern Sie die aktuelle Position in persistenten Daten
        saveData(&myPos);
        
        // Erstellen Sie eine logische Nachricht aus der Position
        char logMessage[100]; // Passen Sie die Puffergröße nach Bedarf an
        logMessageFromPosition(&myPos, logMessage, sizeof(logMessage));

        // Schreiben Sie einen Log-Eintrag
        writeToLog(logMessage);

        sleep(2); // Simuliere einen Sensorleseintervall (in Sekunden)
    }


    // Aufräumarbeiten (normalerweise wird dies nicht erreicht)
    if (msgctl(msg_queue_id, IPC_RMID, NULL) == -1) {
        perror("[G] Error while deleting the message queue.");
        exit(EXIT_FAILURE);
    }

    return 0;
}