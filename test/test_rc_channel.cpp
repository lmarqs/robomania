#include <host_test.h>
#include <rc_channel.h>

static RcChannel channel() { return RcChannel(1000, 1500, 2000, 20); }

void test_centre_pulse_is_position_zero(void) { TEST_ASSERT_FLOAT_WITHIN(1e-6f, 0.0f, channel().position(1500)); }

void test_pulse_inside_dead_zone_is_position_zero(void) {
  TEST_ASSERT_FLOAT_WITHIN(1e-6f, 0.0f, channel().position(1519));
  TEST_ASSERT_FLOAT_WITHIN(1e-6f, 0.0f, channel().position(1481));
}

void test_high_pulse_is_position_one(void) { TEST_ASSERT_FLOAT_WITHIN(1e-6f, 1.0f, channel().position(2000)); }

void test_low_pulse_is_position_minus_one(void) { TEST_ASSERT_FLOAT_WITHIN(1e-6f, -1.0f, channel().position(1000)); }

void test_half_way_up_is_position_one_half(void) { TEST_ASSERT_FLOAT_WITHIN(1e-6f, 0.5f, channel().position(1750)); }

void test_uneven_sides_scale_each_side_on_its_own(void) {
  RcChannel uneven(1100, 1500, 1700, 0);
  TEST_ASSERT_FLOAT_WITHIN(1e-6f, -0.5f, uneven.position(1300));
  TEST_ASSERT_FLOAT_WITHIN(1e-6f, 0.5f, uneven.position(1600));
}

void test_pulse_past_the_ends_is_clamped(void) {
  TEST_ASSERT_FLOAT_WITHIN(1e-6f, 1.0f, channel().position(2100));
  TEST_ASSERT_FLOAT_WITHIN(1e-6f, -1.0f, channel().position(900));
}

void test_reversed_channel_maps_low_pulse_to_position_one(void) {
  RcChannel reversed(2000, 1500, 1000, 0);
  TEST_ASSERT_FLOAT_WITHIN(1e-6f, 1.0f, reversed.position(1000));
  TEST_ASSERT_FLOAT_WITHIN(1e-6f, -0.5f, reversed.position(1750));
}

void test_level_runs_from_zero_at_low_to_one_at_high(void) {
  TEST_ASSERT_FLOAT_WITHIN(1e-6f, 0.0f, channel().level(1000));
  TEST_ASSERT_FLOAT_WITHIN(1e-6f, 0.5f, channel().level(1500));
  TEST_ASSERT_FLOAT_WITHIN(1e-6f, 1.0f, channel().level(2000));
}

void test_level_is_clamped(void) {
  TEST_ASSERT_FLOAT_WITHIN(1e-6f, 0.0f, channel().level(800));
  TEST_ASSERT_FLOAT_WITHIN(1e-6f, 1.0f, channel().level(2200));
}

int main(int, char**) {
  UNITY_BEGIN();
  RUN_TEST(test_centre_pulse_is_position_zero);
  RUN_TEST(test_pulse_inside_dead_zone_is_position_zero);
  RUN_TEST(test_high_pulse_is_position_one);
  RUN_TEST(test_low_pulse_is_position_minus_one);
  RUN_TEST(test_half_way_up_is_position_one_half);
  RUN_TEST(test_uneven_sides_scale_each_side_on_its_own);
  RUN_TEST(test_pulse_past_the_ends_is_clamped);
  RUN_TEST(test_reversed_channel_maps_low_pulse_to_position_one);
  RUN_TEST(test_level_runs_from_zero_at_low_to_one_at_high);
  RUN_TEST(test_level_is_clamped);
  return UNITY_END();
}
