#ifdef __AVR__
#include "board_pwm_out_pin.h"

// Arduino's init() runs every PWM timer from the CPU clock divided by 64: Timer0 counts 256 steps per period, in fast
// PWM, and the others count up and down, 510 steps, in phase-correct PWM. That is 976 Hz and 490 Hz at 16 MHz.
static const uint32_t ANALOG_WRITE_PRESCALER = 64;
static const uint32_t TIMER0_STEPS = 256;
static const uint32_t PHASE_CORRECT_STEPS = 510;

static uint32_t analogWriteHz(uint8_t timer) {
  const bool timer0 = timer == TIMER0A || timer == TIMER0B;
  return F_CPU / ANALOG_WRITE_PRESCALER / (timer0 ? TIMER0_STEPS : PHASE_CORRECT_STEPS);
}

// A 16-bit Timer1 with an input-capture register can run at any frequency: mode 14 counts from 0 to ICR1.
#if defined(TCCR1A) && defined(ICR1) && defined(WGM13)
#define HAS_TIMER1 1

static bool isTimer1Pin(uint8_t timer) { return timer == TIMER1A || timer == TIMER1B; }

static const uint16_t PRESCALERS[] = {1, 8, 64, 256, 1024};
static const uint8_t CLOCK_SELECTS[] = {_BV(CS10), _BV(CS11), _BV(CS11) | _BV(CS10), _BV(CS12), _BV(CS12) | _BV(CS10)};

// Timer1 drives both its pins at one frequency: 0 while it runs as Arduino's init() set it.
static uint32_t timer1RequestedHz = 0;
static uint32_t timer1ActualHz = 0;
static uint16_t timer1Top = 0;
static uint8_t timer1AnalogWritePins = 0;

static bool startTimer1(uint32_t frequencyHz) {
  for (uint8_t i = 0; i < sizeof(PRESCALERS) / sizeof(PRESCALERS[0]); i++) {
    const uint32_t steps = F_CPU / PRESCALERS[i] / frequencyHz;
    if (steps > 65536UL) {
      continue;
    }
    if (steps < 2) {
      return false;
    }
    timer1Top = steps - 1;
    TCCR1A = _BV(WGM11);
    TCCR1B = _BV(WGM13) | _BV(WGM12) | CLOCK_SELECTS[i];
    ICR1 = timer1Top;
    TCNT1 = 0;
    timer1RequestedHz = frequencyHz;
    timer1ActualHz = F_CPU / PRESCALERS[i] / steps;
    return true;
  }
  return false;
}
#endif

bool BoardPwmOutPin::begin() {
  if (actualHz) {
    return true;
  }
  const uint8_t timer = digitalPinToTimer(pin);
  if (timer == NOT_ON_TIMER) {
    return false;
  }
  if (!requestedHz) {
#ifdef HAS_TIMER1
    if (isTimer1Pin(timer)) {
      if (timer1RequestedHz) {
        return false;
      }
      timer1AnalogWritePins++;
    }
#endif
    pinMode(pin, OUTPUT);
    analogWrite(pin, 0);
    actualHz = analogWriteHz(timer);
    return true;
  }
#ifdef HAS_TIMER1
  if (!isTimer1Pin(timer) || timer1AnalogWritePins) {
    return false;
  }
  if (timer1RequestedHz != requestedHz && (timer1RequestedHz || !startTimer1(requestedHz))) {
    return false;
  }
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
  usesTimer1 = true;
  actualHz = timer1ActualHz;
  return true;
#else
  return false;
#endif
}

void BoardPwmOutPin::write(uint16_t duty) {
  if (!actualHz) {
    return;
  }
#ifdef HAS_TIMER1
  if (usesTimer1) {
    const bool channelA = digitalPinToTimer(pin) == TIMER1A;
    const uint8_t connect = channelA ? _BV(COM1A1) : _BV(COM1B1);
    const uint32_t highSteps = ((uint32_t)duty * (timer1Top + 1UL) + DUTY_MAX / 2) / DUTY_MAX;
    if (highSteps == 0) {
      TCCR1A &= ~connect;
      digitalWrite(pin, LOW);
      return;
    }
    // The pin stays high for one step more than the compare value, so the top value keeps it high all period.
    if (channelA) {
      OCR1A = highSteps - 1;
    } else {
      OCR1B = highSteps - 1;
    }
    TCCR1A |= connect;
    return;
  }
#endif
  analogWrite(pin, ((uint32_t)duty * 255 + DUTY_MAX / 2) / DUTY_MAX);
}
#endif
