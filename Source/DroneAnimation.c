// #include <ncurses.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <ncursesw/curses.h>
#include <locale.h>
#include "RoboticSystem.h"

// prototypes for wide-character functions
int addwstr(const wchar_t *wstr);
int addnwstr(const wchar_t *wstr, int n);
int waddwstr(WINDOW *win, const wchar_t *wstr);
int waddnwstr(WINDOW *win, const wchar_t *wstr, int n);
int mvaddwstr(int y, int x, const wchar_t *wstr);
int mvaddnwstr(int y, int x, const wchar_t *wstr, int n);
int mvwaddwstr(WINDOW *win, int y, int x, const wchar_t *wstr);
int mvwaddnwstr(WINDOW *win, int y, int x, const wchar_t *wstr, int n);

WINDOW *gridWin;
WINDOW *menuWin;
WINDOW *animationWin;

wchar_t *obstacles[] = {
    L"🧱",
    L"🌳",
    L"🌲",
    NULL};

void draw_obstacles_random(struct SharedMemory *sharedData) 
{
    srand(123); // Seed the random number generator

    int num_obstacles = (sizeof(obstacles) / sizeof(obstacles[0])) - 1;
    for (int i = 0; i < MAX_Y; ++i) {
        for (int j = 0; j < MAX_X; ++j) {
            if (sharedData->Grid[i][j] == 1) {
                // Randomly choose an obstacle
                // int obs_index = 10+i+j % num_obstacles;
                int obs_index = rand() % num_obstacles;
                mvwaddwstr(gridWin, i, j, obstacles[obs_index]);
            }
        }
    }
}

void draw_element(struct Position2D pos2D, wchar_t* plotElement, int colorPair)
{
    attron(COLOR_PAIR(colorPair));
    mvwaddwstr(gridWin, pos2D.YPos, pos2D.XPos, plotElement);
    attroff(COLOR_PAIR(colorPair));
}


void Init(){
    // Necessary for unicode characters
    setlocale(LC_ALL, "");

    // Initialize ncurses
    initscr();
    curs_set(0);
    keypad(stdscr, TRUE);

    // Check for and enable coloring
    if (!has_colors())
    {
        endwin();
        printf("Your terminal does not support color\n");
        exit(1);
    }
    if (start_color() != OK)
    {
        endwin();
        exit(1);
    }

    // Define color combinations
    init_pair(0, COLOR_BLACK, COLOR_WHITE); 
    init_pair(1, COLOR_RED, COLOR_BLACK); 
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_WHITE, COLOR_BLUE); 

    // create windows
    menuWin = newwin(4, COLS, 0, 0); // 3 rows, full columns, start at top
    gridWin = newwin(MAX_Y, COLS, 4, 0); // 3 rows, full columns, start at top
    wbkgd(gridWin, COLOR_PAIR(0)); 

    //refresh windows;
    wrefresh(menuWin);
    wrefresh(gridWin);
}

void draw_menu(struct SharedMemory* sharedData){
    mvwprintw(menuWin, 0,0,"Drone Position: %d, %d", sharedData->GPSPosition.XPos, sharedData->GPSPosition.YPos);
    mvwprintw(menuWin, 1,0,"Target Position: %d, %d", sharedData->TargetPosition.XPos, sharedData->TargetPosition.YPos);
    mvwprintw(menuWin, 2,0,"Has Package: %s", sharedData->MyPackageData.HasPackage ? "True" : "False");
    mvwprintw(menuWin, 3,0,"Is Dropping: %s", sharedData->MyPackageData.IsDropping ? "True" : "False");
}

int main() {
    wchar_t DroneCharacter[] = L"🚁";
    wchar_t TargetCharacter[] = L"🎯";
    wchar_t Dropping[] = L"📦❌✅🛑⏬";

    Init();

    key_t key = ftok("/tmp", 's');
    if (key == -1)
    {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    // Erstellen oder Zugriff auf den Shared Memory
    int shmID = shmget(SHMKEY, sizeof(struct SharedMemory), IPC_CREAT | 0644);
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
        // clear windows
        wclear(menuWin);
        wclear(gridWin);

        // draw UI
        draw_obstacles_random(sharedData);
        draw_element(sharedData->GPSPosition, DroneCharacter, 1);
        draw_element(sharedData->TargetPosition, TargetCharacter, 3);
        draw_menu(sharedData);

        // refresh UI
        wrefresh(menuWin);
        wrefresh(gridWin);

        // sleep for 100ms
        usleep(100000);
    }

    // Delete windows and end ncurses
    delwin(gridWin);
    delwin(menuWin);
    endwin();
    return 0;
}