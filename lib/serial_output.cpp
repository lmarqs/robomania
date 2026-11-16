#include "serial_output.h"

#ifdef __AVR__
#include <stdarg.h>
#include <stdio.h>
#endif

void SerialOutput::begin(const char* name) {
  Serial.begin(115200);
  Serial.println();
  Serial.print("sketch ");
  Serial.println(name);
}

size_t SerialOutput::write(uint8_t c) { return Serial.write(c); }

void SerialOutput::flush() { Serial.flush(); }

#ifdef __AVR__
size_t SerialOutput::printf(const char* format, ...) {
  char text[PRINTF_CAPACITY];
  va_list arguments;
  va_start(arguments, format);
  vsnprintf(text, sizeof(text), format, arguments);
  va_end(arguments);
  return print(text);
}
#endif
