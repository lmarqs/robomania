#include <host_test.h>
#include <pulse_out.h>

#include <cmath>

#include "fake_pwm_out_pin.h"

static PulseRange servoRange() { return PulseRange(1000.0f, 1500.0f, 2000.0f); }

void test_write_before_begin_sends_nothing(void) {
  FakePwmOutPin pin;
  PulseOut out(&pin, servoRange());
  out.write(0.5f);
  TEST_ASSERT_EQUAL(0, pin.writes);
}

void test_begin_starts_the_pin_and_sends_nothing(void) {
  FakePwmOutPin pin;
  PulseOut out(&pin, servoRange());
  TEST_ASSERT_TRUE(out.begin());
  TEST_ASSERT_TRUE(pin.started);
  TEST_ASSERT_EQUAL(0, pin.writes);
}

void test_write_after_a_failed_begin_sends_nothing(void) {
  FakePwmOutPin pin;
  pin.starts = false;
  PulseOut out(&pin, servoRange());
  TEST_ASSERT_FALSE(out.begin());
  out.write(0.0f);
  TEST_ASSERT_EQUAL(0, pin.writes);
}

void test_write_sends_the_duty_of_the_pulse_for_the_position(void) {
  FakePwmOutPin pin;
  PulseOut out(&pin, servoRange());
  out.begin();
  // At 50 Hz: 1000 µs is 3276.75 steps, 1500 µs 4915.1 and 2000 µs 6553.5.
  out.write(-1.0f);
  TEST_ASSERT_EQUAL(3277, pin.duty);
  out.write(0.0f);
  TEST_ASSERT_EQUAL(4915, pin.duty);
  out.write(1.0f);
  TEST_ASSERT_EQUAL(6554, pin.duty);
}

void test_write_uses_the_frequency_the_pin_runs_at(void) {
  FakePwmOutPin pin;
  pin.hz = 200;
  PulseOut out(&pin, servoRange());
  out.begin();
  out.write(-1.0f);
  TEST_ASSERT_EQUAL(13107, pin.duty);
}

void test_write_clamps_a_position_past_either_end(void) {
  FakePwmOutPin pin;
  PulseOut out(&pin, servoRange());
  out.begin();
  out.write(3.0f);
  TEST_ASSERT_EQUAL(6554, pin.duty);
}

void test_write_sends_the_centre_pulse_for_a_nan_position(void) {
  FakePwmOutPin pin;
  PulseOut out(&pin, servoRange());
  out.begin();
  out.write(NAN);
  TEST_ASSERT_EQUAL(4915, pin.duty);
}

void test_pulse_is_the_range_pulse_for_the_position(void) {
  FakePwmOutPin pin;
  PulseOut out(&pin, servoRange());
  TEST_ASSERT_FLOAT_WITHIN(1e-3f, 1750.0f, out.pulseUs(0.5f));
}

int main(int, char**) {
  UNITY_BEGIN();
  RUN_TEST(test_write_before_begin_sends_nothing);
  RUN_TEST(test_begin_starts_the_pin_and_sends_nothing);
  RUN_TEST(test_write_after_a_failed_begin_sends_nothing);
  RUN_TEST(test_write_sends_the_duty_of_the_pulse_for_the_position);
  RUN_TEST(test_write_uses_the_frequency_the_pin_runs_at);
  RUN_TEST(test_write_clamps_a_position_past_either_end);
  RUN_TEST(test_write_sends_the_centre_pulse_for_a_nan_position);
  RUN_TEST(test_pulse_is_the_range_pulse_for_the_position);
  return UNITY_END();
}
