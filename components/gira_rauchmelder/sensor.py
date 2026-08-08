import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    DEVICE_CLASS_DURATION,
    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_VOLTAGE,
    STATE_CLASS_MEASUREMENT,
    STATE_CLASS_TOTAL_INCREASING,
    UNIT_CELSIUS,
    UNIT_HOUR,
    UNIT_VOLT,
)

from . import CONF_GIRA_RAUCHMELDER_ID, GiraRauchmelderComponent

DEPENDENCIES = ["gira_rauchmelder"]

CONF_BATTERY_VOLTAGE = "battery_voltage"
CONF_TEMPERATURE_1 = "temperature_1"
CONF_TEMPERATURE_2 = "temperature_2"
CONF_SMOKE_CHAMBER_VALUE = "smoke_chamber_value"
CONF_SMOKE_CHAMBER_POLLUTION = "smoke_chamber_pollution"
CONF_SMOKE_CHAMBER_ALARM_COUNT = "smoke_chamber_alarm_count"
CONF_UPTIME_HOURS = "uptime_hours"
CONF_ALARM_COUNT_LOCAL_TEMP = "alarm_count_local_temp"
CONF_ALARM_COUNT_LOCAL_TEST = "alarm_count_local_test"
CONF_ALARM_COUNT_WIRED = "alarm_count_wired"
CONF_ALARM_COUNT_WIRELESS = "alarm_count_wireless"
CONF_REMOTE_TEST_COUNT_WIRED = "remote_test_count_wired"
CONF_REMOTE_TEST_COUNT_WIRELESS = "remote_test_count_wireless"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_GIRA_RAUCHMELDER_ID): cv.use_id(GiraRauchmelderComponent),
        cv.Optional(CONF_BATTERY_VOLTAGE): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_TEMPERATURE_1): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_TEMPERATURE_2): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_SMOKE_CHAMBER_VALUE): sensor.sensor_schema(
            accuracy_decimals=4, state_class=STATE_CLASS_MEASUREMENT
        ),
        cv.Optional(CONF_SMOKE_CHAMBER_POLLUTION): sensor.sensor_schema(
            state_class=STATE_CLASS_MEASUREMENT
        ),
        cv.Optional(CONF_SMOKE_CHAMBER_ALARM_COUNT): sensor.sensor_schema(
            state_class=STATE_CLASS_TOTAL_INCREASING
        ),
        cv.Optional(CONF_UPTIME_HOURS): sensor.sensor_schema(
            unit_of_measurement=UNIT_HOUR,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_DURATION,
            state_class=STATE_CLASS_TOTAL_INCREASING,
        ),
        cv.Optional(CONF_ALARM_COUNT_LOCAL_TEMP): sensor.sensor_schema(
            state_class=STATE_CLASS_TOTAL_INCREASING
        ),
        cv.Optional(CONF_ALARM_COUNT_LOCAL_TEST): sensor.sensor_schema(
            state_class=STATE_CLASS_TOTAL_INCREASING
        ),
        cv.Optional(CONF_ALARM_COUNT_WIRED): sensor.sensor_schema(
            state_class=STATE_CLASS_TOTAL_INCREASING
        ),
        cv.Optional(CONF_ALARM_COUNT_WIRELESS): sensor.sensor_schema(
            state_class=STATE_CLASS_TOTAL_INCREASING
        ),
        cv.Optional(CONF_REMOTE_TEST_COUNT_WIRED): sensor.sensor_schema(
            state_class=STATE_CLASS_TOTAL_INCREASING
        ),
        cv.Optional(CONF_REMOTE_TEST_COUNT_WIRELESS): sensor.sensor_schema(
            state_class=STATE_CLASS_TOTAL_INCREASING
        ),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_GIRA_RAUCHMELDER_ID])

    if CONF_BATTERY_VOLTAGE in config:
        sens = await sensor.new_sensor(config[CONF_BATTERY_VOLTAGE])
        cg.add(hub.set_battery_voltage_sensor(sens))
    if CONF_TEMPERATURE_1 in config:
        sens = await sensor.new_sensor(config[CONF_TEMPERATURE_1])
        cg.add(hub.set_temperature1_sensor(sens))
    if CONF_TEMPERATURE_2 in config:
        sens = await sensor.new_sensor(config[CONF_TEMPERATURE_2])
        cg.add(hub.set_temperature2_sensor(sens))
    if CONF_SMOKE_CHAMBER_VALUE in config:
        sens = await sensor.new_sensor(config[CONF_SMOKE_CHAMBER_VALUE])
        cg.add(hub.set_smoke_chamber_value_sensor(sens))
    if CONF_SMOKE_CHAMBER_POLLUTION in config:
        sens = await sensor.new_sensor(config[CONF_SMOKE_CHAMBER_POLLUTION])
        cg.add(hub.set_smoke_chamber_pollution_sensor(sens))
    if CONF_SMOKE_CHAMBER_ALARM_COUNT in config:
        sens = await sensor.new_sensor(config[CONF_SMOKE_CHAMBER_ALARM_COUNT])
        cg.add(hub.set_smoke_chamber_alarm_count_sensor(sens))
    if CONF_UPTIME_HOURS in config:
        sens = await sensor.new_sensor(config[CONF_UPTIME_HOURS])
        cg.add(hub.set_uptime_hours_sensor(sens))
    if CONF_ALARM_COUNT_LOCAL_TEMP in config:
        sens = await sensor.new_sensor(config[CONF_ALARM_COUNT_LOCAL_TEMP])
        cg.add(hub.set_alarm_count_local_temp_sensor(sens))
    if CONF_ALARM_COUNT_LOCAL_TEST in config:
        sens = await sensor.new_sensor(config[CONF_ALARM_COUNT_LOCAL_TEST])
        cg.add(hub.set_alarm_count_local_test_sensor(sens))
    if CONF_ALARM_COUNT_WIRED in config:
        sens = await sensor.new_sensor(config[CONF_ALARM_COUNT_WIRED])
        cg.add(hub.set_alarm_count_wired_sensor(sens))
    if CONF_ALARM_COUNT_WIRELESS in config:
        sens = await sensor.new_sensor(config[CONF_ALARM_COUNT_WIRELESS])
        cg.add(hub.set_alarm_count_wireless_sensor(sens))
    if CONF_REMOTE_TEST_COUNT_WIRED in config:
        sens = await sensor.new_sensor(config[CONF_REMOTE_TEST_COUNT_WIRED])
        cg.add(hub.set_remote_test_count_wired_sensor(sens))
    if CONF_REMOTE_TEST_COUNT_WIRELESS in config:
        sens = await sensor.new_sensor(config[CONF_REMOTE_TEST_COUNT_WIRELESS])
        cg.add(hub.set_remote_test_count_wireless_sensor(sens))
