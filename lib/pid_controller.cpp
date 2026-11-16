#include "pid_controller.h"

static float clampTo(float value, float low, float high) { return value < low ? low : (value > high ? high : value); }

PidController::PidController(PidGains gains, float outMin, float outMax)
    : currentGains(gains), outMin(outMin), outMax(outMax) {}

void PidController::setGains(PidGains gains) { currentGains = gains; }

PidGains PidController::gains() const { return currentGains; }

void PidController::setOutputLimits(float outMin, float outMax) {
  this->outMin = outMin;
  this->outMax = outMax;
}

void PidController::reset() {
  accumulatedError = 0.0f;
  hasPrevious = false;
}

float PidController::update(float setpoint, float measurement, float dtS) {
  const float error = setpoint - measurement;
  float measurementRate = 0.0f;
  if (dtS > 0.0f) {
    accumulatedError += error * dtS;
    if (hasPrevious) {
      measurementRate = (measurement - previousMeasurement) / dtS;
    }
  }
  previousMeasurement = measurement;
  hasPrevious = true;

  if (currentGains.ki != 0.0f) {
    const float a = outMin / currentGains.ki;
    const float b = outMax / currentGains.ki;
    accumulatedError = clampTo(accumulatedError, a < b ? a : b, a < b ? b : a);
  }
  return clampTo(currentGains.kp * error + currentGains.ki * accumulatedError - currentGains.kd * measurementRate,
                 outMin, outMax);
}

float PidController::integral() const { return accumulatedError; }
