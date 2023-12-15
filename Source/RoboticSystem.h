#ifndef ROBOTICSYSTEM_H
#define ROBOTICSYSTEM_H


#define MAX_X 80
#define MAX_Y 24

// 
#define DROPMSGTYPE 9

// Größe des Umfelds (ungerade für Symmetrie)
#define WINDOW_SIZE 3  

// Dateipfad für die Datenspeicherung
#define PERSISTENT_DATA_FILE "data_and_logs/gps_sensor_data.txt"

// Dateipfad für das Log
#define LOG_FILE "data_and_logs/gps_sensor_log.txt"

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

struct SharedMemory {
    int Grid[MAX_Y][MAX_X];
    struct PackageData MyPackageData;
    struct Position2D GPSPosition;
};

#endif // ROBOTICSYSTEM_H
