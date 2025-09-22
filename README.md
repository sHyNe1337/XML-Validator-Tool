# XML-Validator

Ein C++ Command-Line-Tool zur Validierung von XML-Dateien mit automatischer XSD-Schema-Erkennung.

## Features

- **XML Well-formedness-Prüfung**: Überprüft die grundlegende XML-Syntax
- **XSD-Schema-Validierung**: Automatische Suche und Validierung gegen gleichnamige XSD-Dateien
- **Automatische Dateierkennung**: Findet automatisch XML-Dateien im aktuellen Verzeichnis
- **Logging**: Erstellt detaillierte Log-Dateien für jede Validierung
- **Flexible Parameter**: Verschiedene Ausführungsmodi je nach Bedarf

## Verwendung

### Grundlegende Syntax
```
XMLValidator [Optionen]
```

### Verfügbare Optionen

| Option | Beschreibung |
|--------|-------------|
| `:P` oder `:Path <pfad>` | Verzeichnispfad zur XML-Datei |
| `:N` oder `:Name <dateiname>` | Name der XML-Datei |
| `:V` oder `:Validierung` | XML-Validierung aktivieren |
| `:T` oder `:Transformieren` | XML-Transformation aktivieren (in Entwicklung) |

### Anwendungsbeispiele

#### 1. Automatische Erkennung (Empfohlen)
```cmd
XMLValidator
```
- Findet die erste XML-Datei im aktuellen Verzeichnis
- Aktiviert automatisch Validierung und Transformation

#### 2. Spezifische Datei validieren
```cmd
XMLValidator :P "C:\Dokumente" :N "firma.xml"
```
- Aktiviert automatisch Validierung und Transformation für die angegebene Datei

#### 3. Nur Validierung
```cmd
XMLValidator :P "C:\Dokumente" :N "firma.xml" :V
```

#### 4. Validierung mit Transformation
```cmd
XMLValidator :P "C:\Dokumente" :N "firma.xml" :V :T
```

## Automatische Parameterlogik

Das Tool wendet intelligente Standardwerte an:

- **Keine Parameter**: Erste XML-Datei im aktuellen Ordner → `:V` und `:T` werden gesetzt
- **Nur `:P` und `:N`**: → `:V` und `:T` werden automatisch aktiviert
- **`:P`, `:N` und `:T`**: → `:V` wird automatisch aktiviert (Transformation erfordert Validierung)

## XSD-Schema-Erkennung

Der Validator sucht automatisch nach XSD-Dateien:
- Gleicher Dateiname wie die XML-Datei
- Gleicher Pfad wie die XML-Datei
- Beispiel: `firma.xml` → sucht nach `firma.xsd`

## Ausgabe und Logging

### Console-Ausgabe
```
=== XML-Validierung gestartet ===
XML-Datei: .\firma.xml
Starte XML-Validierung...
XSD-Schema gefunden: .\firma.xsd
Validierung erfolgreich
Transformation in process
Programmausführung erfolgreich abgeschlossen.
```

### Log-Dateien
- Automatische Erstellung: `dateiname.log` (z.B. `firma.log`)
- Enthält detaillierte Validierungsinformationen und Fehlermeldungen
- Zeitstempel und vollständige Fehlerdetails bei Parsing-Problemen

## Fehlermeldungen

Das Tool liefert präzise Fehlermeldungen:
- **Fehlercode**: COM-spezifische Fehlercodes
- **Zeilennummer**: Exakte Position des Fehlers
- **Spaltenposition**: Genaue Stelle in der Zeile
- **Fehlerbeschreibung**: Verständliche Erklärung des Problems

## Systemanforderungen

- **Betriebssystem**: Windows (COM-basiert)
- **Abhängigkeiten**: 
  - MSXML6 (Microsoft XML Core Services)
  - Visual C++ Runtime

## Technische Details

- **Sprache**: C++ mit COM-Integration
- **XML-Parser**: Microsoft MSXML6 (DOMDocument60)
- **Schema-Unterstützung**: XMLSchemaCache60
- **Encoding**: Unicode (UTF-16) Support

## Rückgabecodes

| Code | Bedeutung |
|------|-----------|
| `0` | Erfolgreiche Ausführung |
| `1` | Fehler bei Ausführung oder Validierung |

## Bekannte Einschränkungen

- Transformation ist derzeit nur als Platzhalter implementiert
- Ausschließlich Windows-Plattform (COM-Abhängigkeit)
- Benötigt installiertes MSXML6