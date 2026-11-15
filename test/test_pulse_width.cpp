#include <host_test.h>
#include <pulse_width.h>

#include <cmath>

void test_duty_is_the_pulse_share_of_the_period(void) {
  // 1500 µs of a 20 ms period is 7.5 %, and 65535 × 0.075 is 4915.125.
  TEST_ASSERT_EQUAL(4915, pulseDuty(1500.0f, 50));
}

void test_duty_rounds_to_the_nearest_step(void) {
  // 2000 µs at 50 Hz is 6553.5 steps; 1000 µs at 200 Hz is 13107.
  TEST_ASSERT_EQUAL(6554, pulseDuty(2000.0f, 50));
  TEST_ASSERT_EQUAL(13107, pulseDuty(1000.0f, 200));
  // 999.99 µs at 50 Hz is 3276.7 steps, which a floor would make 3276.
  TEST_ASSERT_EQUAL(3277, pulseDuty(999.99f, 50));
}

void test_pulse_longer_than_the_period_keeps_the_pin_high(void) {
  TEST_ASSERT_EQUAL(PULSE_DUTY_MAX, pulseDuty(20000.0f, 50));
  TEST_ASSERT_EQUAL(PULSE_DUTY_MAX, pulseDuty(30000.0f, 50));
}

void test_zero_or_negative_pulse_keeps_the_pin_low(void) {
  TEST_ASSERT_EQUAL(0, pulseDuty(0.0f, 50));
  TEST_ASSERT_EQUAL(0, pulseDuty(-10.0f, 50));
}

void test_nan_pulse_keeps_the_pin_low(void) { TEST_ASSERT_EQUAL(0, pulseDuty(NAN, 50)); }

static PulseRange range() { return PulseRange(1000.0f, 1500.0f, 2000.0f); }

void test_range_maps_minus_one_zero_and_one_to_its_three_pulses(void) {
  TEST_ASSERT_FLOAT_WITHIN(1e-3f, 1000.0f, range().pulseUs(-1.0f));
  TEST_ASSERT_FLOAT_WITHIN(1e-3f, 1500.0f, range().pulseUs(0.0f));
  TEST_ASSERT_FLOAT_WITHIN(1e-3f, 2000.0f, range().pulseUs(1.0f));
}

void test_range_scales_each_side_on_its_own(void) {
  PulseRange uneven(1100.0f, 1500.0f, 1700.0f);
  TEST_ASSERT_FLOAT_WITHIN(1e-3f, 1300.0f, uneven.pulseUs(-0.5f));
  TEST_ASSERT_FLOAT_WITHIN(1e-3f, 1600.0f, uneven.pulseUs(0.5f));
}

void test_range_clamps_a_position_past_either_end(void) {
  TEST_ASSERT_FLOAT_WITHIN(1e-3f, 2000.0f, range().pulseUs(1.5f));
  TEST_ASSERT_FLOAT_WITHIN(1e-3f, 1000.0f, range().pulseUs(-2.0f));
}

void test_reversed_range_maps_position_one_to_the_shorter_pulse(void) {
  PulseRange reversed(2000.0f, 1500.0f, 1000.0f);
  TEST_ASSERT_FLOAT_WITHIN(1e-3f, 1000.0f, reversed.pulseUs(1.0f));
  TEST_ASSERT_FLOAT_WITHIN(1e-3f, 1750.0f, reversed.pulseUs(-0.5f));
}

void test_range_with_low_at_center_runs_one_way(void) {
  PulseRange forwardOnly(1000.0f, 1000.0f, 2000.0f);
  TEST_ASSERT_FLOAT_WITHIN(1e-3f, 1250.0f, forwardOnly.pulseUs(0.25f));
  TEST_ASSERT_FLOAT_WITHIN(1e-3f, 1000.0f, forwardOnly.pulseUs(-1.0f));
}

void test_range_maps_a_nan_position_to_its_center(void) {
  TEST_ASSERT_FLOAT_WITHIN(1e-3f, 1500.0f, range().pulseUs(NAN));
}

int main(int, char**) {
  UNITY_BEGIN();
  RUN_TEST(test_duty_is_the_pulse_share_of_the_period);
  RUN_TEST(test_duty_rounds_to_the_nearest_step);
  RUN_TEST(test_pulse_longer_than_the_period_keeps_the_pin_high);
  RUN_TEST(test_zero_or_negative_pulse_keeps_the_pin_low);
  RUN_TEST(test_nan_pulse_keeps_the_pin_low);
  RUN_TEST(test_range_maps_minus_one_zero_and_one_to_its_three_pulses);
  RUN_TEST(test_range_scales_each_side_on_its_own);
  RUN_TEST(test_range_clamps_a_position_past_either_end);
  RUN_TEST(test_reversed_range_maps_position_one_to_the_shorter_pulse);
  RUN_TEST(test_range_with_low_at_center_runs_one_way);
  RUN_TEST(test_range_maps_a_nan_position_to_its_center);
  return UNITY_END();
}
