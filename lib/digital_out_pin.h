#pragma once

/** A pin that turns its load on or off. */
class DigitalOutPin {
 public:
  /** Makes the pin an output and turns it off. Returns false when the pin cannot run. */
  virtual bool begin() = 0;
  virtual void write(bool on) = 0;
};
