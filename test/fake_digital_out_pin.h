#pragma once

#include <digital_out_pin.h>

/** Records what a driver asks of a digital output pin. */
class FakeDigitalOutPin : public DigitalOutPin {
 public:
  bool starts = true;
  bool started = false;
  int writes = 0;
  bool on = false;

  bool begin() override { return started = starts; }
  void write(bool value) override {
    writes++;
    on = value;
  }
};
