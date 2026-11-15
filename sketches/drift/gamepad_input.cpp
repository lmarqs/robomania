#include "./gamepad_input.h"

#include "./log.h"

const int32_t STICK_MAX = 512;
const int32_t STICK_DEAD_ZONE = 40;
const float GAIN_STEP = 0.1f;

Controller* GamepadInput::controller = nullptr;

static float stickToUnit(int32_t value) {
  if (abs(value) < STICK_DEAD_ZONE) {
    return 0.0f;
  }
  return constrain((float)value / STICK_MAX, -1.0f, 1.0f);
}

static void logGamepad(const char* event, Controller* c) {
  const ControllerProperties properties = c->getProperties();
  const uint8_t* a = properties.btaddr;
  LOG("gamepad %d %s: %s, vid %04x pid %04x, %02X:%02X:%02X:%02X:%02X:%02X\n", c->index(), event,
      c->getModelName().c_str(), properties.vendor_id, properties.product_id, a[0], a[1], a[2], a[3], a[4], a[5]);
}

GamepadInput::GamepadInput(float gain) { this->gain = gain; }

void GamepadInput::begin() {
  BP32.setup(
      [](Controller* c) {
        if (controller != nullptr) {
          logGamepad("ignored, another drives", c);
          return;
        }
        controller = c;
        logGamepad("connected", c);
      },
      [](Controller* c) {
        if (controller != c) {
          logGamepad("disconnected, was ignored", c);
          return;
        }
        controller = nullptr;
        logGamepad("disconnected", c);
      });
  BP32.forgetBluetoothKeys();
  BP32.enableVirtualDevice(false);
}

bool GamepadInput::update() {
  if (!BP32.update() || !connected() || !controller->hasData()) {
    return false;
  }
  adjustGain(controller->dpad());
  return true;
}

bool GamepadInput::connected() { return controller != nullptr && controller->isConnected(); }

void GamepadInput::adjustGain(uint8_t dpad) {
  const uint8_t pressed = dpad & ~lastDpad;
  lastDpad = dpad;
  if (!(pressed & (DPAD_UP | DPAD_DOWN))) {
    return;
  }
  gain = constrain(gain + (pressed & DPAD_UP ? GAIN_STEP : -GAIN_STEP), 0.0f, 1.0f);
  LOG("gain %.1f\n", gain);
}

DriveCommand GamepadInput::command() {
  // Bluepad32 reports a stick pushed down as positive, so up on the right stick drives forward.
  return {stickToUnit(controller->axisX()), -stickToUnit(controller->axisRY()), gain};
}

void GamepadInput::logInput() {
  LOG("lx %d ry %d dpad %02x buttons %04x battery %u", controller->axisX(), controller->axisRY(), controller->dpad(),
      controller->buttons(), controller->battery());
}
