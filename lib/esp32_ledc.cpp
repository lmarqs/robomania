#ifdef ESP32
#include "esp32_ledc.h"

#if ESP_ARDUINO_VERSION_MAJOR < 3

#include <Arduino.h>
#include <esp32-hal-ledc.h>
#include <soc/soc_caps.h>

#ifdef SOC_LEDC_SUPPORT_HS_MODE
static const uint8_t CHANNEL_COUNT = SOC_LEDC_CHANNEL_NUM * 2;
#else
static const uint8_t CHANNEL_COUNT = SOC_LEDC_CHANNEL_NUM;
#endif

// The core drives channels 2k and 2k + 1 from one timer, so both must share a frequency and resolution.
static const uint8_t PAIR_COUNT = CHANNEL_COUNT / 2;
// The core gives each speed group four timers: pair k runs on timer k % 4.
static const uint8_t TIMERS_PER_GROUP = 4;
static const uint8_t BOTH_CHANNELS = 0b11;

struct ChannelPair {
  uint32_t frequencyHz;
  uint8_t bits;
  uint8_t usedChannels;
};

static ChannelPair pairs[PAIR_COUNT];

static int8_t takeChannel(uint8_t pair) {
  uint8_t offset = pairs[pair].usedChannels & 1 ? 1 : 0;
  pairs[pair].usedChannels |= 1 << offset;
  return pair * 2 + offset;
}

int8_t esp32LedcTakeChannel(uint32_t frequencyHz, uint8_t bits) {
  for (uint8_t pair = 0; pair < PAIR_COUNT; pair++) {
    ChannelPair& p = pairs[pair];
    if (p.usedChannels && p.usedChannels != BOTH_CHANNELS && p.frequencyHz == frequencyHz && p.bits == bits) {
      return takeChannel(pair);
    }
  }

  for (uint8_t pair = 0; pair < PAIR_COUNT; pair++) {
    if (!pairs[pair].usedChannels) {
      pairs[pair].frequencyHz = frequencyHz;
      pairs[pair].bits = bits;
      return takeChannel(pair);
    }
  }

  return -1;
}

void esp32LedcReleaseChannel(int8_t channel) { pairs[channel / 2].usedChannels &= ~(1 << (channel % 2)); }

int8_t esp32LedcReserveTimer() {
  for (uint8_t timer = 0; timer < TIMERS_PER_GROUP; timer++) {
    bool free = true;
    for (uint8_t pair = timer; pair < PAIR_COUNT; pair += TIMERS_PER_GROUP) {
      free = free && !pairs[pair].usedChannels;
    }

    if (free) {
      for (uint8_t pair = timer; pair < PAIR_COUNT; pair += TIMERS_PER_GROUP) {
        pairs[pair].usedChannels = BOTH_CHANNELS;
      }
      return timer;
    }
  }

  return -1;
}

void esp32LedcReleaseTimer(int8_t timer) {
  for (uint8_t pair = timer; pair < PAIR_COUNT; pair += TIMERS_PER_GROUP) {
    pairs[pair].usedChannels = 0;
  }
}

#endif
#endif
