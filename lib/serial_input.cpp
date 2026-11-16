#include "serial_input.h"

bool SerialInput::ready() {
  while (!complete && Serial.available()) {
    complete = reader.feed(static_cast<char>(Serial.read()));
  }
  return complete;
}

String SerialInput::take() {
  complete = false;
  return String(reader.line());
}
