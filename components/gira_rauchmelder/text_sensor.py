import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor

from . import CONF_GIRA_RAUCHMELDER_ID, GiraRauchmelderComponent

DEPENDENCIES = ["gira_rauchmelder"]

CONF_SERIAL_NUMBER = "serial_number"
CONF_RAW_STATUS_BYTE3 = "raw_status_byte3"
CONF_RAW_CMD08 = "raw_cmd08"
CONF_RAW_CMD0F = "raw_cmd0f"
CONF_RAW_CMD86 = "raw_cmd86"
CONF_LAST_UNHANDLED_FRAME = "last_unhandled_frame"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_GIRA_RAUCHMELDER_ID): cv.use_id(GiraRauchmelderComponent),
        cv.Optional(CONF_SERIAL_NUMBER): text_sensor.text_sensor_schema(),
        cv.Optional(CONF_RAW_STATUS_BYTE3): text_sensor.text_sensor_schema(),
        cv.Optional(CONF_RAW_CMD08): text_sensor.text_sensor_schema(),
        cv.Optional(CONF_RAW_CMD0F): text_sensor.text_sensor_schema(),
        cv.Optional(CONF_RAW_CMD86): text_sensor.text_sensor_schema(),
        cv.Optional(CONF_LAST_UNHANDLED_FRAME): text_sensor.text_sensor_schema(),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_GIRA_RAUCHMELDER_ID])

    if CONF_SERIAL_NUMBER in config:
        ts = await text_sensor.new_text_sensor(config[CONF_SERIAL_NUMBER])
        cg.add(hub.set_serial_number_text_sensor(ts))
    if CONF_RAW_STATUS_BYTE3 in config:
        ts = await text_sensor.new_text_sensor(config[CONF_RAW_STATUS_BYTE3])
        cg.add(hub.set_raw_status_byte3_text_sensor(ts))
    if CONF_RAW_CMD08 in config:
        ts = await text_sensor.new_text_sensor(config[CONF_RAW_CMD08])
        cg.add(hub.set_raw_cmd08_text_sensor(ts))
    if CONF_RAW_CMD0F in config:
        ts = await text_sensor.new_text_sensor(config[CONF_RAW_CMD0F])
        cg.add(hub.set_raw_cmd0f_text_sensor(ts))
    if CONF_RAW_CMD86 in config:
        ts = await text_sensor.new_text_sensor(config[CONF_RAW_CMD86])
        cg.add(hub.set_raw_cmd86_text_sensor(ts))
    if CONF_LAST_UNHANDLED_FRAME in config:
        ts = await text_sensor.new_text_sensor(config[CONF_LAST_UNHANDLED_FRAME])
        cg.add(hub.set_last_unhandled_frame_text_sensor(ts))
