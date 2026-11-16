#ifdef ESP32
#include <driver/ledc.h>

#include "board_pwm_out_pin.h"
#if ESP_ARDUINO_VERSION_MAJOR < 3
#include "esp32_ledc.h"
#else
#include <esp32-hal-periman.h>
#endif

// The rate analogWrite() starts at on both cores.
static const uint32_t ANALOG_WRITE_HZ = 1000;
// The slowest clock LEDC counts on these chips, the C3's 40 MHz crystal: a resolution that fits it fits the others.
static const uint32_t LEDC_CLOCK_HZ = 40000000;
// A duty has 16 bits, so a finer resolution would only repeat steps.
static const uint8_t DUTY_BITS = 16;

/** The finest resolution LEDC can count at `frequencyHz`: each period must last 2^bits clock ticks. */
static uint8_t bitsFor(uint32_t frequencyHz) {
  uint8_t bits = LEDC_TIMER_BIT_MAX - 1 < DUTY_BITS ? LEDC_TIMER_BIT_MAX - 1 : DUTY_BITS;
  while (bits > 1 && (LEDC_CLOCK_HZ >> bits) < frequencyHz) {
    bits--;
  }
  return bits;
}

bool BoardPwmOutPin::begin() {
  if (actualHz) {
    return true;
  }
  const uint32_t hz = requestedHz ? requestedHz : ANALOG_WRITE_HZ;
  bits = bitsFor(hz);
#if ESP_ARDUINO_VERSION_MAJOR < 3
  channel = esp32LedcTakeChannel(hz, bits);
  if (channel < 0) {
    log_e("No LEDC channel left for pin %u at %u Hz with %u bits", pin, (unsigned)hz, bits);
    return false;
  }
  pinMode(pin, OUTPUT);
  actualHz = ledcSetup(channel, hz, bits);
  if (!actualHz) {
    log_e("LEDC channel %d cannot run at %u Hz with %u bits", channel, (unsigned)hz, bits);
    esp32LedcReleaseChannel(channel);
    channel = -1;
    return false;
  }
  ledcAttachPin(pin, channel);
#else
  if (!ledcAttach(pin, hz, bits)) {
    return false;
  }
  // ledcReadFreq() reads 0 while the duty is 0, so the timer the core picked is asked directly.
  const ledc_channel_handle_t* bus = (ledc_channel_handle_t*)perimanGetPinBus(pin, ESP32_BUS_TYPE_LEDC);
  actualHz = ledc_get_freq((ledc_mode_t)(bus->channel / SOC_LEDC_CHANNEL_NUM), (ledc_timer_t)bus->timer_num);
#endif
  return actualHz != 0;
}

void BoardPwmOutPin::write(uint16_t duty) {
  if (!actualHz) {
    return;
  }
  const uint32_t level = ((uint64_t)duty << bits) / DUTY_MAX;
#if ESP_ARDUINO_VERSION_MAJOR < 3
  ledcWrite(channel, level);
#else
  ledcWrite(pin, level);
#endif
}
#endif
