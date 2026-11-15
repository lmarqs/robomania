#include "servo.h"

#include <Arduino.h>

#include "pulse_width.h"

Servo::Servo(PwmOutPin* pwm, uint32_t minPulseUs, uint32_t maxPulseUs) {
  this->pwm = pwm;
  this->minPulseUs = minPulseUs;
  this->maxPulseUs = maxPulseUs;
}

bool Servo::begin() { return pwm->begin(); }

void Servo::writeMicroseconds(uint32_t pulseUs) {
  pulseUs = constrain(pulseUs, minPulseUs, maxPulseUs);
  pwm->write(pulseDuty(pulseUs, pwm->frequency()));
}

PositionalServo::PositionalServo(PwmOutPin* pwm, uint16_t range, uint32_t minPulseUs, uint32_t maxPulseUs)
    : Servo(pwm, minPulseUs, maxPulseUs) {
  this->range = range;
}

void PositionalServo::write(int32_t angle) {
  angle = constrain(angle, 0, range);
  writeMicroseconds(map(angle, 0, range, minPulseUs, maxPulseUs));
}

ContinuousServo::ContinuousServo(PwmOutPin* pwm, uint32_t minPulseUs, uint32_t maxPulseUs)
    : Servo(pwm, minPulseUs, maxPulseUs) {}

void ContinuousServo::move(int32_t speed) {
  speed = constrain(speed, -100, 100);
  writeMicroseconds(map(speed, -100, 100, minPulseUs, maxPulseUs));
}

void ContinuousServo::stop() { writeMicroseconds((minPulseUs + maxPulseUs) / 2); }
