---
title: Pins and pulses
summary: Why the library sends PWM but reads pulses, and how one range turns a position into a pulse and back.
family: Programming
art: signal
level: intermediate
state: draft
order: 4
---

A [servo](../servo/), an [ESC](../brushless-and-esc/) and an RC receiver all speak the same signal: a pulse between
about 1000 and 2000 µs, repeated around 50 times a second. The library handles it in three layers, and each layer
works in its own unit:

| Layer | Sending a pulse | Reading a pulse |
| --- | --- | --- |
| Position, from -1 to 1 | `PulseOut`, inside `Esc`, `ReversibleEsc` and the servos | `RcChannel` |
| Pulse, in µs | `PulseRange::pulseUs()` | `PulseRange::position()` |
| Pin, in the hardware's unit | `PwmOutPin`: a duty | `PulseInPin`: a width in µs |

## Why PWM out, but pulse in

The two pins look as if they should be twins, a `PwmOutPin` and a `PwmInPin`. They are not, because the hardware
works in a different unit on each side.

- **Sending**: the ESP32's LEDC hardware repeats a square wave at a fixed frequency and takes a **duty**, how much of
  each period stays high (see [PWM signals](../pwm/)). So the output pin speaks duty, and `pulseDuty()` converts the
  pulse a servo needs: 1500 µs of a 20 ms period is 7.5 %, or 4915 of 65535 steps.
- **Reading**: an interrupt fires on each edge of the pin and notes the time. The gap between a rising and a falling
  edge is the pulse's **width**, already in µs. The input never measures a whole period, so it cannot report a duty,
  and a receiver's signal carries its meaning in the width alone.

A `PwmInPin` would also time the period and report a duty and a frequency. Nothing in the project needs those, so the
library reads a `PulseInPin`, named for what it measures.

## One range, both ways

A `PulseRange` holds three pulses: one end, the middle and the other end of a travel. Sending, it turns a position
into a pulse; reading, it turns a pulse back into a position, with the same arithmetic run backwards. Each side of the
middle scales on its own, so a steering servo whose straight-ahead pulse sits off the middle still reaches both
locks, and a range given from high to low reverses the travel.

The two ESC drivers are the same `PulseOut` with a different middle:

| Driver | Low, middle, high | Throttle 0 sends |
| --- | --- | --- |
| `Esc`, one way | 1000, 1000, 2000 µs | 1000 µs: stopped |
| `ReversibleEsc`, an RC car's | 1000, 1500, 2000 µs | 1500 µs: neutral |

Swap them and an RC car reverses at full power when told to stop, or a one-way motor runs at half throttle. That is
why each keeps its own name, though the code under both is one.

## How the files are named

The interfaces live in `pin.h`, one per kind of pin: `InPin` and `OutPin` for a level, `PwmOutPin` and
`PulseInPin`. They build without the Arduino core, so a test can hand a driver a fake pin and check the duty it
writes, with no board.

A file named `<kind>_pin` holds the classes that give that kind of pin on a board: `digital_pin`, `analog_pin`,
`pwm_out_pin` and `pulse_in_pin`. A class is named for what the pin carries and which way it goes, with `Esp` in
front when it runs only on the ESP32: `EspPwmOutPin` sends PWM through LEDC, and `EspPulseInPin` reads pulses by
interrupt. The [Library](../../library/) page shows every header.
