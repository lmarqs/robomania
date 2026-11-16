---
description: Defines each Robomania term once, for the hardware, its signals and the code that drives it, with the synonyms each replaces.
audience: Whoever names a thing in the firmware, a page of docs/ or the website, or a conversation about them; knows Arduino
---
# Glossary

Each concept has one name here, so a search for it finds every page and header about it.

## Hardware

**Board**:
The circuit board a sketch is built for and flashed to, such as an Arduino Nano, an ESP-01S or an ESP32-CAM.
_Avoid_: chip, when the board is meant

**Chip family**:
The kind of microcontroller on a board, which decides what its hardware can do: ESP32, ESP8266 or AVR. The library
supports the ESP32 and ESP8266 families and, among AVR chips, the ATmega328P of the Uno and Nano.
_Avoid_: architecture, board, when the family is meant

**Pin**:
One of a board's GPIO pins, used in one direction for one kind of signal: on or off, a PWM output, an analog voltage
or received pulses.

**Device**:
Hardware a sketch reaches through pins or a bus: a motor, a servo, an IMU, a camera.
_Avoid_: peripheral

**H-bridge**:
Four switches around a DC motor that connect each of its terminals to the supply or to ground, so the motor can turn
either way. An H-bridge chip holds one or more of them.
_Avoid_: motor driver, driver chip

**Half H-bridge**:
Two of an H-bridge's four switches: one terminal's connection to the supply or to ground. On its own it turns a motor
one way only, so hardware that turns a motor both ways holds a whole H-bridge for it, however its inputs are driven.
_Avoid_: half-bridge

**ESC**:
An electronic speed controller: a device that powers a motor from a servo pulse. A reversing ESC, as in an RC car,
turns the motor either way; a one-way ESC turns it one way only. An ESC arms, and will run the motor, only after it
has seen its stop pulse for a few seconds.
_Avoid_: motor controller

## Signals

**Level**:
Where a signal stands between low and high. A digital pin has two levels, low and high; as a number, 0 is low, 1 is
high, and the values between say how much of the way, as a PWM duty or a knob's setting does.
_Avoid_: value, amount, when a level is meant

**PWM**:
A square wave at a fixed frequency whose high time in each period carries the value.

**Duty**:
How much of each PWM period a pin stays high, as a number from 0, low all period, to 65535, high all period, on every
board, rather than a percentage.
_Avoid_: duty cycle

**Pulse**:
One high stretch of a signal, read or sent for its width alone. A servo pulse lasts from 500 to 2500 µs and repeats at a
fixed rate, usually 50 times a second.
_Avoid_: PPM, when one channel's pulse is meant

**Pulse width**:
How long a pulse stays high, in microseconds.
_Avoid_: pulse length, duration

**Pulse range**:
The three pulse widths of a travel: one end, the center and the other end. The same range turns a position into a
pulse to send and a received pulse back into a position.

**Center**:
The pulse width in the middle of a pulse range, for position 0.
_Avoid_: middle

**Neutral**:
The center pulse of a reversing ESC: its stop pulse.
_Avoid_: idle, zero

**Channel**:
One signal of an RC receiver, such as throttle or steering, which it sends as servo pulses on a wire of its own.
_Avoid_: channel alone for an LEDC channel

**LEDC channel**:
One of the ESP32's hardware PWM outputs. Each one counts on an LEDC timer, which sets its frequency.

**Dead zone**:
The pulse widths around the center that an RC receiver channel reads as the center, so a stick that rests a little
off still reads as stopped.
_Avoid_: dead band

## Commands

**Travel**:
The whole way something can move, from one end to the other: a stick's, a steering servo's, an RC receiver channel's.

**Position**:
Where something stands along a travel, from -1 at one end through 0 at the center to 1 at the other end: a stick, a
steering servo, an RC receiver channel.

**Throttle**:
How hard and which way a motor turns, from -1, full reverse, through 0, stopped, to 1, full forward. A motor that
cannot reverse stops for any throttle below 0.
_Avoid_: speed, power

**Stop**:
Throttle 0. Each motor stops in its own way: an H-bridge motor coasts, and an ESC gets its stop pulse and decides
how the motor stops.

**Coast**:
Let a motor spin down freely, with no power on its terminals.
_Avoid_: free-run

**Brake**:
Stop a motor fast by connecting its terminals together. Whether an H-bridge brakes or coasts with both inputs low
depends on its chip.

**Controller**:
The device a person drives a robot with: a Bluetooth gamepad or an RC transmitter. A PID controller and a flight
controller are terms of their own.
_Avoid_: controller, for an ESC, a microcontroller or a class that decides what a robot does

**Angle**:
Where a positional servo's horn points, in degrees from 0 to its range, such as 180.

## Code

**Driver**:
A class in `lib/` that runs one device, such as a motor or the IMU, and takes its commands in the device's unit.
The word names nothing else: hardware is a chip, such as an H-bridge chip, and whoever holds the controls is the
person driving.

**Sketch**:
A program for one robot or one demonstration, in `sketches/`, that wires drivers to pins and decides what the robot
does.
