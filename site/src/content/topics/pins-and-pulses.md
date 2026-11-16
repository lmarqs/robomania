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
| Position, from -1 to 1 | `PulseOut`, inside `PulseMotor` and `PositionalServo` | `RcChannel` |
| Pulse, in µs | `PulseRange::pulseUs()` | `PulseRange::position()` |
| Pin, in the hardware's unit | `PwmOutPin`: a duty | `PulseInPin`: a width in µs |

## Why PWM out, but pulse in

The two pins look as if they should be twins, a `PwmOutPin` and a `PwmInPin`. They are not, because the hardware
works in a different unit on each side.

- **Sending**: the ESP32's LEDC hardware repeats a square wave at a fixed frequency and takes a **duty**, how much of
  each period stays high (see [PWM signals](../pwm/)). So the output pin speaks duty, and `PulseOut` converts the
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

An ESC is a `PulseMotor`, and its range says which kind:

| ESC | Range | Throttle 0 sends |
| --- | --- | --- |
| One way | `PulseRange::oneWay(1000, 2000)`: 1000, 1000, 2000 µs | 1000 µs: stopped |
| An RC car's, which reverses | `PulseRange(1000, 1500, 2000)` | 1500 µs: neutral |

Swap them and an RC car reverses at full power when told to stop, or a one-way motor runs at half throttle, so a
sketch spells out the range its ESC needs.

## How the files are named

Each interface has a header of its own, one per kind of pin: `digital_in_pin.h` and `digital_out_pin.h` for a level,
`analog_in_pin.h` for a voltage, `pwm_out_pin.h` and `pulse_in_pin.h`. They build without the Arduino core, so a test
can hand a driver a fake pin and check the duty it writes, with no board.

The class a sketch creates puts `Board` in front of the interface it gives: `BoardPwmOutPin` sends PWM and
`BoardPulseInPin` reads pulses on the board the sketch is built for, whether an ESP32, an ESP8266 or an Arduino Nano.
Its file is the class name in lower case with underscores, `board_pwm_out_pin.h`, and where the boards differ, each
board's code sits in a file of its own, such as `board_pwm_out_pin_avr.cpp`. A class that runs only on the ESP32
starts with `Esp32` instead, such as `Esp32Camera`. The [Library](../../library/) page shows every header.
