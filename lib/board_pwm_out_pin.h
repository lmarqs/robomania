#pragma once

#include <Arduino.h>
#ifdef ESP32
#include <esp_arduino_version.h>
#endif

#include "pwm_out_pin.h"

/**
 * A PWM pin on the board the sketch is built for. With no frequency it runs at the rate analogWrite() uses, on any pin
 * analogWrite() drives: the PWM Arduino calls "analog", for a LED's brightness or a motor's speed. With a frequency,
 * such as 50 Hz for a servo, it runs at that one:
 *
 * - ESP32: on any output pin, through the LEDC peripheral, at up to 16 bits.
 * - ESP8266: on GPIO0 to GPIO16 but the flash chip's GPIO6 to GPIO11, or GPIO6, 7, 8 and 11 on an ESP8285, through
 *   the core's waveform generator.
 * - AVR: only on Timer1's pins, 9 and 10 on an Uno or Nano, which then share one frequency. begin() returns false on
 *   any other pin, or when the other Timer1 pin runs at another frequency or at analogWrite's rate.
 */
class BoardPwmOutPin : public PwmOutPin {
 private:
  uint8_t pin;
  uint32_t requestedHz;
  uint32_t actualHz = 0;
#if defined(ESP32)
  uint8_t bits = 0;
#if ESP_ARDUINO_VERSION_MAJOR < 3
  int8_t channel = -1;
#endif
#elif defined(ESP8266)
  uint32_t periodCycles = 0;
#elif defined(__AVR__)
  bool usesTimer1 = false;
#endif

 public:
  /** `frequencyHz` 0, the default, runs at the board's analogWrite() rate. */
  explicit BoardPwmOutPin(uint8_t pin, uint32_t frequencyHz = 0);

  bool begin() override;
  void write(uint16_t duty) override;
  uint32_t frequencyHz() override;
};
