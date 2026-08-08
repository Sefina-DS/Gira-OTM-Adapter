import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.const import CONF_ID

CODEOWNERS = ["@user"]
DEPENDENCIES = ["uart"]
AUTO_LOAD = ["sensor", "binary_sensor", "text_sensor"]
MULTI_CONF = False

gira_rauchmelder_ns = cg.esphome_ns.namespace("gira_rauchmelder")
GiraRauchmelderComponent = gira_rauchmelder_ns.class_(
    "GiraRauchmelderComponent", cg.PollingComponent, uart.UARTDevice
)

CONF_GIRA_RAUCHMELDER_ID = "gira_rauchmelder_id"

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(GiraRauchmelderComponent),
        }
    )
    .extend(cv.polling_component_schema("60s"))
    .extend(uart.UART_DEVICE_SCHEMA)
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)
