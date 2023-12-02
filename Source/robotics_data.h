// robotics_data.h
#ifndef ROBOTICS_DATA_H
#define ROBOTICS_DATA_H

struct SensorData {
    double distance;
    double temperature;
    // Weitere Sensordaten nach Bedarf hinzufügen
};

struct ControlCommand {
    // Beispiel für Steuerbefehle (können je nach Bedarf angepasst werden)
    int motorControl;
    // Weitere Steuerbefehle nach Bedarf hinzufügen
};

#endif // ROBOTICS_DATA_H