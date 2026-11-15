#pragma once

#include <Arduino.h>

#if __has_include("local.h")
#include "local.h"
#endif

// What drives the car: a Bluetooth gamepad through Bluepad32, or a PWM RC receiver's channels.
#define DRIFT_CONTROLLER_GAMEPAD 1
#define DRIFT_CONTROLLER_RECEIVER 2
#ifndef DRIFT_CONTROLLER
#define DRIFT_CONTROLLER DRIFT_CONTROLLER_GAMEPAD
#endif
#if DRIFT_CONTROLLER != DRIFT_CONTROLLER_GAMEPAD && DRIFT_CONTROLLER != DRIFT_CONTROLLER_RECEIVER
#error "DRIFT_CONTROLLER must be DRIFT_CONTROLLER_GAMEPAD or DRIFT_CONTROLLER_RECEIVER"
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

// The receiver's channel pins.
#ifndef DRIFT_RECEIVER_THROTTLE_PIN
#define DRIFT_RECEIVER_THROTTLE_PIN 17
#endif
#ifndef DRIFT_RECEIVER_STEERING_PIN
#define DRIFT_RECEIVER_STEERING_PIN 5
#endif

// The receiver's pulses at one end, the centre and the other end of every channel, and the dead zone around the
// centre. A transmitter's end points, trims and channel reverse set the channel to match.
#ifndef DRIFT_RECEIVER_LOW_US
#define DRIFT_RECEIVER_LOW_US 1000
#endif
#ifndef DRIFT_RECEIVER_CENTER_US
#define DRIFT_RECEIVER_CENTER_US 1500
#endif
#ifndef DRIFT_RECEIVER_HIGH_US
#define DRIFT_RECEIVER_HIGH_US 2000
#endif
#ifndef DRIFT_RECEIVER_DEAD_ZONE_US
#define DRIFT_RECEIVER_DEAD_ZONE_US 20
#endif

// How long, in milliseconds, the throttle or steering channel may go without a pulse before the car stops.
#ifndef DRIFT_RECEIVER_TIMEOUT_MS
#define DRIFT_RECEIVER_TIMEOUT_MS 100
#endif

// 1 shows the status on the board's LED, on GPIO2. It is off by default with a receiver, whose channels are often
// wired to GPIO2, where the LED's output would fight the receiver's.
#ifndef DRIFT_STATUS_LED
#define DRIFT_STATUS_LED (DRIFT_CONTROLLER == DRIFT_CONTROLLER_GAMEPAD)
#endif

// The largest throttle the controller reaches, as a fraction of the ESC's range each way.
#ifndef DRIFT_THROTTLE_MAX
#define DRIFT_THROTTLE_MAX 0.5f
#endif
static_assert(DRIFT_THROTTLE_MAX >= 0.0f && DRIFT_THROTTLE_MAX <= 1.0f, "DRIFT_THROTTLE_MAX must be 0 to 1");

// The throttle's expo, from 0 (linear) to 1 (cubic): the output is (1 - expo) * stick + expo * stick^3, so full stick
// still gives full throttle and half stick gives less than half.
#ifndef DRIFT_THROTTLE_EXPO
#define DRIFT_THROTTLE_EXPO 0.67f
#endif
static_assert(DRIFT_THROTTLE_EXPO >= 0.0f && DRIFT_THROTTLE_EXPO <= 1.0f, "DRIFT_THROTTLE_EXPO must be 0 to 1");

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

// 1 writes the controller, status, gain and input lines to the serial port; 0 leaves only errors. Printing in the loop
// delays the servo and ESC, so it stays off unless a car is being set up or debugged.
#ifndef DRIFT_LOG
#define DRIFT_LOG 0
#endif

// How often, in milliseconds, the serial port gets a line of input while a controller drives and DRIFT_LOG is 1;
// 0 turns that line off.
#ifndef DRIFT_LOG_INPUT_MS
#define DRIFT_LOG_INPUT_MS 250
#endif
