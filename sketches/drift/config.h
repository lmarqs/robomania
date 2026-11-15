#pragma once

#include <Arduino.h>

#if __has_include("local.h")
#include "local.h"
#endif

// ESP32 WROOM-32. None of these straps the boot mode, as GPIO0, 2, 12 and 15 do; 21 and 22 are the core's I2C pins.
const uint8_t LED_PIN = 2;
const uint8_t STEERING_PIN = 13;
const uint8_t ESC_PIN = 14;
const uint8_t I2C_SDA_PIN = 21;
const uint8_t I2C_SCL_PIN = 22;

// The servo's pulses at full left, centre and full right. A car's steering stops short of the servo's own travel, so
// these come from the car: too wide a range stalls the servo against the stops.
#ifndef DRIFT_STEERING_LEFT_US
#define DRIFT_STEERING_LEFT_US 1000
#endif
#ifndef DRIFT_STEERING_CENTER_US
#define DRIFT_STEERING_CENTER_US 1500
#endif
#ifndef DRIFT_STEERING_RIGHT_US
#define DRIFT_STEERING_RIGHT_US 2000
#endif

// The largest throttle the triggers reach, as a fraction of the ESC's range each way.
#ifndef DRIFT_THROTTLE_MAX
#define DRIFT_THROTTLE_MAX 0.5f
#endif
static_assert(DRIFT_THROTTLE_MAX >= 0.0f && DRIFT_THROTTLE_MAX <= 1.0f, "DRIFT_THROTTLE_MAX must be 0 to 1");

// The yaw rate, in degrees per second, at which a gain of 1 alone steers to full lock.
#ifndef DRIFT_FULL_LOCK_DPS
#define DRIFT_FULL_LOCK_DPS 200.0f
#endif

// The gain the counter-steer starts with. At 0 the car drives as plain RC, so the sign below can be checked first.
#ifndef DRIFT_GAIN
#define DRIFT_GAIN 0.0f
#endif

// 1 when the sensor is mounted upside down, or when the counter-steer turns the wrong way: the car then spins harder
// as the gain rises.
#ifndef DRIFT_GYRO_REVERSED
#define DRIFT_GYRO_REVERSED 0
#endif
