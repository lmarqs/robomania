#pragma once

#include <Arduino.h>

#include "line_reader.h"

/** Collects what the serial port receives into lines, as LineReader splits them, without waiting for a whole line. */
class SerialInput {
 private:
  LineReader reader;
  bool complete = false;

 public:
  /** Takes the characters that arrived. Returns true once a whole line is waiting. */
  bool ready();

  /** The waiting line, spaces kept. The next ready() starts a new line. */
  String take();
};
