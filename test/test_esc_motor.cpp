#include <esc_motor.h>
#include <host_test.h>
#include <math.h>

#include "fake_pwm_out_pin.h"

// Duties on a 16-bit pin at 50 Hz: 1000 µs is 3277 steps, 1500 µs 4915, 2000 µs 6554.

void test_esc_arms_at_the_minimum_pulse(void) {
  FakePwmOutPin pin;
  EscMotor esc(&pin);
  TEST_ASSERT_TRUE(esc.begin());
  TEST_ASSERT_EQUAL(1, pin.writes);
  TEST_ASSERT_EQUAL(3277, pin.duty);
}

void test_esc_runs_from_minimum_at_zero_to_maximum_at_one(void) {
  FakePwmOutPin pin;
  EscMotor esc(&pin);
  esc.begin();
  esc.write(1.0f);
  TEST_ASSERT_EQUAL(6554, pin.duty);
  TEST_ASSERT_FLOAT_WITHIN(1e-3f, 1500.0f, esc.pulseUs(0.5f));
}

void test_esc_holds_a_negative_throttle_at_the_minimum(void) {
  FakePwmOutPin pin;
  EscMotor esc(&pin);
  esc.begin();
  esc.write(-1.0f);
  TEST_ASSERT_EQUAL(3277, pin.duty);
}

void test_esc_that_cannot_start_sends_nothing(void) {
  FakePwmOutPin pin;
  pin.starts = false;
  EscMotor esc(&pin);
  TEST_ASSERT_FALSE(esc.begin());
  esc.write(1.0f);
  TEST_ASSERT_EQUAL(0, pin.writes);
}
void test_esc_stops_at_the_minimum_pulse(void) {
  FakePwmOutPin pin;
  TEST_ASSERT_FLOAT_WITHIN(1e-3f, 1000.0f, EscMotor(&pin).pulseUs(0.0f));
}

void test_esc_stops_on_a_nan_throttle(void) {
  FakePwmOutPin pin;
  EscMotor esc(&pin);
  esc.begin();
  esc.write(1.0f);
  esc.write(NAN);
  TEST_ASSERT_EQUAL(3277, pin.duty);
}

int main(int, char**) {
  UNITY_BEGIN();
  RUN_TEST(test_esc_arms_at_the_minimum_pulse);
  RUN_TEST(test_esc_runs_from_minimum_at_zero_to_maximum_at_one);
  RUN_TEST(test_esc_holds_a_negative_throttle_at_the_minimum);
  RUN_TEST(test_esc_that_cannot_start_sends_nothing);
  RUN_TEST(test_esc_stops_at_the_minimum_pulse);
  RUN_TEST(test_esc_stops_on_a_nan_throttle);
  return UNITY_END();
}
