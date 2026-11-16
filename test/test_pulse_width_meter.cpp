#include <host_test.h>
#include <pulse_width_meter.h>

void test_width_is_zero_before_any_pulse(void) {
  PulseWidthMeter meter;
  TEST_ASSERT_EQUAL(0, meter.widthUs(1000, 50000));
}

void test_width_is_the_time_from_rising_to_falling_edge(void) {
  PulseWidthMeter meter;
  meter.recordEdge(true, 1000);
  meter.recordEdge(false, 2500);
  TEST_ASSERT_EQUAL(1500, meter.widthUs(2600, 50000));
}

void test_a_falling_edge_before_any_rising_one_is_ignored(void) {
  PulseWidthMeter meter;
  meter.recordEdge(false, 1200);
  TEST_ASSERT_EQUAL(0, meter.widthUs(1300, 50000));
}

void test_width_is_zero_once_the_pulse_is_older_than_max_age(void) {
  PulseWidthMeter meter;
  meter.recordEdge(true, 1000);
  meter.recordEdge(false, 2500);
  TEST_ASSERT_EQUAL(1500, meter.widthUs(52500, 50000));
  TEST_ASSERT_EQUAL(0, meter.widthUs(52501, 50000));
}

void test_width_holds_the_last_pulse_while_a_new_one_is_high(void) {
  PulseWidthMeter meter;
  meter.recordEdge(true, 1000);
  meter.recordEdge(false, 2500);
  meter.recordEdge(true, 21000);
  TEST_ASSERT_EQUAL(1500, meter.widthUs(21500, 50000));
}

void test_width_spans_the_micros_wrap(void) {
  PulseWidthMeter meter;
  meter.recordEdge(true, 0xFFFFFF00UL);
  meter.recordEdge(false, 0x00000400UL);
  TEST_ASSERT_EQUAL(1280, meter.widthUs(0x00000500UL, 50000));
}

void test_an_expired_pulse_stays_forgotten_after_the_age_wraps_back(void) {
  PulseWidthMeter meter;
  meter.recordEdge(true, 1000);
  meter.recordEdge(false, 2500);
  meter.forgetExpired(52501, 50000);
  TEST_ASSERT_EQUAL(0, meter.widthUs(2600, 50000));
}

void test_a_pulse_exactly_at_the_age_limit_is_kept(void) {
  PulseWidthMeter meter;
  meter.recordEdge(true, 1000);
  meter.recordEdge(false, 2500);
  meter.forgetExpired(52500, 50000);
  TEST_ASSERT_EQUAL(1500, meter.widthUs(52500, 50000));
}

void test_a_pulse_after_forgetting_is_measured_normally(void) {
  PulseWidthMeter meter;
  meter.recordEdge(true, 1000);
  meter.recordEdge(false, 2500);
  meter.forgetExpired(52501, 50000);
  meter.recordEdge(true, 60000);
  meter.recordEdge(false, 61200);
  TEST_ASSERT_EQUAL(1200, meter.widthUs(61300, 50000));
}

int main(int, char**) {
  UNITY_BEGIN();
  RUN_TEST(test_width_is_zero_before_any_pulse);
  RUN_TEST(test_width_is_the_time_from_rising_to_falling_edge);
  RUN_TEST(test_a_falling_edge_before_any_rising_one_is_ignored);
  RUN_TEST(test_width_is_zero_once_the_pulse_is_older_than_max_age);
  RUN_TEST(test_width_holds_the_last_pulse_while_a_new_one_is_high);
  RUN_TEST(test_width_spans_the_micros_wrap);
  RUN_TEST(test_an_expired_pulse_stays_forgotten_after_the_age_wraps_back);
  RUN_TEST(test_a_pulse_exactly_at_the_age_limit_is_kept);
  RUN_TEST(test_a_pulse_after_forgetting_is_measured_normally);
  return UNITY_END();
}
