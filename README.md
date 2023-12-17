# RoSiBa
Entwicklung eines einfachen Robotik-Simulationssystems mit Bash

Autoren: 
- Eike Rodenbäck
- Wilko Lühring
- Silas Köhnke
- Timon Faß

https://github.com/WilkoLu/RoSiBa/assets/56156680/6e03eef0-fa98-49b5-bfa5-a884b5eab088


## Zusammenarbeit
Dieses Projekt wurde mithilfe der [Live Share Extension](https://learn.microsoft.com/en-us/visualstudio/liveshare/) in Visual Studio Code entwickelt. Die Live Share Extension ermöglicht es Entwicklern, in Echtzeit zusammenzuarbeiten, Code zu teilen und gemeinsam an Projekten zu arbeiten. Die Commits für dieses Projekt wurden aktiv gemeinsam erstellt.


## Systementwurf

![Systementwurf drawio](https://github.com/WilkoLu/RoSiBa/assets/56156680/69f526e9-f327-4abb-9692-156f53a1b51a)


## Prozessimplementierung

Für jeden der Sensoren im Systementwurf ist eine C-File vorhanden. Somit sind ein GPS-Sensor für die Position, ein Package-Sensor zum Überprüfen des Status des Tragens eines Paketes und ein Surrounding-Sensor für das Überprüfen der Umgebung der Drohne vorhanden. Darüber hinaus gibt es einen Controller, einen Tower und das allgemeine Robotic System. Die Aktuatoren sind durch die Engine und den Dropper gegeben.

### GPS-Sensor

In der GPSSensor.c wird die Hauptlogik eines fiktiven Sensorprozesses implementiert, der eine Positionsinformation über einen Shared Memory an eine Nachrichtenwarteschlange sendet, Logeinträge erstellt und die Daten speichert. Die Funktionen 'saveData' und 'loadData' dienen dem Speichern und Laden von Positionsdaten in eine binäre Datei mit dem Namen PERSISTENT_DATA_FILE. Die Funktion 'logMessageFromPosition' erstellt eine logische Nachricht aus einer Position (struct Position2D) und speichert sie im angegebenen Puffer (buffer). Die erstellte Nachricht enthält die X- und Y-Position der übergebenen Position. Die Funktion 'send_message' sendet eine Nachricht an die Nachrichtenwarteschlange (msg_queue_id). Die Nachricht enthält eine Position (Pos2D) vom Typ struct Position2D mit einem spezifizierten Nachrichtentyp (msg_type). In der main-Funktion wird schließlich ein Schlüssel für den Shared Memory generiert und die Shared Memory ID geholt. Dann wird der Shared Memory angehängt und eine Nachrichtenwarteschlange (message queue) wird erstellt oder sich mit dieser verbunden. Im Anschluss simuliert die main-Funktion eine Schleife für die Sensoraktivität, liest die GPS-Position aus dem Shared Memory, sendet diese Position an die Nachrichtenwarteschlange und erstellt Logeinträge. Durch Warten (sleep) für das angegebene Intervall (SAMPLERATE_GPSSENSOR) wird Sensoraktivität simuliert. Schließlich wird im Falle eines normalen Programmendes, das normalerweise nicht erreicht wird, die Nachrichtenwarteschlange gelöscht.

### Package-Sensor
### Surrounding-Sensor

## Nutzung der Interprozesskommunikation


## Logging und Datensicherung


## Ausführung

WSL nötig, erst bash compileAll.sh, dann bash Tower.sh

Windows 11 nötig für Emojis?
