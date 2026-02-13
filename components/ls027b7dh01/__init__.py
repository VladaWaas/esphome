import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import spi
from esphome import pins
from esphome.const import CONF_ID, CONF_LAMBDA, CONF_UPDATE_INTERVAL

DEPENDENCIES = ["spi"]
AUTO_LOAD = ["display"]

ls027b7dh01_ns = cg.esphome_ns.namespace("ls027b7dh01")
LS027B7DH01 = ls027b7dh01_ns.class_(
    "LS027B7DH01",
    cg.PollingComponent,
    spi.SPIDevice
)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(LS027B7DH01),
    cv.Optional(CONF_LAMBDA): cv.lambda_,
    cv.Optional(CONF_UPDATE_INTERVAL, default="1s"): cv.update_interval,
}).extend(cv.polling_component_schema("1s")).extend(spi.spi_device_schema(cs_pin_required=True))

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await spi.register_spi_device(var, config)
    
    if CONF_LAMBDA in config:
        lambda_ = await cg.process_lambda(
            config[CONF_LAMBDA], [], return_type=cg.void
        )
        cg.add(var.set_writer(lambda_))
