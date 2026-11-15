#include "./beam_settings.h"

#include "./config.h"

const SettingField BEAM_SETTINGS[BEAM_SETTING_COUNT] = {
    SettingField::text("ap_ssid", 1, 32, DEFAULT_AP_SSID),
    SettingField::secret("ap_password", 8, 63, DEFAULT_AP_PASSWORD),
    SettingField::number("angle_offset_deg", -30.0f, 30.0f, DEFAULT_ANGLE_OFFSET_DEG),
    SettingField::number("throttle_min", 0.0f, 1.0f, DEFAULT_THROTTLE_MIN),
    SettingField::number("throttle_mid", 0.0f, 1.0f, DEFAULT_THROTTLE_MID),
    SettingField::number("throttle_max", 0.0f, 1.0f, DEFAULT_THROTTLE_MAX),
    SettingField::number("tilt_limit_deg", 0.0f, 90.0f, DEFAULT_TILT_LIMIT_DEG, true),
};

static bool bandOrdered(float min, float mid, float max) { return min <= mid && mid <= max; }

const char* checkThrottleOrder(const SerialSettings& settings, uint8_t field, float value) {
  if (field < THROTTLE_MIN || field > THROTTLE_MAX) {
    return nullptr;
  }
  const float min = field == THROTTLE_MIN ? value : settings.number(THROTTLE_MIN);
  const float mid = field == THROTTLE_MID ? value : settings.number(THROTTLE_MID);
  const float max = field == THROTTLE_MAX ? value : settings.number(THROTTLE_MAX);
  return bandOrdered(min, mid, max) ? nullptr : "order";
}

BoardSettings activeSettings(const SerialSettings& settings) {
  BoardSettings active{settings.text(AP_SSID),
                       settings.text(AP_PASSWORD),
                       settings.number(ANGLE_OFFSET_DEG),
                       settings.number(THROTTLE_MIN),
                       settings.number(THROTTLE_MID),
                       settings.number(THROTTLE_MAX),
                       settings.number(TILT_LIMIT_DEG),
                       false};
  if (!bandOrdered(active.throttleMin, active.throttleMid, active.throttleMax)) {
    active.throttleMin = active.throttleMid = active.throttleMax = 0.0f;
    active.bandRejected = true;
  }
  return active;
}
