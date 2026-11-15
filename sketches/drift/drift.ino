#include <Arduino.h>
#include <Bluepad32.h>
#include <Wire.h>
#include <digital_pin.h>
#include <esc.h>
#include <imu.h>
#include <ledc_pin.h>
#include <servo.h>
#include <steering_assist.h>

#include "./config.h"

const uint32_t LED_BLINK_MS = 500;
const int32_t STICK_MAX = 512;
const int32_t STICK_DEAD_ZONE = 40;
const int32_t TRIGGER_MAX = 1023;
const float GAIN_STEP = 0.1f;

// The servo and the ESC share one 50 Hz LEDC timer; 16 bits make a step of 0.3 µs at that rate.
static EspLedcOutPin steeringPin(STEERING_PIN, 50, 16);
static EspLedcOutPin escPin(ESC_PIN, 50, 16);
static Servo steering(&steeringPin, min(DRIFT_STEERING_LEFT_US, DRIFT_STEERING_RIGHT_US),
                      max(DRIFT_STEERING_LEFT_US, DRIFT_STEERING_RIGHT_US));
static ReversibleEsc esc(&escPin);
static Mpu6050Imu imu(&Wire);
static SteeringAssist assist(DRIFT_FULL_LOCK_DPS);
static DigitalOutPin led(LED_PIN);

static Controller* controller = nullptr;
static bool imuReady = false;
static float gain = DRIFT_GAIN;
static uint8_t lastDpad = 0;

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
  if (!imuReady || !imu.read(sample)) {
    return 0.0f;
  }
  return DRIFT_GYRO_REVERSED ? -sample.gz : sample.gz;
}

static void drive(Controller* c) {
  adjustGain(c->dpad());
  const float throttle = (float)(c->throttle() - c->brake()) / TRIGGER_MAX;
  esc.write(constrain(throttle, -1.0f, 1.0f) * DRIFT_THROTTLE_MAX);
  writeSteering(assist.steer(stickToUnit(c->axisX()), yawRateDps(), gain));
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
  imuReady = imu.begin();
  Serial.println(imuReady ? "MPU6050 ready" : "error MPU6050 not found, steering without assist");

  BP32.setup([](Controller* c) { controller = controller == nullptr ? c : controller; },
             [](Controller* c) {
               if (controller == c) {
                 controller = nullptr;
                 stop();
               }
             });
  BP32.forgetBluetoothKeys();
  BP32.enableVirtualDevice(false);
  Serial.printf("gain %.1f\n", gain);
}

void loop() {
  if (BP32.update() && controller && controller->isConnected() && controller->hasData()) {
    drive(controller);
  }
  led.write(controller || (millis() / LED_BLINK_MS) % 2 ? HIGH : LOW);
  // Yields to the idle task, which feeds the watchdog; Bluepad32 delivers gamepad data far less often than this.
  delay(1);
}
