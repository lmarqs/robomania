#pragma once

/** What the driver asks for: steering and throttle each from -1 to 1, and the gyro's gain from 0 to 1. */
struct DriveCommand {
  float steer;
  float throttle;
  float gain;
};

/** A controller the car drives from. The sketch builds with one, chosen by DRIFT_CONTROLLER. */
class DriveInput {
 public:
  virtual void begin() = 0;
  /** Takes what the controller sent; returns true when a new command is ready. */
  virtual bool update() = 0;
  /** Whether a controller is connected and sending. */
  virtual bool connected() = 0;
  /** The latest command; steering and throttle are 0 until the first one arrives. */
  virtual DriveCommand command() = 0;
  /** Writes the raw input behind the latest command to the serial port, for the log's input line. */
  virtual void logInput() = 0;
};
