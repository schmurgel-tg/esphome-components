import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    UNIT_CELSIUS, 
    CONF_UNIT_OF_MEASUREMENT, 
    CONF_DEVICE_CLASS, 
    DEVICE_CLASS_TEMPERATURE, 
    STATE_CLASS_MEASUREMENT,         
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
CONF_SENSORS_CUSTOM = "sensors_custom"

def validate_custom_command(value):
    """Validiert, dass benutzerdefinierte Kommandos genau 2 Zeichen haben."""
    value = cv.string(value)
    if not (len(value) == 2 and value.isalnum()):
        raise cv.Invalid(f"Custom command '{value}' must be exactly two alphanumeric characters long.")
    return value


# **Definition der einzelnen Temperatur-Sensoren**
CONFIG_SCHEMA = cv.Schema(
    {        
    cv.GenerateID(CONF_WR3223_ID): cv.use_id(WR3223),

    # **Standard-Sensoren mit IntelliSense (NUR vordefinierte Werte)**
    cv.Optional(CONF_SENSORS, default=[]): cv.ensure_list(
        cv.Schema(
            {
                cv.GenerateID(CONF_SENSOR_POLLING_COMPONENT_ID): cv.declare_id(WR3223SensorPollingComponent),                                                
                cv.Required(CONF_COMMAND): cv.one_of(*SENSOR_COMMANDS.keys(), lower=False),            
                cv.Optional(CONF_NAME): cv.string,
                cv.Optional(CONF_UNIT_OF_MEASUREMENT): cv.string,
                cv.Optional(CONF_DEVICE_CLASS): cv.string,
                cv.Optional(CONF_UPDATE_INTERVAL, default="5s"): cv.positive_time_period_milliseconds,
            }
        ).extend(sensor.sensor_schema(state_class=STATE_CLASS_MEASUREMENT)),
    ),

    # **Benutzerdefinierte Sensoren (MÜSSEN genau 2 Zeichen haben + Pflichtfelder)**
    cv.Optional(CONF_SENSORS_CUSTOM, default=[]): cv.ensure_list(
        cv.Schema(
            {
                cv.GenerateID(CONF_SENSOR_POLLING_COMPONENT_ID): cv.declare_id(WR3223SensorPollingComponent),
                cv.Required(CONF_COMMAND): cv.All(cv.string, validate_custom_command),
                cv.Required(CONF_NAME): cv.string,  # Pflichtfelder
                cv.Required(CONF_UNIT_OF_MEASUREMENT): cv.string,
                cv.Required(CONF_DEVICE_CLASS): cv.string,
                cv.Optional(CONF_UPDATE_INTERVAL, default="5s"): cv.positive_time_period_milliseconds,
            }
        ).extend(sensor.sensor_schema(state_class=STATE_CLASS_MEASUREMENT)),
    ),

}).extend(cv.COMPONENT_SCHEMA)


async def generate_sensor_code(parent, sensor_config):
    """Erstellt den Code für einen einzelnen Sensor."""
    
    command = sensor_config[CONF_COMMAND]
    name = sensor_config.get(CONF_NAME, SENSOR_COMMANDS.get(command, ("", "", ""))[0])  # Falls Standard, nehme `SENSOR_COMMANDS`
    unit = sensor_config.get(CONF_UNIT_OF_MEASUREMENT, SENSOR_COMMANDS.get(command, ("", "", ""))[1])
    device_class = sensor_config.get(CONF_DEVICE_CLASS, SENSOR_COMMANDS.get(command, ("", "", ""))[2])

    sens = await sensor.new_sensor(sensor_config)
    cg.add(sens.set_name(name))

    if unit:
        cg.add(sens.set_unit_of_measurement(unit))

    if device_class:
        cg.add(sens.set_device_class(device_class))

    var = cg.new_Pvariable(
        sensor_config[CONF_SENSOR_POLLING_COMPONENT_ID],  
        parent,
        sensor_config[CONF_UPDATE_INTERVAL],
        sens,
        command
    )

    await cg.register_component(var, sensor_config)


async def to_code(config):
    """ESPHome Code-Generierung für WR3223 Sensoren"""    

    # WR3223 Hauptkomponente abrufen
    parent = await cg.get_variable(config[CONF_WR3223_ID])

    # Standard-Sensoren durchgehen
    for sensor_config in config.get(CONF_SENSORS, []):
        await generate_sensor_code(parent, sensor_config)

    # Benutzerdefinierte Sensoren durchgehen
    for sensor_config in config.get(CONF_SENSORS_CUSTOM, []):
        await generate_sensor_code(parent, sensor_config)