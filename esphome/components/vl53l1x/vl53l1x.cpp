
/*
STMicroelectronic VL53L1X ultra lite driver (STSW-IMG009), VL53L1 API Core
has been adapted and modified so it functions using the ESPHome component framework.

The STMicroelectronic VL53L1X ultra lite API Core
is subject to the following copyright.
*/

/*
 Copyright (c) 2017, STMicroelectronics - All Rights Reserved

*******************************************************************************

 License terms: BSD 3-clause "New" or "Revised" License.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.

 3. Neither the name of the copyright holder nor the names of its contributors
 may be used to endorse or promote products derived from this software
 without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************
*/

/*
Development of this ESPHome also referenced/used the VL53L1X Distance sensor library
by SparkFun Electronics which has the following copyright.

==== MIT License ====
Copyright © 2022 SparkFun Electronics
Permission is hereby granted, free of charge, to any person obtaining a copy of this
software and associated documentation files (the “Software”), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
=====================
*/


#include "vl53l1x.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace vl53l1x {
static const char *const TAG = "vl53l1x.sensor";

static const uint16_t SOFT_RESET											                                    = 0x0000;
static const uint16_t VL53L1_I2C_SLAVE__DEVICE_ADDRESS					                          = 0x0001;
static const uint16_t VL53L1_VHV_CONFIG__TIMEOUT_MACROP_LOOP_BOUND                        = 0x0008;
static const uint16_t ALGO__CROSSTALK_COMPENSATION_PLANE_OFFSET_KCPS 		                  = 0x0016;
static const uint16_t ALGO__CROSSTALK_COMPENSATION_X_PLANE_GRADIENT_KCPS 	                = 0x0018;
static const uint16_t ALGO__CROSSTALK_COMPENSATION_Y_PLANE_GRADIENT_KCPS 	                = 0x001A;
static const uint16_t ALGO__PART_TO_PART_RANGE_OFFSET_MM					                        = 0x001E;
static const uint16_t MM_CONFIG__INNER_OFFSET_MM							                            = 0x0020;
static const uint16_t MM_CONFIG__OUTER_OFFSET_MM 							                            = 0x0022;
static const uint16_t GPIO_HV_MUX__CTRL									                                  = 0x0030;
static const uint16_t GPIO__TIO_HV_STATUS       							                            = 0x0031;
static const uint16_t SYSTEM__INTERRUPT_CONFIG_GPIO 						                          = 0x0046;
static const uint16_t PHASECAL_CONFIG__TIMEOUT_MACROP     				                        = 0x004B;
static const uint16_t RANGE_CONFIG__TIMEOUT_MACROP_A_HI   				                        = 0x005E;
static const uint16_t RANGE_CONFIG__VCSEL_PERIOD_A        				                        = 0x0060;
static const uint16_t RANGE_CONFIG__VCSEL_PERIOD_B						                            = 0x0063;
static const uint16_t RANGE_CONFIG__TIMEOUT_MACROP_B_HI  					                        = 0x0061;
static const uint16_t RANGE_CONFIG__TIMEOUT_MACROP_B_LO  					                        = 0x0062;
static const uint16_t RANGE_CONFIG__SIGMA_THRESH 							                            = 0x0064;
static const uint16_t RANGE_CONFIG__MIN_COUNT_RATE_RTN_LIMIT_MCPS			                    = 0x0066;
static const uint16_t RANGE_CONFIG__VALID_PHASE_HIGH      				                        = 0x0069;
static const uint16_t VL53L1_SYSTEM__INTERMEASUREMENT_PERIOD				                      = 0x006C;
static const uint16_t SYSTEM__THRESH_HIGH 								                                = 0x0072;
static const uint16_t SYSTEM__THRESH_LOW 									                                = 0x0074;
static const uint16_t SD_CONFIG__WOI_SD0                  				                        = 0x0078;
static const uint16_t SD_CONFIG__INITIAL_PHASE_SD0        				                        = 0x007A;
static const uint16_t ROI_CONFIG__USER_ROI_CENTRE_SPAD					                          = 0x007F;
static const uint16_t ROI_CONFIG__USER_ROI_REQUESTED_GLOBAL_XY_SIZE		                    = 0x0080;
static const uint16_t SYSTEM__SEQUENCE_CONFIG								                              = 0x0081;
static const uint16_t VL53L1_SYSTEM__GROUPED_PARAMETER_HOLD 				                      = 0x0082;
static const uint16_t SYSTEM__INTERRUPT_CLEAR       						                          = 0x0086;
static const uint16_t SYSTEM__MODE_START                 					                        = 0x0087;
static const uint16_t VL53L1_RESULT__RANGE_STATUS							                            = 0x0089;
static const uint16_t VL53L1_RESULT__DSS_ACTUAL_EFFECTIVE_SPADS_SD0		                    = 0x008C;
static const uint16_t RESULT__AMBIENT_COUNT_RATE_MCPS_SD					                        = 0x0090;
static const uint16_t VL53L1_RESULT__FINAL_CROSSTALK_CORRECTED_RANGE_MM_SD0				        = 0x0096;
static const uint16_t VL53L1_RESULT__PEAK_SIGNAL_COUNT_RATE_CROSSTALK_CORRECTED_MCPS_SD0 	= 0x0098;
static const uint16_t VL53L1_RESULT__OSC_CALIBRATE_VAL					                          = 0x00DE;
static const uint16_t VL53L1_FIRMWARE__SYSTEM_STATUS                                      = 0x00E5;
static const uint16_t VL53L1_IDENTIFICATION__MODEL_ID                                     = 0x010F;
static const uint16_t VL53L1_ROI_CONFIG__MODE_ROI_CENTRE_SPAD				                      = 0x013E;

static const uint8_t VL51L1X_DEFAULT_CONFIGURATION[] = {
	0x00, /* 0x2d : set bit 2 and 5 to 1 for fast plus mode (1MHz I2C), else don't touch */
	0x01, /* 0x2e : bit 0 if I2C pulled up at 1.8V, else set bit 0 to 1 (pull up at AVDD) */
	0x01, /* 0x2f : bit 0 if GPIO pulled up at 1.8V, else set bit 0 to 1 (pull up at AVDD) */
	0x01, /* 0x30 : set bit 4 to 0 for active high interrupt and 1 for active low (bits 3:0 must be 0x1), use SetInterruptPolarity() */
	0x02, /* 0x31 : bit 1 = interrupt depending on the polarity, use CheckForDataReady() */
	0x00, /* 0x32 : not user-modifiable */
	0x02, /* 0x33 : not user-modifiable */
	0x08, /* 0x34 : not user-modifiable */
	0x00, /* 0x35 : not user-modifiable */
	0x08, /* 0x36 : not user-modifiable */
	0x10, /* 0x37 : not user-modifiable */
	0x01, /* 0x38 : not user-modifiable */
	0x01, /* 0x39 : not user-modifiable */
	0x00, /* 0x3a : not user-modifiable */
	0x00, /* 0x3b : not user-modifiable */
	0x00, /* 0x3c : not user-modifiable */
	0x00, /* 0x3d : not user-modifiable */
	0xff, /* 0x3e : not user-modifiable */
	0x00, /* 0x3f : not user-modifiable */
	0x0F, /* 0x40 : not user-modifiable */
	0x00, /* 0x41 : not user-modifiable */
	0x00, /* 0x42 : not user-modifiable */
	0x00, /* 0x43 : not user-modifiable */
	0x00, /* 0x44 : not user-modifiable */
	0x00, /* 0x45 : not user-modifiable */
	0x20, /* 0x46 : interrupt configuration 0->level low detection, 1-> level high, 2-> Out of window, 3->In window, 0x20-> New sample ready , TBC */
	0x0b, /* 0x47 : not user-modifiable */
	0x00, /* 0x48 : not user-modifiable */
	0x00, /* 0x49 : not user-modifiable */
	0x02, /* 0x4a : not user-modifiable */
	0x0a, /* 0x4b : not user-modifiable */
	0x21, /* 0x4c : not user-modifiable */
	0x00, /* 0x4d : not user-modifiable */
	0x00, /* 0x4e : not user-modifiable */
	0x05, /* 0x4f : not user-modifiable */
	0x00, /* 0x50 : not user-modifiable */
	0x00, /* 0x51 : not user-modifiable */
	0x00, /* 0x52 : not user-modifiable */
	0x00, /* 0x53 : not user-modifiable */
	0xc8, /* 0x54 : not user-modifiable */
	0x00, /* 0x55 : not user-modifiable */
	0x00, /* 0x56 : not user-modifiable */
	0x38, /* 0x57 : not user-modifiable */
	0xff, /* 0x58 : not user-modifiable */
	0x01, /* 0x59 : not user-modifiable */
	0x00, /* 0x5a : not user-modifiable */
	0x08, /* 0x5b : not user-modifiable */
	0x00, /* 0x5c : not user-modifiable */
	0x00, /* 0x5d : not user-modifiable */
	0x01, /* 0x5e : not user-modifiable */
	0xdb, /* 0x5f : not user-modifiable */
	0x0f, /* 0x60 : not user-modifiable */
	0x01, /* 0x61 : not user-modifiable */
	0xf1, /* 0x62 : not user-modifiable */
	0x0d, /* 0x63 : not user-modifiable */
	0x01, /* 0x64 : Sigma threshold MSB (mm in 14.2 format for MSB+LSB), use SetSigmaThreshold(), default value 90 mm  */
	0x68, /* 0x65 : Sigma threshold LSB */
	0x00, /* 0x66 : Min count Rate MSB (MCPS in 9.7 format for MSB+LSB), use SetSignalThreshold() */
	0x80, /* 0x67 : Min count Rate LSB */
	0x08, /* 0x68 : not user-modifiable */
	0xb8, /* 0x69 : not user-modifiable */
	0x00, /* 0x6a : not user-modifiable */
	0x00, /* 0x6b : not user-modifiable */
	0x00, /* 0x6c : Intermeasurement period MSB, 32 bits register, use SetIntermeasurementInMs() */
	0x00, /* 0x6d : Intermeasurement period */
	0x0f, /* 0x6e : Intermeasurement period */
	0x89, /* 0x6f : Intermeasurement period LSB */
	0x00, /* 0x70 : not user-modifiable */
	0x00, /* 0x71 : not user-modifiable */
	0x00, /* 0x72 : distance threshold high MSB (in mm, MSB+LSB), use SetD:tanceThreshold() */
	0x00, /* 0x73 : distance threshold high LSB */
	0x00, /* 0x74 : distance threshold low MSB ( in mm, MSB+LSB), use SetD:tanceThreshold() */
	0x00, /* 0x75 : distance threshold low LSB */
	0x00, /* 0x76 : not user-modifiable */
	0x01, /* 0x77 : not user-modifiable */
	0x0f, /* 0x78 : not user-modifiable */
	0x0d, /* 0x79 : not user-modifiable */
	0x0e, /* 0x7a : not user-modifiable */
	0x0e, /* 0x7b : not user-modifiable */
	0x00, /* 0x7c : not user-modifiable */
	0x00, /* 0x7d : not user-modifiable */
	0x02, /* 0x7e : not user-modifiable */
	0xc7, /* 0x7f : ROI center, use SetROI() */
	0xff, /* 0x80 : XY ROI (X=Width, Y=Height), use SetROI() */
	0x9B, /* 0x81 : not user-modifiable */
	0x00, /* 0x82 : not user-modifiable */
	0x00, /* 0x83 : not user-modifiable */
	0x00, /* 0x84 : not user-modifiable */
	0x01, /* 0x85 : not user-modifiable */
	0x00, /* 0x86 : clear interrupt, use ClearInterrupt() */
	0x00  /* 0x87 : start ranging, use StartRanging() or StopRanging(), If you want an automatic start after VL53L1X_init() call, put 0x40 in location 0x87 */
};

// NOTE: Corban has changed some timings here. More importantly, I've changed the loop time to get a faster response.

static const uint16_t INIT_TIMEOUT  = 250;  // default timing budget = 100ms, so 250ms should be more than enough time
static const uint16_t TIMING_BUDGET = 100;  // new timing budget is maximum allowable = 500 ms // CORBAN: 500 -> 100
static const uint16_t INTERMEASUREMENT_PERIOD = 120; // CORBAN
static const uint16_t LOOP_TIME =  200;  // loop executes every 50ms // CORBAN: was 1000? Why?

// Sensor Initialisation
void VL53L1XComponent::setup() {
  uint32_t start_time;
	uint8_t state = 0;
	uint16_t addr;
	bool is_dataready;

  start_time = millis();
	while ((millis() - start_time) < INIT_TIMEOUT ) {
    if (!this->boot_state(&state)) {
			this->error_code_ = COMMUNICATION_FAILED;
			this->mark_failed();
		  return;
	  }
		if (state) break;
	}

	if (!state) {
		this->error_code_ = BOOT_TIMEOUT;
    this->mark_failed();
		return;
	}

	for (addr = 0x002D; addr <= 0x0087; addr++) {
		if (!this->vl53l1x_write_byte(addr,VL51L1X_DEFAULT_CONFIGURATION[addr - 0x002D])) {
		  ESP_LOGE(TAG, "Error writing default configuration: address = 0x%X", addr);
			this->error_code_ = COMMUNICATION_FAILED;
      this->mark_failed();
      return;
		}
	}

  if (!this->check_sensor_id()) {
      this->error_code_ = WRONG_CHIP_ID;
      this->mark_failed();
      return;
	}

	// 0xEBAA = VL53L4CD must run with SHORT distance mode
  if ((this->sensor_id_ == 0xEBAA) && (distance_mode_ == LONG)) {
		this->distance_mode_ = SHORT;
		this->distance_mode_overriden_ = true;
	}

	// kick off initialisation by starting ranging
	if (!this->start_ranging()) {
		this->error_code_ = COMMUNICATION_FAILED;
	  this->mark_failed();
	  return;
  }

  start_time = millis();
	while ((millis() - start_time) < INIT_TIMEOUT ) {
		// ranging started now wait for data ready
    if (!this->check_for_dataready(&is_dataready)) {
			this->error_code_ = COMMUNICATION_FAILED;
			this->mark_failed();
		  return;
	  }
		if (is_dataready) break;
	}

	if (!is_dataready) {
    this->error_code_ = DATAREADY_TIMEOUT;
    this->mark_failed();
		return;
	}

	if (!this->clear_interrupt()) {
		this->error_code_ = COMMUNICATION_FAILED;
		this->mark_failed();
		return;
	}

	if (!this->stop_ranging()) {
		this->error_code_ = COMMUNICATION_FAILED;
	  this->mark_failed();
	  return;
	}

	if (!this->write_byte(VL53L1_VHV_CONFIG__TIMEOUT_MACROP_LOOP_BOUND, 0x09)) {
		ESP_LOGW(TAG, "Error writing Config Timeout Macro");
		this->error_code_ = COMMUNICATION_FAILED;
    this->mark_failed();
    return;
	}
	if (!this->write_byte(0x0B, 0))  {
		ESP_LOGW(TAG, "Error writing Start VHV from the Previous Temperature");
		this->error_code_ = COMMUNICATION_FAILED;
    this->mark_failed();
    return;
  }

// CORBAN: Commented
	if (!this->set_timing_budget(TIMING_BUDGET)) {
		this->error_code_ = COMMUNICATION_FAILED;
		this->mark_failed();
		return;
	}

  if (!this->set_intermeasurement_period(INTERMEASUREMENT_PERIOD)) { // CORBAN: Changed from timing budget
		this->error_code_ = COMMUNICATION_FAILED;
		this->mark_failed();
		return;
	}

	if (!this->set_distance_mode(distance_mode_)) {
		this->error_code_ = COMMUNICATION_FAILED;
		this->mark_failed();
		return;
	}

	if (!this->start_ranging()) {
		this->error_code_ = COMMUNICATION_FAILED;
	  this->mark_failed();
	  return;
  }
}

void VL53L1XComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "VL53L1X:");
	if (this->sensor_id_ == 0xEACC)
			ESP_LOGI(TAG,"  Sensor ID: VL53L1X");
		else {
		  ESP_LOGI(TAG,"  Sensor ID: VL53L4CD");
	    if (this->distance_mode_overriden_)
			  ESP_LOGW(TAG,"  VL53L4CD Distance Mode overriden: must be SHORT");
		}

  switch (this->error_code_) {
    case COMMUNICATION_FAILED:
      ESP_LOGE(TAG, "  Sensor communication failed");
      break;
    case WRONG_CHIP_ID:
      ESP_LOGE(TAG, "  Sensor does not have VL53L1X or VL53L4CD sensor ID!");
      break;
    case BOOT_TIMEOUT:
      ESP_LOGE(TAG, "  Timeout on waiting for sensor to boot");
      break;
		case DATAREADY_TIMEOUT:
      ESP_LOGE(TAG, "  Timeout on waiting for data ready");
      break;
    case NONE:
      ESP_LOGI(TAG, "  Setup successful");
			if (this->distance_mode_ == SHORT)
	      ESP_LOGD(TAG,"  Distance Mode: SHORT");
	    else
	      ESP_LOGD(TAG,"  Distance Mode: LONG");

			ESP_LOGD(TAG,"  Timing Budget: %ims",TIMING_BUDGET);
      ESP_LOGD(TAG,"  Intermediate Period: %ims",TIMING_BUDGET);
      break;
   }
   LOG_I2C_DEVICE(this);
   LOG_UPDATE_INTERVAL(this);
}

void VL53L1XComponent::loop() {
	bool is_dataready;
  // only run loop if not updating and every LOOP_TIME
	if (this->running_update_ || ((millis() - this->last_loop_time_) < LOOP_TIME) || this->is_failed() )
    return;

  if (!this->check_for_dataready(&is_dataready)) {
		return;
	}

	if (!is_dataready) {
		this->last_loop_time_ = millis();
		return;
	}

  // data ready now
  if (!this->get_distance(&this->distance_)) return;
	if (!this->clear_interrupt()) return;
	if (!this->stop_ranging()) return;
	if(!this->get_range_status()) return;

	if (!this->start_ranging()) {
		this->mark_failed();
		return;
	}
  this->last_loop_time_ = millis();
}

void VL53L1XComponent::update() {
  this->running_update_ = true;
	if ((this->distance_!= 0) && (this->range_status_ != UNDEFINED)) {
		if ((this->distance_sensor_ != nullptr) && (this->range_status_sensor_ != nullptr)) {
      this->distance_sensor_->publish_state(this->distance_);
      this->range_status_sensor_->publish_state(this->range_status_);
	  }
		else {
		  ESP_LOGV(TAG, "Range data found but unable to publish");
		}
	}
	else {
	  ESP_LOGV(TAG, "No Range data found to publish");
	}

	this->running_update_ = false;
}


bool VL53L1XComponent::clear_interrupt() {
	if (!this->vl53l1x_write_byte(SYSTEM__INTERRUPT_CLEAR, 0x01)) {
		ESP_LOGW(TAG, "Error writing Clear Interrupt");
    this->status_set_warning();
    return false;
  }
	return true;
}

bool VL53L1XComponent::get_interrupt_polarity(uint8_t *interrupt_polarity) {
	uint8_t temp;

	if (!this->vl53l1x_read_byte(GPIO_HV_MUX__CTRL, &temp)) {
    ESP_LOGW(TAG, "Error reading Interrupt Polarity");
    this->status_set_warning();
    return false;
	}
	temp = temp & 0x10;
	*interrupt_polarity = !(temp >> 4);
	return true;
}

bool VL53L1XComponent::start_ranging() {
	/* Clear interrupt trigger */
	if (!this->vl53l1x_write_byte(SYSTEM__INTERRUPT_CLEAR, 0x01))  {
		ESP_LOGW(TAG, "Error writing Clear Interrupt");
    this->status_set_warning();
    return false;
  }
	if (!this->vl53l1x_write_byte(SYSTEM__MODE_START, 0x40)) {
		ESP_LOGW(TAG, "Error writing Start Ranging");
    this->status_set_warning();
    return false;
  }
	return true;
}


bool VL53L1XComponent::start_oneshot_ranging() {
  /* Clear interrupt trigger */
	if (!this->vl53l1x_write_byte(SYSTEM__INTERRUPT_CLEAR, 0x01))  {
		ESP_LOGW(TAG, "Error writing Clear Interrupt");
    this->status_set_warning();
    return false;
  }
	/* Enable VL53L1X one-shot ranging */
	if (!this->vl53l1x_write_byte(SYSTEM__MODE_START, 0x10)) {
		ESP_LOGW(TAG, "Error writing Start Ranging");
    this->status_set_warning();
    return false;
  }
	return true;
}

bool VL53L1XComponent::stop_ranging() {
	/* Disable VL53L1X */
	if (!this->vl53l1x_write_byte(SYSTEM__MODE_START, 0x00)) {
		ESP_LOGW(TAG, "Error writing Start Ranging");
    this->status_set_warning();
    return false;
  }
	return true;
}

bool VL53L1XComponent::check_for_dataready(bool *is_dataready) {
	uint8_t temp;
	uint8_t int_polarity;

	if (!get_interrupt_polarity(&int_polarity)) return false;

	if (!this->vl53l1x_read_byte(GPIO__TIO_HV_STATUS, &temp)) {
    ESP_LOGW(TAG, "Error reading Data Ready");
    this->status_set_warning();
    return false;
	}

	*is_dataready =((temp & 1) == int_polarity);
	return true;
}

bool VL53L1XComponent::set_timing_budget(uint16_t timing_budget_ms) {
	bool ok;
  DistanceMode mode;

	if ( !get_distance_mode(&mode) ) return false;

	if (mode == SHORT) {
		switch (timing_budget_ms) {
		  case 15: // only available in short distance mode
			  ok = this->vl53l1x_write_byte_16(RANGE_CONFIG__TIMEOUT_MACROP_A_HI, 0x001D);
        if (!ok) break;
				ok = this->vl53l1x_write_byte_16(RANGE_CONFIG__TIMEOUT_MACROP_B_HI, 0x0027);
			  break;
		  case 20:
			  ok = this->vl53l1x_write_byte_16(RANGE_CONFIG__TIMEOUT_MACROP_A_HI, 0x0051);
				if (!ok) break;
			  ok = this->vl53l1x_write_byte_16(RANGE_CONFIG__TIMEOUT_MACROP_B_HI, 0x006E);
			  break;
		  case 33:
			  ok = this->vl53l1x_write_byte_16(RANGE_CONFIG__TIMEOUT_MACROP_A_HI, 0x00D6);
				if (!ok) break;
			  ok = this->vl53l1x_write_byte_16(RANGE_CONFIG__TIMEOUT_MACROP_B_HI, 0x006E);
			  break;
		  case 50:
			  ok = this->vl53l1x_write_byte_16(RANGE_CONFIG__TIMEOUT_MACROP_A_HI, 0x01AE);
				if (!ok) break;
			  ok = this->vl53l1x_write_byte_16(RANGE_CONFIG__TIMEOUT_MACROP_B_HI, 0x01E8);
			  break;
		  case 100:
			  ok = this->vl53l1x_write_byte_16(RANGE_CONFIG__TIMEOUT_MACROP_A_HI, 0x02E1);
				if (!ok) break;
			  ok = this->vl53l1x_write_byte_16(RANGE_CONFIG__TIMEOUT_MACROP_B_HI, 0x0388);
			  break;
		  case 200:
			  ok = this->vl53l1x_write_byte_16(RANGE_CONFIG__TIMEOUT_MACROP_A_HI, 0x03E1);
				if (!ok) break;
			  ok = this->vl53l1x_write_byte_16(RANGE_CONFIG__TIMEOUT_MACROP_B_HI, 0x0496);
			  break;
		  case 500:
			  ok = this->vl53l1x_write_byte_16(RANGE_CONFIG__TIMEOUT_MACROP_A_HI, 0x0591);
				if (!ok) break;
			  ok = this->vl53l1x_write_byte_16(RANGE_CONFIG__TIMEOUT_MACROP_B_HI, 0x05C1);
			  break;
		  default:
			  ESP_LOGD(TAG,"Invalid set timing budget ms value = %i", timing_budget_ms);
				this->status_set_warning();
			  return false;
		}
		if (!ok) {
			ESP_LOGW(TAG, "Error writing Set Time Budget values");
      this->status_set_warning();
      return false;
		}
	}
	else {
		switch (timing_budget_ms) {
		  case 20:
			  ok = this->vl53l1x_write_byte_16(RANGE_CONFIG__TIMEOUT_MACROP_A_HI, 0x001E);
				if (!ok) break;
			  ok = this->vl53l1x_write_byte_16(RANGE_CONFIG__TIMEOUT_MACROP_B_HI, 0x0022);
			  break;
		  case 33:
			  ok = this->vl53l1x_write_byte_16(RANGE_CONFIG__TIMEOUT_MACROP_A_HI, 0x0060);
				if (!ok) break;
			  ok = this->vl53l1x_write_byte_16(RANGE_CONFIG__TIMEOUT_MACROP_B_HI, 0x006E);
			  break;
		  case 50:
			  ok = this->vl53l1x_write_byte_16(RANGE_CONFIG__TIMEOUT_MACROP_A_HI, 0x00AD);
				if (!ok) break;
			  ok = this->vl53l1x_write_byte_16(RANGE_CONFIG__TIMEOUT_MACROP_B_HI, 0x00C6);
			  break;
		  case 100:
			  ok = this->vl53l1x_write_byte_16(RANGE_CONFIG__TIMEOUT_MACROP_A_HI, 0x01CC);
				if (!ok) break;
			  ok = this->vl53l1x_write_byte_16(RANGE_CONFIG__TIMEOUT_MACROP_B_HI, 0x01EA);
			  break;
		  case 200:
			  ok = this->vl53l1x_write_byte_16(RANGE_CONFIG__TIMEOUT_MACROP_A_HI, 0x02D9);
				if (!ok) break;
			  ok = this->vl53l1x_write_byte_16(RANGE_CONFIG__TIMEOUT_MACROP_B_HI, 0x02F8);
			  break;
		  case 500:
			  ok = this->vl53l1x_write_byte_16(RANGE_CONFIG__TIMEOUT_MACROP_A_HI, 0x048F);
				if (!ok) break;
			  ok = this->vl53l1x_write_byte_16(RANGE_CONFIG__TIMEOUT_MACROP_B_HI, 0x04A4);
			  break;
		  default:
			  ESP_LOGD(TAG,"Invalid Set Timing Budget ms value = %i", timing_budget_ms);
				this->status_set_warning();
			  return false;
		}
		if (!ok) {
			ESP_LOGW(TAG, "Error writing Set Time Budget values");
      this->status_set_warning();
      return false;
		}
	}
	return true;
}

bool VL53L1XComponent::get_timing_budget(uint16_t *timing_budget_ms) {

	uint16_t raw_timing_budget;

  *timing_budget_ms = 0;

	if (!this->vl53l1x_read_byte_16(RANGE_CONFIG__TIMEOUT_MACROP_A_HI, &raw_timing_budget)) {
		ESP_LOGW(TAG, "Error reading Timing Budget");
    this->status_set_warning();
    return false;
  }
	switch (raw_timing_budget) {
	  case 0x001D:
		  *timing_budget_ms = 15;
		  break;
	  case 0x0051:
	  case 0x001E:
		  *timing_budget_ms = 20;
		  break;
	  case 0x00D6:
	  case 0x0060:
		  *timing_budget_ms = 33;
		  break;
	  case 0x01AE:
	  case 0x00AD:
		  *timing_budget_ms = 50;
		  break;
	  case 0x02E1:
	  case 0x01CC:
		  *timing_budget_ms = 100;
		  break;
	  case 0x03E1:
	  case 0x02D9:
		  *timing_budget_ms = 200;
		  break;
	  case 0x0591:
	  case 0x048F:
		  *timing_budget_ms = 500;
		  break;
	  default:
		  ESP_LOGD(TAG,"Invalid Get Timing Budget value: raw value = 0x%04X", raw_timing_budget);
			break;
	}
	return true;
}

bool VL53L1XComponent::get_distance_mode(DistanceMode *mode) {
	uint8_t raw_distance_mode;

  if (!this->vl53l1x_read_byte(PHASECAL_CONFIG__TIMEOUT_MACROP, &raw_distance_mode)) {
    ESP_LOGW(TAG, "Error reading Distance Mode");
    this->status_set_warning();
    return false;
  }

	if (raw_distance_mode == 0x14) {
		*mode = SHORT;
		return true;
	}

	if (raw_distance_mode == 0x0A) {
		*mode = LONG;
		return true;
	}

  // should never get here
	ESP_LOGW(TAG, "Get Distance Mode - invalid value");
  this->status_set_warning();
  return false;
}


bool VL53L1XComponent::set_distance_mode(DistanceMode distance_mode) {
  bool ok;
	uint16_t timing_budget;

	if (!this->get_timing_budget(&timing_budget)) return false;

	switch (distance_mode) {
	  case SHORT:
		  ok = vl53l1x_write_byte(PHASECAL_CONFIG__TIMEOUT_MACROP, 0x14);
			if (!ok) break;
		  ok = vl53l1x_write_byte(RANGE_CONFIG__VCSEL_PERIOD_A, 0x07);
		  if (!ok) break;
			ok = vl53l1x_write_byte(RANGE_CONFIG__VCSEL_PERIOD_B, 0x05);
		  if (!ok) break;
			ok = vl53l1x_write_byte(RANGE_CONFIG__VALID_PHASE_HIGH, 0x38);
		  if (!ok) break;
			ok = vl53l1x_write_byte_16(SD_CONFIG__WOI_SD0, 0x0705);
		  if (!ok) break;
			ok = vl53l1x_write_byte_16(SD_CONFIG__INITIAL_PHASE_SD0, 0x0606);
		  break;
	  case LONG:
		  ok = vl53l1x_write_byte(PHASECAL_CONFIG__TIMEOUT_MACROP, 0x0A);
		  if (!ok) break;
			ok = vl53l1x_write_byte(RANGE_CONFIG__VCSEL_PERIOD_A, 0x0F);
		  if (!ok) break;
			ok = vl53l1x_write_byte(RANGE_CONFIG__VCSEL_PERIOD_B, 0x0D);
		  if (!ok) break;
			ok = vl53l1x_write_byte(RANGE_CONFIG__VALID_PHASE_HIGH, 0xB8);
		  if (!ok) break;
			ok = vl53l1x_write_byte_16(SD_CONFIG__WOI_SD0, 0x0F0D);
		  if (!ok) break;
			ok = vl53l1x_write_byte_16(SD_CONFIG__INITIAL_PHASE_SD0, 0x0E0E);
		  break;
	  default:
		  // should never happen
		  ESP_LOGD(TAG,"Set Distance Mode - invalid distance mode");
			this->status_set_warning();
			return false;
	}
	if (!ok ) {
		ESP_LOGW(TAG, "Error writing distance mode setup values");
    this->status_set_warning();
    return false;
	}
	return true;
}

bool VL53L1XComponent::set_intermeasurement_period(uint16_t intermeasurement_ms) {

	uint16_t timing_budget_ms,clock_pll;
	uint32_t intermeasurement_period;

  if (!get_timing_budget(&timing_budget_ms)) return false;

	if (intermeasurement_ms < timing_budget_ms) {
		ESP_LOGW(TAG, "Set Intermeasurement ms < Timing Budget ms");
		ESP_LOGW(TAG, "OR Timing Budget not set before Intermeasurement Period");
    this->status_set_warning();
    return false;
	}

  if (!this->vl53l1x_read_byte_16(VL53L1_RESULT__OSC_CALIBRATE_VAL, &clock_pll)) {
		ESP_LOGW(TAG, "Error reading Intermeasurement period: OSC_CALIBRATE_VAL");
    this->status_set_warning();
    return false;
	}

	clock_pll = clock_pll & 0x3FF;
	//ESP_LOGD(TAG, "set imp clock_pll = %i", clock_pll);
	intermeasurement_period =
	  static_cast<uint32_t>(clock_pll * intermeasurement_ms * 1.075);
  //ESP_LOGD(TAG, "imp =  %i", intermeasurement_period);
	if (!this->vl53l1x_write_bytes_16(VL53L1_SYSTEM__INTERMEASUREMENT_PERIOD,
	             reinterpret_cast<const uint16_t *>(&intermeasurement_period), 2)) {
    ESP_LOGW(TAG, "Error writing Intermeasurement period");
    this->status_set_warning();
    return false;
	}
	return true;
}


bool VL53L1XComponent::get_intermeasurement_period(uint16_t *intermeasurement_ms) {
	uint16_t clock_pll;
	uint32_t tmp;

	if (!this->vl53l1x_read_bytes_16(VL53L1_SYSTEM__INTERMEASUREMENT_PERIOD,
	             reinterpret_cast<uint16_t *>(&tmp), 2)) {
		ESP_LOGW(TAG, "Error reading Intermeasurment Period");
    this->status_set_warning();
    return false;
	}

	*intermeasurement_ms = (uint16_t)tmp;

  if (!this->vl53l1x_read_byte_16(VL53L1_RESULT__OSC_CALIBRATE_VAL, &clock_pll)) {
		ESP_LOGW(TAG, "Error reading Intermeasurement period: OSC_CALIBRATE_VAL");
    this->status_set_warning();
    return false;
	}

	clock_pll = clock_pll & 0x3FF;
	//ESP_LOGD(TAG, "get imp clock_pll = %i", clock_pll);
	//ESP_LOGD(TAG, "raw imp =  %i", *intermeasurement_ms);
	*intermeasurement_ms = (uint16_t)(*intermeasurement_ms / (clock_pll * 1.065));
  //ESP_LOGD(TAG, "converted imp =  %i", *intermeasurement_ms);
	return true;
}

bool VL53L1XComponent::boot_state(uint8_t *state) {
	// state: 1 = booted, 0 = not booted
  if (!this->vl53l1x_read_byte(VL53L1_FIRMWARE__SYSTEM_STATUS, state)) {
    ESP_LOGW(TAG, "Error reading Boot State");
    this->status_set_warning();
    return false;
  }
	return true;
}


bool VL53L1XComponent::check_sensor_id() {
  if (!this->vl53l1x_read_byte_16(VL53L1_IDENTIFICATION__MODEL_ID, &this->sensor_id_)) {
		ESP_LOGW(TAG, "Error reading Device ID");
    this->status_set_warning();
    return false;
	}
	// 0xEACC = VL53L1X, 0xEBAA = VL53L4CD
	if ((this->sensor_id_ == 0xEACC) || (this->sensor_id_ == 0xEBAA)) {
		return true;
	}
	else {
		return false;
	}
}

bool VL53L1XComponent::get_distance(uint16_t *distance) {
  if (!this->vl53l1x_read_byte_16(VL53L1_RESULT__FINAL_CROSSTALK_CORRECTED_RANGE_MM_SD0,
	     distance)) {
		ESP_LOGW(TAG, "Error reading Distance");
    this->status_set_warning();
    return false;
	}
	return true;
}


// numeric range status codes do not entirely follow the ST API values
// enum RangeStatus (vl53l1x.h) defines the numeric range status codes and are named accordingly

bool VL53L1XComponent::get_range_status() {
	uint8_t status;

	if (!this->vl53l1x_read_byte(VL53L1_RESULT__RANGE_STATUS, &status)) {
    ESP_LOGW(TAG, "Error reading Range Status");
    this->status_set_warning();
    return false;
  }

	status = status & 0x1F;
	switch (status) {
    case 9:
		  this->range_status_ = RANGE_VALID;
		  break;
    case 6:
		  this->range_status_ = SIGMA_FAIL_WARNING;
		  break;
    case 4:
		  this->range_status_ = SIGNAL_FAIL_WARNING;
		  break;
    case 5:
		  this->range_status_ = OUT_OF_BOUNDS_FAIL;
		  break;
    case 7:
		  this->range_status_ = WRAP_AROUND_FAIL;
		  break;
	  default:
		  this->range_status_ = UNDEFINED;
		  break;
	}
	return true;
}

i2c::ErrorCode VL53L1XComponent::vl53l1x_write_register(uint16_t a_register, const uint8_t *data, size_t len) {
	uint8_t new_len = len+2;
  uint8_t buffer[new_len];
  buffer[0] = (uint8_t)(a_register >> 8);
  buffer[1] = (uint8_t)(a_register & 0xFF);
  for (uint8_t i = 0; i < len; i++) {
    buffer[i + 2] = data[i];
  }
  return this->write(buffer, new_len, true);
}

bool VL53L1XComponent::vl53l1x_write_bytes(uint16_t a_register, const uint8_t *data, uint8_t len) {
    return this->vl53l1x_write_register(a_register, data, len) == i2c::ERROR_OK;
}

bool VL53L1XComponent::vl53l1x_write_bytes_16(uint8_t a_register, const uint16_t *data, uint8_t len) {
	// we have to copy in order to be able to change byte order
  std::unique_ptr<uint16_t[]> temp{new uint16_t[len]};
  for (size_t i = 0; i < len; i++)
    temp[i] = i2c::htoi2cs(data[i]);
  return (this->vl53l1x_write_register(a_register, reinterpret_cast<const uint8_t *>(temp.get()), len * 2) == i2c::ERROR_OK);
}

bool VL53L1XComponent::vl53l1x_write_byte(uint16_t a_register, uint8_t data) {
    return this->vl53l1x_write_bytes(a_register, &data, 1);
}

bool VL53L1XComponent::vl53l1x_write_byte_16(uint16_t a_register, uint16_t data) {
  return this->vl53l1x_write_bytes_16(a_register, &data, 1);
}

i2c::ErrorCode VL53L1XComponent::vl53l1x_read_register(uint16_t a_register, uint8_t *data, size_t len) {
	i2c::ErrorCode error_code;
	bool no_error;
  uint8_t buffer[2];
  buffer[0] = (uint8_t)(a_register >> 8);
  buffer[1] = (uint8_t)(a_register & 0xFF);

  //commented out loop used in driver, but does not seem to be necessary for ESP32
  uint8_t maxAttempts = 5;
	for (uint8_t i = 0; i < maxAttempts; i++) {
    error_code = this->write(buffer, 2, false);
	  no_error = (error_code == i2c::ERROR_OK);
		if (no_error) {
			ESP_LOGV(TAG, "Read register write attempts = %i",i);
			break;
		}
	}

  if (no_error) {
		return this->read(data, len);
	}
	else {
	  return error_code;
	}
}

bool VL53L1XComponent::vl53l1x_read_bytes(uint16_t a_register, uint8_t *data, uint8_t len) {
    return this->vl53l1x_read_register(a_register, data, len) == i2c::ERROR_OK;
}

bool VL53L1XComponent::vl53l1x_read_byte(uint16_t a_register, uint8_t *data) {
    return this->vl53l1x_read_register(a_register, data, 1) == i2c::ERROR_OK;
}

bool VL53L1XComponent::vl53l1x_read_bytes_16(uint16_t a_register, uint16_t *data, uint8_t len) {
  if (this->vl53l1x_read_register(a_register, reinterpret_cast<uint8_t *>(data), len * 2) != i2c::ERROR_OK)
    return false;
  for (size_t i = 0; i < len; i++)
    data[i] = i2c::i2ctohs(data[i]);
  return true;
}

bool VL53L1XComponent::vl53l1x_read_byte_16(uint16_t a_register, uint16_t *data) {
	return this->vl53l1x_read_bytes_16(a_register, data, 1);
}


float VL53L1XComponent::get_setup_priority() const { return setup_priority::DATA; }

} // namespace VL53L1X
} // namespace esphome
