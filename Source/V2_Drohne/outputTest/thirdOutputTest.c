#include <ncurses.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include "./../robotics_system.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/msg.h>

#define MAX_X 80
#define MAX_Y 24

int obstacles[MAX_Y][MAX_X] = {0}; // 2D-Array für Hindernisse

void draw_drone(int x, int y) {
    mvprintw(y, x, "  ^  ");
    mvprintw(y + 1, x, " /X\\ ");
    mvprintw(y + 2, x, "|---|");
    refresh();
}

void draw_obstacles(struct SharedData *sharedData) {
    for (int i = 0; i < MAX_Y; ++i) {
        for (int j = 0; j < MAX_X; ++j) {
            if (sharedData->obstacles[i][j] == 1) {
                mvprintw(i, j, "#");
            }
            else if (sharedData->obstacles[i][j] == 2)
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



    int x = 0;
    int y = 0;

    while (1) {
        clear(); // Lösche den Bildschirm
        draw_obstacles(sharedData);
        // draw_drone(x, y);

        x += 2;
        y += 1;

        if (x >= (COLS - 5) || y >= (LINES - 3)) {
            x = 0;
            y = 0;
        }

        // Setze einige Hindernisse im 2D-Array
        obstacles[5][20] = 1;
        obstacles[10][40] = 1;
        obstacles[15][60] = 1;
        obstacles[20][10] = 1;

        usleep(100000); // Pause für 0.1 Sekunden
    }

    endwin(); // Beende das curses-System
    return 0;
}
