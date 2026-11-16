#include "blink_schedule.h"

void BlinkSchedule::setMode(BlinkMode mode) {
  if (mode == mode_) {
    return;
  }
  mode_ = mode;
  // Entering BLINK waits a full period before the first toggle.
  started_ = false;
}

void BlinkSchedule::setPeriod(uint32_t periodMs) { periodMs_ = clampPeriod(periodMs); }

void BlinkSchedule::restore(BlinkMode mode, uint32_t periodMs) {
  mode_ = mode;
  periodMs_ = clampPeriod(periodMs);
  started_ = false;
}

bool BlinkSchedule::update(uint32_t nowMs) {
  if (mode_ != BlinkMode::BLINK) {
    const bool desired = mode_ == BlinkMode::ON;
    if (desired == output_) {
      return false;
    }
    output_ = desired;
    return true;
  }

  if (!started_) {
    started_ = true;
    lastToggleMs_ = nowMs;
    return false;
  }

  // Unsigned subtraction stays correct when millis() rolls over.
  if (nowMs - lastToggleMs_ < periodMs_) {
    return false;
  }

  output_ = !output_;
  lastToggleMs_ = nowMs;
  return true;
}

uint32_t BlinkSchedule::clampPeriod(uint32_t periodMs) {
  if (periodMs < PERIOD_MIN_MS) {
    return PERIOD_MIN_MS;
  }
  if (periodMs > PERIOD_MAX_MS) {
    return PERIOD_MAX_MS;
  }
  return periodMs;
}
