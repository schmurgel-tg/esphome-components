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

# Standard-Device-Class aller Relais (None = generisches boolesches Verhalten)
DEFAULT_RELAIS_DEVICE_CLASS = None

# Definition der möglichen Relais als Tuple (Name, Default Device Class, Bit-Flag)
RELAIS_OPTIONS = {
    "kompressor": ("Kompressor", DEFAULT_RELAIS_DEVICE_CLASS, 1),
    "zusatzheizung": ("Zusatzheizung", DEFAULT_RELAIS_DEVICE_CLASS, 2),
    "erdwaermetauscher": ("Erdwärmetauscher", DEFAULT_RELAIS_DEVICE_CLASS, 4),
    "bypass": ("Bypass", DEFAULT_RELAIS_DEVICE_CLASS, 8),
    "vorheizregister": ("Vorheizregister", DEFAULT_RELAIS_DEVICE_CLASS, 16),
    "netzrelais_bypass": ("Netzrelais Bypass", DEFAULT_RELAIS_DEVICE_CLASS, 32),
    "bedienteil_aktiv": ("Bedienteil aktiv", DEFAULT_RELAIS_DEVICE_CLASS, 64),
    "bedienung_via_rs": (
        "Bedienung via RS-Schnittstelle",
        DEFAULT_RELAIS_DEVICE_CLASS,
        128,
    ),
    "luftstufe_vorhanden": ("Luftstufe Vorhanden", DEFAULT_RELAIS_DEVICE_CLASS, 256),
    "ww_nachheizregister": (
        "Warmwasser Nachheizregister",
        DEFAULT_RELAIS_DEVICE_CLASS,
        512,
    ),
    "magnetventil": ("Magnetventil", DEFAULT_RELAIS_DEVICE_CLASS, 2048),
    "vorheizen_aktiv": ("Vorheizen aktiv", DEFAULT_RELAIS_DEVICE_CLASS, 4096),
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
                        cv.Optional(CONF_DEVICE_CLASS, default=RELAIS_OPTIONS[k][1]): binary_sensor.validate_device_class,
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
    for key, (default_name, default_class, bit_flag) in RELAIS_OPTIONS.items():
        sensor_config = relais_sensors.get(key, {})  # Holt das Relais aus der YAML oder gibt `{}` zurück

        # Falls `skip: true`, den Sensor nicht erstellen
        if sensor_config.get(CONF_DEACTIVATE, False):
            continue        
        # Falls keine `entity_category:` existiert, Standardwert setzen
        sensor_config.setdefault(CONF_ENTITY_CATEGORY, cv.entity_category(ENTITY_CATEGORY_DIAGNOSTIC))

        sensor = await binary_sensor.new_binary_sensor(sensor_config)

        cg.add(var.register_relais_sensor(bit_flag, sensor))
