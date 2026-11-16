#pragma once

#include <stddef.h>
#include <stdint.h>

#include "blink_schedule.h"

/**
 * What a request did to the setting. Accepted: the setting changed, so the caller stores it and drives the lamp.
 * Unchanged: the request was valid and changed nothing. Rejected: the request was malformed and nothing was applied.
 */
enum class BlinkOutcome { Accepted, Rejected, Unchanged };

/**
 * Applies a mode request, where arg is "blink", "on" or "off". On Accepted the schedule has advanced to `now`, so
 * output() and statusJson() are current.
 */
BlinkOutcome applyMode(BlinkSchedule& schedule, const char* arg, uint32_t now);

/**
 * Applies a period request, where arg is a positive integer of milliseconds with nothing after it. The schedule
 * clamps the value, so a period out of range is Accepted at the nearest limit.
 */
BlinkOutcome applyPeriod(BlinkSchedule& schedule, const char* arg, uint32_t now);

/** Writes the mode, period and lamp state as JSON into buf, up to n bytes. */
void statusJson(const BlinkSchedule& schedule, char* buf, size_t n);
