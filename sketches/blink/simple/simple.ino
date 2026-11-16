#include <Arduino.h>
#include <board.h>
#include <board_digital_out_pin.h>
#include <serial_io.h>

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
