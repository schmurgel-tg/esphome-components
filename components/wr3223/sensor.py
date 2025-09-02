import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor as sensor_comp
from esphome.const import (
    UNIT_CELSIUS,
    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_FREQUENCY,
    STATE_CLASS_MEASUREMENT,
    CONF_DEVICE_CLASS,
    CONF_NAME,
    CONF_SENSORS,
    CONF_ID,    
    CONF_UPDATE_INTERVAL,
    CONF_UNIT_OF_MEASUREMENT,
    CONF_ACCURACY_DECIMALS,
)

# WR3223 Namespace holen (bereits in __init__.py definiert)
from . import WR3223, CONF_WR3223_ID, CONF_DEACTIVATE, wr3223_ns

# Sensor-Polling-Komponente (für alle Sensoren)
WR3223SensorPollingComponent = wr3223_ns.class_(
    "WR3223SensorPollingComponent", cg.PollingComponent
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
    "NA": ("Drehzahl Abluft", "rpm", DEVICE_CLASS_FREQUENCY),
    "NZ": ("Drehzahl Zuluft", "rpm", DEVICE_CLASS_FREQUENCY),
}

CONF_COMMAND = "command"
CONF_SENSOR_POLLING_COMPONENT_ID = "polling_component_id"
CONF_SENSORS_CUSTOM = "sensors_custom"


def validate_custom_command(value):
    """Validiert, dass benutzerdefinierte Kommandos genau 2 Zeichen haben."""
    value = cv.string(value)
    if not (len(value) == 2 and value.isalnum()):
        raise cv.Invalid(
            f"Custom command '{value}' must be exactly two alphanumeric characters long."
        )
    return value


# **Definition der einzelnen Temperatur-Sensoren**
CONFIG_SCHEMA = (
    sensor_comp.sensor_schema()
    .extend(
        {
            # Default Id setzen, damit YAML nichts angeben muss:
            cv.Optional(CONF_ID, default="wr3223_sensor"): cv.declare_id(sensor_comp.Sensor),

            cv.GenerateID(CONF_WR3223_ID): cv.use_id(WR3223),

            # **Standard-Sensoren mit IntelliSense (NUR vordefinierte Werte)**
            cv.Optional(CONF_SENSORS, default={}): cv.Schema(
                {
                    cv.Optional(k, default={}): cv.Schema(
                        {
                            cv.GenerateID(
                                CONF_SENSOR_POLLING_COMPONENT_ID
                            ): cv.declare_id(WR3223SensorPollingComponent),
                            cv.Optional(
                                CONF_DEACTIVATE, default=False
                            ): cv.boolean,  # Sensor deaktivieren
                        }
                    )
                    .extend(sensor.SENSOR_SCHEMA)
                    .extend(cv.polling_component_schema("60s"))
                    for k in SENSOR_COMMANDS.keys()
                }
            ),
            # **Benutzerdefinierte Sensoren (MÜSSEN genau 2 Zeichen haben + Pflichtfelder)**
            cv.Optional(CONF_SENSORS_CUSTOM, default=[]): cv.ensure_list(
                sensor_comp.sensor_schema(state_class=STATE_CLASS_MEASUREMENT)
                .extend(
                    {
                        cv.GenerateID(CONF_SENSOR_POLLING_COMPONENT_ID): cv.declare_id(
                            WR3223SensorPollingComponent
                        ),
                        cv.Required(CONF_COMMAND): cv.All(
                            cv.string, validate_custom_command
                        ),
                        cv.Required(CONF_NAME): cv._validate_entity_name,
                        cv.Optional(
                            CONF_UNIT_OF_MEASUREMENT
                        ): sensor_comp.validate_unit_of_measurement,
                        cv.Optional(CONF_DEVICE_CLASS): sensor_comp.validate_device_class,
                    }
                )
                .extend(cv.polling_component_schema("60s")),
            ),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
)


async def generate_sensor_code(parent, sensor_config):
    """Erstellt den Code für einen einzelnen Sensor."""

    command = sensor_config[CONF_COMMAND]

    sens = await sensor.new_sensor(sensor_config)

    var = cg.new_Pvariable(
        sensor_config[CONF_SENSOR_POLLING_COMPONENT_ID],
        parent,
        sensor_config[CONF_UPDATE_INTERVAL],
        sens,
        command,
    )

    await cg.register_component(var, sensor_config)


async def to_code(config):
    """ESPHome Code-Generierung für WR3223 Sensoren"""

    # WR3223 Hauptkomponente abrufen
    parent = await cg.get_variable(config[CONF_WR3223_ID])

    # Standard-Sensoren (Dictionary: {command: sensor_config})
    for command, sensor_config in config.get(CONF_SENSORS, {}).items():
        if sensor_config.get(CONF_DEACTIVATE, False):
            continue  # Sensor nicht erstellen, wenn deaktiviert

        sensor_config[CONF_COMMAND] = command
        # Standardwerte setzen, falls nicht vorhanden
        sensor_config.setdefault(
            CONF_NAME, SENSOR_COMMANDS[command][0]
        )  # Name aus `SENSOR_COMMANDS`
        sensor_config.setdefault(CONF_UNIT_OF_MEASUREMENT, SENSOR_COMMANDS[command][1])
        sensor_config.setdefault(CONF_DEVICE_CLASS, SENSOR_COMMANDS[command][2])
        sensor_config.setdefault(CONF_ACCURACY_DECIMALS, 1)

        await generate_sensor_code(parent, sensor_config)

    # Benutzerdefinierte Sensoren durchgehen
    for sensor_config in config.get(CONF_SENSORS_CUSTOM, []):
        await generate_sensor_code(parent, sensor_config)
