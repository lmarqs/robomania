#pragma once

#include <Arduino.h>

/**
 * A pin that drives a servo, at 50 Hz through a BoardPwmOutPin, on the board the sketch is built for:
 * `BoardPwmOutPin servoPin(BOARD_SERVO_PIN, 50);`.
 */
#if defined(__AVR__)
// An Uno or Nano runs a frequency of its own only on Timer1's pins, 9 and 10.
static const uint8_t BOARD_SERVO_PIN = 9;
#else
// The ESP32 and ESP8266 run one on any output pin; GPIO13 is free on both.
static const uint8_t BOARD_SERVO_PIN = 13;
#endif
