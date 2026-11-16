---
title: H-bridge
summary: Four switches around a motor that let it spin forward, backward, brake or coast.
family: Electronics
art: bridge
level: beginner
state: draft
order: 2
example: h-bridge
---

To reverse a DC motor you swap its two leads. An H-bridge does that with four switches, drawn in an H around the
motor. Close the top-left and bottom-right switches and current flows one way; close the other two and it flows the
other way. Try the switches in the example above.

Two combinations matter as much as forward and backward:

- **Brake.** Both leads tied to the same side. A spinning motor acts as a generator, the current it makes fights its
  own spin, and it stops fast.
- **Coast.** Every switch open. The motor rolls to a stop on its own.

And one combination must never happen: both switches on one side closed at once. That connects the supply straight
to ground through the switches, called shoot-through, and it destroys them.

## In the project's code

Real H-bridges come as chips, with transistors for switches. The project's `FullHBridgeMotor` driver uses three
pins for each motor: two direction pins and one PWM pin for speed. Like every motor in the library, it takes a
throttle from -1, full reverse, to 1, full forward.

| | IN1 | IN2 | PWM |
| --- | --- | --- | --- |
| `write(throttle)`, throttle above 0 | high | low | the throttle's share of full duty |
| `write(throttle)`, throttle below 0 | low | high | the throttle's share of full duty, without its sign |
| `write(0)` | low | low | 0 |

What both direction pins low does, brake or coast, depends on the chip; its datasheet says. The driver's header is
on the [Library](../../library/#full_h_bridge_motor.h) page.

<div class="gap">

The H-bridge chips this project has been used with, and how they are wired to the ESP32.

</div>
