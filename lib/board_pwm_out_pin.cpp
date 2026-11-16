#include "board_pwm_out_pin.h"

BoardPwmOutPin::BoardPwmOutPin(uint8_t pin, uint32_t frequencyHz) : pin(pin), requestedHz(frequencyHz) {}

uint32_t BoardPwmOutPin::frequencyHz() { return actualHz; }
