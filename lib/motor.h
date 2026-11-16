#pragma once

/**
 * A motor driven by a throttle: from -1, full reverse, through 0, stopped, to 1, full forward. A throttle past either
 * end is clamped to it, and NaN stops the motor. A motor that cannot reverse, such as a one-way ESC's, stops for any
 * throttle below 0.
 */
class Motor {
 public:
  /** Starts the motor's pins with the motor stopped. Returns false when a pin cannot run. */
  virtual bool begin() = 0;
  virtual void write(float throttle) = 0;
};
