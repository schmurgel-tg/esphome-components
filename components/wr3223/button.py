import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import button
from esphome.const import ENTITY_CATEGORY_CONFIG, CONF_ID, CONF_NAME

from . import wr3223_ns, CONF_WR3223_RELAIS_COMPONENT_ID, CONF_DEACTIVATE
from .binary_sensor import WR3223RelaisComponent

WR3223RelaisUpdateButton = wr3223_ns.class_(
    "WR3223RelaisUpdateButton", button.Button, cg.Component
)

CONFIG_SCHEMA = (
    button.button_schema(
        WR3223RelaisUpdateButton,
        icon="mdi:update",
        entity_category=ENTITY_CATEGORY_CONFIG,
    )
    .extend(
        {            
            cv.GenerateID(CONF_WR3223_RELAIS_COMPONENT_ID): cv.use_id(WR3223RelaisComponent),
            cv.Optional(CONF_NAME, default="Update Relais"): cv.string_strict,
            cv.Optional(CONF_DEACTIVATE, default=False): cv.boolean,
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
)


async def to_code(config):
    relais = await cg.get_variable(config[CONF_WR3223_RELAIS_COMPONENT_ID])
    conf = config[CONF_ID]
    if not conf.get(CONF_DEACTIVATE):
        var = cg.new_Pvariable(conf, relais)
        await cg.register_component(var, config)
        await button.register_button(var, config)