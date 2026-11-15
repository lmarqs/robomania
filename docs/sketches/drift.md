---
description: Explains how the drift car turns a gamepad or an RC receiver into steering and throttle, how its gyro counter-steers, how to set it up for a car, and when it stops.
audience: Whoever wires, drives or tunes the drift car, or changes its sketch
keywords: gamepad, Bluepad32, RC receiver, PWM, servo, ESC, MPU6050, yaw rate, gyro gain, counter-steer
---
# Drift

The car that runs `sketches/drift` steers with a hobby servo and drives with a forward/reverse car ESC, both on 50 Hz
pulses, from a Bluetooth gamepad or a PWM RC receiver, picked when the sketch is built. An MPU6050 measures how fast the car spins, and the sketch adds counter-steer
against that spin, as an RC drift gyro does: when the tail swings out, the wheels turn into the slide, which damps the
spin so the car holds the drift. The gyro acts on the steering only; the throttle stays with the driver.

![The drift car's FreeRTOS tasks on each core, the peripherals they drive and the pins they use, with a gamepad](drift-architecture.svg)

## Choosing the controller

`DRIFT_CONTROLLER` in `sketches/drift/local.h` picks what drives the car:

```c
#define DRIFT_CONTROLLER DRIFT_CONTROLLER_RECEIVER
```

| Value | Drives from |
| --- | --- |
| `DRIFT_CONTROLLER_GAMEPAD`, the default | A Bluetooth gamepad through Bluepad32 |
| `DRIFT_CONTROLLER_RECEIVER` | A PWM RC receiver's channels |

Any other value stops the build with an error. A build carries one controller; switching means building and flashing
again.

## Wiring

| GPIO | To | Setting |
| --- | --- | --- |
| 21 | ESC signal | `DRIFT_ESC_PIN` |
| 23 | Steering servo signal | `DRIFT_STEERING_PIN` |
| 27 | MPU6050 SDA | `DRIFT_I2C_SDA_PIN` |
| 26 | MPU6050 SCL | `DRIFT_I2C_SCL_PIN` |
| 2 | The board's LED | `DRIFT_STATUS_LED` |
| 5 | Receiver throttle channel, with `DRIFT_CONTROLLER_RECEIVER` | `DRIFT_RECEIVER_THROTTLE_PIN` |
| 17 | Receiver steering channel, with `DRIFT_CONTROLLER_RECEIVER` | `DRIFT_RECEIVER_STEERING_PIN` |
| none | Receiver gain channel, with `DRIFT_CONTROLLER_RECEIVER` | `DRIFT_RECEIVER_GAIN_PIN` |

Each pin but the LED's is a `#define` that `local.h` can change, as [Setting it up for a car](#setting-it-up-for-a-car)
says. GPIO12 is not one to pick for SDA: it sets the flash voltage at reset, and an MPU6050 breakout's pull-up on SDA
holds it high, which selects 1.8 V and the board does not boot.

The receiver's ground goes to the ESP32's ground. Its signal pins must stay at 3.3 V or less, since the ESP32's pins are
not 5 V tolerant; most 2.4 GHz receivers send 3.3 V pulses even when powered from 5 V, which a multimeter on a signal
pin confirms. The [RC receiver sketch](rc-receiver.md) shows which channel arrives on which pin.

The MPU6050 lies flat, so its Z axis points up and its Z rate is the car's yaw rate.

## Controls

### Gamepad

- **Left stick, sideways**: steering. A deflection under 40 of 512 counts as centred.
- **Right stick, up and down**: up drives forward; down brakes, then reverses, as the ESC does it. The throttle
  follows how far the stick is pushed, through the `DRIFT_THROTTLE_EXPO` curve, with the same dead zone as the
  steering.
- **D-pad up and down**: the gyro's gain, in steps of 0.1 from 0 to 1. Each change prints `gain <value>` on the
  serial port when [the log](#serial-log) is on.

The first gamepad to connect drives the car; the car forgets paired gamepads at every boot.

### RC receiver

- **Throttle channel**: forward from the centre pulse up to `DRIFT_RECEIVER_HIGH_US`, brake and reverse down to
  `DRIFT_RECEIVER_LOW_US`, through the same `DRIFT_THROTTLE_EXPO` curve and `DRIFT_THROTTLE_MAX` as the gamepad.
- **Steering channel**: right toward `DRIFT_RECEIVER_HIGH_US`, left toward `DRIFT_RECEIVER_LOW_US`.
- **Gain channel**, when `DRIFT_RECEIVER_GAIN_PIN` is set: the gyro's gain, from 0 at `DRIFT_RECEIVER_LOW_US` to 1 at
  `DRIFT_RECEIVER_HIGH_US`, for a knob or switch on the transmitter. Without it, or while it sends no pulse, the gain
  stays at `DRIFT_GAIN`.

A pulse within `DRIFT_RECEIVER_DEAD_ZONE_US` of the centre counts as centred. A channel that drives the car the wrong
way is flipped with `DRIFT_RECEIVER_THROTTLE_REVERSED` or `DRIFT_RECEIVER_STEERING_REVERSED`, or with the transmitter's
channel reverse.

When the throttle or the steering channel sends no pulse for `DRIFT_RECEIVER_TIMEOUT_MS`, the car sends neutral
throttle and centre steering until both pulse again. A receiver set to keep sending its own failsafe pulses on signal
loss drives the car with those instead.

## Status LED

With a receiver, the LED is off unless `DRIFT_STATUS_LED` is 1, as [Setting it up for a car](#setting-it-up-for-a-car)
says. The board's LED stays lit while a controller is connected and the MPU6050 works. Otherwise it blinks once a second:

| LED | Means |
| --- | --- |
| Steady | Gamepad connected, MPU6050 working |
| Short blink, 150 ms | MPU6050 not found at boot, or its last read failed |
| Long blink, 600 ms | No gamepad connected, or no pulses from the receiver |
| Short and long blinks alternating | Neither |

The MPU6050 is read only while a controller drives the car, so a sensor that stops answering shows only once a
controller is connected.

## Serial log

The sketch writes errors to the serial port at 115200 baud, such as `error MPU6050 not found`. With `DRIFT_LOG` set to
1 it also writes these lines, between Bluepad32's own with a gamepad; it is off by default because printing in the loop delays the
servo and ESC:

| Line | When |
| --- | --- |
| `gamepad <n> connected: <model>, vid <vid> pid <pid>, <address>` | A gamepad connects and drives the car. `ignored, another drives` instead of `connected` means a second gamepad connected and does not drive. |
| `gamepad <n> disconnected: …` | That gamepad disconnects. |
| `receiver throttle gpio <n>, steering gpio <n>, gain gpio <n>` | At boot with a receiver; gain gpio -1 means none. |
| `status controller connected\|missing, imu working\|failed` | Either changes; the LED shows the same. |
| `input lx … ry … dpad … buttons … battery … \| yaw … steer … esc … gain …` | Every `DRIFT_LOG_INPUT_MS` while a gamepad drives: the raw left stick X and right stick Y, from -511 to 512 with down positive, the D-pad, buttons and battery, then the yaw rate in °/s, the steering and throttle sent, each -1 to 1, and the gain. |
| `input throttle … steering … gain … us \| yaw … steer … esc … gain …` | The same while a receiver drives, with the pulse width of each channel in µs, 0 for none, in place of the gamepad's input. |
| `gain <value>` | At boot, and at each D-pad gain change with a gamepad. |


The car must stand still while it boots: the sketch measures the gyro's bias then, for about a second. The ESC gets
the neutral pulse from power-on, which it needs to arm, so the motor stays stopped until the throttle moves.
Without an MPU6050 the car boots, prints `error MPU6050 not found, steering without assist` and drives as plain RC.

## Setting it up for a car

Each setting is a `#define` in `sketches/drift/local.h`, which Git ignores; the defaults are in
`sketches/drift/config.h`.

| Setting | Default | What it sets |
| --- | --- | --- |
| `DRIFT_STEERING_LEFT_US`, `_CENTER_US`, `_RIGHT_US` | 1000, 1500, 2000 | The servo pulses at full left, straight ahead and full right. Set them to where the car's steering stops, so the servo never stalls against them. |
| `DRIFT_CONTROLLER` | `DRIFT_CONTROLLER_GAMEPAD` | What drives the car, as [Choosing the controller](#choosing-the-controller) says. |
| `DRIFT_RECEIVER_THROTTLE_PIN`, `_STEERING_PIN`, `_GAIN_PIN` | 5, 17, -1 | The GPIO of each receiver channel; a gain pin of -1 keeps the gain at `DRIFT_GAIN`. |
| `DRIFT_RECEIVER_LOW_US`, `_CENTER_US`, `_HIGH_US` | 1000, 1500, 2000 | The receiver's pulses at one end, the centre and the other end of every channel. |
| `DRIFT_RECEIVER_DEAD_ZONE_US` | 20 | How far from the centre pulse a channel still counts as centred. |
| `DRIFT_RECEIVER_THROTTLE_REVERSED`, `_STEERING_REVERSED` | 0, 0 | 1 flips that channel. |
| `DRIFT_RECEIVER_TIMEOUT_MS` | 100 | How long the throttle or steering channel may go without a pulse before the car stops. |
| `DRIFT_STATUS_LED` | 1 with a gamepad, 0 with a receiver | 1 shows the status on the LED on GPIO2. Leave it 0 while a receiver channel is wired to GPIO2, where the LED's output would fight the receiver's. |
| `DRIFT_THROTTLE_MAX` | 0.5 | The largest throttle the controller reaches, as a fraction of the ESC's range each way. |
| `DRIFT_THROTTLE_EXPO` | 0.67 | The throttle's expo, 0 to 1: the throttle is (1 − expo) × stick + expo × stick³ of `DRIFT_THROTTLE_MAX`, so half stick gives about a quarter at 0.67. |
| `DRIFT_GAIN` | 0 | The gain at boot. |
| `DRIFT_FULL_LOCK_DPS` | 200 | The yaw rate, in °/s, at which a gain of 1 alone steers to full lock. |
| `DRIFT_GYRO_REVERSED` | 0 | 1 flips the counter-steer, for a sensor mounted upside down. |
| `DRIFT_LOG` | 0 | 1 writes the controller, status, gain and input lines of the [serial log](#serial-log). |
| `DRIFT_LOG_INPUT_MS` | 250 | How often, in ms, the input line is written while a controller drives and `DRIFT_LOG` is 1; 0 turns it off. |

To check the counter-steer's direction: with the car lifted, raise the gain to 0.5, with the D-pad, the gain channel
or `DRIFT_GAIN`, and turn the car clockwise by hand, seen from above. The front wheels must turn left. If they turn right, set `DRIFT_GYRO_REVERSED` to 1.

## Not handled

- A gamepad whose Bluetooth link stalls keeps counting as connected until Bluepad32 reports the disconnect, which can
  take several seconds; until then the car drives on its last command. Cut the ESC's power to stop it.
- The MPU6050 reads at most 250 °/s, so a faster spin counts as 250 °/s and gets less counter-steer than it calls
  for.
- With a gamepad, the gain resets to `DRIFT_GAIN` at every boot.
- With a receiver, Bluetooth still runs on core 0, as the Bluepad32 core starts it, though nothing reads a gamepad.
