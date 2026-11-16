#include "continuous_servo_motor.h"

ContinuousServoMotor::ContinuousServoMotor(PwmOutPin* pwm, uint32_t minPulseUs, uint32_t maxPulseUs)
    : out(pwm, PulseRange::linear(minPulseUs, maxPulseUs)) {}

bool ContinuousServoMotor::begin() { return out.begin(); }

void ContinuousServoMotor::write(float throttle) { out.write(throttle); }
