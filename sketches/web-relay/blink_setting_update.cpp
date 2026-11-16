#include "blink_setting_update.h"

#include <stdio.h>
#include <stdlib.h>

#include "blink_setting.h"

BlinkOutcome applyMode(BlinkSchedule& schedule, const char* arg, uint32_t now) {
  BlinkMode mode;
  if (!parseBlinkMode(arg, mode)) {
    return BlinkOutcome::Rejected;
  }
  if (mode == schedule.mode()) {
    return BlinkOutcome::Unchanged;
  }
  schedule.setMode(mode);
  schedule.update(now);
  return BlinkOutcome::Accepted;
}

BlinkOutcome applyPeriod(BlinkSchedule& schedule, const char* arg, uint32_t now) {
  char* end;
  const long requested = strtol(arg, &end, 10);
  if (end == arg || *end != '\0' || requested <= 0) {
    return BlinkOutcome::Rejected;
  }

  const uint32_t before = schedule.period();
  schedule.setPeriod(static_cast<uint32_t>(requested));
  if (schedule.period() == before) {
    return BlinkOutcome::Unchanged;
  }
  schedule.update(now);
  return BlinkOutcome::Accepted;
}

void statusJson(const BlinkSchedule& schedule, char* buf, size_t n) {
  snprintf(buf, n, "{\"mode\":\"%s\",\"period\":%lu,\"lamp\":%s}", blinkModeName(schedule.mode()),
           static_cast<unsigned long>(schedule.period()), schedule.output().isOn() ? "true" : "false");
}
