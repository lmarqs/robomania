#include <Arduino.h>
#include <board_digital_out_pin.h>
#include <board_led.h>
#include <serial_output.h>

BoardDigitalOutPin led(BOARD_LED_PIN, BOARD_LED_ON);
SerialOutput output;

void setup() {
  output.begin("blink/simple");
  led.begin();
}

void loop() {
  output.println("HIGH");
  led.write(true);
  delay(1000);
  output.println("LOW");
  led.write(false);
  delay(1000);
}
