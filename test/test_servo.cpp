#include <host_test.h>
#include <servo.h>

#include "fake_pwm_out_pin.h"

// Duties on a 16-bit pin at 50 Hz: 544 µs is 1783 steps, 1472 µs 4823, 2400 µs 7864.

void test_positional_servo_stays_limp_until_the_first_write(void) {
  FakePwmOutPin pin;
  PositionalServo servo(&pin);
  TEST_ASSERT_TRUE(servo.begin());
  TEST_ASSERT_EQUAL(0, pin.writes);
}

void test_positional_servo_maps_0_90_and_180_degrees_to_its_pulses(void) {
  FakePwmOutPin pin;
  PositionalServo servo(&pin);
  servo.begin();
  servo.write(0);
  TEST_ASSERT_EQUAL(1783, pin.duty);
  servo.write(90);
  TEST_ASSERT_EQUAL(4823, pin.duty);
  servo.write(180);
  TEST_ASSERT_EQUAL(7864, pin.duty);
}

void test_positional_servo_clamps_an_angle_past_its_range(void) {
  FakePwmOutPin pin;
  PositionalServo servo(&pin);
  TEST_ASSERT_FLOAT_WITHIN(1e-3f, 2400.0f, servo.pulseUs(270));
  TEST_ASSERT_FLOAT_WITHIN(1e-3f, 544.0f, servo.pulseUs(-10));
}

void test_positional_servo_scales_to_its_range(void) {
  FakePwmOutPin pin;
  PositionalServo servo(&pin, 270, 500, 2500);
  TEST_ASSERT_FLOAT_WITHIN(1e-3f, 1500.0f, servo.pulseUs(135));
}

void test_continuous_servo_stops_at_the_middle_pulse(void) {
  FakePwmOutPin pin;
  ContinuousServo servo(&pin);
  servo.begin();
  servo.move(100);
  servo.stop();
  TEST_ASSERT_EQUAL(4915, pin.duty);
}

void test_continuous_servo_runs_from_full_reverse_to_full_forward(void) {
  FakePwmOutPin pin;
  ContinuousServo servo(&pin);
  servo.begin();
  servo.move(-100);
  TEST_ASSERT_EQUAL(3277, pin.duty);
  servo.move(100);
  TEST_ASSERT_EQUAL(6554, pin.duty);
}

int main(int, char**) {
  UNITY_BEGIN();
  RUN_TEST(test_positional_servo_stays_limp_until_the_first_write);
  RUN_TEST(test_positional_servo_maps_0_90_and_180_degrees_to_its_pulses);
  RUN_TEST(test_positional_servo_clamps_an_angle_past_its_range);
  RUN_TEST(test_positional_servo_scales_to_its_range);
  RUN_TEST(test_continuous_servo_stops_at_the_middle_pulse);
  RUN_TEST(test_continuous_servo_runs_from_full_reverse_to_full_forward);
  return UNITY_END();
}
