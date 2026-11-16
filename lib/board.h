#pragma once

#include <Arduino.h>

/**
 * The LED on the board the sketch is built for, and the level that lights it, so a sketch can blink it without knowing
 * the board: `BoardDigitalOutPin led(BOARD_LED_PIN, BOARD_LED_ON);`.
 */
#if defined(ESP8266)
// GPIO2, as on the ESP-01S, wired from 3.3 V to the pin.
static const uint8_t BOARD_LED_PIN = 2;
static const uint8_t BOARD_LED_ON = LOW;
#elif defined(ESP32)
// GPIO2, as on an ESP32 dev board.
static const uint8_t BOARD_LED_PIN = 2;
static const uint8_t BOARD_LED_ON = HIGH;
#else
static const uint8_t BOARD_LED_PIN = LED_BUILTIN;
static const uint8_t BOARD_LED_ON = HIGH;
#endif
