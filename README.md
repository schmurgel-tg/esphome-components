## ESPHome-Komponenten für WR3223-Lüftungs-/Wärmepumpensteuerung
Mit diesem Projekt kannst du eine Hermes WR 3223-basierte Heizungs- bzw. Lüftungssteuerung (Wärmepumpe, Lüfterstufen, Zusatzheizung etc.) über ESPHome in Home Assistant einbinden.

**Achtung:** Im gegensatz zur alten Version, kann diese aktuell nur lesend auf die Hermes WR3223 zugreifen. Das steuern kommt in einem späteren Update.

### Funktionen im Überblick
- Abfragen von Temperaturwerten (z.B. Verdampfer, Kondensator, Außenluft etc.)
- Erfassen und Überwachen diverser Relaiszustände (Kompressor, Bypass, Zusatzheizung etc.)
- ~~Setzen von Lüfterstufen~~
- ~~Aktivieren/Deaktivieren von Wärmepumpe und Zusatzheizung~~
- Auslesen von Fehler- und Statusmeldungen
- ~~Einstellen der Zuluftsolltemperatur und anderer Parameter~~
- Komfortable Ansteuerung direkt aus Home Assistant

## Installation und Einbindung
1. Vorbereitung
Du benötigst ESPHome (meist über Home Assistant Add-on installiert oder eigenständig nutzbar).  
Dein ESP32/ESP8266 muss mit der WR3223-Steuerung seriell verbunden sein (UART).  
Achte dabei auf:  
Baudrate (i. d. R. 9600)  
Datenbits / Parität / Stopbits (hier im Projekt: 7 Datenbits, EVEN-Parität, 1 Stoppbit)  
Richtige Zuordnung von TX-/RX-Pins am Mikrocontroller (siehe Bild)  
![Alternativtext](https://github.com/schmurgel-tg/esphome/blob/main/images/pinout.jpg "Serial port pinout")  
3. Externe Komponente einbinden
Damit ESPHome diese Custom Components direkt aus diesem Git-Repository lädt, kannst du in deiner esphome:-Konfiguration den Bereich für externe Komponenten konfigurieren.  
Dafür fügst du in deiner YAML-Datei Folgendes hinzu (diese Einbindung ist ein Beispiel; ggf. anpassen):  
```yaml
external_components:
  - source:
      type: git
      url: https://github.com/schmurgel-tg/esphome-components
      # Optional: bestimmter Branch oder Tag, z.B.:
      # ref: main
    # In vielen Projekten kann man "components" weglassen,
    # da alle relevanten Dateien eingebunden werden.
    # components: ["wr3223_controller", "wr3223_sensorConnectors", ... ]
```

## Minimalkonfiguration mit alle Sensoren mit Standardwerten:
```yaml
external_components:
  - source:
      type: git
      url: https://github.com/schmurgel-tg/esphome-components

uart:
  - id: uart_bus
    tx_pin: GPIO19
    rx_pin: GPIO18
    baud_rate: 9600
    data_bits: 7
    parity: EVEN
    stop_bits: 1

wr3223:
  uart_id: uart_bus

sensor:
  - platform: wr3223

binary_sensor:
  - platform: wr3223
```

## Detailkonfiguration
```yaml
uart:
  - id: uart_bus
    tx_pin: GPIO19
    rx_pin: GPIO18
    baud_rate: 9600
    data_bits: 7
    parity: EVEN
    stop_bits: 1

wr3223:
  uart_id: uart_bus

# Sensoren
sensor:
  - platform: wr3223
    sensors: # hier alle sensoren mit bekannten kommandos (diese haben standardwerte)
      T1:
        name: "Verdampfertemperatur" # der Name kann überchrieben werden
      T2:
        deactivate: true # alle Temperatur kommaadnos werden per Default angelegt, soll wird eins nicht benötigt, kann es deaktiviert werden
      T3:
        update_interval: "15s" # es kann der Updateintervall für jedes einzelen gesetzt werden, standardmßig sind es 60s
    sensors_custom: # hier können eigene Kommando hinzugefügt werden
      - command: "XZ"
        name: "Custom-Kommand"
        unit_of_measurement: V
        device_class: temperature
      - command: "LS"
        name: "Aktuelle Luftstufe"
        unit_of_measurement: Level             

# Binär-Sensoren
binary_sensor:
  - platform: wr3223
    relais_sensors:
      zusatzheizung:
        deactivate: true # Dieser Sensor wird nicht erstellt
      kompressor:
        name: "Anderer Name"
        entity_category: "diagnostic"
        device_class: "power"  # Falls gewünscht, kann die `device_class` überschrieben werden        
      bypass:
        name: "Relais: Bypass"
        entity_category: "config"
```

