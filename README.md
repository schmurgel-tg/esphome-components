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
Richtige Zuordnung von TX-/RX-Pins am Mikrocontroller (siehe Bild):  
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

select:
  - platform: wr3223

switch:
  - platform: wr3223

number:
  - platform: wr3223
```

## Detailkonfiguration
```yaml
# Sensoren
sensor:
  - platform: wr3223
    sensors: # hier können alle Sensoren mit bekannten Kommandos angepasst oder deaktiviert werden
      T1:
        name: "Verdampfertemperatur" # z.B. der Name kann überschrieben werden
      T2:
        deactivate: true # wird ein Kommando nicht benötigt, kann es deaktiviert werden
      T3:
        update_interval: "15s" # Der standard Update-Intervall liegt für alle bei 60s, dieser kann hier angepasst werden
    sensors_custom: # In diesem Bereich können eigene Kommando hinzugefügt werden (siehe Kommandoliste für WR-3223)
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
Abweichend von älteren Versionen besitzen die Relais ohne weitere Angabe keine
`device_class` mehr und erscheinen damit als einfache An/Aus-Sensoren
("Aktiv"/"Aus").


# Lüftungsstufe konfigurieren

Die WR3223-Komponente erstellt standardmäßig ein `select` zur Wahl der Lüftungsstufe. 
Die vier Optionen `AUS`, `Luftstufe 1`, `Luftstufe 2` und `Luftstufe 3` sind bereits vorkonfiguriert. 

Soll das Select angepasst oder ganz abgeschaltet werden, kann dies über den Bereich `selects:` erfolgen:

```yaml
select:
  - platform: wr3223
    selects:
      ventilation_level:
        # deactivate: true  # falls das Dropdown nicht benötigt wird
        options: ["AUS", "1", "2", "3"] # optionales überschreiben der Anzeigwerte der Luftstufen
        # wird automatisch über die Status-Komponente aktualisiert -> kein UpdateIntervall
```

# Status-Schalter

Neben dem Dropdown für die Lüftungsstufe können auch die Ventilatorstellwerte
der einzelnen Stufen über `number`-Slider angepasst werden. Die Werte liegen
zwischen 40 % und 100 %.

```yaml
number:
  - platform: wr3223
    numbers:
      vent_level_1_speed:
        name: "Luftstufe 1 [%]"
      vent_level_2_speed:
        name: "Luftstufe 2 [%]"
      vent_level_3_speed:
        name: "Luftstufe 3 [%]"
```

Zusätzlich legt die Komponente drei Schalter an, mit denen sich Wärmepumpe,
Zusatzheizung und Kühlung ein- oder ausschalten lassen. Bei Bedarf können die
Schalter deaktiviert werden:

```yaml
switch:
  - platform: wr3223
    switches:
      heat_pump:
        # deactivate: true
      additional_heating:
        # deactivate: true
      cooling:
        # deactivate: true
```

![Alternativtext](https://github.com/schmurgel-tg/esphome/blob/main/images/20230101_174032.jpg) "Anschlussbeispiel")  

