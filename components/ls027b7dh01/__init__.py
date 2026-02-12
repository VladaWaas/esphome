import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import spi
from esphome.components import display as esphome_display
from esphome.const import (
    CONF_ID,
    CONF_LAMBDA,
    CONF_PAGES,
)

CODEOWNERS = ["@esphome/core"]
AUTO_LOAD = ["display"]
DEPENDENCIES = ["spi"]

ls027b7dh01_ns = cg.esphome_ns.namespace("ls027b7dh01")
LS027B7DH01 = ls027b7dh01_ns.class_(
    "LS027B7DH01", cg.PollingComponent, spi.SPIDevice, esphome_display.DisplayBuffer
)

CONFIG_SCHEMA = (
    esphome_display.FULL_DISPLAY_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(LS027B7DH01),
        }
    )
    .extend(cv.polling_component_schema("1s"))
    .extend(spi.spi_device_schema(cs_pin_required=True))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await spi.register_spi_device(var, config)
    await esphome_display.register_display(var, config)
