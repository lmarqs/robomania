#include <drv8833_motor.h>
#include <host_test.h>

#include "fake_pwm_out_pin.h"

struct Bridge {
  FakePwmOutPin in1, in2;
  Drv8833Motor motor{&in1, &in2};
};

void test_drv8833_starts_both_pins_low(void) {
  Bridge bridge;
  TEST_ASSERT_TRUE(bridge.motor.begin());
  TEST_ASSERT_TRUE(bridge.in1.started && bridge.in2.started);
  TEST_ASSERT_EQUAL(0, bridge.in1.duty);
  TEST_ASSERT_EQUAL(0, bridge.in2.duty);
}

void test_drv8833_reports_a_pin_that_cannot_start(void) {
  Bridge bridge;
  bridge.in2.starts = false;
  TEST_ASSERT_FALSE(bridge.motor.begin());
}

void test_drv8833_drives_forward_on_in1_and_backward_on_in2(void) {
  Bridge bridge;
  bridge.motor.begin();
  bridge.motor.write(0.25f);
  TEST_ASSERT_EQUAL(16384, bridge.in1.duty);
  TEST_ASSERT_EQUAL(0, bridge.in2.duty);
  bridge.motor.write(-0.25f);
  TEST_ASSERT_EQUAL(0, bridge.in1.duty);
  TEST_ASSERT_EQUAL(16384, bridge.in2.duty);
}

void test_drv8833_coasts_at_zero(void) {
  Bridge bridge;
  bridge.motor.begin();
  bridge.motor.write(1.0f);
  bridge.motor.write(0.0f);
  TEST_ASSERT_EQUAL(0, bridge.in1.duty);
  TEST_ASSERT_EQUAL(0, bridge.in2.duty);
}

int main(int, char**) {
  UNITY_BEGIN();
  RUN_TEST(test_drv8833_starts_both_pins_low);
  RUN_TEST(test_drv8833_reports_a_pin_that_cannot_start);
  RUN_TEST(test_drv8833_drives_forward_on_in1_and_backward_on_in2);
  RUN_TEST(test_drv8833_coasts_at_zero);
  return UNITY_END();
}
