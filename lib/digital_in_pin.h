#pragma once

/** A pin read as on or off: on when it is at the level its circuit makes active. */
class DigitalInPin {
 public:
  /** Returns false when the pin cannot run. */
  virtual bool begin() = 0;
  /** Whether the pin is at its active level. */
  virtual bool read() = 0;
};
