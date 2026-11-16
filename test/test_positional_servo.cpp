#include <host_test.h>
#include <positional_servo.h>

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
  servo.write(0.0f);
  TEST_ASSERT_EQUAL(1783, pin.duty);
  servo.write(90.0f);
  TEST_ASSERT_EQUAL(4823, pin.duty);
  servo.write(180.0f);
  TEST_ASSERT_EQUAL(7864, pin.duty);
}

void test_positional_servo_takes_a_fraction_of_a_degree(void) {
  FakePwmOutPin pin;
  PositionalServo servo(&pin, 180.0f, 1000, 2000);
  TEST_ASSERT_FLOAT_WITHIN(1e-3f, 1502.7778f, servo.pulseUs(90.5f));
}

void test_positional_servo_clamps_an_angle_past_its_range(void) {
  FakePwmOutPin pin;
  PositionalServo servo(&pin);
  TEST_ASSERT_FLOAT_WITHIN(1e-3f, 2400.0f, servo.pulseUs(270.0f));
  TEST_ASSERT_FLOAT_WITHIN(1e-3f, 544.0f, servo.pulseUs(-10.0f));
}

void test_positional_servo_scales_to_its_range(void) {
  FakePwmOutPin pin;
  PositionalServo servo(&pin, 270.0f, 500, 2500);
  TEST_ASSERT_FLOAT_WITHIN(1e-3f, 1500.0f, servo.pulseUs(135.0f));
}

int main(int, char**) {
  UNITY_BEGIN();
  RUN_TEST(test_positional_servo_stays_limp_until_the_first_write);
  RUN_TEST(test_positional_servo_maps_0_90_and_180_degrees_to_its_pulses);
  RUN_TEST(test_positional_servo_takes_a_fraction_of_a_degree);
  RUN_TEST(test_positional_servo_clamps_an_angle_past_its_range);
  RUN_TEST(test_positional_servo_scales_to_its_range);
  return UNITY_END();
}
