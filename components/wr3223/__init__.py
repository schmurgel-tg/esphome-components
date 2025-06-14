import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart, binary_sensor, text_sensor
from esphome.components.binary_sensor import new_binary_sensor
from esphome.const import(    
    CONF_UART_ID,
    CONF_UPDATE_INTERVAL,
    CONF_NAME,
    CONF_DEVICE_CLASS, 
    CONF_ENTITY_CATEGORY, 
    DEVICE_CLASS_PROBLEM,
    ENTITY_CATEGORY_DIAGNOSTIC
)

# ESPHome Namespace für WR3223
wr3223_ns = cg.esphome_ns.namespace("wr3223")

# Hauptklasse für das WR3223-Gerät
WR3223 = wr3223_ns.class_("WR3223", cg.PollingComponent)
WR3223Connector = wr3223_ns.class_("WR3223Connector", cg.Component)
WR3223ErrorComponent = wr3223_ns.class_("WR3223ErrorComponent", cg.PollingComponent)


# Automatisches Laden des UART-Moduls
AUTO_LOAD = ["uart", "text_sensor", "binary_sensor"]

CONF_WR3223_ID = "wr3223_id"
CONF_WR3223_CONNECTOR_ID = "wr3223_connector_id"
CONF_WR3223_ERROR_COMPONENT_ID = "wr3223_error_component_id"
CONF_DEACTIVATE = "deactivate"
CONF_ERROR_POLLING = "error_polling"
CONF_ERROR_STATUS = "error_status_sensor"
CONF_ERROR_TEXT = "error_text_sensor"

# YAML-Validierung für ESPHome
CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(CONF_WR3223_ID): cv.declare_id(WR3223),    
    cv.GenerateID(CONF_WR3223_CONNECTOR_ID): cv.declare_id(WR3223Connector),
    cv.GenerateID(CONF_WR3223_ERROR_COMPONENT_ID): cv.declare_id(WR3223ErrorComponent),
    cv.Required(CONF_UART_ID): cv.use_id(uart.UARTComponent),
    cv.Optional(CONF_ERROR_POLLING, default={}): cv.Schema({
        cv.Optional(CONF_UPDATE_INTERVAL, default="60s"): cv.update_interval,
        cv.Optional(CONF_DEACTIVATE, default=False): cv.boolean,  # Option zum Deaktivieren
        cv.Optional(CONF_ERROR_STATUS, default={
            CONF_NAME: "FEHLER", 
            CONF_DEVICE_CLASS: DEVICE_CLASS_PROBLEM, 
            CONF_ENTITY_CATEGORY: ENTITY_CATEGORY_DIAGNOSTIC,
            }): binary_sensor.binary_sensor_schema().extend({
                cv.Optional(CONF_DEACTIVATE, default=False): cv.boolean,  # Option zum Deaktivieren
        }),
        cv.Optional(CONF_ERROR_TEXT, default={
            CONF_NAME: "FEHLER Text",            
            CONF_ENTITY_CATEGORY: ENTITY_CATEGORY_DIAGNOSTIC
        }): text_sensor.text_sensor_schema().extend({
            cv.Optional(CONF_DEACTIVATE, default=False): cv.boolean,  # Option zum Deaktivieren
        }), 
    }),
}).extend(cv.COMPONENT_SCHEMA).extend(cv.polling_component_schema("30s"))

# Code-Generierung für ESPHome
async def to_code(config):
    # Hauptkomponente erzeugen
    var = cg.new_Pvariable(config[CONF_WR3223_ID], await cg.get_variable(config[CONF_UART_ID]))    
    await cg.register_component(var, config)

    # WR3223Connector als eigene Komponente registrieren
    connector = cg.new_Pvariable(config[CONF_WR3223_CONNECTOR_ID], await cg.get_variable(config[CONF_UART_ID]))
    cg.add(var.set_connector(connector))  # Verbinde den Connector mit WR3223
    
    await cg.register_component(connector, {})


    error_polling = config.get(CONF_ERROR_POLLING)        
    
    if error_polling.get(CONF_DEACTIVATE) != True: # standardmäßg erzeugen wir die error_component        
        
        error_status_config = error_polling.get(CONF_ERROR_STATUS)
        error_text_config = error_polling.get(CONF_ERROR_TEXT)

        if(error_status_config.get(CONF_DEACTIVATE) != True or error_text_config.get(CONF_DEACTIVATE) != True):
            # WR3223ErrorComponent instanziieren und registrieren
            error_component = cg.new_Pvariable(config[CONF_WR3223_ERROR_COMPONENT_ID], var, error_polling.get(CONF_UPDATE_INTERVAL, cv.update_interval("60s")))
            await cg.register_component(error_component, config)
            
            if error_status_config.get(CONF_DEACTIVATE) != True:
                sensor_error_status = await new_binary_sensor(error_status_config)
                cg.add(error_component.register_status_sensor(sensor_error_status))
    
            if error_text_config.get(CONF_DEACTIVATE) != True:
                sensor_error_text = await text_sensor.new_text_sensor(error_text_config)
                cg.add(error_component.register_text_sensor(sensor_error_text))