#include <Arduino.h>
#include <board_digital_out_pin.h>
#include <board_led.h>
#include <serial_input.h>
#include <serial_output.h>

BoardDigitalOutPin led(BOARD_LED_PIN, BOARD_LED_ON);
SerialInput input;
SerialOutput output;
int blinkDelay = 1000;
bool ledOn = false;
unsigned long lastToggle = 0;

void setup() {
  output.begin("blink/timer");
  led.begin();
}

void loop() {
  while (input.ready()) {
    String line = input.take();
    if (isDigit(line[0])) {
      blinkDelay = constrain(line.toInt(), 100, 2000);
      output.print("delay ");
      output.println(blinkDelay);
    }
  }
  if (millis() - lastToggle >= blinkDelay) {
    lastToggle = millis();
    ledOn = !ledOn;
    output.println(ledOn ? "HIGH" : "LOW");
    led.write(ledOn);
  }
}
