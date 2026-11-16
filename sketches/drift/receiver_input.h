#pragma once

#include <board_pulse_in_pin.h>

#include "./drive_input.h"

/**
 * A PWM RC receiver: one channel steers and one throttles, and the gain stays where it starts. It counts as connected
 * while both channels pulse, so the car stops when the receiver loses its signal and stops sending, or comes loose.
 */
class ReceiverInput : public DriveInput {
 private:
  BoardPulseInPin throttlePin, steeringPin;
  float gain;
  uint32_t lastUpdateMs = 0;
  uint32_t throttleUs = 0, steeringUs = 0;

 public:
  explicit ReceiverInput(float gain);
  void begin() override;
  bool update() override;
  bool connected() override;
  DriveCommand command() override;
  void logInput() override;
};
