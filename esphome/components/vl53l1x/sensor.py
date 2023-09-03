import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, sensor
from esphome.const import (
    CONF_ID,
    CONF_DISTANCE,
    CONF_STATUS,
    UNIT_CENTIMETER,
    UNIT_EMPTY,
    DEVICE_CLASS_DISTANCE,
    DEVICE_CLASS_EMPTY,
    STATE_CLASS_MEASUREMENT,
)

CODEOWNERS = ["@mrtoy-me"]
DEPENDENCIES = ["i2c"]

vl53l1x_ns = cg.esphome_ns.namespace("vl53l1x")
DistanceMode = vl53l1x_ns.enum("DistanceMode")
VL53L1XComponent = vl53l1x_ns.class_(
    "VL53L1XComponent", cg.PollingComponent, i2c.I2CDevice, sensor.Sensor
)

DISTANCE_MODES = {
    "short": DistanceMode.SHORT,
    "long": DistanceMode.LONG, 
}

CONF_DISTANCE_MODE = "distance_mode"
CONF_RANGE_STATUS = "range_status"
UNIT_MILLIMETER ="mm"

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(VL53L1XComponent),
            cv.Optional(CONF_DISTANCE_MODE, default="long"): cv.enum(
                DISTANCE_MODES, upper=False
            ),
            cv.Required(CONF_DISTANCE): sensor.sensor_schema(
                unit_of_measurement=UNIT_MILLIMETER,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_DISTANCE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Required(CONF_RANGE_STATUS): sensor.sensor_schema(
                unit_of_measurement=UNIT_EMPTY,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_EMPTY,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
        }
    )
    .extend(cv.polling_component_schema("60s"))
    .extend(i2c.i2c_device_schema(0x29))
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    if CONF_DISTANCE in config:
        sens = await sensor.new_sensor(config[CONF_DISTANCE])
        cg.add(var.set_distance_sensor(sens))
    if CONF_RANGE_STATUS in config:    
        sens = await sensor.new_sensor(config[CONF_RANGE_STATUS])
        cg.add(var.set_range_status_sensor(sens)) 

    cg.add(var.config_distance_mode(config[CONF_DISTANCE_MODE]))