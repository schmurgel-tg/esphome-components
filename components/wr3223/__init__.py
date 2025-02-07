import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.const import CONF_UART_ID

# ESPHome Namespace für WR3223
wr3223_ns = cg.esphome_ns.namespace("wr3223")

# Hauptklasse für das WR3223-Gerät
WR3223 = wr3223_ns.class_("WR3223", cg.PollingComponent)


# Automatisches Laden des UART-Moduls
AUTO_LOAD = ["uart"]

CONF_WR3223_ID = "wr3223_id"
CONF_UPDATE_INTERVAL_SENSOR_POLLING = "sensor_polling_intervall"

# YAML-Validierung für ESPHome
CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(CONF_WR3223_ID): cv.declare_id(WR3223),
    cv.Required(CONF_UART_ID): cv.use_id(uart.UARTComponent)    
}).extend(cv.COMPONENT_SCHEMA)

# Code-Generierung für ESPHome
async def to_code(config):
    var = cg.new_Pvariable(config[CONF_WR3223_ID], await cg.get_variable(config[CONF_UART_ID]))
    await cg.register_component(var, config)