#pragma once

#include <Arduino.h>

/**
 * Prints to the serial port: print(), println() and printf() as Serial has them on the ESP32 and ESP8266. The AVR core
 * has no printf(), so this class gives it one, which prints up to PRINTF_CAPACITY - 1 characters of each call, and
 * prints a float as `?`, since avr-libc's default printf leaves floats out to save flash.
 */
class SerialOutput : public Print {
 public:
  /**
   * Opens the port at 115200 baud, then prints a blank line and `sketch <name>`, which the website listens for.
   * Returns true: a board always has its serial port.
   */
  bool begin(const char* name);
  size_t write(uint8_t c) override;
  /** Waits until everything printed has left the port, such as before a restart. */
  void flush() override;

#ifdef __AVR__
  static const size_t PRINTF_CAPACITY = 64;

  size_t printf(const char* format, ...) __attribute__((format(printf, 2, 3)));
#endif
};
