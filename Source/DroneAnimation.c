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
#define NAME_WIDTH 30
#define VALUE_WIDTH 2
#define EMOJI_WIDTH 2

// Prototypen für wide-character Funktionen
int addwstr(const wchar_t *wstr);
int addnwstr(const wchar_t *wstr, int n);
int waddwstr(WINDOW *win, const wchar_t *wstr);
int waddnwstr(WINDOW *win, const wchar_t *wstr, int n);
int mvaddwstr(int y, int x, const wchar_t *wstr);
int mvaddnwstr(int y, int x, const wchar_t *wstr, int n);
int mvwaddwstr(WINDOW *win, int y, int x, const wchar_t *wstr);
int mvwaddnwstr(WINDOW *win, int y, int x, const wchar_t *wstr, int n);

// Globale Variablen für die Windows
WINDOW *gridWin;
WINDOW *menuWin;
WINDOW *borderWin;

/// @brief Berechnet die Position im erweiterten Grid. Die x-Koordinate wird verdoppelt, da die Zeichen im Terminal nicht quadratisch sind. Auch um Emojis darstellen zu können.
int to_extended_grid_pos(int pos)
{
    return pos * 2;
}

// Sammlung von Hindernis-Zeichen
wchar_t *obstacles[] = {
    L"🧱",
    L"🌳",
    L"🌲",
    NULL};

/// @brief Zeichnet die Hindernisse mit zufälligen Hindernis-Zeichen
/// @param sharedData Pointer auf das Shared Memory
/// @param colorPair Farbkombination
void draw_obstacles(struct SharedData *sharedData, int colorPair) 
{
    srand(123);
    // Anzahl der Hindernisse ermitteln
    int num_obstacles = (sizeof(obstacles) / sizeof(obstacles[0])) - 1;
    // Zeichnen aller Hindernisse
    for (int i = 0; i < MAX_Y; i++) {
        for (int j = 0; j < MAX_X; j++) {
            if (sharedData->Grid[i][j] == 1) {
                int obs_index = rand() % num_obstacles;
                // Zeichne das Hindernis
                wattron(gridWin, COLOR_PAIR(colorPair));
                mvwaddwstr(gridWin, i, to_extended_grid_pos(j), obstacles[obs_index]);
                wattroff(gridWin, COLOR_PAIR(colorPair));
            }
        }
    }
}

/// @brief Zeichnet ein Element an der Position
/// @param pos2D Position des Elements
/// @param plotElement Zeichen des Elements
/// @param colorPair Farbkombination
void draw_element(struct Position2D pos2D, wchar_t* plotElement, int colorPair)
{
    wattron(gridWin, COLOR_PAIR(colorPair));
    mvwaddwstr(gridWin, pos2D.YPos, to_extended_grid_pos(pos2D.XPos), plotElement);
    wattroff(gridWin, COLOR_PAIR(colorPair));
}

void Init(){
    // Benötigt für die Verwendung von Unicode-Zeichen
    setlocale(LC_ALL, "");

    // Initialisiere ncurses
    initscr();
    curs_set(0);
    keypad(stdscr, TRUE);

    // Überprüfe, ob Farben unterstützt werden
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

    // Definiere Farbkombinationen
    init_pair(0, COLOR_WHITE, COLOR_BLACK); 
    init_pair(1, 236, 236); 
    init_pair(2, COLOR_WHITE, (8));


    // Erstelle Windows
    // 4 Zeilen, volle Breite, starte oben
    menuWin = newwin(4, COLS, 0, 0);
    // Zeilen entsprechen der Höhe des Grids, volle Breite, starte unterhalb des Menüs
    gridWin = newwin(MAX_Y, 2 * MAX_X + 1, 5, 1);

    borderWin = newwin(MAX_Y + 2, 2 * MAX_X + 3, 4, 0);
    box(borderWin, 0, 0);

    // Aktualisiere;
    wrefresh(menuWin);
    wrefresh(borderWin);
    wrefresh(gridWin);
}

/// @brief Zeichnet das Menü
/// @param sharedData 
void draw_menu(struct SharedData* sharedData) {
    mvwprintw(menuWin, 0, 0, "%-*s %*d, %*d", NAME_WIDTH, "🚁 Drone Position:", VALUE_WIDTH, sharedData->GPSPosition.XPos, VALUE_WIDTH, sharedData->GPSPosition.YPos);
    mvwprintw(menuWin, 1, 0, "%-*s %*d, %*d", NAME_WIDTH, "🎯 Target Position:", VALUE_WIDTH, sharedData->TargetPosition.XPos, VALUE_WIDTH, sharedData->TargetPosition.YPos);
    mvwprintw(menuWin, 2, 0, "%-*s %*s", NAME_WIDTH, "📦 Has Package:", VALUE_WIDTH, sharedData->MyPackageData.HasPackage ? " ✅" : "❌");
    mvwprintw(menuWin, 3, 0, "%-*s %*s", NAME_WIDTH, "⏬ Is Dropping:", VALUE_WIDTH, sharedData->MyPackageData.IsDropping ? " ✅" : "❌");
}

int main() {
    wchar_t DroneCharacter[] = L"🚁";
    wchar_t TargetCharacter[] = L"🎯";
    wchar_t PackageCharacter[] = L"📦";
    Init();
    
    // Get Shared Memory
    struct SharedData *sharedData = getShm();

    while (1) 
    {
        // Lösche den Inhalt der Windows
        wclear(menuWin);
        wclear(gridWin);

        // Zeichne die Elemente    
        draw_obstacles(sharedData, 1);
        draw_element(sharedData->TargetPosition, TargetCharacter, 0);
        draw_element(sharedData->PackageDropPosition, PackageCharacter, 0);
        draw_element(sharedData->GPSPosition, DroneCharacter, 0);
        draw_menu(sharedData);

        // Aktualisiere die Windows
        wrefresh(menuWin);
        wrefresh(gridWin);

        // Warte
        usleep(UI_REFRESH_RATE_MS * 1000); 
    }

    // Lösche windows und beende ncurses
    delwin(gridWin);
    delwin(menuWin);
    endwin();
    return 0;
}