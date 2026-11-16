#include <host_test.h>
#include <pwm_out_pin.h>

#include <cmath>

// A pulse's duty is its share of the period: 1500 µs at 50 Hz is 0.075 of 20 ms.

void test_duty_is_the_level_share_of_duty_max(void) { TEST_ASSERT_EQUAL(4915, PwmOutPin::duty(0.075f)); }

void test_duty_rounds_to_the_nearest_step(void) {
  // 6553.5, 13107 and 3276.7 steps.
  TEST_ASSERT_EQUAL(6554, PwmOutPin::duty(0.1f));
  TEST_ASSERT_EQUAL(13107, PwmOutPin::duty(0.2f));
  TEST_ASSERT_EQUAL(3277, PwmOutPin::duty(0.0499995f));
}

void test_level_of_one_or_more_keeps_the_pin_high(void) {
  TEST_ASSERT_EQUAL(PwmOutPin::DUTY_MAX, PwmOutPin::duty(1.0f));
  TEST_ASSERT_EQUAL(PwmOutPin::DUTY_MAX, PwmOutPin::duty(1.5f));
}

void test_zero_or_negative_level_keeps_the_pin_low(void) {
  TEST_ASSERT_EQUAL(0, PwmOutPin::duty(0.0f));
  TEST_ASSERT_EQUAL(0, PwmOutPin::duty(-0.5f));
}

void test_nan_level_keeps_the_pin_low(void) { TEST_ASSERT_EQUAL(0, PwmOutPin::duty(NAN)); }

int main(int, char**) {
  UNITY_BEGIN();
  RUN_TEST(test_duty_is_the_level_share_of_duty_max);
  RUN_TEST(test_duty_rounds_to_the_nearest_step);
  RUN_TEST(test_level_of_one_or_more_keeps_the_pin_high);
  RUN_TEST(test_zero_or_negative_level_keeps_the_pin_low);
  RUN_TEST(test_nan_level_keeps_the_pin_low);
  return UNITY_END();
}
