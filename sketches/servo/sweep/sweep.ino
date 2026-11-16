#include <Arduino.h>
#include <board_pwm_out_pin.h>
#include <positional_servo.h>
#include <serial_output.h>

// An Uno or Nano sets a PWM frequency only on Timer1's pins, 9 and 10.
#ifdef __AVR__
const uint8_t SERVO_PIN = 9;
#else
const uint8_t SERVO_PIN = 13;
#endif

BoardPwmOutPin servoPin(SERVO_PIN, 50);
PositionalServo servo(&servoPin);
SerialOutput output;

void setup() {
  output.begin("servo/sweep");
  servo.begin();
}

void loop() {
  for (int angle = 0; angle <= 180; angle += 1) {
    servo.write(angle);
    output.print("angle ");
    output.println(angle);
    delay(15);
  }

  for (int angle = 179; angle >= 1; angle -= 1) {
    servo.write(angle);
    output.print("angle ");
    output.println(angle);
    delay(15);
  }
}
