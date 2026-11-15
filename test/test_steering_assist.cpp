#include <host_test.h>
#include <steering_assist.h>

void test_zero_gain_passes_driver_steering(void) {
  SteeringAssist assist(200.0f);
  TEST_ASSERT_FLOAT_WITHIN(1e-6f, 0.4f, assist.steer(0.4f, 150.0f, 0.0f));
}

void test_clockwise_spin_steers_left(void) {
  SteeringAssist assist(200.0f);
  TEST_ASSERT_FLOAT_WITHIN(1e-6f, -0.25f, assist.steer(0.0f, 100.0f, 0.5f));
}

void test_counter_clockwise_spin_steers_right(void) {
  SteeringAssist assist(200.0f);
  TEST_ASSERT_FLOAT_WITHIN(1e-6f, 0.25f, assist.steer(0.0f, -100.0f, 0.5f));
}

void test_full_gain_at_full_lock_rate_reaches_lock(void) {
  SteeringAssist assist(200.0f);
  TEST_ASSERT_FLOAT_WITHIN(1e-6f, -1.0f, assist.steer(0.0f, 200.0f, 1.0f));
}

void test_counter_steer_adds_to_driver_steering(void) {
  SteeringAssist assist(200.0f);
  TEST_ASSERT_FLOAT_WITHIN(1e-6f, 0.3f, assist.steer(0.5f, 40.0f, 1.0f));
}

void test_output_clamped_to_lock(void) {
  SteeringAssist assist(200.0f);
  TEST_ASSERT_FLOAT_WITHIN(1e-6f, -1.0f, assist.steer(-0.8f, 400.0f, 1.0f));
  TEST_ASSERT_FLOAT_WITHIN(1e-6f, 1.0f, assist.steer(0.8f, -400.0f, 1.0f));
}

void test_gain_clamped_to_unit_range(void) {
  SteeringAssist assist(200.0f);
  TEST_ASSERT_FLOAT_WITHIN(1e-6f, -0.5f, assist.steer(0.0f, 100.0f, 3.0f));
  TEST_ASSERT_FLOAT_WITHIN(1e-6f, 0.0f, assist.steer(0.0f, 100.0f, -1.0f));
}

int main(int, char**) {
  UNITY_BEGIN();
  RUN_TEST(test_zero_gain_passes_driver_steering);
  RUN_TEST(test_clockwise_spin_steers_left);
  RUN_TEST(test_counter_clockwise_spin_steers_right);
  RUN_TEST(test_full_gain_at_full_lock_rate_reaches_lock);
  RUN_TEST(test_counter_steer_adds_to_driver_steering);
  RUN_TEST(test_output_clamped_to_lock);
  RUN_TEST(test_gain_clamped_to_unit_range);
  return UNITY_END();
}
