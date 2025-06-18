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
WR3223StatusValueHolder = wr3223_ns.class_("WR3223StatusValueHolder")
WR3223StatusComponent = wr3223_ns.class_("WR3223StatusComponent", cg.PollingComponent)
WR3223ModeValueHolder = wr3223_ns.class_("WR3223ModeValueHolder")
WR3223ModeComponent = wr3223_ns.class_("WR3223ModeComponent", cg.PollingComponent)
WR3223RelaisComponent = wr3223_ns.class_("WR3223RelaisComponent", cg.PollingComponent)


# Automatisches Laden der Module
AUTO_LOAD = ["uart", "text_sensor", "binary_sensor", "switch", "select", "number", "button"]

CONF_WR3223_ID = "wr3223_id"
CONF_WR3223_CONNECTOR_ID = "wr3223_connector_id"
CONF_WR3223_ERROR_COMPONENT_ID = "wr3223_error_component_id"
CONF_WR3223_STATUS_COMPONENT_ID = "wr3223_status_component_id"
CONF_WR3223_STATUS_HOLDER_ID = "wr3223_status_holder_id"
CONF_WR3223_MODE_COMPONENT_ID = "wr3223_mode_component_id"
CONF_WR3223_MODE_HOLDER_ID = "wr3223_mode_holder_id"
CONF_WR3223_RELAIS_COMPONENT_ID = "wr3223_relais_component_id"
CONF_DEACTIVATE = "deactivate"
CONF_ERROR_POLLING = "error_polling"
CONF_ERROR_STATUS = "error_status_sensor"
CONF_ERROR_TEXT = "error_text_sensor"
CONF_STATUS_UPDATE_INTERVAL = "status_update_interval"
CONF_MODE_UPDATE_INTERVAL = "mode_update_interval"
CONF_RELAIS_UPDATE_INTERVAL = "relais_update_interval"

def validate_status_interval(value):
    value = cv.update_interval(value)
    if value.total_milliseconds > 20000:
        raise cv.Invalid("Status update interval must not exceed 20s")
    return value

# YAML-Validierung für ESPHome
CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(CONF_WR3223_ID): cv.declare_id(WR3223),    
    cv.GenerateID(CONF_WR3223_CONNECTOR_ID): cv.declare_id(WR3223Connector),
    cv.GenerateID(CONF_WR3223_ERROR_COMPONENT_ID): cv.declare_id(WR3223ErrorComponent),
    cv.GenerateID(CONF_WR3223_STATUS_COMPONENT_ID): cv.declare_id(WR3223StatusComponent),
    cv.GenerateID(CONF_WR3223_STATUS_HOLDER_ID): cv.declare_id(WR3223StatusValueHolder),
    cv.GenerateID(CONF_WR3223_MODE_COMPONENT_ID): cv.declare_id(WR3223ModeComponent),
    cv.GenerateID(CONF_WR3223_MODE_HOLDER_ID): cv.declare_id(WR3223ModeValueHolder),
    cv.GenerateID(CONF_WR3223_RELAIS_COMPONENT_ID): cv.declare_id(WR3223RelaisComponent),
    cv.Optional(CONF_STATUS_UPDATE_INTERVAL, default="10s"): validate_status_interval,
    cv.Optional(CONF_MODE_UPDATE_INTERVAL, default="60s"): cv.update_interval,
    cv.Optional(CONF_RELAIS_UPDATE_INTERVAL, default="60s"): cv.update_interval,
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
    connector = cg.new_Pvariable(
        config[CONF_WR3223_CONNECTOR_ID], 
        await cg.get_variable(config[CONF_UART_ID])
    )    
    await cg.register_component(connector, {})
    cg.add(var.set_connector(connector))  # Verbinde den Connector mit WR3223    

    # Relais component wird nun immer gebaut, damit andere module das nutzen koennen
    relais_component = cg.new_Pvariable(
        config[CONF_WR3223_RELAIS_COMPONENT_ID],
        connector,
        config[CONF_RELAIS_UPDATE_INTERVAL],
    )
    await cg.register_component(relais_component, {})
    cg.add(var.set_relais_component(relais_component)) # Verbinde die RelaisComponent mit WR3223

    holder = cg.new_Pvariable(config[CONF_WR3223_STATUS_HOLDER_ID])
    status_component = cg.new_Pvariable(
        config[CONF_WR3223_STATUS_COMPONENT_ID],
        var,
        config[CONF_STATUS_UPDATE_INTERVAL],
        holder,
    )
    await cg.register_component(status_component, {})

    mode_holder = cg.new_Pvariable(config[CONF_WR3223_MODE_HOLDER_ID])
    mode_component = cg.new_Pvariable(
        config[CONF_WR3223_MODE_COMPONENT_ID],
        var,
        config[CONF_MODE_UPDATE_INTERVAL],
        mode_holder,
    )
    await cg.register_component(mode_component, {})

    

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
