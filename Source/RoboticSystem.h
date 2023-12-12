#ifndef ROBOTICSYSTEM_H
#define ROBOTICSYSTEM_H


#define MAX_X 80
#define MAX_Y 24

struct SharedMemory {
    int Grid[MAX_Y][MAX_X];
    PackageData MyPackageData;
    Position2D GPSPosition;
};

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
    Position2D GPSPosition;
};

struct SurroundingMessage { 
    long MsgType;
    DroneSurrounding Surrounding;
};

struct PackageDataMessage { 
    long MsgType;
    PackageData PackageInfo;
};

struct DirectionMessage { 
    long MsgType;
    Direction FlyDirection;
};

struct DropMessage { 
    long MsgType;
    bool DropPackage;
};

#endif // ROBOTICSYSTEM_H
