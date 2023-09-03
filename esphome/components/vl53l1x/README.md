
## ESPHome component for VL53L1X and VL53L4CD
STMicroelectronic VL53L1X ultra lite driver (STSW-IMG009), VL53L1 API Core
has been adapted and modified so it functions using the ESPHome component framework.
Calibration and ROI functions have not been implemented.<BR>

The STMicroelectronic VL53L1X ultra lite API Core is
Copyright (c) 2017, STMicroelectronics - All Rights Reserved.<BR>
VL53L1 Core may be distributed under the terms of 'BSD 3-clause "New" or "Revised" License'

**All Copyright licences are shown in vl53l1x.cpp and licence.md files**

## Usage: VL53L1X component
Copy components files to a ***components*** directory under your homeassistant's esphome directory.<BR>
The following yaml can then be used so ESPHome accesses the component files:
```
external_components:
  - source: components
```
The component uses the sensor's default i2c address of 0x29.<BR>
Timing budget (measurement period) is set internally at 500ms. The sensor is operating by ranging continuously every 500ms, but measurements are published at the specified update interval. The update interval should be greater than 1 second.<BR>

YAML Configuration of ***distance_mode:*** can be either ***short*** or ***long***.<BR>
However, VL53L4CD sensor can only be short and if VL53L4CD is detected, distance mode is set to ***short***.<BR>

Two sensors must be configured ***distance:*** and ***range_status:***<BR>
Distance has units mm while range status gives the status code of the distance measurement.<BR>
The following range status descriptions are a summary of explanations provided in STMicroelectronic VL53L1X ultra lite driver, UM2510 user manual.<BR>
Range status values are as follows:<BR>

0 = RANGE VALID<BR>

1 = SIGMA FAIL WARNING<BR> 
(poor measurement repeatability or standard deviation)

2 = SIGNAL FAIL WARNING<BR> 
(return signal is too week to return a good measurement)

3 = OUT OF BOUNDS ERROR<BR> 
(generally occurs when target is at or more than sensor maximum distance)

4 = WRAP AROUND ERROR<BR> 
(occurs when the target is very reflective and the<BR> 
distance to the target is more than sensor maximum distance)<BR> 

5 = UNDEFINED<BR>

## Example YAML
```
external_components:
  - source: components

#example configure I2C
i2c:
  - id: bus_a 
    sda: 21
    scl: 22
    scan: true

sensor:
  - platform: vl53l1x
    i2c_id: bus_a
    distance_mode: long
    distance:
      name: "Distance"
    range_status:
      name: "Range Status"
    update_interval: 60s
```