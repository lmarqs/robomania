#pragma once

/**
 * Counter-steer against a car's yaw rate, added to the driver's steering, as an RC drift gyro does: when the tail
 * swings out, the wheels turn into the slide in proportion to how fast the car spins, which damps the spin so the car
 * holds the drift.
 */
class SteeringAssist {
 private:
  float fullLockDps;

 public:
  /** `fullLockDps` is the yaw rate, in degrees per second, at which a gain of 1 alone turns the wheels to full lock. */
  explicit SteeringAssist(float fullLockDps);

  /**
   * The steering to send, from -1 (full left) to 1 (full right). `driverSteer` runs over the same range,
   * `yawRateDps` is positive while the car turns clockwise seen from above, and `gain`, from 0 (no assist) to 1, is
   * clamped to that range.
   */
  float steer(float driverSteer, float yawRateDps, float gain) const;
};
