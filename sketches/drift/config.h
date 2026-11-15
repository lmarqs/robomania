#pragma once

#include <Arduino.h>

#if __has_include("local.h")
#include "local.h"
#endif

// ESP32 WROOM-32 GPIO numbers.
#ifndef DRIFT_ESC_PIN
#define DRIFT_ESC_PIN 21
#endif
#ifndef DRIFT_STEERING_PIN
#define DRIFT_STEERING_PIN 23
#endif
#ifndef DRIFT_I2C_SDA_PIN
#define DRIFT_I2C_SDA_PIN 27
#endif
#ifndef DRIFT_I2C_SCL_PIN
#define DRIFT_I2C_SCL_PIN 26
#endif
const uint8_t ESC_PIN = DRIFT_ESC_PIN;
const uint8_t STEERING_PIN = DRIFT_STEERING_PIN;
const uint8_t I2C_SDA_PIN = DRIFT_I2C_SDA_PIN;
const uint8_t I2C_SCL_PIN = DRIFT_I2C_SCL_PIN;

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

// How often, in milliseconds, the serial port gets a line of gamepad input while a gamepad drives; 0 turns it off.
#ifndef DRIFT_LOG_INPUT_MS
#define DRIFT_LOG_INPUT_MS 250
#endif
