import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import (
    DEVICE_CLASS_BATTERY,
    DEVICE_CLASS_POWER,
    DEVICE_CLASS_PROBLEM,
    DEVICE_CLASS_SMOKE,
)

from . import CONF_GIRA_RAUCHMELDER_ID, GiraRauchmelderComponent

DEPENDENCIES = ["gira_rauchmelder"]

CONF_BATTERY_LOW = "battery_low"
CONF_BATTERY_POWERED = "battery_powered"
CONF_BUTTON_PRESSED = "button_pressed"
CONF_DEFECT_ACTIVE = "defect_active"
CONF_DEFECT_TEMP1 = "defect_temp1"
CONF_DEFECT_TEMP2 = "defect_temp2"
CONF_SMOKE_ALARM = "smoke_alarm"
CONF_WIRED_ALARM = "wired_alarm"
CONF_WIRELESS_ALARM = "wireless_alarm"
CONF_LOCAL_TEST_ALARM = "local_test_alarm"
CONF_WIRED_TEST_ALARM = "wired_test_alarm"
CONF_WIRELESS_TEST_ALARM = "wireless_test_alarm"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_GIRA_RAUCHMELDER_ID): cv.use_id(GiraRauchmelderComponent),
        cv.Optional(CONF_BATTERY_LOW): binary_sensor.binary_sensor_schema(
            device_class=DEVICE_CLASS_BATTERY
        ),
        cv.Optional(CONF_BATTERY_POWERED): binary_sensor.binary_sensor_schema(
            device_class=DEVICE_CLASS_POWER
        ),
        cv.Optional(CONF_BUTTON_PRESSED): binary_sensor.binary_sensor_schema(),
        cv.Optional(CONF_DEFECT_ACTIVE): binary_sensor.binary_sensor_schema(
            device_class=DEVICE_CLASS_PROBLEM
        ),
        cv.Optional(CONF_DEFECT_TEMP1): binary_sensor.binary_sensor_schema(
            device_class=DEVICE_CLASS_PROBLEM
        ),
        cv.Optional(CONF_DEFECT_TEMP2): binary_sensor.binary_sensor_schema(
            device_class=DEVICE_CLASS_PROBLEM
        ),
        cv.Optional(CONF_SMOKE_ALARM): binary_sensor.binary_sensor_schema(
            device_class=DEVICE_CLASS_SMOKE
        ),
        cv.Optional(CONF_WIRED_ALARM): binary_sensor.binary_sensor_schema(
            device_class=DEVICE_CLASS_SMOKE
        ),
        cv.Optional(CONF_WIRELESS_ALARM): binary_sensor.binary_sensor_schema(
            device_class=DEVICE_CLASS_SMOKE
        ),
        cv.Optional(CONF_LOCAL_TEST_ALARM): binary_sensor.binary_sensor_schema(),
        cv.Optional(CONF_WIRED_TEST_ALARM): binary_sensor.binary_sensor_schema(),
        cv.Optional(CONF_WIRELESS_TEST_ALARM): binary_sensor.binary_sensor_schema(),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_GIRA_RAUCHMELDER_ID])

    if CONF_BATTERY_LOW in config:
        bs = await binary_sensor.new_binary_sensor(config[CONF_BATTERY_LOW])
        cg.add(hub.set_battery_low_binary_sensor(bs))
    if CONF_BATTERY_POWERED in config:
        bs = await binary_sensor.new_binary_sensor(config[CONF_BATTERY_POWERED])
        cg.add(hub.set_battery_powered_binary_sensor(bs))
    if CONF_BUTTON_PRESSED in config:
        bs = await binary_sensor.new_binary_sensor(config[CONF_BUTTON_PRESSED])
        cg.add(hub.set_button_pressed_binary_sensor(bs))
    if CONF_DEFECT_ACTIVE in config:
        bs = await binary_sensor.new_binary_sensor(config[CONF_DEFECT_ACTIVE])
        cg.add(hub.set_defect_active_binary_sensor(bs))
    if CONF_DEFECT_TEMP1 in config:
        bs = await binary_sensor.new_binary_sensor(config[CONF_DEFECT_TEMP1])
        cg.add(hub.set_defect_temp1_binary_sensor(bs))
    if CONF_DEFECT_TEMP2 in config:
        bs = await binary_sensor.new_binary_sensor(config[CONF_DEFECT_TEMP2])
        cg.add(hub.set_defect_temp2_binary_sensor(bs))
    if CONF_SMOKE_ALARM in config:
        bs = await binary_sensor.new_binary_sensor(config[CONF_SMOKE_ALARM])
        cg.add(hub.set_smoke_alarm_binary_sensor(bs))
    if CONF_WIRED_ALARM in config:
        bs = await binary_sensor.new_binary_sensor(config[CONF_WIRED_ALARM])
        cg.add(hub.set_wired_alarm_binary_sensor(bs))
    if CONF_WIRELESS_ALARM in config:
        bs = await binary_sensor.new_binary_sensor(config[CONF_WIRELESS_ALARM])
        cg.add(hub.set_wireless_alarm_binary_sensor(bs))
    if CONF_LOCAL_TEST_ALARM in config:
        bs = await binary_sensor.new_binary_sensor(config[CONF_LOCAL_TEST_ALARM])
        cg.add(hub.set_local_test_alarm_binary_sensor(bs))
    if CONF_WIRED_TEST_ALARM in config:
        bs = await binary_sensor.new_binary_sensor(config[CONF_WIRED_TEST_ALARM])
        cg.add(hub.set_wired_test_alarm_binary_sensor(bs))
    if CONF_WIRELESS_TEST_ALARM in config:
        bs = await binary_sensor.new_binary_sensor(config[CONF_WIRELESS_TEST_ALARM])
        cg.add(hub.set_wireless_test_alarm_binary_sensor(bs))
