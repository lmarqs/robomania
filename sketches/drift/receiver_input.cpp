#include "./receiver_input.h"

#include <rc_channel.h>

#include "./config.h"
#include "./log.h"

// A receiver repeats a channel's pulse every 20 ms or less; reading each 10 ms catches every new one.
const uint32_t UPDATE_EVERY_MS = 10;

static const RcChannel STICK(DRIFT_RECEIVER_LOW_US, DRIFT_RECEIVER_CENTER_US, DRIFT_RECEIVER_HIGH_US,
                             DRIFT_RECEIVER_DEAD_ZONE_US);

static uint32_t pulseOf(EspPulseInPin& pin) {
  const uint32_t widthUs = pin.read(DRIFT_RECEIVER_TIMEOUT_MS * 1000);
  return RcChannel::isServoPulse(widthUs) ? widthUs : 0;
}

ReceiverInput::ReceiverInput(float gain)
    : throttlePin(DRIFT_RECEIVER_THROTTLE_PIN), steeringPin(DRIFT_RECEIVER_STEERING_PIN) {
  this->gain = gain;
}

void ReceiverInput::begin() {
  const bool throttleReady = throttlePin.begin();
  const bool steeringReady = steeringPin.begin();
  if (!throttleReady || !steeringReady) {
    Serial.println("error a receiver pin has no interrupt");
  }
  LOG("receiver throttle gpio %d, steering gpio %d\n", DRIFT_RECEIVER_THROTTLE_PIN, DRIFT_RECEIVER_STEERING_PIN);
}

bool ReceiverInput::update() {
  const uint32_t nowMs = millis();
  if (nowMs - lastUpdateMs < UPDATE_EVERY_MS) {
    return false;
  }
  lastUpdateMs = nowMs;
  throttleUs = pulseOf(throttlePin);
  steeringUs = pulseOf(steeringPin);
  return connected();
}

bool ReceiverInput::connected() { return throttleUs != 0 && steeringUs != 0; }

DriveCommand ReceiverInput::command() { return {STICK.position(steeringUs), STICK.position(throttleUs), gain}; }

void ReceiverInput::logInput() { LOG("throttle %u steering %u us", throttleUs, steeringUs); }
