---
description: Explains how the drift car turns a gamepad into steering and throttle, how its gyro counter-steers, how to set it up for a car, and when it stops.
audience: Whoever wires, drives or tunes the drift car, or changes its sketch
keywords: gamepad, Bluepad32, servo, ESC, MPU6050, yaw rate, gyro gain, counter-steer
---
# Drift

The car that runs `sketches/drift` steers with a hobby servo and drives with a forward/reverse car ESC, both on 50 Hz
pulses, from a Bluetooth gamepad. An MPU6050 measures how fast the car spins, and the sketch adds counter-steer
against that spin, as an RC drift gyro does: when the tail swings out, the wheels turn into the slide, which damps the
spin so the car holds the drift. The gyro acts on the steering only; the throttle stays with the driver.

![The drift car's FreeRTOS tasks on each core, the peripherals they drive and the pins they use](drift-architecture.svg)

## Wiring

| GPIO | To | Setting |
| --- | --- | --- |
| 21 | ESC signal | `DRIFT_ESC_PIN` |
| 23 | Steering servo signal | `DRIFT_STEERING_PIN` |
| 12 | MPU6050 SDA | `DRIFT_I2C_SDA_PIN` |
| 13 | MPU6050 SCL | `DRIFT_I2C_SCL_PIN` |
| 2 | The board's LED | |

Each pin but the LED's is a `#define` that `local.h` can change, as [Setting it up for a car](#setting-it-up-for-a-car)
says. GPIO12 sets the flash voltage at reset: held high while the board boots, it selects 1.8 V and the board does not
start. An MPU6050 breakout such as the GY-521 pulls SDA up to its supply, so with SDA on GPIO12 a WROOM-32 board fails
to boot unless the pull-up is removed or the flash voltage is fixed in eFuse. Move SDA with `DRIFT_I2C_SDA_PIN` if the
board does not boot with the sensor connected.

The MPU6050 lies flat, so its Z axis points up and its Z rate is the car's yaw rate.

## Controls

- **Left stick, sideways**: steering. A deflection under 40 of 512 counts as centred.
- **Right trigger**: forward. **Left trigger**: brake, then reverse, as the ESC does it.
- **D-pad up and down**: the gyro's gain, in steps of 0.1 from 0 to 1. Each change prints `gain <value>` on the
  serial port at 115200 baud.

The first gamepad to connect drives the car; the car forgets paired gamepads at every boot. The LED blinks once a
second while no gamepad is connected and stays lit while one is.

## Boot

The car must stand still while it boots: the sketch measures the gyro's bias then, for about a second. The ESC gets
the neutral pulse from power-on, which it needs to arm, so the motor stays stopped until a trigger is pressed.
Without an MPU6050 the car boots, prints `error MPU6050 not found, steering without assist` and drives as plain RC.

## Setting it up for a car

Each setting is a `#define` in `sketches/drift/local.h`, which Git ignores; the defaults are in
`sketches/drift/config.h`.

| Setting | Default | What it sets |
| --- | --- | --- |
| `DRIFT_STEERING_LEFT_US`, `_CENTER_US`, `_RIGHT_US` | 1000, 1500, 2000 | The servo pulses at full left, straight ahead and full right. Set them to where the car's steering stops, so the servo never stalls against them. |
| `DRIFT_THROTTLE_MAX` | 0.5 | The largest throttle the triggers reach, as a fraction of the ESC's range each way. |
| `DRIFT_GAIN` | 0 | The gain at boot. |
| `DRIFT_FULL_LOCK_DPS` | 200 | The yaw rate, in °/s, at which a gain of 1 alone steers to full lock. |
| `DRIFT_GYRO_REVERSED` | 0 | 1 flips the counter-steer, for a sensor mounted upside down. |

To check the counter-steer's direction: with the car lifted, raise the gain to 0.5 and turn the car clockwise by hand,
seen from above. The front wheels must turn left. If they turn right, set `DRIFT_GYRO_REVERSED` to 1.

## Not handled

- A gamepad whose Bluetooth link stalls keeps counting as connected until Bluepad32 reports the disconnect, which can
  take several seconds; until then the car drives on its last command. Cut the ESC's power to stop it.
- The MPU6050 reads at most 250 °/s, so a faster spin counts as 250 °/s and gets less counter-steer than it calls
  for.
- The gain resets to `DRIFT_GAIN` at every boot.
