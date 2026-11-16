#include <continuous_servo_motor.h>
#include <host_test.h>

#include "fake_pwm_out_pin.h"

// Duties on a 16-bit pin at 50 Hz: 1000 µs is 3277 steps, 1500 µs 4915, 2000 µs 6554.

void test_continuous_servo_stays_still_until_the_first_write(void) {
  FakePwmOutPin pin;
  ContinuousServoMotor servo(&pin);
  TEST_ASSERT_TRUE(servo.begin());
  TEST_ASSERT_EQUAL(0, pin.writes);
}

void test_continuous_servo_stops_at_the_middle_pulse(void) {
  FakePwmOutPin pin;
  ContinuousServoMotor servo(&pin);
  servo.begin();
  servo.write(1.0f);
  servo.write(0.0f);
  TEST_ASSERT_EQUAL(4915, pin.duty);
}

void test_continuous_servo_runs_from_full_reverse_to_full_forward(void) {
  FakePwmOutPin pin;
  ContinuousServoMotor servo(&pin);
  servo.begin();
  servo.write(-1.0f);
  TEST_ASSERT_EQUAL(3277, pin.duty);
  servo.write(1.0f);
  TEST_ASSERT_EQUAL(6554, pin.duty);
}

int main(int, char**) {
  UNITY_BEGIN();
  RUN_TEST(test_continuous_servo_stays_still_until_the_first_write);
  RUN_TEST(test_continuous_servo_stops_at_the_middle_pulse);
  RUN_TEST(test_continuous_servo_runs_from_full_reverse_to_full_forward);
  return UNITY_END();
}
