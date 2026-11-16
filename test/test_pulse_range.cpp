#include <host_test.h>
#include <pulse_range.h>

#include <cmath>

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

void test_position_is_the_inverse_of_pulse(void) {
  TEST_ASSERT_FLOAT_WITHIN(1e-6f, -1.0f, range().position(1000.0f));
  TEST_ASSERT_FLOAT_WITHIN(1e-6f, 0.0f, range().position(1500.0f));
  TEST_ASSERT_FLOAT_WITHIN(1e-6f, 0.5f, range().position(1750.0f));
  TEST_ASSERT_FLOAT_WITHIN(1e-6f, 1.0f, range().position(2000.0f));
}

void test_position_scales_each_side_on_its_own(void) {
  PulseRange uneven(1100.0f, 1500.0f, 1700.0f);
  TEST_ASSERT_FLOAT_WITHIN(1e-6f, -0.5f, uneven.position(1300.0f));
  TEST_ASSERT_FLOAT_WITHIN(1e-6f, 0.5f, uneven.position(1600.0f));
}

void test_position_clamps_a_pulse_past_either_end(void) {
  TEST_ASSERT_FLOAT_WITHIN(1e-6f, 1.0f, range().position(2100.0f));
  TEST_ASSERT_FLOAT_WITHIN(1e-6f, -1.0f, range().position(900.0f));
}

void test_reversed_range_reads_the_shorter_pulse_as_position_one(void) {
  PulseRange reversed(2000.0f, 1500.0f, 1000.0f);
  TEST_ASSERT_FLOAT_WITHIN(1e-6f, 1.0f, reversed.position(1000.0f));
  TEST_ASSERT_FLOAT_WITHIN(1e-6f, -0.5f, reversed.position(1750.0f));
}

void test_one_way_range_reads_its_low_end_as_zero_and_below_it_as_minus_one(void) {
  PulseRange forwardOnly(1000.0f, 1000.0f, 2000.0f);
  TEST_ASSERT_FLOAT_WITHIN(1e-6f, 0.0f, forwardOnly.position(1000.0f));
  TEST_ASSERT_FLOAT_WITHIN(1e-6f, 0.25f, forwardOnly.position(1250.0f));
  TEST_ASSERT_FLOAT_WITHIN(1e-6f, -1.0f, forwardOnly.position(900.0f));
}

void test_position_of_a_nan_pulse_is_zero(void) { TEST_ASSERT_FLOAT_WITHIN(1e-6f, 0.0f, range().position(NAN)); }

int main(int, char**) {
  UNITY_BEGIN();
  RUN_TEST(test_range_maps_minus_one_zero_and_one_to_its_three_pulses);
  RUN_TEST(test_range_scales_each_side_on_its_own);
  RUN_TEST(test_range_clamps_a_position_past_either_end);
  RUN_TEST(test_reversed_range_maps_position_one_to_the_shorter_pulse);
  RUN_TEST(test_range_with_low_at_center_runs_one_way);
  RUN_TEST(test_range_maps_a_nan_position_to_its_center);
  RUN_TEST(test_position_is_the_inverse_of_pulse);
  RUN_TEST(test_position_scales_each_side_on_its_own);
  RUN_TEST(test_position_clamps_a_pulse_past_either_end);
  RUN_TEST(test_reversed_range_reads_the_shorter_pulse_as_position_one);
  RUN_TEST(test_one_way_range_reads_its_low_end_as_zero_and_below_it_as_minus_one);
  RUN_TEST(test_position_of_a_nan_pulse_is_zero);
  return UNITY_END();
}
