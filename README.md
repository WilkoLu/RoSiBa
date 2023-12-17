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

### Controller

### Tower

Das Tower-Skript dient dem Zweck, alle Prozesse zu überwachen und sie bei Bedarf neu zu starten. Die Funktion start_process wird verwendet, um einen Prozess im Hintergrund zu starten. Der übergebene Prozessname ist das erste Argument ($1). Die Funktion startet den Prozess im Hintergrund (&) und gibt eine Nachricht aus, die den gestarteten Prozess und seine Prozess-ID (PID) anzeigt. Die cleanup-Funktion wird aufgerufen, um die Simulation sauber zu beenden. Sie gibt eine Meldung aus und beendet alle Kindprozesse, schließt alle Ausgabefenster (pkill -TERM -P $$), beendet den Prozess der Animation der Drohne "DroneAnimation.out" und löscht die Interprozesskommunikationsmittel, nämlich die Message Queues und den Shared Memory. Durch die Zeile
```
trap cleanup SIGINT
```
wird die Funktion cleanup aufgerufen, wenn das Skript ein SIGINT-Signal durch die Tastenkombination Ctrl+C empfängt. Im Anschluss erfolgt die Überwachung der Prozesse in einer Endlosschleife. Die überwachten Prozesse sind "Controller.out", "GPSSensor.out", "Surroundingsensor.out", "Engine.out", "Dropper.out", "PackageSensor.out" und "DroneAnimation.out". Falls einer dieser Prozesse nicht läuft, wird er mit der Funktion start_process neu gestartet. Die Überwachung erfolgt mithilfe von pgrep, das nach Prozess-IDs sucht. In der Überwachung der DroneAnimation wird über
```bash
cmd.exe /C start wsl bash -c "cd $current_dir && ./DroneAnimation.out;"
```
zusätzlich automatisch eine neue Windows-Eingabeaufforderung für die Animation geöffnet. Am Ende wird das Skript für 5 Sekunden pausiert, bevor es erneut die Prozesse überwacht. Dies dient dazu, die Systemressourcen zu schonen und nicht unnötig oft nach den Prozessen zu suchen. Das Skript stellt zusammenfassend sicher, dass die definierten Prozesse kontinuierlich laufen, und ermöglicht eine einfache Wartung und Bereinigung bei Bedarf.

### GPS-Sensor

In der GPSSensor.c wird die Hauptlogik eines fiktiven Sensorprozesses implementiert, der eine Positionsinformation über einen Shared Memory an eine Nachrichtenwarteschlange sendet, Logeinträge erstellt und die Daten speichert. Die Funktionen 'saveData' und 'loadData' dienen dem Speichern und Laden von Positionsdaten in eine binäre Datei mit dem Namen PERSISTENT_DATA_FILE. Die Funktion 'logMessageFromPosition' erstellt eine logische Nachricht aus einer Position (struct Position2D) und speichert sie im angegebenen Puffer (buffer). Die erstellte Nachricht enthält die X- und Y-Position der übergebenen Position. Die Funktion 'send_message' sendet eine Nachricht an die Nachrichtenwarteschlange (msg_queue_id). Die Nachricht enthält eine Position (Pos2D) vom Typ struct Position2D mit einem spezifizierten Nachrichtentyp (msg_type). In der main-Funktion wird schließlich die Shared Memory ID geholt. Dann wird der Shared Memory angehängt und eine Nachrichtenwarteschlange (Message Queue) wird erstellt oder sich mit dieser verbunden. Im Anschluss simuliert die main-Funktion eine Schleife für die Sensoraktivität, liest die GPS-Position aus dem Shared Memory, sendet diese Position an die Nachrichtenwarteschlange und erstellt Logeinträge. Durch Warten (sleep) für das angegebene Intervall (SAMPLERATE_GPSSENSOR) wird Sensoraktivität simuliert. Schließlich wird im Falle eines normalen Programmendes, das normalerweise nicht erreicht wird, die Nachrichtenwarteschlange gelöscht.

### Package-Sensor

Die PackageSensor.c implementiert einen einfachen Prozess (Packageprocess), der mit einem Shared Memory und einer Nachrichtenwarteschlange (Message Queue) arbeitet. Der Prozess simuliert Sensoraktivität, generiert Paketdaten und sendet diese über die Nachrichtenwarteschlange. Die Funktion 'send_package_message' sendet Paketdaten über eine Nachrichtenwarteschlange und protokolliert gleichzeitig den Vorgang. Die gesendeten Paketdaten sind zwei Booleans, die enthalten, ob die Drohne gerade ein Paket besitzt (HasPackage) und ob diese das Paket gerade absetzt (IsDropping). Die main-Funktion beginnt damit, die Shared Memory-ID zu erhalten und sich dann an den Shared Memory anzuhängen. Danach wird eine Nachrichtenwarteschlange erstellt oder mit einer vorhandenen verbunden. Die Paketdaten im Shared Memory werden initialisiert, und anschließend wird die Hauptlogik des Sensorprozesses in einer Endlosschleife implementiert. Innerhalb dieser Schleife wird die simulierte Sensoraktivität durchgeführt, die Paketdaten aktualisiert und über die send_package_message-Funktion an die Nachrichtenwarteschlange gesendet. Es wird auch eine Pause entsprechend der SAMPLERATE_PACKAGESENSOR eingefügt, um einen Simulationszyklus für den Sensor zu emulieren. Schließlich werden Aufräumarbeiten durchgeführt, wobei die Nachrichtenwarteschlange gelöscht wird. Es ist zu beachten, dass der Code normalerweise die Endlosschleife nicht verlassen sollte. Falls jedoch der Code die Endlosschleife verlässt, werden die Aufräumarbeiten durchgeführt. 

### Surrounding-Sensor

Die Surroundingsensor.c implementiert die Hauptlogik eines Sensorprozesses, der Umgebungsinformationen eines Roboters liest und diese Informationen über eine Nachrichtenwarteschlange an andere Prozesse sendet. Die Funktion 'sendSurroundingmessage' sendet eine Nachricht über eine Nachrichtenwarteschlange. Sie erstellt eine Instanz der Struktur SurroundingMessage, füllt sie mit den übergebenen Werten und sendet sie mit msgsnd an die Warteschlange. Zusätzlich wird ein Log-Eintrag erstellt, der die gesendeten Umgebungsinformationen protokolliert. Die Funktion 'isWithinGrid' überprüft, ob die angegebenen Koordinaten innerhalb eines festgelegten Rasters liegen. Die Funktion 'CalculateSurroundings' ist dazu da, um die Umgebungsinformationen (Top, TopRight, Right, BottomRight, Bottom, BottomLeft, Left, TopLeft) basierend auf den aktuellen GPS-Positionskoordinaten, die im Shared Memory (sharedData) gespeichert sind, zu berechnen. Die main-Funktion beginnt mit dem Abruf der Shared-Memory-ID und der Prozess verbindet sich mit dem Shared Memory. Ebenso wird eine Nachrichtenwarteschlange erstellt oder verbunden. Ein initialer Umgebungszustand mySurrounding wird erstellt und die Hauptschleife, in welcher der Sensor simuliert wird, beginnt Umgebungsinformationen zu berechnen und Nachrichten an die Warteschlange zu senden. Die Schleife wird durch usleep für eine bestimmte Zeit (Sensorleseintervall) unterbrochen. Nachdem die Hauptschleife beendet wurde, was normalerweise nicht passiert, werden Aufräumarbeiten durchgeführt, darunter das Löschen der Nachrichtenwarteschlange.

### Engine

### Dropper

## Nutzung der Interprozesskommunikation


## Logging und Datensicherung


## Ausführung

Alle C-Files werden durch das Ausführen des Bash-Skriptes compileAll.sh mit
```
bash  compileAll.sh
```
kompiliert. Das Starten des Programmes erfolgt im Anschluss durch das Ausführen des Tower.sh Skriptes.
```
bash Tower.sh
```


WSL nötig

Windows 11 nötig für Emojis?
