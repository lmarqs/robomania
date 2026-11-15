#include <Arduino.h>
#include <Bluepad32.h>
#include <Wire.h>
#include <board_led.h>
#include <esc.h>
#include <imu.h>
#include <ledc_pin.h>
#include <servo.h>
#include <steering_assist.h>

#include "./config.h"

const uint32_t BLINK_SLOT_MS = 1000;
const uint32_t SHORT_BLINK_MS = 150;
const uint32_t LONG_BLINK_MS = 600;
const int32_t STICK_MAX = 512;
const int32_t STICK_DEAD_ZONE = 40;
const int32_t TRIGGER_MAX = 1023;
const float GAIN_STEP = 0.1f;

const uint8_t STATUS_GAMEPAD = 1 << 0;
const uint8_t STATUS_IMU = 1 << 1;
const uint8_t STATUS_ALL = STATUS_GAMEPAD | STATUS_IMU;

// The servo and the ESC share one 50 Hz LEDC timer; 16 bits make a step of 0.3 µs at that rate.
static EspLedcOutPin steeringPin(STEERING_PIN, 50, 16);
static EspLedcOutPin escPin(ESC_PIN, 50, 16);
static Servo steering(&steeringPin, min(DRIFT_STEERING_LEFT_US, DRIFT_STEERING_RIGHT_US),
                      max(DRIFT_STEERING_LEFT_US, DRIFT_STEERING_RIGHT_US));
static ReversibleEsc esc(&escPin);
static Mpu6050Imu imu(&Wire);
static SteeringAssist assist(DRIFT_FULL_LOCK_DPS);
static BoardLed led;

static Controller* controller = nullptr;
static bool imuFound = false;
static uint8_t status = 0;
static float gain = DRIFT_GAIN;
static uint8_t lastDpad = 0;
static uint32_t lastInputLogMs = 0;

static void setStatus(uint8_t bit, bool works) {
  const uint8_t next = works ? status | bit : status & ~bit;
  if (next != status) {
    Serial.printf("status gamepad %s, imu %s\n", next & STATUS_GAMEPAD ? "connected" : "missing",
                  next & STATUS_IMU ? "working" : "failed");
  }
  status = next;
}

static void logGamepad(const char* event, Controller* c) {
  const ControllerProperties properties = c->getProperties();
  const uint8_t* a = properties.btaddr;
  Serial.printf("gamepad %d %s: %s, vid %04x pid %04x, %02X:%02X:%02X:%02X:%02X:%02X\n", c->index(), event,
                c->getModelName().c_str(), properties.vendor_id, properties.product_id, a[0], a[1], a[2], a[3], a[4],
                a[5]);
}

static float stickToUnit(int32_t value) {
  if (abs(value) < STICK_DEAD_ZONE) {
    return 0.0f;
  }
  return constrain((float)value / STICK_MAX, -1.0f, 1.0f);
}

static void writeSteering(float steer) {
  const float toward = steer >= 0.0f ? DRIFT_STEERING_RIGHT_US : DRIFT_STEERING_LEFT_US;
  steering.writeMicroseconds(lroundf(DRIFT_STEERING_CENTER_US + fabsf(steer) * (toward - DRIFT_STEERING_CENTER_US)));
}

static void stop() {
  esc.write(0.0f);
  writeSteering(0.0f);
}

static void adjustGain(uint8_t dpad) {
  const uint8_t pressed = dpad & ~lastDpad;
  lastDpad = dpad;
  if (!(pressed & (DPAD_UP | DPAD_DOWN))) {
    return;
  }
  gain = constrain(gain + (pressed & DPAD_UP ? GAIN_STEP : -GAIN_STEP), 0.0f, 1.0f);
  Serial.printf("gain %.1f\n", gain);
}

static float yawRateDps() {
  ImuSample sample;
  if (!imuFound) {
    return 0.0f;
  }
  const bool read = imu.read(sample);
  setStatus(STATUS_IMU, read);
  if (!read) {
    return 0.0f;
  }
  return DRIFT_GYRO_REVERSED ? -sample.gz : sample.gz;
}

/**
 * Whether the LED is lit at `nowMs`: steady while everything in STATUS_ALL works; otherwise a short blink each second
 * for the IMU, a long one for the gamepad, alternating when neither works.
 */
static bool statusLed(uint8_t working, uint32_t nowMs) {
  const uint8_t missing = working ^ STATUS_ALL;
  if (!missing) {
    return true;
  }
  const bool showImu = missing == STATUS_IMU || (missing == STATUS_ALL && (nowMs / BLINK_SLOT_MS) % 2 == 0);
  return nowMs % BLINK_SLOT_MS < (showImu ? SHORT_BLINK_MS : LONG_BLINK_MS);
}

static void drive(Controller* c) {
  adjustGain(c->dpad());
  const float throttle = constrain((float)(c->throttle() - c->brake()) / TRIGGER_MAX, -1.0f, 1.0f) * DRIFT_THROTTLE_MAX;
  const float yaw = yawRateDps();
  const float steer = assist.steer(stickToUnit(c->axisX()), yaw, gain);
  esc.write(throttle);
  writeSteering(steer);

  const uint32_t now = millis();
  if (DRIFT_LOG_INPUT_MS > 0 && now - lastInputLogMs >= DRIFT_LOG_INPUT_MS) {
    lastInputLogMs = now;
    Serial.printf("input x %d throttle %d brake %d dpad %02x buttons %04x battery %u | yaw %.1f steer %.2f esc %.2f\n",
                  c->axisX(), c->throttle(), c->brake(), c->dpad(), c->buttons(), c->battery(), yaw, steer, throttle);
  }
}

void setup() {
  Serial.begin(115200);
  led.begin();

  if (!steering.begin() || !esc.begin()) {
    Serial.println("error a PWM pin has no LEDC channel");
  }
  stop();

  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  Wire.setClock(400000);
  // A stalled read fails within 10 ms, where a hang would leave the car on its last command.
  Wire.setTimeOut(10);
  // The gyro's bias is measured now, so the car must stand still while it boots.
  imuFound = imu.begin();
  setStatus(STATUS_IMU, imuFound);
  Serial.println(imuFound ? "MPU6050 ready" : "error MPU6050 not found, steering without assist");

  BP32.setup(
      [](Controller* c) {
        if (controller != nullptr) {
          logGamepad("ignored, another drives", c);
          return;
        }
        controller = c;
        logGamepad("connected", c);
        setStatus(STATUS_GAMEPAD, true);
      },
      [](Controller* c) {
        if (controller != c) {
          logGamepad("disconnected, was ignored", c);
          return;
        }
        controller = nullptr;
        logGamepad("disconnected", c);
        setStatus(STATUS_GAMEPAD, false);
        stop();
      });
  BP32.forgetBluetoothKeys();
  BP32.enableVirtualDevice(false);
  Serial.printf("gain %.1f\n", gain);
}

void loop() {
  if (BP32.update() && controller && controller->isConnected() && controller->hasData()) {
    drive(controller);
  }
  led.write(statusLed(status, millis()) ? HIGH : LOW);
  // Sleeps a tick so core 1's idle task runs; Bluepad32 delivers gamepad data far less often than this.
  delay(1);
}
