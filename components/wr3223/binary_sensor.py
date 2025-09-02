import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import (
    CONF_DEVICE_CLASS,
    CONF_ENTITY_CATEGORY,
    CONF_NAME,
    ENTITY_CATEGORY_DIAGNOSTIC,
)

# WR3223 Namespace holen (bereits in __init__.py definiert)
from . import (
    CONF_DEACTIVATE,
    CONF_WR3223_RELAIS_COMPONENT_ID,
    wr3223_ns,
)

WR3223RelaisComponent = wr3223_ns.class_("WR3223RelaisComponent", cg.PollingComponent)

CONF_RELAIS_SENSORS = "relais_sensors"

# Definition der möglichen Relais als Tuple (Name, Default Device Class, Bit-Flag)
RELAIS_OPTIONS = {
    "kompressor": ("Kompressor", 1),
    "zusatzheizung": ("Zusatzheizung", 2),
    "erdwaermetauscher": ("Erdwärmetauscher", 4),
    "bypass": ("Bypass", 8),
    "vorheizregister": ("Vorheizregister", 16),
    "netzrelais_bypass": ("Netzrelais Bypass", 32),
    "bedienteil_aktiv": ("Bedienteil aktiv", 64),
    "bedienung_via_rs": (
        "Bedienung via RS-Schnittstelle",
        128,
    ),
    "luftstufe_vorhanden": ("Luftstufe Vorhanden", 256),
    "ww_nachheizregister": (
        "Warmwasser Nachheizregister",
        512,
    ),
    "magnetventil": ("Magnetventil", 2048),
    "vorheizen_aktiv": ("Vorheizen aktiv", 4096),
}

# Hauptschema für die Komponente
CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_WR3223_RELAIS_COMPONENT_ID): cv.use_id(WR3223RelaisComponent),
        cv.Optional(CONF_RELAIS_SENSORS, default={}): cv.Schema(
            {
                cv.Optional(k, default={}): binary_sensor.binary_sensor_schema().extend(
                    {
                        cv.Optional(CONF_DEACTIVATE, default=False): cv.boolean,  # Option zum Deaktivieren
                        cv.Optional(CONF_NAME, default=RELAIS_OPTIONS[k][0]): cv._validate_entity_name,
                    }
                )
                for k in RELAIS_OPTIONS.keys()
            }
        ),
    }
).extend(cv.COMPONENT_SCHEMA)


# Code-Generierung
async def to_code(config):
    # WR3223RelaisComponent abrufen
    var = await cg.get_variable(config[CONF_WR3223_RELAIS_COMPONENT_ID])

    relais_sensors = config.get(CONF_RELAIS_SENSORS, {})

    # Alle Relais aus RELAIS_OPTIONS durchgehen
    for key, (default_name, bit_flag) in RELAIS_OPTIONS.items():
        sensor_config = relais_sensors.get(key, {})  # Holt das Relais aus der YAML oder gibt `{}` zurück

        # Falls `skip: true`, den Sensor nicht erstellen
        if sensor_config.get(CONF_DEACTIVATE, False):
            continue
        # Falls keine `entity_category:` existiert, Standardwert setzen
        sensor_config.setdefault(CONF_ENTITY_CATEGORY, cv.entity_category(ENTITY_CATEGORY_DIAGNOSTIC))

        sensor = await binary_sensor.new_binary_sensor(sensor_config)

        cg.add(var.register_relais_sensor(bit_flag, sensor))
