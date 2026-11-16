#pragma once

#include <Arduino.h>
#include <board_digital_out_pin.h>
#include <board_pwm_out_pin.h>
#include <full_h_bridge_motor.h>
#include <motor.h>

#include "wheel_mixer.h"

BoardPwmOutPin pwm1(15, 5000);
BoardDigitalOutPin in11(04);
BoardDigitalOutPin in21(16);
FullHBridgeMotor motor1(&pwm1, &in11, &in21);

BoardPwmOutPin pwm2(18, 5000);
BoardDigitalOutPin in12(05);
BoardDigitalOutPin in22(17);
FullHBridgeMotor motor2(&pwm2, &in12, &in22);

BoardPwmOutPin pwm3(25, 5000);
BoardDigitalOutPin in13(27);
BoardDigitalOutPin in23(26);
FullHBridgeMotor motor3(&pwm3, &in13, &in23);

BoardPwmOutPin pwm4(13, 5000);
BoardDigitalOutPin in14(14);
BoardDigitalOutPin in24(12);
FullHBridgeMotor motor4(&pwm4, &in14, &in24);

// The wheels run at four fifths of full speed at most.
const float MAX_THROTTLE = 0.8f;

class CarPilot {
 private:
  Motor* motors[4] = {&motor1, &motor2, &motor3, &motor4};

 public:
  void begin() {
    for (Motor* motor : motors) {
      motor->begin();
    }
  }

  void stop() {
    for (Motor* motor : motors) {
      motor->write(0.0f);
    }
  }

  void drive(const WheelSpeeds& speeds) {
    for (size_t i = 0; i < 4; i++) {
      motors[i]->write(MAX_THROTTLE * speeds.values[i] / STICK_MAX);
    }
  }
};
