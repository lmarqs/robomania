#pragma once

#include <serial_settings.h>
#include <stdint.h>

/** The settings a visitor sends over the serial port, in the order `cfg get` lists them. */
enum BeamSetting : uint8_t {
  AP_SSID,
  AP_PASSWORD,
  ANGLE_OFFSET_DEG,
  THROTTLE_MIN,
  THROTTLE_MID,
  THROTTLE_MAX,
  TILT_LIMIT_DEG,
  BEAM_SETTING_COUNT
};

/** Each setting's key, limits and compile-time default, from config.h. */
extern const SettingField BEAM_SETTINGS[BEAM_SETTING_COUNT];

/**
 * Refuses a throttle that would break min <= mid <= max against the other two's current values, with "order". So a
 * client that changes more than one throttle sends, each time, one whose new value keeps the order. From an ordered
 * band that differs from an ordered target such a throttle always exists: when the new mid lies above the current
 * max, the new max is at least the new mid and is accepted, and symmetrically for the min. So the band widens or
 * narrows to the target without a refusal.
 */
const char* checkThrottleOrder(const SerialSettings& settings, uint8_t field, float value);

/** What the sketch runs with: each stored value, else its default. */
struct BoardSettings {
  const char* apSsid;
  const char* apPassword;
  float angleOffsetDeg;
  float throttleMin;
  float throttleMid;
  float throttleMax;
  float tiltLimitDeg;
  /** True when the throttles did not satisfy 0 <= min <= mid <= max <= 1, so the band is 0 and no motor spins. */
  bool bandRejected;
};

/**
 * The values to run with. Throttles that break min <= mid <= max, which only a changed default can cause, give a band
 * of 0 with bandRejected set.
 */
BoardSettings activeSettings(const SerialSettings& settings);
