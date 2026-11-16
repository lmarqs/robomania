# Robomania

Firmware for a 1:10 RC car run by an ESP32, built with arduino-cli: on the
[Bluepad32](https://github.com/ricardoquesada/bluepad32) ESP32 core for the sketches that read a Bluetooth gamepad,
and on Espressif's ESP32 core for the rest. The repository holds a library of drivers in
`lib/` and the sketches that use it in `sketches/`. The car's own sketch is not written yet;
[docs/sketches](docs/sketches/README.md) says what each sketch does, and `sketches/blink/simple` proves the toolchain
works.
Other boards run on the core each `sketch.yaml` pins: `sketches/web-relay` and the blink steps on an ESP-01S, the
blink, servo and rc-receiver sketches also on an Arduino Nano, and `sketches/usb-key` on a Digispark ATtiny85.

This README is for someone who knows Arduino basics and wants to build and flash a sketch. It covers the toolchain
and the code layout; the chassis and its printed parts are in [Hardware](#hardware).

## Build a sketch

[mise](https://mise.jdx.dev) installs every tool the build needs, at the versions `mise.toml` pins: arduino-cli,
Python, uv, ruff, clang-format, the-cook, Node and pnpm. From the repository root:

```sh
mise install
mise run '*:setup'
mise run arduino:compile sketches/blink/simple
```

`mise run '*:setup'` runs each `setup` task: `python:setup` installs pyserial into `.venv`, which every compile needs
because the core's esptool fails without it, and `node:setup` installs the website's packages and the browser its tests
use. The first compile of a sketch also downloads its core into `.arduino/`. `mise tasks` lists every task.

A sketch with more than one board has a profile for each in its `sketch.yaml`, and the task builds the default one.
Build another with `mise exec -- arduino-cli compile <sketch> --profile <profile>`.

`mise run arduino:compile-all` builds every profile of every sketch and stops at the first that fails.

A sketch reads its passwords and the values of one build of its robot from `<sketch>/local.h`, which Git ignores;
the sketch's page in [docs/sketches](docs/sketches/README.md) lists the names it reads.

## Test the drivers

```sh
mise run lib:test
```

The task builds each program in `test/` with the machine's `g++` and runs it. The tests cover the drivers in `lib/`
that compute without hardware; `test/host_test.h` gives them their assertions.

## Build and test the website

Robomania, the project's website, lives in `site/`. After `mise run '*:setup'`:

```sh
mise run arduino:firmware
mise run site:build
mise run site:test
```

`site:build` type-checks the site with `astro check` before it builds.

`arduino:firmware` compiles every sketch that has an ESP profile and writes the images and a `manifest.json` to
`site/public/firmware/`, which Git ignores; the site's Flash page and project pages read them. It refuses to build a
sketch that has a `local.h`, since that file's passwords would ship to every visitor; `--allow-local` overrides it.
`site:dev` serves the site with live reload, and `site:test` runs the unit and browser tests against a build.

## Edit in VS Code

`.vscode/` sets up the Microsoft C/C++ extension: IntelliSense reads `.build/compile_commands.json`, and saving a
C or C++ file formats it with `.clang-format`. `arduino:compile` writes the database: it merges the compile database
of every sketch compiled so far and points each sketch's entry at its `.ino`. Compile a sketch once before editing it.

## Flash a sketch

The website's Flash page puts a sketch on an ESP32, ESP32-C3 or ESP8266 board over USB from Chrome or Edge, with no
tools installed. With the tools, connect the board over USB and pass its serial port, such as `/dev/ttyUSB0`:

```sh
mise run arduino:run sketches/blink/simple --port /dev/ttyUSB0 --monitor
```

The task compiles and uploads; `--monitor` then opens the serial monitor at 115200 baud, the speed every sketch with
serial output uses, until Ctrl+] closes it. Your user needs read and write access to the port: on Debian and Ubuntu,
membership in the `dialout` group. A board that uploads through its own USB bootloader, such as the Digispark, takes
no `--port`.
Flashing has not been tried on a board yet; report what fails.

## Code layout

Each sketch is a folder with a `sketch.yaml` that pins the board, the core version and the libraries, one or two
levels under `sketches/`: `sketches/<name>/`, or `sketches/<project>/<variant>/` for a project whose variants are
steps of one ladder. A sketch's path under `sketches/`, such as `blink/simple`, names it in the firmware manifest.
`sketches/blink/simple/sketch.yaml` is the one to copy. A sketch uses the drivers by listing the path to `lib`,
`../../lib` at one level and `../../../lib` at two, under `libraries`.

The drivers in `lib/`:

| Files | Drives |
| --- | --- |
| `digital_in_pin.h`, `digital_out_pin.h`, `analog_in_pin.h`, `pwm_out_pin.h`, `pulse_in_pin.h` | The pin interfaces, free of the Arduino core: a level in or out, a voltage in, a PWM duty out, a pulse width in |
| `board_*_pin.*` | Each pin interface on the board the sketch is built for: the ESP32, the ESP8266 or an AVR such as the Nano |
| `board_led.h` | The LED on the board the sketch is built for |
| `line_reader.*` | Serial bytes split into lines |
| `serial_input.*`, `serial_output.*` | The serial port as lines: read without waiting, printed with `Print` and `printf()` |
| `motor.h` | The motor interface: a throttle from -1, full reverse, to 1, full forward |
| `full_h_bridge_motor.*`, `half_h_bridge_motor.*` | A DC motor through a full or half H-bridge |
| `esc_motor.*`, `reversible_esc_motor.*` | A motor through its electronic speed controller: a forward-only brushless one, or an RC car's that also reverses |
| `continuous_servo_motor.*` | A continuous-rotation (360°) servo as a motor |
| `positional_servo.*` | A positional servo, such as 0° to 180° |
| `rc_channel.*` | An RC receiver channel's pulse width as a stick position or a knob level |
| `pulse_range.*` | A position from -1 to 1 as a pulse between low, centre and high, and back |
| `pulse_out.*` | A servo-style pulse on a PWM pin, set by a position from -1 to 1, that the ESC and servo drivers share |
| `mpu6050_imu.*` | An MPU6050 accelerometer and gyroscope on I2C |
| `complementary_filter.*` | One steady angle out of an accelerometer's and a gyroscope's readings |
| `pid_controller.*` | A PID controller |
| `steering_assist.*` | Counter-steer against a car's yaw rate, as an RC drift gyro adds |
| `esp32_ledc.*` | The LEDC channels and timers the PWM pins and the camera share on the ESP32's core 2 |
| `esp32_camera*` | The camera of an AI Thinker ESP32-CAM |
| `esp32_web_server.*` | An HTTP and WebSocket server on the ESP-IDF `httpd` |

Another sketch uses the drivers through the Robomania library: download `robomania-<version>.zip` from a
[`lib-v` release](https://github.com/lmarqs/robomania/releases) and install it with
`ARDUINO_LIBRARY_ENABLE_UNSAFE_INSTALL=true arduino-cli lib install --zip-path robomania-<version>.zip`, or in the
Arduino IDE with Sketch > Include Library > Add .ZIP Library. It builds for the ESP32, the ESP8266 and AVR boards
such as the Uno and Nano; each `board_*_pin.h` says what its pin can do on each.

[docs/](docs/README.md) explains [how the parts fit](docs/architecture.md) and the [standards](docs/standards/README.md)
the code follows. `.clang-format` sets the layout; apply it with `mise exec -- clang-format -i <file>`.

## Hardware

- Chassis: [DKS chassis guide](https://dukedoks.com/portfolio/guia-chasis-rc/) and its printable
  [1:10 adaptable DKS basic chassis](https://www.printables.com/model/447947-chassis-110-adaptable-dks-basic)
- Motor: [A2212 motor mount](https://www.printables.com/model/845086-a2212-motor-mount-for-chassis-110-adaptable-dks-ba)

<!-- cook:begin io.github.lmarqs.cookbook:git#worktrees/worktrees -->
<!-- io.github.lmarqs.cookbook:git#worktrees --instructions=AGENTS.md --instructions=README.md -->
- Worktrees: branch `<branch>` is checked out at `.worktrees/<branch>`; `.worktrees/README.md` says how to
  create one ready to run and remove it.
<!-- cook:end io.github.lmarqs.cookbook:git#worktrees/worktrees -->
