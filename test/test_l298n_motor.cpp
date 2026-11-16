#include <host_test.h>
#include <l298n_motor.h>
#include <math.h>

#include "fake_digital_out_pin.h"
#include "fake_pwm_out_pin.h"

struct Bridge {
  FakePwmOutPin enable;
  FakeDigitalOutPin in1, in2;
  L298nMotor motor{&enable, &in1, &in2};
};

void test_l298n_starts_its_pins_stopped(void) {
  Bridge bridge;
  TEST_ASSERT_TRUE(bridge.motor.begin());
  TEST_ASSERT_TRUE(bridge.enable.started && bridge.in1.started && bridge.in2.started);
  TEST_ASSERT_FALSE(bridge.in1.on || bridge.in2.on);
  TEST_ASSERT_EQUAL(0, bridge.enable.duty);
}

void test_l298n_reports_a_pin_that_cannot_start(void) {
  Bridge bridge;
  bridge.enable.starts = false;
  TEST_ASSERT_FALSE(bridge.motor.begin());
}

void test_l298n_drives_forward_with_in1_and_the_throttle_as_duty(void) {
  Bridge bridge;
  bridge.motor.begin();
  bridge.motor.write(0.5f);
  TEST_ASSERT_TRUE(bridge.in1.on);
  TEST_ASSERT_FALSE(bridge.in2.on);
  TEST_ASSERT_EQUAL(32768, bridge.enable.duty);
}

void test_l298n_drives_backward_with_in2(void) {
  Bridge bridge;
  bridge.motor.begin();
  bridge.motor.write(-1.0f);
  TEST_ASSERT_FALSE(bridge.in1.on);
  TEST_ASSERT_TRUE(bridge.in2.on);
  TEST_ASSERT_EQUAL(65535, bridge.enable.duty);
}

void test_l298n_coasts_at_zero(void) {
  Bridge bridge;
  bridge.motor.begin();
  bridge.motor.write(1.0f);
  bridge.motor.write(0.0f);
  TEST_ASSERT_FALSE(bridge.in1.on || bridge.in2.on);
  TEST_ASSERT_EQUAL(0, bridge.enable.duty);
}

void test_l298n_clamps_past_full_and_stops_on_nan(void) {
  Bridge bridge;
  bridge.motor.begin();
  bridge.motor.write(2.0f);
  TEST_ASSERT_EQUAL(65535, bridge.enable.duty);
  bridge.motor.write(NAN);
  TEST_ASSERT_FALSE(bridge.in1.on || bridge.in2.on);
  TEST_ASSERT_EQUAL(0, bridge.enable.duty);
}

int main(int, char**) {
  UNITY_BEGIN();
  RUN_TEST(test_l298n_starts_its_pins_stopped);
  RUN_TEST(test_l298n_reports_a_pin_that_cannot_start);
  RUN_TEST(test_l298n_drives_forward_with_in1_and_the_throttle_as_duty);
  RUN_TEST(test_l298n_drives_backward_with_in2);
  RUN_TEST(test_l298n_coasts_at_zero);
  RUN_TEST(test_l298n_clamps_past_full_and_stops_on_nan);
  return UNITY_END();
}
