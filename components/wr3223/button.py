import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import button
from esphome.const import ENTITY_CATEGORY_CONFIG, CONF_ID, CONF_NAME

from . import (
    wr3223_ns,
    CONF_WR3223_RELAIS_COMPONENT_ID,
    CONF_WR3223_STATUS_COMPONENT_ID,
    CONF_WR3223_MODE_COMPONENT_ID,
    CONF_DEACTIVATE,
    WR3223StatusComponent,
    WR3223ModeComponent,
)
from .binary_sensor import WR3223RelaisComponent

WR3223RelaisUpdateButton = wr3223_ns.class_(
    "WR3223RelaisUpdateButton", button.Button, cg.Component
)
WR3223SaveStateButton = wr3223_ns.class_(
    "WR3223SaveStateButton", button.Button, cg.Component
)
WR3223RestoreStateButton = wr3223_ns.class_(
    "WR3223RestoreStateButton", button.Button, cg.Component
)

CONF_BUTTONS = "buttons"
CONF_UPDATE = "update_relais"
CONF_SAVE = "save_state"
CONF_RESTORE = "restore_state"

BUTTON_SCHEMAS = {
    CONF_UPDATE: button.button_schema(
        WR3223RelaisUpdateButton,
        icon="mdi:update",
        entity_category=ENTITY_CATEGORY_CONFIG,
    ).extend(
        {
            cv.Optional(CONF_NAME, default="Update Relais"): cv.string_strict,
            cv.Optional(CONF_DEACTIVATE, default=False): cv.boolean,
        }
    ),
    CONF_SAVE: button.button_schema(
        WR3223SaveStateButton,
        icon="mdi:content-save",
        entity_category=ENTITY_CATEGORY_CONFIG,
    ).extend(
        {
            cv.Optional(CONF_NAME, default="Config speichern"): cv.string_strict,
            cv.Optional(CONF_DEACTIVATE, default=False): cv.boolean,
        }
    ),
    CONF_RESTORE: button.button_schema(
        WR3223RestoreStateButton,
        icon="mdi:restore",
        entity_category=ENTITY_CATEGORY_CONFIG,
    ).extend(
        {
            cv.Optional(CONF_NAME, default="Config laden"): cv.string_strict,
            cv.Optional(CONF_DEACTIVATE, default=False): cv.boolean,
        }
    ),
}

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_WR3223_RELAIS_COMPONENT_ID): cv.use_id(
            WR3223RelaisComponent
        ),
        cv.GenerateID(CONF_WR3223_STATUS_COMPONENT_ID): cv.use_id(
            WR3223StatusComponent
        ),
        cv.GenerateID(CONF_WR3223_MODE_COMPONENT_ID): cv.use_id(WR3223ModeComponent),
        cv.Optional(CONF_BUTTONS, default={}): cv.Schema(
            {
                cv.Optional(key, default={}): schema
                for key, schema in BUTTON_SCHEMAS.items()
            }
        ),
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    relais = await cg.get_variable(config[CONF_WR3223_RELAIS_COMPONENT_ID])
    status = await cg.get_variable(config[CONF_WR3223_STATUS_COMPONENT_ID])
    mode = await cg.get_variable(config[CONF_WR3223_MODE_COMPONENT_ID])

    buttons_conf = config.get(CONF_BUTTONS, {})

    if not buttons_conf:
        buttons_conf = {}

    async def build(conf_key, cls, *args):
        conf = buttons_conf.get(conf_key, {})
        if conf.get(CONF_DEACTIVATE):
            return
        var = cg.new_Pvariable(conf[CONF_ID], *args)
        await cg.register_component(var, conf)
        await button.register_button(var, conf)

    await build(CONF_UPDATE, WR3223RelaisUpdateButton, relais)
    await build(CONF_SAVE, WR3223SaveStateButton, mode, status)
    await build(CONF_RESTORE, WR3223RestoreStateButton, mode, status)
