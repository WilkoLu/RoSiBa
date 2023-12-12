#include <ncurses.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include "RoboticSystem.h"

void draw_obstacles(struct SharedMemory *sharedData) 
{
    for (int i = 0; i < MAX_Y; ++i) {
        for (int j = 0; j < MAX_X; ++j) {
            if (sharedData->Grid[i][j] == 1) {
                mvprintw(i, j, "#");
            }
            else if (sharedData->Grid[i][j] == 2)
            {
                mvprintw(i, j, "^");
            }
        }
    }
    refresh();
}

int main() {

    initscr(); // Initialisiere das curses-System
    curs_set(0); // Setze den Cursor unsichtbar
    keypad(stdscr, TRUE); // Aktiviere Tastatureingabe

    key_t key = ftok("/tmp", 's');
    if (key == -1)
    {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    // Erstellen oder Zugriff auf den Shared Memory
    int shmID = shmget(key, sizeof(struct SharedMemory), IPC_CREAT | 0644);
    if (shmID == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    // Attach des Shared Memory
    struct SharedMemory *sharedData = shmat(shmID, NULL, 0);
    if (sharedData == (void *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }


    while (1) 
    {

        clear(); // Lösche den Bildschirm
        draw_obstacles(sharedData);
 
        usleep(100000); // Pause für 0.1 Sekunden
    }

    endwin(); // Beende das curses-System
    return 0;
}