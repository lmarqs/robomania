#include <Arduino.h>
#include <Wire.h>
#include <board_led.h>
#include <esc.h>
#include <imu.h>
#include <ledc_pin.h>
#include <pulse_out.h>
#include <steering_assist.h>

#include "./config.h"
#include "./gamepad_input.h"
#include "./log.h"
#include "./receiver_input.h"

const uint32_t BLINK_SLOT_MS = 1000;
const uint32_t SHORT_BLINK_MS = 150;
const uint32_t LONG_BLINK_MS = 600;

const uint8_t STATUS_CONTROLLER = 1 << 0;
const uint8_t STATUS_IMU = 1 << 1;
const uint8_t STATUS_ALL = STATUS_CONTROLLER | STATUS_IMU;

// The servo and the ESC share one 50 Hz LEDC timer; 16 bits make a step of 0.3 µs at that rate.
static EspLedcOutPin steeringPin(STEERING_PIN, 50, 16);
static EspLedcOutPin escPin(ESC_PIN, 50, 16);
static PulseOut steering(&steeringPin,
                         PulseRange(DRIFT_STEERING_LEFT_US, DRIFT_STEERING_CENTER_US, DRIFT_STEERING_RIGHT_US));
static ReversibleEsc esc(&escPin);
static Mpu6050Imu imu(&Wire);
static SteeringAssist assist(DRIFT_FULL_LOCK_DPS);
static BoardLed led;

#if DRIFT_CONTROLLER == DRIFT_CONTROLLER_RECEIVER
static ReceiverInput input(DRIFT_GAIN);
#else
static GamepadInput input(DRIFT_GAIN);
#endif

static bool imuFound = false;
static uint8_t status = 0;
static uint32_t lastInputLogMs = 0;

static void setStatus(uint8_t bit, bool works) {
  const uint8_t next = works ? status | bit : status & ~bit;
  if (next != status) {
    LOG("status controller %s, imu %s\n", next & STATUS_CONTROLLER ? "connected" : "missing",
        next & STATUS_IMU ? "working" : "failed");
  }
  status = next;
}

/** RC expo: the same ends, a flatter middle, so small stick moves give finer control. */
static float expo(float value, float amount) { return (1.0f - amount) * value + amount * value * value * value; }


static void stop() {
  esc.write(0.0f);
  steering.write(0.0f);
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
 * for the IMU, a long one for the controller, alternating when neither works.
 */
static bool statusLed(uint8_t working, uint32_t nowMs) {
  const uint8_t missing = working ^ STATUS_ALL;
  if (!missing) {
    return true;
  }
  const bool showImu = missing == STATUS_IMU || (missing == STATUS_ALL && (nowMs / BLINK_SLOT_MS) % 2 == 0);
  return nowMs % BLINK_SLOT_MS < (showImu ? SHORT_BLINK_MS : LONG_BLINK_MS);
}

static void logInput(const DriveCommand& command, float yaw, float steer, float throttle) {
  const uint32_t now = millis();
  if (!DRIFT_LOG || DRIFT_LOG_INPUT_MS == 0 || now - lastInputLogMs < DRIFT_LOG_INPUT_MS) {
    return;
  }
  lastInputLogMs = now;
  LOG("input ");
  input.logInput();
  LOG(" | yaw %.1f steer %.2f esc %.2f gain %.2f\n", yaw, steer, throttle, command.gain);
}

static void drive(const DriveCommand& command) {
  const float throttle = expo(command.throttle, DRIFT_THROTTLE_EXPO) * DRIFT_THROTTLE_MAX;
  const float yaw = yawRateDps();
  const float steer = assist.steer(command.steer, yaw, command.gain);
  esc.write(throttle);
  steering.write(steer);
  logInput(command, yaw, steer, throttle);
}

void setup() {
  Serial.begin(115200);
  if (DRIFT_STATUS_LED) {
    led.begin();
  }

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
  if (imuFound) {
    LOG("MPU6050 ready\n");
  } else {
    Serial.println("error MPU6050 not found, steering without assist");
  }

  input.begin();
  LOG("gain %.1f\n", DRIFT_GAIN);
}

void loop() {
  const bool fresh = input.update();
  const bool connected = input.connected();
  setStatus(STATUS_CONTROLLER, connected);
  if (!connected) {
    stop();
  }
  if (fresh) {
    drive(input.command());
  }
  if (DRIFT_STATUS_LED) {
    led.write(statusLed(status, millis()) ? HIGH : LOW);
  }
  // Sleeps a tick so core 1's idle task runs; Bluepad32 delivers gamepad data far less often than this.
  delay(1);
}
