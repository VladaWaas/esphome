import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import display, spi
from esphome.const import CONF_ID, CONF_LAMBDA

from . import ls027b7dh01_ns, LS027B7DH01, CONF_LS027B7DH01_ID

DEPENDENCIES = ["spi"]

# Platform schema pro display
CONFIG_SCHEMA = display.BASIC_DISPLAY_SCHEMA.extend({
    cv.GenerateID(): cv.declare_id(LS027B7DH01),
}).extend(cv.polling_component_schema("1s")).extend(spi.spi_device_schema(cs_pin_required=True))

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await display.register_display(var, config)
    await spi.register_spi_device(var, config)
