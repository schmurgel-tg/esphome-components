import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import select
from esphome.const import CONF_ID, CONF_UPDATE_INTERVAL, CONF_OPTIONS

from . import (
    WR3223,
    WR3223StatusComponent,
    wr3223_ns,
    CONF_WR3223_ID,
    CONF_WR3223_STATUS_COMPONENT_ID,
)

WR3223VentilationLevelSelect = wr3223_ns.class_(
    "WR3223VentilationLevelSelect", select.Select, cg.PollingComponent
)

DEFAULT_OPTIONS = ["AUS", "1", "2", "3"]

CONFIG_SCHEMA = (
    select.select_schema(WR3223VentilationLevelSelect, icon="mdi:fan")
    .extend(
        {
            cv.GenerateID(CONF_WR3223_ID): cv.use_id(WR3223),
            cv.GenerateID(CONF_WR3223_STATUS_COMPONENT_ID): cv.use_id(WR3223StatusComponent),
            cv.Optional(CONF_UPDATE_INTERVAL, default="30s"): cv.update_interval,
            cv.Optional(CONF_OPTIONS, default=DEFAULT_OPTIONS): cv.All(
                cv.ensure_list(cv.string_strict), cv.Length(min=4, max=4)
            ),
        }
    )
    .extend(cv.polling_component_schema("30s"))
)

async def to_code(config):
    parent = await cg.get_variable(config[CONF_WR3223_ID])
    status_comp = await cg.get_variable(config[CONF_WR3223_STATUS_COMPONENT_ID])
    var = cg.new_Pvariable(
        config[CONF_ID], parent, config[CONF_UPDATE_INTERVAL], status_comp
    )
    await cg.register_component(var, config)
    await select.register_select(
        var, config, options=config.get(CONF_OPTIONS, DEFAULT_OPTIONS)
    )
