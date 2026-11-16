#include <Arduino.h>
#include <board.h>
#include <board_digital_out_pin.h>
#include <serial_io.h>

BoardDigitalOutPin led(BOARD_LED_PIN, BOARD_LED_ON);
SerialInput input;
SerialOutput output;
int blinkDelay = 1000;

void setup() {
  output.begin("blink/input");
  led.begin();
}

void loop() {
  if (input.ready()) {
    String line = input.take();
    if (isDigit(line[0])) {
      blinkDelay = constrain(line.toInt(), 100, 2000);
      output.print("delay ");
      output.println(blinkDelay);
    }
  }
  output.println("HIGH");
  led.write(true);
  delay(blinkDelay);
  output.println("LOW");
  led.write(false);
  delay(blinkDelay);
}
