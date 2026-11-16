#include <host_test.h>
#include <math.h>
#include <pulse_motor.h>

#include "fake_pwm_out_pin.h"

// Duties on a 16-bit pin at 50 Hz: 1000 µs is 3277 steps, 1500 µs 4915, 2000 µs 6554.

static PulseRange reversible() { return PulseRange::linear(1000.0f, 2000.0f); }
static PulseRange oneWay() { return PulseRange::oneWay(1000.0f, 2000.0f); }

void test_begin_sends_the_stop_pulse(void) {
  FakePwmOutPin pin;
  PulseMotor motor(&pin, reversible());
  TEST_ASSERT_TRUE(motor.begin());
  TEST_ASSERT_EQUAL(1, pin.writes);
  TEST_ASSERT_EQUAL(4915, pin.duty);
}

void test_a_motor_that_cannot_start_sends_nothing(void) {
  FakePwmOutPin pin;
  pin.starts = false;
  PulseMotor motor(&pin, reversible());
  TEST_ASSERT_FALSE(motor.begin());
  motor.write(1.0f);
  TEST_ASSERT_EQUAL(0, pin.writes);
}

void test_runs_from_full_reverse_to_full_forward(void) {
  FakePwmOutPin pin;
  PulseMotor motor(&pin, reversible());
  motor.begin();
  motor.write(-1.0f);
  TEST_ASSERT_EQUAL(3277, pin.duty);
  motor.write(1.0f);
  TEST_ASSERT_EQUAL(6554, pin.duty);
}

void test_stops_on_a_nan_throttle(void) {
  FakePwmOutPin pin;
  PulseMotor motor(&pin, reversible());
  motor.begin();
  motor.write(-1.0f);
  motor.write(NAN);
  TEST_ASSERT_EQUAL(4915, pin.duty);
}

void test_one_way_motor_arms_at_the_low_pulse(void) {
  FakePwmOutPin pin;
  PulseMotor motor(&pin, oneWay());
  motor.begin();
  TEST_ASSERT_EQUAL(3277, pin.duty);
}

void test_one_way_motor_runs_from_low_at_zero_to_high_at_one(void) {
  FakePwmOutPin pin;
  PulseMotor motor(&pin, oneWay());
  motor.begin();
  motor.write(1.0f);
  TEST_ASSERT_EQUAL(6554, pin.duty);
  TEST_ASSERT_FLOAT_WITHIN(1e-3f, 1500.0f, motor.pulseUs(0.5f));
}

void test_one_way_motor_stops_for_a_negative_throttle(void) {
  FakePwmOutPin pin;
  PulseMotor motor(&pin, oneWay());
  motor.begin();
  motor.write(1.0f);
  motor.write(-1.0f);
  TEST_ASSERT_EQUAL(3277, pin.duty);
}

int main(int, char**) {
  UNITY_BEGIN();
  RUN_TEST(test_begin_sends_the_stop_pulse);
  RUN_TEST(test_a_motor_that_cannot_start_sends_nothing);
  RUN_TEST(test_runs_from_full_reverse_to_full_forward);
  RUN_TEST(test_stops_on_a_nan_throttle);
  RUN_TEST(test_one_way_motor_arms_at_the_low_pulse);
  RUN_TEST(test_one_way_motor_runs_from_low_at_zero_to_high_at_one);
  RUN_TEST(test_one_way_motor_stops_for_a_negative_throttle);
  return UNITY_END();
}
