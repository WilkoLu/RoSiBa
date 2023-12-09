#ifndef ROBOTICS_SYSTEM_H
#define ROBOTICS_SYSTEM_H

#define MSGSIZE 128

#define MAX_X 80
#define MAX_Y 24

// Struktur für die Nachrichtenwarteschlange
struct message {
    long msgType;
    char msgText[MSGSIZE];
};

struct SharedData {
    int obstacles[MAX_Y][MAX_X];
    int posX; // eventuell durch suchen in sensor.c loesen
    int posY;
};

#endif // ROBOTICS_SYSTEM_H
