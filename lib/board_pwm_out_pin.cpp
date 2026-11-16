#include "board_pwm_out_pin.h"

// A duty times a period's steps. The ESP8266 counts a period in CPU cycles, millions at a servo's 50 Hz, so it needs 64
// bits; the others count at most 65536 steps, which fit 32, and AVR's 64-bit division would cost about 350 bytes of
// flash.
#ifdef ESP8266
typedef uint64_t StepProduct;
#else
typedef uint32_t StepProduct;
#endif

BoardPwmOutPin::BoardPwmOutPin(uint8_t pin, uint32_t frequencyHz) : pin(pin), requestedHz(frequencyHz) {}

void BoardPwmOutPin::write(uint16_t duty) {
  if (!actualHz) {
    return;
  }
  writeSteps(((StepProduct)duty * steps + DUTY_MAX / 2) / DUTY_MAX);
}

uint32_t BoardPwmOutPin::frequencyHz() { return actualHz; }
