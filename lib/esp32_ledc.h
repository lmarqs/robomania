#pragma once

#ifdef ESP32
#include <esp_arduino_version.h>
#include <stdint.h>

// Core 3 allocates LEDC channels and timers itself, and BoardPwmOutPin uses that allocator there.
#if ESP_ARDUINO_VERSION_MAJOR < 3

/**
 * Takes an LEDC channel for a pin at `frequencyHz` with `bits` of resolution: one whose timer is free or already
 * runs at the same frequency and resolution, counting up from channel 0. Returns -1 when none is left. The core's
 * analogWrite() takes its channels from the top down without telling this allocator, so the two meet only when the
 * channels run out.
 */
int8_t esp32LedcTakeChannel(uint32_t frequencyHz, uint8_t bits);

/** Frees a channel that esp32LedcTakeChannel() returned. */
void esp32LedcReleaseChannel(int8_t channel);

/**
 * Reserves one LEDC timer, with every channel it drives in every speed group, for a driver that sets up LEDC through
 * ESP-IDF itself, such as the camera's XCLK. Returns the timer, whose channels are 2 × timer and the one after, or -1
 * when every timer drives a channel already.
 */
int8_t esp32LedcReserveTimer();

/** Frees a timer that esp32LedcReserveTimer() returned, with every channel it reserved. */
void esp32LedcReleaseTimer(int8_t timer);

#endif
#endif
