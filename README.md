# RoSiBa - Entwicklung eines einfachen Robotik-Simulationssystems mit Bash und C

Autoren - Gruppe C: 
- [@Eike Rodenbaeck](https://github.com/EikeJoo)
- [@Wilko Luehring](https://github.com/WilkoLu)
- [@Silas Köhnke](https://github.com/Silas-K)
- [@Timon Faß](https://github.com/TimonFass)

## Ausführung der Simulation

Zum Ausführen des Codes wird die Bibliothek `ncurses` benötigt. Diese wird mit folgendem Befehl installiert:
```sh
sudo apt-get install libncurses5-dev libncursesw5-dev
```
Die folgenden Befehle sind im directory `Source` durchzuführen.
Alle C-Files werden durch das Ausführen des Bash-Skriptes compileAll.sh mit
```sh
bash  compileAll.sh
```
kompiliert. Das Starten des Programmes erfolgt im Anschluss durch das Ausführen des Tower.sh Skriptes.
```sh
bash Tower.sh
```
Um die Simulation zu verwenden, muss Windows Subsystem for Linux (WSL) verwendet werden, da im Verlauf automatisch ein Windows Terminal geöffnet wird. In der Simulation erhält die Drohne ein Ziel und Hindernisse. Sie wird zum Ziel fliegen, die Hindernisse umgehen, ihr Paket droppen und danach zurück zum Startpunkt fliegen. Die Logdateien sind im Verzeichnis `data_and_logs` zu finden. Eine Beispielausführung der Simulation ist im unten aufgeführten Video dargestellt.


https://github.com/WilkoLu/RoSiBa/assets/56156680/80c29adb-f74d-4a3d-9a76-72967226575d

  
## Einleitung

Dieses Projekt beschäftigt sich mit der Aufgabe, ein einfaches Robotik-Simulationssystem auf einem Linux-basierten Betriebssystem zu entwickeln. Das System soll darüber hinaus Prozesse zur Simulation von Sensordaten und Aktuatorsteuerung beinhalten. Die Kommunikation zwischen diesen Prozessen soll über Interprozesskommunikationsmechanismen erfolgen. Zusätzlich soll ein Skript zur Automatisierung von Start, Überwachung und Shutdown des Simulationssystems erstellt werden. Basierend auf dieser Aufgabe hat sich die Idee ergeben, eine fliegende Drohne zu simulieren. Diese soll eine Strecke zu einen Ziel zurückliegen und dort ein Paket abliefern. Auf dem Weg begegnet sie Hindernissen, denen sie ausweichen muss. Dafür wird die Drohne mit drei Sensoren ausgestattet, welche die Position, das Paket und die Umgebung überwachen. Entsprechend der Messwerte wird die Drohne über einen Controller und die Aktuatoren eines Motors und eines Droppers (zum Abliefern des Paketes) gesteuert. Genaueres dazu ist im Kapitel *Systementwurf* nachzulesen. Außerdem wird während der Umsetzung der Drohnen-Simulation Logging integriert.

## Zusammenarbeit

Dieses Projekt wurde mithilfe der [Live Share](https://learn.microsoft.com/en-us/visualstudio/liveshare/) [Extension in Visual Studio Code](https://marketplace.visualstudio.com/items?itemName=MS-vsliveshare.vsliveshare) entwickelt. Die Live Share Extension ermöglicht es Entwicklern, in Echtzeit zusammenzuarbeiten, Code zu teilen und gemeinsam an Projekten zu arbeiten. Die Commits für dieses Projekt wurden aktiv gemeinsam erstellt.

## Systementwurf

Das hier umgesetzte Robotik-Simulationssystem mit Bash verfolgt die Idee einer Drohne, die Pakete an Zielpunkte liefert und danach wieder zu ihrem Ausgangspunkt zurückfliegt. Der Systementwurf ist in der folgenden Abbildung dargestellt: 

![Systementwurf drawio](https://github.com/WilkoLu/RoSiBa/assets/56156680/7ce67eaf-9ee6-4d7f-8ec4-3a5f8cb7b67e)

Hierin ist zu erkennen, dass es einen Tower gibt, welcher die Aufgabe verfolgt, alle Prozesse zu starten und zu überwachen. Im System selbst ist der Controller die wichtigste Instanz. Er ist der Vermittler zwischen Sensoren und Aktoren und verwaltet die Zielposition und erstellt den Shared Memory, sowie die Message Queues. Die Drohne hat drei Sensoren, die ihre Daten an den Controller senden. Der GPS-Sensor misst die Position der Drohne, der Surroundingsensor überwacht die Umgebung, um die Drohne herum und der Packagesensor misst, ob die Drohne gerade ein Paket trägt oder nicht. Entsprechend der Messwerte der Sensoren sendet der Controller Befehle an die Aktuatoren. Hier gibt es den Motor (Engine), welcher die dafür sorgt, dass die Drohne sich in die richtige Richtung bewegt und den Dropper, welcher das Paket fallen lässt, falls er das Kommando dafür erhält. Die Kommunikation zwischen Sensoren, Controller und Aktuatoren erfolgt über Nachrichtenwarteschlangen (Message Queues). Darüber hinaus gibt es einen Shared Memory zur Verwaltung der Daten, der gleichzeitig die Realität abbilden soll. Der Dropper schreibt die `drop data`, also ob er gedroppt in diesen, während die Engine die aktuelle GPS-Position hineinschreibt. Aus dem Shared Memory lesen die jeweiligen Sensoren die Informationen über die Pakete, die Umgebung und die GPS-Position.   

Im unteren Teil der Abbildung wird der Shared Memory genauer betrachtet. Im Shared Memory befindet sich zum einen das Grid, also das Feld, in welchem sich die Drohne bewegt, zum anderen auch die GPS-Position, die Daten der Pakete und die Zielposition. Die Positionen (`Position2D`) bestehen aus der x-Position und der y-Position. Die Package Data enthält zwei Booleans, ob die Drohne gerade ein Paket hat und ob sie dieses gerade fallen lässt. Der enum `Direction` besteht aus den vier Bewegungsrichtungen der Drohne (oben, unten, links und rechts). Die Umgebung der Drohne `DroneSurrounding` enthält alle Positionen um die Drohne herum (oben, oben rechts, rechts, unten rechts, unten, unten links, links und oben links). Wie dieser Entwurf in Software umgesetzt wurde, wird im Kapitel *Prozessimplementierung* genauer beschrieben. 

## Prozessimplementierung

Für jeden der Sensoren im Systementwurf ist eine C-File vorhanden. Somit sind ein GPS-Sensor für die Position, ein Package-Sensor zum Überprüfen des Status des Tragens eines Paketes und ein Surrounding-Sensor für das Überprüfen der Umgebung der Drohne vorhanden. Darüber hinaus gibt es einen Controller, einen Tower und das allgemeine Robotic System. Die Aktuatoren sind durch die Engine und den Dropper gegeben.

### Robotic System

Das Robotic System in `RoboticSystem.c` implementiert Funktionen für die Verwaltung von Message Queues und Shared Memory in einem Robotersteuerungssystem. Die Funktion `directionToString` wandelt die Richtung (enum Direction) in einen String um. Es wird ein switch-Statement verwendet, um die verschiedenen Richtungen (UP, DOWN, RIGHT, LEFT) abzudecken. Wenn die Richtung nicht erkannt wird, wird "UNKNOWN" zurückgegeben. Die Funktion `boolToString` konvertiert einen boolschen Wert in einen String ("true" oder "false"). Die Funktion `createMessageQueue` erstellt eine Message Queue und gibt ihre ID zurück. Dabei wird `msgget` verwendet, um eine Message Queue mit den angegebenen Flags zu erstellen. Bei Fehlern wird eine Fehlermeldung ausgegeben und das Programm wird mit exit(EXIT_FAILURE) beendet. Durch `getMessageQueue` wird eine existierende Message Queue abgerufen und ihre ID zurückgegeben. Das `msgget` wird mit dem Flag 0 verwendet, um auf eine bestehende Message Queue zuzugreifen. Bei Fehlern wird eine Fehlermeldung ausgegeben und das Programm wird mit exit(EXIT_FAILURE) beendet. Das Shared Memory wird durch die Funktion `createShm` erstellt. Diese gibt einen Zeiger auf diesen Bereich zurück. Dann wird `shmget` verwendet, um einen Shared Memory Bereich mit den angegebenen Flags zu erstellen. Bei Fehlern wird wieder eine Fehlermeldung ausgegeben und das Programm wird mit exit(EXIT_FAILURE) beendet. Anschließend wird `shmat` verwendet, um den erstellten Shared Memory Bereich zu befestigen. Wenn dies fehlschlägt, wird wie in den anderen Fällen eine Fehlermeldung ausgegeben und das Programm wird beendet. Die Funktion `getShm` ruft einen existierenden Shared Memory Bereich ab und gibt einen Zeiger auf diesen zurück. Auch diese Funktion wird mit dem Flag 0 verwendet, um auf einen existierenden Shared Memory Bereich zuzugreifen. Bei Fehlern wird auch an dieser Stelle eine Fehlermeldung ausgegeben und das Programm wird mit exit(EXIT_FAILURE) beendet. Dann wird wieder `shmat` verwendet, um den existierenden Shared Memory Bereich zu befestigen. Wenn dies fehlschlägt, wird ebenfalls eine Fehlermeldung ausgegeben und das Programm wird beendet.

Die Header-Datei `RoboticSystem.h` enthält Definitionen, Konstanten, Strukturen und Funktionen für das Robotiksystem. Zuerst werden die nötigen Konstanten, dann die Strukturen, Nachrichtenstrukturen, der gemeinsame Speicher und die Funktionsprototypen definiert.

### Controller

Der Controller in `Controller.c` ist das Herzstück der Simulation. Er implementiert die Hauptlogik eines Controllerprozesses für ein Robotiksystem. Der Controllerprozess empfängt Daten von den unterschiedlichen Sensoren, wie GPS-Position, Umgebungsinformationen und Paketdaten, und trifft basierend auf diesen Daten Entscheidungen. Der Code verwendet Nachrichtenwarteschlangen (msg_queue) und Shared Memory (shm) für die Kommunikation zwischen verschiedenen Prozessen. Dafür sind im Controller unterschiedliche Funktionen implementiert. Über die Funktion `ReceiveGPSPosMessage` erhält der Controller die GPS-Position der Drohne. Durch die Funktion `ReceiveNewSurroundingMessage` erhält er Informationen des Umgebungssensors. Entsprechend gibt es eine Funktion `ReceiveNewPackageDataMessage`, über welche der Controller die Informationen über das Package erhält. 

Um Befehle zu senden und damit Aktionen der Drohne hervorzurufen, gibt es folgende Funktionen. Über die Funktion `SendDirectionMessage` wird die Flugrichtung an die Drohne übermittelt, die Funktion `SendDropMessage` übermittelt Drop-Befehle. Des Weiteren gibt es die Funktion `calculateNextStep`, welche die nächste Flugrichtung basierend auf der aktuellen GPS-Position, der Zielposition und der Umgebung berechnet. Die Funktion `generateObstacles` generiert Hindernisse im Grid des Shared Memory, während die Funktion `makeNextStep` entscheidet, welchen nächsten Schritt der Roboter unter Berücksichtigung der GPS-Position, Zielposition und Umgebung machen soll, und dann den entsprechenden Steuerbefehl sendet. In der main-Funktion wird schließlich eine Zielposition für den Roboter generiert, der Shared Memory und die Message Queues initialisiert, das Grid mit den Hindernissen initialisiert und die Endlosschleife gestartet, in welcher der Controller auf die Sensordaten wartet und Entscheidungen trifft.

### Tower

Das Tower-Skript dient dem Zweck, alle Prozesse zu überwachen und sie bei Bedarf neu zu starten. Die Funktion start_process wird verwendet, um einen Prozess im Hintergrund zu starten. Der übergebene Prozessname ist das erste Argument ('$1'). Die Funktion startet den Prozess im Hintergrund ("&") und gibt eine Nachricht aus, die den gestarteten Prozess und seine Prozess-ID (PID) anzeigt. Die cleanup-Funktion wird aufgerufen, um die Simulation sauber zu beenden. Sie gibt eine Meldung aus und beendet alle Kindprozesse, schließt alle Ausgabefenster (`pkill -TERM -P $$`), beendet den Prozess der Animation der Drohne "DroneAnimation.out" und löscht die Interprozesskommunikationsmittel, nämlich die Message Queues und den Shared Memory. Durch die Zeile
```
trap cleanup SIGINT
```
wird die Funktion cleanup aufgerufen, wenn das Skript ein SIGINT-Signal durch die Tastenkombination Ctrl+C empfängt. Im Anschluss erfolgt die Überwachung der Prozesse in einer Endlosschleife. Die überwachten Prozesse sind "Controller.out", "GPSSensor.out", "Surroundingsensor.out", "Engine.out", "Dropper.out", "PackageSensor.out" und "DroneAnimation.out". Falls einer dieser Prozesse nicht läuft, wird er mit der Funktion start_process neu gestartet. Die Überwachung erfolgt mithilfe von `pgrep`, das nach Prozess-IDs sucht. In der Überwachung der DroneAnimation wird über
```bash
cmd.exe /C start wsl bash -c "cd $current_dir && ./DroneAnimation.out;"
```
zusätzlich automatisch eine neue Windows-Eingabeaufforderung für die Animation geöffnet. Am Ende wird das Skript für 5 Sekunden pausiert, bevor es erneut die Prozesse überwacht. Dies dient dazu, die Systemressourcen zu schonen und nicht unnötig oft nach den Prozessen zu suchen. Das Skript stellt zusammenfassend sicher, dass die definierten Prozesse kontinuierlich laufen, und ermöglicht eine einfache Wartung und Bereinigung bei Bedarf.

### GPS-Sensor

In der `GPSSensor.c` wird die Hauptlogik eines fiktiven Sensorprozesses implementiert, der eine Positionsinformation über einen Shared Memory an eine Nachrichtenwarteschlange sendet, Logeinträge erstellt und die Daten speichert. Die Funktionen `saveData` und `loadData` dienen dem Speichern und Laden von Positionsdaten in eine binäre Datei mit dem Namen `PERSISTENT_DATA_FILE`. Die Funktion `logMessageFromPosition` erstellt eine logische Nachricht aus einer Position (struct Position2D) und speichert sie im angegebenen Puffer (buffer). Die erstellte Nachricht enthält die X- und Y-Position der übergebenen Position. Die Funktion `send_message` sendet eine Nachricht an die Nachrichtenwarteschlange (msg_queue_id). Die Nachricht enthält eine Position (Pos2D) vom Typ struct Position2D mit einem spezifizierten Nachrichtentyp (msg_type). In der main-Funktion wird schließlich die Shared Memory ID geholt. Dann wird der Shared Memory angehängt und eine Nachrichtenwarteschlange (Message Queue) wird erstellt oder sich mit dieser verbunden. Im Anschluss simuliert die main-Funktion eine Schleife für die Sensoraktivität, liest die GPS-Position aus dem Shared Memory, sendet diese Position an die Nachrichtenwarteschlange und erstellt Logeinträge. Durch Warten (sleep) für das angegebene Intervall (SAMPLERATE_GPSSENSOR) wird Sensoraktivität simuliert. Schließlich wird im Falle eines normalen Programmendes, das normalerweise nicht erreicht wird, die Nachrichtenwarteschlange gelöscht.

### Package-Sensor

Die `PackageSensor.c` implementiert einen einfachen Prozess (Packageprocess), der mit einem Shared Memory und einer Nachrichtenwarteschlange (Message Queue) arbeitet. Der Prozess simuliert Sensoraktivität, generiert Paketdaten und sendet diese über die Nachrichtenwarteschlange. Die Funktion `send_package_message` sendet Paketdaten über eine Nachrichtenwarteschlange und protokolliert gleichzeitig den Vorgang. Die gesendeten Paketdaten sind zwei Booleans, die enthalten, ob die Drohne gerade ein Paket besitzt (HasPackage) und ob diese das Paket gerade absetzt (IsDropping). Die main-Funktion beginnt damit, die Shared Memory-ID zu erhalten und sich dann an den Shared Memory anzuhängen. Danach wird eine Nachrichtenwarteschlange erstellt oder mit einer vorhandenen verbunden. Die Paketdaten im Shared Memory werden initialisiert, und anschließend wird die Hauptlogik des Sensorprozesses in einer Endlosschleife implementiert. Innerhalb dieser Schleife wird die simulierte Sensoraktivität durchgeführt, die Paketdaten aktualisiert und über die send_package_message-Funktion an die Nachrichtenwarteschlange gesendet. Es wird auch eine Pause entsprechend der SAMPLERATE_PACKAGESENSOR eingefügt, um einen Simulationszyklus für den Sensor zu emulieren. Schließlich werden Aufräumarbeiten durchgeführt, wobei die Nachrichtenwarteschlange gelöscht wird. Es ist zu beachten, dass der Code normalerweise die Endlosschleife nicht verlassen sollte. Falls jedoch der Code die Endlosschleife verlässt, werden die Aufräumarbeiten durchgeführt. 

### Surrounding-Sensor

Die `Surroundingsensor.c` implementiert die Hauptlogik eines Sensorprozesses, der Umgebungsinformationen eines Roboters liest und diese Informationen über eine Nachrichtenwarteschlange an andere Prozesse sendet. Die Funktion `sendSurroundingmessage` sendet eine Nachricht über eine Nachrichtenwarteschlange. Sie erstellt eine Instanz der Struktur SurroundingMessage, füllt sie mit den übergebenen Werten und sendet sie mit msgsnd an die Warteschlange. Zusätzlich wird ein Log-Eintrag erstellt, der die gesendeten Umgebungsinformationen protokolliert. Die Funktion `isWithinGrid` überprüft, ob die angegebenen Koordinaten innerhalb eines festgelegten Rasters liegen. Die Funktion `CalculateSurroundings` ist dazu da, um die Umgebungsinformationen (Top, TopRight, Right, BottomRight, Bottom, BottomLeft, Left, TopLeft) basierend auf den aktuellen GPS-Positionskoordinaten, die im Shared Memory (sharedData) gespeichert sind, zu berechnen. Die main-Funktion beginnt mit dem Abruf der Shared-Memory-ID und der Prozess verbindet sich mit dem Shared Memory. Ebenso wird eine Nachrichtenwarteschlange erstellt oder verbunden. Ein initialer Umgebungszustand mySurrounding wird erstellt und die Hauptschleife, in welcher der Sensor simuliert wird, beginnt Umgebungsinformationen zu berechnen und Nachrichten an die Warteschlange zu senden. Die Schleife wird durch usleep für eine bestimmte Zeit (Sensorleseintervall) unterbrochen. Nachdem die Hauptschleife beendet wurde, was normalerweise nicht passiert, werden Aufräumarbeiten durchgeführt, darunter das Löschen der Nachrichtenwarteschlange.

### Engine

In der `Engine.c` wird die Hauptlogik eines Motors für ein robotisches System implementiert. Dieses System empfängt Steuerbefehle über eine Nachrichtenwarteschlange und führt basierend auf diesen Befehlen Aktionen aus. Dabei wird Shared Memory verwendet. Die enthaltene Funktion `enumToString` wandelt einen ENUM-Wert (enum Direction) in einen entsprechenden Text um. Sie verwendet dafür eine switch-Anweisung. Die Funktion `receiveDirectionMessage` empfängt eine Nachricht von einer Nachrichtenwarteschlange (msg_queue_id). Die empfangene Nachricht wird in der Struktur DirectionMessage gespeichert. Falls ein Fehler beim Empfangen auftritt, wird eine Fehlermeldung ausgegeben und das Programm wird beendet.

Die main-Funktion ist der Einstiegspunkt der simulierten Engine. Die Funktion `getShm()` wird aufgerufen, um auf den Shared Memory zuzugreifen und ein struct SharedMemory zurückzugeben. Durch den Aufruf der Funktion `getMessageQueue()` erhält das Programm die ID der Nachrichtenwarteschlange. Die Hauptlogik des Motors wird in einer Endlosschleife ausgeführt. In dieser werden Steuerbefehle von der Nachrichtenwarteschlange empfangen. Diese Steuerbefehle werden interpretiert, und entsprechende Aktionen (Up, Down, Left, Right) ausgeführt. Im Anschluss wird die Position des Roboters im gemeinsamen Speicher aktualisiert, und der Log-Eintrag wird erstellt. Da der Code in einer Endlosschleife ausgeführt wird, wartet das Programm kontinuierlich auf Steuerbefehle und führt Motoraktionen aus, solange es läuft.

### Dropper

Die File `Dropper.c` implementiert die Hauptlogik eines Droppers, der auf Steuerbefehle wartet und dann entsprechende Aktionen ausführt. Der Code verwendet ebenfalls Nachrichtenwarteschlangen und Shared Memory für die Kommunikation zwischen verschiedenen Prozessen. Die Funktion `receiveDropMessage` empfängt eine Nachricht aus der Warteschlange mit der ID `msg_queue_id` und dem Nachrichtentyp `msg_type`. Die empfangene Nachricht wird in einer Struktur namens `DropMessage` gespeichert. Die Funktion gibt den Booleschen Wert aus der empfangenen Nachricht über den Zeiger `drop` zurück. In der main-Funktion wird zunächst die Funktion `getShm` verwendet, um auf den Shared Memory zuzugreifen. Im Anschluss wird die Funktion `getMessageQueue` verwendet, um die ID der Nachrichtenwarteschlange zu erhalten. Die Hauptlogik des Droppers beginnt in der darauffolgenden Endlosschleife. Hier wartet der Dropper auf Befehle. In der Schleife wird die Funktion `receiveDropMessage` aufgerufen, um Steuerbefehle aus der Nachrichtenwarteschlange zu empfangen. Basierend auf den Steuerbefehlen wird das Droppen des Paketes simuliert, wenn der Steuerbefehl "true" ist. Danach wird der Shared Memory aktualisiert und das Logging durchgeführt.

### Drone Animation

Die `DroneAnimation.c` implementiert die Darstellung des Drohnen-Robotersystems auf einem Windows-Terminal mit Unterstützung von Unicode-Zeichen (insbesondere Emojis). Die Anwendung verwendet die ncurses-Bibliothek für die Erstellung von Benutzeroberflächen im Terminal und das Shared Memory. Oben in der Ausgabe ist ein Menü dargestellt, das dem Benutzer die Position der Drohne und des Ziels, sowie den Status, ob die Drohne ein Paket hat und ob sie dieses gerade droppt, mitteilt. Darunter ist das Grid mit den Hindernissen, der Drohne und dem Zielpunkt dargestellt. 

## Nutzung der Interprozesskommunikation

In diesem Projekt wird die Interprozesskommunikation verwendet, um eine effiziente Kommunikation zwischen den verschiedenen Prozessen zu ermöglichen. Interprozesskommunikation ist entscheidend, um Daten zwischen den einzelnen Komponenten des Robotiksystems auszutauschen, Steuerbefehle zu übermitteln und den aktuellen Zustand des Systems zu synchronisieren. Folgende Methoden wurden dafür in diesem Projekt verwendet.

**Message Queues:**
   - **Erstellung von Message Queues:** Die Funktionen `createMessageQueue` und `getMessageQueue` in der `RoboticSystem.c` ermöglichen die Erstellung und den Zugriff auf Message Queues. Diese Message Queues dienen als Kommunikationsmittel zwischen verschiedenen Prozessen, um Nachrichten auszutauschen.
   - **Nachrichten senden und empfangen:** In den Sensorprozessen, genauer dem `GPSSensor.c`, `PackageSensor.c` und `Surroundingsensor.c`, werden Message Queues verwendet, um Positionsdaten, Paketinformationen und Umgebungsinformationen zu senden. Die Funktionen `send_message`, `send_package_message` und `sendSurroundingmessage` senden Nachrichten an die entsprechenden Message Queues. Auf der anderen Seite empfängt die `receiveDirectionMessage`-Funktion in der `Engine.c` Steuerbefehle über die Message Queue.

 **Shared Memory:**
   - **Erstellung und Zugriff auf Shared Memory:** Die Funktionen `createShm` und `getShm` in der `RoboticSystem.c` ermöglichen die Erstellung und den Zugriff auf Shared Memory. Shared Memory wird verwendet, um Daten zwischen verschiedenen Prozessen zu teilen.
   - **Aktualisierung von Shared Memory:** In den Prozessen wie der `Engine.c` und dem `Dropper.c` wird Shared Memory verwendet, um den aktuellen Zustand des Roboters zu speichern. Die Position des Roboters wird im Shared Memory aktualisiert, um sicherzustellen, dass alle Prozesse auf die gleichen Positionsdaten zugreifen.

**Protokollierung und Log-Einträge:**
   - **Log-Einträge erstellen:** In den Sensorprozessen und der Engine werden Log-Einträge erstellt, um Aktionen und Informationen zu protokollieren. Diese Log-Einträge werden oft in Message Queues gesendet und von anderen Prozessen empfangen. Die `logMessageFromPosition`-Funktion in `GPSSensor.c` ist ein Beispiel dafür.

**Überwachung und Neustart von Prozessen:**
   - **Prozessüberwachung durch das Tower-Skript:** Das Tower-Skript verwendet `pgrep`, um Prozesse zu überwachen. Falls ein Prozess nicht läuft, wird er mithilfe von `start_process` neu gestartet. Dieser Neustart wird durch die Verwendung von Message Queues und Shared Memory koordiniert, um sicherzustellen, dass der Systemzustand konsistent bleibt.

Insgesamt ermöglicht die kombinierte Nutzung von Message Queues und Shared Memory eine effiziente und synchronisierte Kommunikation zwischen den verschiedenen Komponenten des Robotic Systems. Die Message Queues dienen dabei als Kanäle für den Austausch von Daten und Steuerbefehlen, während der Shared Memory als gemeinsamer Speicher für wichtige Systemzustände fungiert. Diese Art der Interprozesskommunikation trägt dazu bei, die Komponenten des Robotiksystems zu koppeln und eine reibungslose Zusammenarbeit zu gewährleisten.

## Logging und Datensicherung

Das Logging erfolgt über die `Logging.c`. Dieses Programm besteht aus zwei Funktionen: `ensureDirectoryExists` und `writeToLog`. Diese Funktionen dienen dazu, sicherzustellen, dass ein bestimmtes Verzeichnis existiert und um Nachrichten in eine Logdatei zu schreiben. Die Funktion `ensureDirectoryExists` überprüft, ob das Verzeichnis eines gegebenen Dateipfades für die Logdatei existiert. Wenn das Verzeichnis nicht existiert, wird versucht, es zu erstellen. Die Funktion verwendet ein temporäres Array tempPath, um den Dateipfad zu kopieren. Dann wird der Dateiname entfernt, um nur den Verzeichnispfad zu erhalten. Anschließend wird mit Hilfe der Funktion `stat` überprüft, ob das Verzeichnis bereits existiert. Falls nicht, wird es mit der Funktion `mkdir` erstellt. Die `writeToLog` Funktion schreibt eine Nachricht in eine Logdatei. Sie ruft zuerst ensureDirectoryExists auf, um sicherzustellen, dass das Verzeichnis für die Logdatei vorhanden ist. Die Funktion öffnet die Logdatei im "Anhänge"-Modus ("a"), was bedeutet, dass neue Daten an das Ende der Datei angehängt werden. Falls die Datei nicht geöffnet werden kann, wird eine Fehlermeldung ausgegeben. Danach wird ein Zeitstempel hinzugefügt, der das aktuelle Datum und die aktuelle Uhrzeit repräsentiert. Die Funktion fprintf wird verwendet, um den Zeitstempel und die Nachricht in die Logdatei zu schreiben. Schließlich wird die Datei geschlossen. Eventuelle Fehlermeldungen werden mit perror auf der Konsole auszugeben.

Die Header-Datei `Logging.h` bietet eine Schnittstelle für die soeben beschriebenen Funktionen. Dies ermöglicht es, dass diese in anderen Teilen des Programmes verwendet werden können, ohne die genauen Implementierungen zu kennen.

## Fazit

Das Projekt zeigt eine umfassende Implementierung eines Robotik-Simulationssystems, das auf einer Linux-basierten Umgebung mit Bash-Skripten und C-Programmen entwickelt wurde. Die Verwendung von Interprozesskommunikation in Form von Message Queues und Shared Memory spielt eine entscheidende Rolle in diesem System und bietet mehrere Vorteile. Zum einen ermöglichen die Message Queues eine zuverlässige und effiziente Kommunikation zwischen den verschiedenen Komponenten des Robotiksystems. Durch den Austausch von Nachrichten über diese Queues können die einzelnen Prozesse in Echtzeit miteinander kommunizieren. Dies ist besonders wichtig, um Sensordaten, Steuerbefehle und Systemzustände synchronisiert und konsistent zu halten. Für diese Methode wurde sich entschieden, da die Sensoren in diesem Fall in derselben Taktfrequenz senden. Um diese Synchronität zu ermöglichen, ist die Echtzeitkommunikation wichtig, was durch Message Queues und Shared Memory besonders umgesetzt werden kann.

Die Verwendung von Shared Memory ermöglicht den gemeinsamen Zugriff auf Daten zwischen verschiedenen Prozessen. Insbesondere die Position der Drohne, der Zustand des Pakets und Informationen über die Umgebung werden über den Shared Memory geteilt. Dadurch wird sichergestellt, dass alle Prozesse stets auf die aktuellsten Daten zugreifen und basierend darauf ihre Aktionen koordinieren können. 

Die Kombination aus Message Queues und Shared Memory ermöglicht es den Sensoren (GPS-Sensor, Package-Sensor, Surrounding-Sensor) und Aktuatoren (Engine, Dropper) effektiv miteinander zu interagieren. Die Sensoren senden kontinuierlich Daten über die Queues, während die Aktuatoren diese Daten lesen und entsprechende Aktionen ausführen. Dieser reibungslose Datenaustausch ist entscheidend für die Simulation realistischer Sensor-Aktor-Interaktionen.

Das Tower-Skript überwacht kontinuierlich die laufenden Prozesse und gewährleistet deren Verfügbarkeit. Falls ein Prozess aus irgendeinem Grund beendet wird, wird er durch das Skript automatisch neu gestartet. Auch dabei ermöglicht die Verwendung von Message Queues und Shared Memory eine zuverlässige Koordination dieses Überwachungs- und Neustartmechanismus.

Zusammenfassend trägt die gewählte Interprozesskommunikationsstrategie in Form von Message Queues und Shared Memory wesentlich zur Robustheit, Effizienz und Synchronisation im Drohnen-Simulationssystem bei. Die Verwendung dieser Mechanismen ermöglicht eine reibungslose Zusammenarbeit der verschiedenen Komponenten, was insbesondere in komplexen Robotik-Simulationen von großer Bedeutung ist. Die klare Aufteilung der Aufgaben auf verschiedene Prozesse, die durch Interprozesskommunikation miteinander kommunizieren, spiegelt eine effektive und gut strukturierte Systemarchitektur wider.


