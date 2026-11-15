#include "./receiver_input.h"

#include "./config.h"

// A receiver repeats a channel's pulse every 20 ms or less; reading each 10 ms catches every new one.
const uint32_t UPDATE_EVERY_MS = 10;
// A width outside this range is noise, not a servo pulse.
const uint32_t SHORTEST_PULSE_US = 500;
const uint32_t LONGEST_PULSE_US = 2500;

static RcChannel channel(bool reversed) {
  if (reversed) {
    return RcChannel(DRIFT_RECEIVER_HIGH_US, DRIFT_RECEIVER_CENTER_US, DRIFT_RECEIVER_LOW_US,
                     DRIFT_RECEIVER_DEAD_ZONE_US);
  }
  return RcChannel(DRIFT_RECEIVER_LOW_US, DRIFT_RECEIVER_CENTER_US, DRIFT_RECEIVER_HIGH_US,
                   DRIFT_RECEIVER_DEAD_ZONE_US);
}

static uint32_t pulseOf(EspPulseInPin& pin) {
  const uint32_t widthUs = pin.read(DRIFT_RECEIVER_TIMEOUT_MS * 1000);
  return widthUs >= SHORTEST_PULSE_US && widthUs <= LONGEST_PULSE_US ? widthUs : 0;
}

ReceiverInput::ReceiverInput(float fixedGain)
    : throttlePin(DRIFT_RECEIVER_THROTTLE_PIN),
      steeringPin(DRIFT_RECEIVER_STEERING_PIN),
      gainPin(DRIFT_RECEIVER_GAIN_PIN < 0 ? 0 : DRIFT_RECEIVER_GAIN_PIN),
      throttle(channel(DRIFT_RECEIVER_THROTTLE_REVERSED)),
      steering(channel(DRIFT_RECEIVER_STEERING_REVERSED)),
      gainKnob(channel(false)) {
  this->hasGainPin = DRIFT_RECEIVER_GAIN_PIN >= 0;
  this->fixedGain = fixedGain;
}

void ReceiverInput::begin() {
  const bool ready = throttlePin.begin() && steeringPin.begin() && (!hasGainPin || gainPin.begin());
  if (!ready) {
    Serial.println("error a receiver pin has no interrupt");
  }
  LOG("receiver throttle gpio %d, steering gpio %d, gain gpio %d\n", DRIFT_RECEIVER_THROTTLE_PIN,
      DRIFT_RECEIVER_STEERING_PIN, DRIFT_RECEIVER_GAIN_PIN);
}

void ReceiverInput::readPulses() {
  throttleUs = pulseOf(throttlePin);
  steeringUs = pulseOf(steeringPin);
  gainUs = hasGainPin ? pulseOf(gainPin) : 0;
}

bool ReceiverInput::update() {
  const uint32_t nowMs = millis();
  if (nowMs - lastUpdateMs < UPDATE_EVERY_MS) {
    return false;
  }
  lastUpdateMs = nowMs;
  readPulses();
  return connected();
}

bool ReceiverInput::connected() { return throttleUs != 0 && steeringUs != 0; }

DriveCommand ReceiverInput::command() {
  // A gain knob that loses its signal falls back to the fixed gain rather than to 0 or full.
  const float gain = gainUs != 0 ? gainKnob.level(gainUs) : fixedGain;
  if (!connected()) {
    return {0.0f, 0.0f, gain};
  }
  return {steering.position(steeringUs), throttle.position(throttleUs), gain};
}

void ReceiverInput::logInput() { LOG("throttle %u steering %u gain %u us", throttleUs, steeringUs, gainUs); }
