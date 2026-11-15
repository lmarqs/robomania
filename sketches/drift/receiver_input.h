#pragma once

#include <pulse_in_pin.h>
#include <rc_channel.h>

#include "./drive_input.h"

/**
 * A PWM RC receiver: one channel steers, one throttles and, when its pin is set, one sets the gain with a knob. It
 * counts as connected while the steering and throttle channels both pulse, so the car stops when the receiver loses
 * its signal and stops sending, or comes loose.
 */
class ReceiverInput : public DriveInput {
 private:
  EspPulseInPin throttlePin, steeringPin, gainPin;
  RcChannel throttle, steering, gainKnob;
  bool hasGainPin;
  float fixedGain;
  uint32_t lastUpdateMs = 0;
  uint32_t throttleUs = 0, steeringUs = 0, gainUs = 0;

  void readPulses();

 public:
  explicit ReceiverInput(float fixedGain);
  void begin() override;
  bool update() override;
  bool connected() override;
  DriveCommand command() override;
  void logInput() override;
};
