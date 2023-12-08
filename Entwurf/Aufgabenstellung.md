## Ziel:
Ein einfaches Robotik-Simulationssystem auf einem Linux-basierten Betriebssystem zu entwickeln. Das System soll Prozesse zur Simulation von Sensordaten und Aktuatorsteuerung beinhalten. Die Kommunikation zwischen diesen Prozessen soll über Interprozesskommunikationsmechanismen erfolgen. Zusätzlich soll ein Skript zur Automatisierung von Start, Überwachung und Shutdown des Simulationssystems erstellt werden.

## Teilaufgaben:

**1. Systementwurf:**
   - Entwerfen Sie eine Systemarchitektur, die mindestens einen Sensorprozess, einen Aktuatorprozess und einen Steuerungsprozess umfasst.
   - Definieren Sie die Datenstrukturen für die Interprozesskommunikation.

**2. Prozessimplementierung:**
   - Implementieren Sie die Sensorprozesse, die simulierte Sensordaten generieren (z.B. Entfernungssensoren, Temperatursensoren).
   - Implementieren Sie die Aktuatorprozesse, die auf Basis der empfangenen Steuerungsbefehle hypothetische Aktionen ausführen (z.B. Motorensteuerung).
   - Implementieren Sie die Steuerungsprozesse, die Sensordaten auswerten und Steuerbefehle an die Aktuatorprozesse sendet.

**3. Interprozesskommunikation:**
   - Verwenden Sie Pipes, Shared Memory und/oder Nachrichtenwarteschlangen, um eine effiziente Kommunikation zwischen den Prozessen zu ermöglichen.

**4. Bash-Skripting:**
   - Erstellen Sie ein Bash-Skript, das:
     - Die Prozesse in der richtigen Reihenfolge startet.
     - Die Prozesse überwacht und bei Bedarf neu startet.
     - Das System ordnungsgemäß herunterfährt und alle Ressourcen freigibt.

**5. Dateisystem und Persistenz:**
   - Speichern Sie die generierten Daten und Logs in einer strukturierten Art und Weise im Dateisystem.
   - Implementieren Sie eine Methode zur Datensicherung und -wiederherstellung.

## Bewertungskriterien:
- Funktionalität und Vollständigkeit der Systemarchitektur und der Prozessimplementierung.
- Korrekte und effiziente Nutzung der Interprozesskommunikation.
- Robustheit und Fehlerbehandlung in den Skripten und Prozessen.
- Sauberkeit und Organisation des Dateisystems, inklusive der Datensicherungs- und Wiederherstellungsmethoden.

## Dokumentation und Abgabe:
- Sie sollen eine Dokumentation anfertigen, die den Systementwurf, die Implementierungsdetails und die Nutzung der Interprozesskommunikation beschreibt.
- Der Quellcode und die Skripte sollen kommentiert und gut strukturiert sein.
- Die Abgabe erfolgt in Form eines Git-Repositories, das den Quellcode, die Skripte, die Konfigurationsdateien und die Dokumentation enthält.

## Zusätzliche Hinweise:
- Die Aufgabe muss in Gruppen von bis zu vier Studierenden bearbeitet werden.
- Die Endabgabe ist in drei Wochen fällig.