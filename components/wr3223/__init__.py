import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.const import CONF_ID, CONF_UPDATE_INTERVAL, CONF_UART_ID

# ESPHome Namespace für WR3223
wr3223_ns = cg.esphome_ns.namespace("wr3223")

# Hauptklasse für das WR3223-Gerät
WR3223 = wr3223_ns.class_("WR3223", cg.PollingComponent)

# Sensor-Polling-Komponente (für alle Sensoren)
WR3223SensorPollingComponent = wr3223_ns.class_("WR3223SensorPollingComponent", cg.PollingComponent)


# Automatisches Laden des UART-Moduls
AUTO_LOAD = ["uart"]

CONF_WR3223_SENSOR_POLLING_ID = "sensor_polling_id"
CONF_UPDATE_INTERVAL_SENSOR_POLLING = "sensor_polling_intervall"

# YAML-Validierung für ESPHome
CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(WR3223),
    # Automatische ID für die Sensor-Polling-Komponente registrieren
    cv.GenerateID(CONF_WR3223_SENSOR_POLLING_ID): cv.declare_id(WR3223SensorPollingComponent),
    cv.Required(CONF_UART_ID): cv.use_id(uart.UARTComponent),
    cv.Optional(CONF_UPDATE_INTERVAL_SENSOR_POLLING, default="50s"): cv.update_interval,  # Standard-Update-Intervall
}).extend(cv.COMPONENT_SCHEMA) #.extend(uart.UART_DEVICE_SCHEMA) 

# Code-Generierung für ESPHome
async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID], await cg.get_variable(config[CONF_UART_ID]))
    await cg.register_component(var, config)
    #await uart.register_uart_device(var, config)

    # Sensor-Polling-Komponente initialisieren und in WR3223 speichern
    polling_component = cg.new_Pvariable(config[CONF_WR3223_SENSOR_POLLING_ID], var, config[CONF_UPDATE_INTERVAL_SENSOR_POLLING])    
    await cg.register_component(polling_component, config)
