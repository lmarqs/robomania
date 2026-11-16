#include <host_test.h>
#include <math.h>
#include <reversible_esc_motor.h>

#include "fake_pwm_out_pin.h"

// Duties on a 16-bit pin at 50 Hz: 1000 µs is 3277 steps, 1500 µs 4915, 2000 µs 6554.

void test_reversible_esc_arms_at_neutral(void) {
  FakePwmOutPin pin;
  ReversibleEscMotor esc(&pin);
  TEST_ASSERT_TRUE(esc.begin());
  TEST_ASSERT_EQUAL(1, pin.writes);
  TEST_ASSERT_EQUAL(4915, pin.duty);
}

void test_reversible_esc_runs_from_reverse_at_minus_one_to_forward_at_one(void) {
  FakePwmOutPin pin;
  ReversibleEscMotor esc(&pin);
  esc.begin();
  esc.write(-1.0f);
  TEST_ASSERT_EQUAL(3277, pin.duty);
  esc.write(1.0f);
  TEST_ASSERT_EQUAL(6554, pin.duty);
}
void test_reversible_esc_stops_at_the_neutral_pulse(void) {
  FakePwmOutPin pin;
  TEST_ASSERT_FLOAT_WITHIN(1e-3f, 1500.0f, ReversibleEscMotor(&pin).pulseUs(0.0f));
}

void test_reversible_esc_stops_on_a_nan_throttle(void) {
  FakePwmOutPin pin;
  ReversibleEscMotor esc(&pin);
  esc.begin();
  esc.write(-1.0f);
  esc.write(NAN);
  TEST_ASSERT_EQUAL(4915, pin.duty);
}

int main(int, char**) {
  UNITY_BEGIN();
  RUN_TEST(test_reversible_esc_arms_at_neutral);
  RUN_TEST(test_reversible_esc_runs_from_reverse_at_minus_one_to_forward_at_one);
  RUN_TEST(test_reversible_esc_stops_at_the_neutral_pulse);
  RUN_TEST(test_reversible_esc_stops_on_a_nan_throttle);
  return UNITY_END();
}
