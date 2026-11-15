---
description: Explains the RC receiver sketch, which prints the pulse width on each pin a PWM RC receiver is wired to, so the wiring can be mapped and the pulses checked.
audience: Whoever wires a PWM RC receiver to an ESP32, such as for the drift car
keywords: RC receiver, PWM, pulse width, channel
---
# RC receiver

`sketches/rc-receiver` reads the servo pulses of a PWM RC receiver's channels and prints their widths, so moving one
stick at a time shows which channel arrives on which pin, where its ends and centre sit, and whether the receiver
holds its signal.

## Wiring

| Channel | GPIO |
| --- | --- |
| 1 | 5 |
| 2 | 17 |
| 3 | 16 |
| 4 | 4 |
| 5 | 2 |
| 6 | 15 |

`RC_RECEIVER_PINS` in `sketches/rc-receiver/local.h` changes the list, channel 1 first, such as
`#define RC_RECEIVER_PINS 5, 17`. Each pin is pulled down, so a pin with nothing on it shows no pulse.

The receiver's ground goes to the ESP32's ground, and its signal pins must stay at 3.3 V or less: the ESP32's pins are
not 5 V tolerant, and some receivers powered from 5 V send 5 V pulses. GPIO 2, 5 and 15 set how the ESP32 boots; if
an upload fails to start with the receiver powered, unplug the receiver's power while uploading.

## Serial lines

At 115200 baud, after `sketch rc-receiver`:

| Line | When |
| --- | --- |
| `channel <n> gpio <pin>` | At boot, once per channel. |
| `error gpio <pin> has no interrupt` | At boot, for a pin that cannot time pulses. |
| `ch1 <us> ch2 <us> …` | Every 100 ms: the last pulse width of each channel in µs, or `-` when none arrived in the last 100 ms. |

A centred stick reads about 1500 µs and its ends about 1000 and 2000 µs; a transmitter's end points and trims move
them. A channel that shows `-` with the transmitter on is not wired, not bound, or is a channel the transmitter does
not send.
