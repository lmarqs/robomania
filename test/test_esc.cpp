#include <esc.h>
#include <host_test.h>

#include "fake_pwm_out_pin.h"

// Duties on a 16-bit pin at 50 Hz: 1000 µs is 3277 steps, 1500 µs 4915, 2000 µs 6554.

void test_esc_arms_at_the_minimum_pulse(void) {
  FakePwmOutPin pin;
  Esc esc(&pin);
  TEST_ASSERT_TRUE(esc.begin());
  TEST_ASSERT_EQUAL(1, pin.writes);
  TEST_ASSERT_EQUAL(3277, pin.duty);
}

void test_esc_runs_from_minimum_at_zero_to_maximum_at_one(void) {
  FakePwmOutPin pin;
  Esc esc(&pin);
  esc.begin();
  esc.write(1.0f);
  TEST_ASSERT_EQUAL(6554, pin.duty);
  TEST_ASSERT_FLOAT_WITHIN(1e-3f, 1500.0f, esc.pulseUs(0.5f));
}

void test_esc_holds_a_negative_throttle_at_the_minimum(void) {
  FakePwmOutPin pin;
  Esc esc(&pin);
  esc.begin();
  esc.write(-1.0f);
  TEST_ASSERT_EQUAL(3277, pin.duty);
}

void test_esc_that_cannot_start_sends_nothing(void) {
  FakePwmOutPin pin;
  pin.starts = false;
  Esc esc(&pin);
  TEST_ASSERT_FALSE(esc.begin());
  esc.write(1.0f);
  TEST_ASSERT_EQUAL(0, pin.writes);
}

void test_reversible_esc_arms_at_neutral(void) {
  FakePwmOutPin pin;
  ReversibleEsc esc(&pin);
  TEST_ASSERT_TRUE(esc.begin());
  TEST_ASSERT_EQUAL(1, pin.writes);
  TEST_ASSERT_EQUAL(4915, pin.duty);
}

void test_reversible_esc_runs_from_reverse_at_minus_one_to_forward_at_one(void) {
  FakePwmOutPin pin;
  ReversibleEsc esc(&pin);
  esc.begin();
  esc.write(-1.0f);
  TEST_ASSERT_EQUAL(3277, pin.duty);
  esc.write(1.0f);
  TEST_ASSERT_EQUAL(6554, pin.duty);
}

void test_esc_and_reversible_esc_stop_with_different_pulses(void) {
  FakePwmOutPin pin;
  TEST_ASSERT_FLOAT_WITHIN(1e-3f, 1000.0f, Esc(&pin).pulseUs(0.0f));
  TEST_ASSERT_FLOAT_WITHIN(1e-3f, 1500.0f, ReversibleEsc(&pin).pulseUs(0.0f));
}

int main(int, char**) {
  UNITY_BEGIN();
  RUN_TEST(test_esc_arms_at_the_minimum_pulse);
  RUN_TEST(test_esc_runs_from_minimum_at_zero_to_maximum_at_one);
  RUN_TEST(test_esc_holds_a_negative_throttle_at_the_minimum);
  RUN_TEST(test_esc_that_cannot_start_sends_nothing);
  RUN_TEST(test_reversible_esc_arms_at_neutral);
  RUN_TEST(test_reversible_esc_runs_from_reverse_at_minus_one_to_forward_at_one);
  RUN_TEST(test_esc_and_reversible_esc_stop_with_different_pulses);
  return UNITY_END();
}
