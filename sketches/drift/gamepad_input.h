#pragma once

#include <Bluepad32.h>

#include "./drive_input.h"

/**
 * A Bluetooth gamepad through Bluepad32: the left stick steers, the right stick's Y axis throttles and the D-pad steps
 * the gain. The first gamepad to connect drives; others are ignored until it disconnects.
 */
class GamepadInput : public DriveInput {
 private:
  static Controller* controller;
  float gain;
  uint8_t lastDpad = 0;

  void adjustGain(uint8_t dpad);

 public:
  explicit GamepadInput(float gain);
  void begin() override;
  bool update() override;
  bool connected() override;
  DriveCommand command() override;
  void logInput() override;
};
