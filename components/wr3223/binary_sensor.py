import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import (    
    CONF_DEVICE_CLASS,
    CONF_ENTITY_CATEGORY,
    CONF_FRIENDLY_NAME,
    CONF_NAME,
    CONF_UPDATE_INTERVAL,
    ENTITY_CATEGORY_DIAGNOSTIC,
)

# WR3223 Namespace holen (bereits in __init__.py definiert)
from . import (
    WR3223,
    CONF_WR3223_ID,
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

# Standardkonfiguration für Relais-Sensoren
RELAIS_SENSOR_SCHEMA = cv.Schema(
    {
        cv.Optional(
            CONF_DEACTIVATE, default=False
        ): cv.boolean,  # Option zum Deaktivieren
    }
).extend(binary_sensor.BINARY_SENSOR_SCHEMA)

# Hauptschema für die Komponente
CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(CONF_WR3223_ID): cv.use_id(WR3223),
            cv.GenerateID(CONF_WR3223_RELAIS_COMPONENT_ID): cv.declare_id(
                WR3223RelaisComponent
            ),
            cv.Optional(CONF_RELAIS_SENSORS, default={}): cv.Schema(
                {
                    cv.Optional(k, default={}): RELAIS_SENSOR_SCHEMA
                    for k in RELAIS_OPTIONS.keys()
                }
            ),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(cv.polling_component_schema("60s"))
)


# Code-Generierung
async def to_code(config):
    # WR3223 Hauptkomponente abrufen
    parent = await cg.get_variable(config[CONF_WR3223_ID])

    # WR3223RelaisComponent instanziieren und registrieren
    var = cg.new_Pvariable(
        config[CONF_WR3223_RELAIS_COMPONENT_ID], parent, config[CONF_UPDATE_INTERVAL]
    )
    await cg.register_component(var, config)

    relais_sensors = config.get(CONF_RELAIS_SENSORS, {})

    # Alle Relais aus RELAIS_OPTIONS durchgehen
    for key, (default_name, default_class, bit_flag) in RELAIS_OPTIONS.items():
        sensor_config = relais_sensors.get(
            key, {}
        )  # Holt das Relais aus der YAML oder gibt `{}` zurück

        # Falls `skip: true`, den Sensor nicht erstellen
        if sensor_config.get(CONF_DEACTIVATE, False):
            continue

        # Falls kein `name:` gesetzt wurde, Standardname aus `key` nutzen
        sensor_config.setdefault(CONF_NAME, key.replace("_", " ").title())
        # Falls keine `id:` existiert, eine generieren
        sensor_config.setdefault(CONF_FRIENDLY_NAME, default_name)
        # Falls keine `device_class:` existiert, Standardwert setzen
        sensor_config.setdefault(CONF_DEVICE_CLASS, default_class)
        # Falls keine `entity_category:` existiert, Standardwert setzen
        sensor_config.setdefault(
            CONF_ENTITY_CATEGORY, cv.entity_category(ENTITY_CATEGORY_DIAGNOSTIC)
        )

        sensor = await binary_sensor.new_binary_sensor(sensor_config)

        cg.add(var.register_relais_sensor(bit_flag, sensor))
