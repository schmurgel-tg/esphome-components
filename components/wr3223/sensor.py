import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    UNIT_CELSIUS, 
    CONF_UNIT_OF_MEASUREMENT, 
    CONF_DEVICE_CLASS, 
    DEVICE_CLASS_TEMPERATURE, 
    STATE_CLASS_MEASUREMENT,     
    CONF_ID,
    CONF_NAME,
    CONF_UPDATE_INTERVAL,
    CONF_SENSORS,    
)

# WR3223 Namespace holen (bereits in __init__.py definiert)
from . import (
    WR3223,
    CONF_WR3223_ID,
    wr3223_ns
)

# Sensor-Polling-Komponente (für alle Sensoren)
WR3223SensorPollingComponent = wr3223_ns.class_("WR3223SensorPollingComponent", cg.PollingComponent)

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
CONF_SENSOR_POLLING_COMPONENT_ID = "polling_component_id"

def validate_command(value):
    value = cv.string(value)
    if value in SENSOR_COMMANDS:  # Falls es ein bekanntes Kommando ist, ist alles gut
        return value
    if len(value) != 2:  # Falls es ein benutzerdefiniertes Kommando ist, prüfen wir die Länge
        raise cv.Invalid("Custom command must be exactly two characters long.")
    return value

# **Definition der einzelnen Temperatur-Sensoren**
CONFIG_SCHEMA = cv.Schema(
    {        
    cv.GenerateID(CONF_WR3223_ID): cv.use_id(WR3223),
    cv.Required(CONF_SENSORS): cv.ensure_list(
        cv.Schema({
            cv.GenerateID(CONF_SENSOR_POLLING_COMPONENT_ID): cv.declare_id(WR3223SensorPollingComponent),                                    
            cv.Required(CONF_COMMAND): cv.enum(SENSOR_COMMANDS, lower=False, space=False, 
                                               additional_validator=validate_command), 
            cv.Optional(CONF_NAME): cv.string,
            cv.Optional(CONF_UNIT_OF_MEASUREMENT): cv.string,
            cv.Optional(CONF_DEVICE_CLASS): cv.string,
            cv.Optional(CONF_UPDATE_INTERVAL, default="5s"): cv.positive_time_period_milliseconds,
        }).extend(sensor.sensor_schema(state_class=STATE_CLASS_MEASUREMENT)),
    ),
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    """ESPHome Code-Generierung für WR3223 Sensoren"""    

    # WR3223 Hauptkomponente abrufen
    parent = await cg.get_variable(config[CONF_WR3223_ID])

    for sensor_config in config[CONF_SENSORS]:
        
        # **Das Kommando holen**
        command = sensor_config[CONF_COMMAND]

        if command in SENSOR_COMMANDS:
            # **Standardwerte setzen**, falls nciht überschrieben
            default_name, default_unit, default_device_class = SENSOR_COMMANDS[command]
            name = sensor_config.get(CONF_NAME, default_name)
            unit = sensor_config.get(CONF_UNIT_OF_MEASUREMENT, default_unit)
            device_class = sensor_config.get(CONF_DEVICE_CLASS, default_device_class)
        else:
             # Falls es ein benutzerdefiniertes Kommando ist, müssen diese Werte zwingend angegeben werden
            if CONF_NAME not in sensor_config:
                raise cv.Invalid(f"Custom command '{command}' requires a name.")
            if CONF_UNIT_OF_MEASUREMENT not in sensor_config:
                raise cv.Invalid(f"Custom command '{command}' requires a unit of measurement.")
            if CONF_DEVICE_CLASS not in sensor_config:
                raise cv.Invalid(f"Custom command '{command}' requires a device class.")
            
            name = sensor_config[CONF_NAME]
            unit = sensor_config[CONF_UNIT_OF_MEASUREMENT]
            device_class = sensor_config[CONF_DEVICE_CLASS]

        sens = await sensor.new_sensor(sensor_config)
        cg.add(sens.set_name(name))

        if unit:
            cg.add(sens.set_unit_of_measurement(unit))

        if device_class:
            cg.add(sens.set_device_class(device_class))

         
        var = cg.new_Pvariable(
            sensor_config[CONF_SENSOR_POLLING_COMPONENT_ID],  # Hier wird die ID aus der Konfiguration verwendet
            parent,
            sensor_config[CONF_UPDATE_INTERVAL],
            sens,
            sensor_config["command"]
        )

        await cg.register_component(var, sensor_config)
        
