#ifndef ROBOTICSYSTEM_H
#define ROBOTICSYSTEM_H

// #define MAX_X 80
// #define MAX_Y 24
#define MAX_X 40
#define MAX_Y 20

// Define Message Queue Types
#define GPSPOSMSGTYPE 1
#define SURROUNDINGMSGTYPE 2
#define PACKAGEDATAMSGTYPE 3
#define FLYDIRECTIONMSGTYPE 8
#define DROPMSGTYPE 9

// Define Sleeptime - Sample rate Simulation in Millisekunden
#define SAMPLERATE_GPSSENSOR 100
#define SAMPLERATE_PACKAGESENSOR 100
#define SAMPLERATE_SURROUNDINGSENSOR 100
#define SAMPLERATE_CONTROLLER 50

#define MOVETIME_ENGINE 500
#define DROPTIME_DROPPERSENSOR 2700

#define UI_REFRESH_RATE_MS 100

// Define key: Numerischer Schlüssel, identifiziert systemweit die Warteschlange
#define MSGKEY 7681

#define SHMKEY 7681

// Größe des Umfelds (ungerade für Symmetrie)
#define WINDOW_SIZE 3  

// Dateipfad für die Datenspeicherung
#define PERSISTENT_DATA_FILE "data_and_logs/gps_sensor_data.txt"

// Dateipfad für das Log
#define CONTROLLER_LOG_FILE "data_and_logs/controller_log.txt"
#define GPS_LOG_FILE "data_and_logs/gps_sensor_log.txt"
#define DROPPER_LOG_FILE "data_and_logs/dropper_sensor_log.txt"
#define ENGINE_LOG_FILE "data_and_logs/engine_sensor_log.txt"
#define PACKAGE_LOG_FILE "data_and_logs/package_sensor_log.txt"
#define SURROUNDING_LOG_FILE "data_and_logs/surrounding_sensor_log.txt"

struct Position2D
{
    int XPos;
    int YPos;
};

struct PackageData 
{
    bool HasPackage;
    bool IsDropping;
};

struct DroneSurrounding
{
    int Top;
    int TopRight;
    int Right;
    int BottomRight;
    int Bottom;
    int BottomLeft;
    int Left;
    int TopLeft;
};

enum Direction
{
    UP,
    DOWN,
    RIGHT,
    LEFT
};

// Strukturen für die Nachrichtenwarteschlange
struct GPSPosMessage { 
    long MsgType;
    struct Position2D GPSPosition;
};

struct SurroundingMessage { 
    long MsgType;
    struct DroneSurrounding Surrounding;
};

struct PackageDataMessage { 
    long MsgType;
    struct PackageData PackageInfo;
};

struct DirectionMessage { 
    long MsgType;
    enum Direction FlyDirection;
};

struct DropMessage { 
    long MsgType;
    bool DropPackage;
};

struct SharedData {
    int Grid[MAX_Y][MAX_X];
    struct PackageData MyPackageData;
    struct Position2D GPSPosition;
    struct Position2D TargetPosition;
    struct Position2D PackageDropPosition;
};

// Funktion Prototypen
char* directionToString(enum Direction direction);
char* boolToString(bool boolean);
struct SharedData* createShm();
struct SharedData* getShm();
int createMessageQueue();
int getMessageQueue();

#endif // ROBOTICSYSTEM_H