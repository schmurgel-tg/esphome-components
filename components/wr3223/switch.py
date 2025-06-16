import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import switch
from esphome.const import CONF_ID, CONF_NAME

from . import (
    WR3223StatusComponent,
    wr3223_ns,
    CONF_WR3223_STATUS_COMPONENT_ID,
    CONF_DEACTIVATE,
)

WR3223StatusSwitch = wr3223_ns.class_("WR3223StatusSwitch", switch.Switch, cg.Component)
WR3223HeatPumpSwitch = wr3223_ns.class_("WR3223HeatPumpSwitch", WR3223StatusSwitch)
WR3223AdditionalHeatingSwitch = wr3223_ns.class_("WR3223AdditionalHeatingSwitch", WR3223StatusSwitch)
WR3223CoolingSwitch = wr3223_ns.class_("WR3223CoolingSwitch", WR3223StatusSwitch)

CONF_SWITCHES = "switches"
CONF_HEAT_PUMP = "heat_pump"
CONF_ADDITIONAL_HEATING = "additional_heating"
CONF_COOLING = "cooling"


def _switch_schema(class_, default_name: str):
    return (
        switch.switch_schema(class_)
        .extend({cv.Optional(CONF_NAME, default=default_name): cv.string_strict})
        .extend({cv.Optional(CONF_DEACTIVATE, default=False): cv.boolean})
        .extend(cv.COMPONENT_SCHEMA)
    )


CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_WR3223_STATUS_COMPONENT_ID): cv.use_id(WR3223StatusComponent),
        cv.Optional(CONF_SWITCHES, default={}): cv.Schema(
            {
                cv.Optional(CONF_HEAT_PUMP, default={}): _switch_schema(WR3223HeatPumpSwitch, "Wärmepumpe"),
                cv.Optional(CONF_ADDITIONAL_HEATING, default={}): _switch_schema(WR3223AdditionalHeatingSwitch, "Zusatzheizung"),
                cv.Optional(CONF_COOLING, default={}): _switch_schema(WR3223CoolingSwitch, "Kühlung"),
            }
        ),
    }
)


async def to_code(config):
    status_comp = await cg.get_variable(config[CONF_WR3223_STATUS_COMPONENT_ID])
    switches_conf = config.get(CONF_SWITCHES, {})

    async def build(key, class_):
        conf = switches_conf.get(key, {})
        if conf.get(CONF_DEACTIVATE):
            return        
        var = await switch.new_switch(conf)
        await cg.register_component(var, conf)
        cg.add(var.set_status_component(status_comp))

    await build(CONF_HEAT_PUMP, WR3223HeatPumpSwitch)
    await build(CONF_ADDITIONAL_HEATING, WR3223AdditionalHeatingSwitch)
    await build(CONF_COOLING, WR3223CoolingSwitch)