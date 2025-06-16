import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import select
from esphome.const import CONF_ID, CONF_OPTIONS

from . import (
    WR3223,
    WR3223StatusComponent,
    wr3223_ns,
    CONF_WR3223_ID,
    CONF_WR3223_STATUS_COMPONENT_ID,
    CONF_DEACTIVATE
)

WR3223VentilationLevelSelect = wr3223_ns.class_(
    "WR3223VentilationLevelSelect", select.Select, cg.Component
)

DEFAULT_OPTIONS = ["AUS", "Luftstufe 1", "Luftstufe 2", "Luftstufe 3"]

CONF_SELECTS = "selects"
CONF_VENTILATION_LEVEL = "ventilation_level"

VENTILATION_LEVEL_SCHEMA = (
    select.select_schema(WR3223VentilationLevelSelect, icon="mdi:fan")
    .extend(
        {
            cv.Optional(CONF_DEACTIVATE, default=False): cv.boolean,            
            cv.Optional(CONF_OPTIONS, default=DEFAULT_OPTIONS): cv.All(
                cv.ensure_list(cv.string_strict), cv.Length(min=4, max=4)
            ),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_WR3223_ID): cv.use_id(WR3223),
        cv.GenerateID(CONF_WR3223_STATUS_COMPONENT_ID): cv.use_id(WR3223StatusComponent),
        cv.Optional(CONF_SELECTS, default={CONF_VENTILATION_LEVEL: {}}): cv.Schema(
            {
                cv.Optional(CONF_VENTILATION_LEVEL, default={}): VENTILATION_LEVEL_SCHEMA,
            }
        ),
    }
).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    parent = await cg.get_variable(config[CONF_WR3223_ID])
    status_comp = await cg.get_variable(config[CONF_WR3223_STATUS_COMPONENT_ID])

    selects_conf = config.get(CONF_SELECTS, {})

    vent_conf = selects_conf.get(CONF_VENTILATION_LEVEL, {})
    if not vent_conf.get(CONF_DEACTIVATE):
        var = cg.new_Pvariable(
            vent_conf[CONF_ID], parent, status_comp
        )
        await cg.register_component(var, vent_conf)
        await select.register_select(
            var, vent_conf, options=vent_conf.get(CONF_OPTIONS, DEFAULT_OPTIONS)
        )
