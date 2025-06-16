import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import select
from esphome.const import CONF_ID, CONF_OPTIONS, CONF_NAME

from . import (
    WR3223,
    WR3223StatusComponent,
    WR3223ModeComponent,
    wr3223_ns,
    CONF_WR3223_ID,
    CONF_WR3223_STATUS_COMPONENT_ID,
    CONF_WR3223_MODE_COMPONENT_ID,
    CONF_DEACTIVATE,
)

WR3223VentilationLevelSelect = wr3223_ns.class_(
    "WR3223VentilationLevelSelect", select.Select, cg.Component
)
WR3223ModeSelect = wr3223_ns.class_("WR3223ModeSelect", select.Select, cg.Component)

DEFAULT_VENTILATION_LEVEL_OPTIONS = ["AUS", "Luftstufe 1", "Luftstufe 2", "Luftstufe 3"]
DEFAULT_MODE_OPTIONS = [
    "AUS",
    "Sommerbetrieb",
    "Sommer-Abluftbetrieb",
    "Winterbetrieb",
    "Handbetrieb",
]

CONF_SELECTS = "selects"
CONF_VENTILATION_LEVEL = "ventilation_level"
CONF_OPERATION_MODE = "operation_mode"

VENTILATION_LEVEL_SCHEMA = (
    select.select_schema(WR3223VentilationLevelSelect, icon="mdi:fan")
    .extend(
        {
            cv.Optional(CONF_NAME, default="Lüftungsstufe"): cv.string_strict,
            cv.Optional(CONF_DEACTIVATE, default=False): cv.boolean,
            cv.Optional(
                CONF_OPTIONS, default=DEFAULT_VENTILATION_LEVEL_OPTIONS
            ): cv.All(cv.ensure_list(cv.string_strict), cv.Length(min=4, max=4)),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
)

MODE_SELECT_SCHEMA = (
    select.select_schema(WR3223ModeSelect, icon="mdi:format-list-bulleted")
    .extend(
        {
            cv.Optional(CONF_NAME, default="Betriebsmodus"): cv.string_strict,
            cv.Optional(CONF_DEACTIVATE, default=False): cv.boolean,
            cv.Optional(CONF_OPTIONS, default=DEFAULT_MODE_OPTIONS): cv.All(
                cv.ensure_list(cv.string_strict), cv.Length(min=5, max=5)
            ),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_WR3223_ID): cv.use_id(WR3223),
        cv.GenerateID(CONF_WR3223_STATUS_COMPONENT_ID): cv.use_id(
            WR3223StatusComponent
        ),
        cv.GenerateID(CONF_WR3223_MODE_COMPONENT_ID): cv.use_id(WR3223ModeComponent),
        cv.Optional(CONF_SELECTS, default={CONF_VENTILATION_LEVEL: {}}): cv.Schema(
            {
                cv.Optional(
                    CONF_VENTILATION_LEVEL, default={}
                ): VENTILATION_LEVEL_SCHEMA,
                cv.Optional(CONF_OPERATION_MODE, default={}): MODE_SELECT_SCHEMA,
            }
        ),
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_WR3223_ID])
    status_comp = await cg.get_variable(config[CONF_WR3223_STATUS_COMPONENT_ID])
    mode_comp = await cg.get_variable(config[CONF_WR3223_MODE_COMPONENT_ID])

    selects_conf = config.get(CONF_SELECTS, {})

    vent_conf = selects_conf.get(CONF_VENTILATION_LEVEL, {})
    if not vent_conf.get(CONF_DEACTIVATE):
        var = cg.new_Pvariable(vent_conf[CONF_ID], parent, status_comp)
        await cg.register_component(var, vent_conf)
        await select.register_select(
            var,
            vent_conf,
            options=vent_conf.get(CONF_OPTIONS, DEFAULT_VENTILATION_LEVEL_OPTIONS),
        )

    mode_conf = selects_conf.get(CONF_OPERATION_MODE, {})
    if not mode_conf.get(CONF_DEACTIVATE):
        mode_var = cg.new_Pvariable(mode_conf[CONF_ID], parent, mode_comp)
        await cg.register_component(mode_var, mode_conf)
        await select.register_select(
            mode_var,
            mode_conf,
            options=mode_conf.get(CONF_OPTIONS, DEFAULT_MODE_OPTIONS),
        )
