import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    UNIT_CELSIUS, 
    CONF_UNIT_OF_MEASUREMENT, 
    CONF_DEVICE_CLASS, 
    DEVICE_CLASS_TEMPERATURE, 
    STATE_CLASS_MEASUREMENT,     
    CONF_NAME
)

# WR3223 Namespace holen (bereits in __init__.py definiert)
from . import (
    WR3223SensorPollingComponent,
    CONF_WR3223_SENSOR_POLLING_ID,
)

# Liste der unterstützten Sensor-Kommandos mit ihren Standardwerten
SENSOR_COMMANDS = {
    # Temperatur-Sensoren
    "T1": ("Verdampfertemperatur", UNIT_CELSIUS, DEVICE_CLASS_TEMPERATURE),
    "T2": ("Kondensatortemperatur", UNIT_CELSIUS, DEVICE_CLASS_TEMPERATURE),
    "T3": ("Außentemperatur", UNIT_CELSIUS, DEVICE_CLASS_TEMPERATURE),
    "T5": ("Nach Wärmetauscher (Fortluft)", UNIT_CELSIUS, DEVICE_CLASS_TEMPERATURE),
    "T6": ("Zulufttemperatur", UNIT_CELSIUS, DEVICE_CLASS_TEMPERATURE),
    "T8": ("Nach Vorheizregister", UNIT_CELSIUS, DEVICE_CLASS_TEMPERATURE),

    # Drehzahl-Sensoren
    "NA": ("Drehzahl Abluft", "rpm", STATE_CLASS_MEASUREMENT),
    "NZ": ("Drehzahl Zuluft", "rpm", STATE_CLASS_MEASUREMENT),
}

CONF_COMMAND = "command"

# **Definition der einzelnen Temperatur-Sensoren**
CONFIG_SCHEMA = cv.Schema({    
    cv.GenerateID(): cv.declare_id(sensor.Sensor),    
    cv.GenerateID(CONF_WR3223_SENSOR_POLLING_ID): cv.use_id(WR3223SensorPollingComponent),    
    cv.Required(CONF_COMMAND): cv.one_of(*SENSOR_COMMANDS.keys(), lower=False),
    cv.Optional(CONF_NAME): cv.string,
    cv.Optional(CONF_UNIT_OF_MEASUREMENT): cv.string,
    cv.Optional(CONF_DEVICE_CLASS): cv.string,    
}).extend(sensor.sensor_schema(state_class=STATE_CLASS_MEASUREMENT))

async def to_code(config):
    """ESPHome Code-Generierung für WR3223 Sensoren"""    
    # **Das Kommando holen**
    command = config[CONF_COMMAND]

    # **Standardwerte setzen**
    default_name, default_unit, default_device_class = SENSOR_COMMANDS[command]

    # Falls kein Name angegeben wurde, den Standardnamen aus `SENSOR_COMMANDS` verwenden
    name = config.get(CONF_NAME, default_name)

    # Falls `unit_of_measurement` nicht überschrieben wurde, Standardwert nehmen
    unit = config.get(CONF_UNIT_OF_MEASUREMENT, default_unit)

    # Falls `device_class` nicht überschrieben wurde, Standardwert nehmen
    device_class = config.get(CONF_DEVICE_CLASS, default_device_class)

    sens = await sensor.new_sensor(config)
    cg.add(sens.set_name(name))

    if unit:
        cg.add(sens.set_unit_of_measurement(unit))

    if device_class:
        cg.add(sens.set_device_class(device_class))
    
    # WR3223 Hauptkomponente abrufen
    polling_component = await cg.get_variable(config[CONF_WR3223_SENSOR_POLLING_ID])

    cg.add(polling_component.register_sensor(sens, command))  # Sensor mit zugehörigem Kommando registrieren
