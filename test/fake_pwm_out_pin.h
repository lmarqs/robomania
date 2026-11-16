#pragma once

#include <pin.h>

/** Records what a driver asks of a PWM pin, running at whatever frequency the test sets. */
class FakePwmOutPin : public PwmOutPin {
 public:
  bool starts = true;
  bool started = false;
  uint32_t hz = 50;
  int writes = 0;
  uint16_t duty = 0;

  bool begin() override { return started = starts; }
  void write(uint16_t value) override {
    writes++;
    duty = value;
  }
  uint32_t frequencyHz() override { return hz; }
};
