import esphome.codegen as cg
import esphome.config_validation as cv
import esphome.final_validate as fv
from esphome.components import light, uart
from esphome.const import CONF_ID

AUTO_LOAD = ["uart"]
DEPENDENCIES = ["esp32", "logger", "light"]

ns = cg.esphome_ns.namespace("shs_test_serial")
SerialTestComponent = ns.class_("SerialTestComponent", cg.Component, uart.UARTComponent)


def validate_transport(config):
    full = fv.full_config.get()
    if full["logger"]["hardware_uart"] != "UART0" or full["logger"]["baud_rate"] != 115200:
        raise cv.Invalid("shs_test_serial requires logger UART0 at 115200 baud")
    if full["esp32"]["framework"]["type"] != "esp-idf":
        raise cv.Invalid("shs_test_serial requires ESP-IDF")
    if "improv_serial" in full and "improv_serial_id" not in config:
        raise cv.Invalid("Connect improv_serial_id so only one reader owns logger UART0")
    if "improv_serial" in full and "uart_id" in full["improv_serial"]:
        raise cv.Invalid("Do not combine shs_test_serial with a separate Improv uart_id")


# Importing the type does not auto-load or instantiate Improv on Ethernet.
from esphome.components.improv_serial import ImprovSerialComponent

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(SerialTestComponent),
    cv.Required("light_id"): cv.use_id(light.LightState),
    cv.Optional("improv_serial_id"): cv.use_id(ImprovSerialComponent),
    cv.Optional("busy", default="return false;"): cv.returning_lambda,
}).extend(cv.COMPONENT_SCHEMA)

FINAL_VALIDATE_SCHEMA = validate_transport


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    cg.add(var.set_light(await cg.get_variable(config["light_id"])))
    busy = await cg.process_lambda(config["busy"], [], return_type=cg.bool_)
    cg.add(var.set_busy(busy))
    if "improv_serial_id" in config:
        improv = await cg.get_variable(config["improv_serial_id"])
        # Use ESPHome's public UART transport hook: this component owns input
        # and forwards complete Improv frames, keeping provisioning intact.
        cg.add_define("USE_IMPROV_SERIAL_UART")
        cg.add(improv.set_uart(var))
        cg.add(var.set_forward_improv(True))
