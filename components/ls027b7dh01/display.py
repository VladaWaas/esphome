import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import display, spi
from esphome.const import CONF_ID, CONF_LAMBDA, CONF_CS_PIN

AUTO_LOAD = ["display"]

DEPENDENCIES = ["spi"]

ls032b7dd02_ns = cg.esphome_ns.namespace("ls027b7dh01")
LS027B7DH01 = ls027b7dh01_ns.class_(
    "LS027B7DH01", cg.PollingComponent, display.DisplayBuffer, spi.SPIDevice
)

CONFIG_SCHEMA = (
    display.FULL_DISPLAY_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(LS027B7DH01),
            cv.Required(CONF_CS_PIN): pins.gpio_output_pin_schema,
        }
    )
    .extend(cv.polling_component_schema("60s"))
    .extend(spi.spi_device_schema())
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await spi.register_spi_device(var, config)

    if CONF_LAMBDA in config:
        lambda_ = await cg.process_lambda(
            config[CONF_LAMBDA], [(display.DisplayRef, "it")], return_type=cg.void
        )
        cg.add(var.set_writer(lambda_))