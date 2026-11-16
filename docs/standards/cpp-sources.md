---
description: Sets how C++ files in lib/ and the sketches name classes and files, shape interfaces and units, hide helpers, guard ESP32-only code, set the serial speed and use comments.
audience: Whoever writes or reviews a .h, .cpp or .ino file in lib/ or sketches/; knows C++ and Arduino
---
# C++ sources

Every `.h`, `.cpp` and `.ino` file in `lib/` and `sketches/` follows the rules below. `.clang-format` sets the
layout, and the Done check in [AGENTS.md](../../AGENTS.md) enforces it.

- A file MUST NOT declare a namespace or use `using namespace`, and a helper only its own file uses MUST be `static`.
  The drivers and sketches are small, so a namespace adds nesting and separates nothing. `static` keeps the helper
  out of the one image that `lib/`, the core and Bluepad32 link into, where a second function of the same name fails
  the link.
- A `lib/` header MUST NOT share its name with a header of the core or of a library a sketch uses. `lib/` is on the
  include path, so its header takes the other's place: the camera driver's files are `esp32_camera*` so that none
  shadows `<esp_camera.h>`, and the LED's constants live in `board_led.h` because the AVR core ships a `board.h`.
- A file in `lib/` that builds only on ESP32 MUST be wrapped in `#ifdef ESP32`, as `lib/esp32_ledc.h` is, and one that
  needs a driver only some chips ship in `__has_include` of its header too, as `lib/esp32_camera.h` is. `lib/` also
  builds for other boards, and arduino-cli compiles every source of a library a sketch lists, so one unguarded
  ESP32-only file breaks the build for every other board.
- A sketch that uses the serial port MUST open it at 115200 baud: `output.begin("<key>")` from
  `lib/serial_output.h` does, and a sketch outside the workbench calls `Serial.begin(115200)`. `arduino:run` and
  `arduino:monitor` open the monitor at 115200 unless told otherwise, and the website reads every board at its
  project's `flash.baud`, so one speed for all sketches means a monitor never shows garbage after switching sketch.
- A comment SHOULD record only a reason the code cannot show, such as a hardware constraint. Nothing checks a
  comment that restates the code, so it drifts from the code. A doc comment on a header's class or method, saying
  what it promises its callers, is not such a restatement.

## Names, files and interfaces in lib/

A class's name says what it is in its last word and which one in the words before, and its file is the class's name
in snake_case. A reader who meets `BoardPwmOutPin` in a sketch then knows it is a `PwmOutPin`, that it runs on any
supported board, and that it lives in `lib/board_pwm_out_pin.h`.

| Kind | Class | File |
| --- | --- | --- |
| Interface: pure virtual, no Arduino core | `<Thing>`: `DigitalOutPin`, `PwmOutPin`, `Motor` | `<thing>.h`, no `.cpp` |
| Implementation on the Arduino API, for every supported board | `Board<Thing>`: `BoardPwmOutPin` | `board_<thing>.h` and `.cpp` |
| Code that runs only on the ESP32 | `Esp32<Thing>`: `Esp32Camera`; functions `esp32<Verb>…()` | `esp32_<thing>.h`, wrapped in `#ifdef ESP32` |
| Driver of a device that implements an interface | `<Device><Interface>`: `L298nMotor`, `PulseMotor` | snake_case of the class |
| Driver or logic with no interface | `<Qualifier><Thing>`: `Mpu6050Imu`, `RcChannel` | snake_case of the class |
| Test double | `Fake<Interface>`: `FakePwmOutPin` | `test/fake_<thing>.h` |

- A header MUST hold one public class, and besides it only the types its interface takes or returns, such as
  `ImuSample` beside `Mpu6050Imu`. One class per file makes the file name the index a reader searches by.
- A `Board` class whose code differs per chip family MUST keep each chip family's part in `board_<thing>_<family>.cpp`
  (`esp32`, `esp8266`, `avr`), guarded by that family's `#if`, so a reader studies one family's hardware in one file.
- A supported board carries an ESP32, an ESP8266 or the ATmega328P of an Uno or Nano. A `Board` class's `begin()` MUST
  return false on a board, or a pin, that cannot do what was asked, rather than fail the build: arduino-cli compiles
  every source of the library into every sketch.
- A value that depends on the board the sketch is built for, and that more than one sketch uses, MUST be a constant
  named `BOARD_<THING>` in `board_<thing>.h`, such as `BOARD_LED_PIN` in `board_led.h`. A value only one class or
  one sketch uses, such as the ADC's full scale in `board_analog_in_pin.cpp` or rc-receiver's default pins, stays in
  that source, chosen with `#if` when only the value differs, so a reader sees every board's value side by side.
- Every interface and every driver MUST start with `bool begin()`, which returns false when the hardware cannot run.
- An interface MUST speak its hardware's unit, the same on every board: a digital pin a `bool`; `PwmOutPin` a duty
  from 0 to `DUTY_MAX`, 65535; `AnalogInPin` a value from 0 to `VALUE_MAX`, 65535, scaled from the board's ADC; and
  `PulseInPin` a width in microseconds. A driver MUST take a command in the device's unit: a motor's throttle from
  -1 to 1, a position from -1 to 1, a level from 0 to 1, an angle in degrees.
- A name holding a physical quantity MUST end in its unit: `Us`, `Ms`, `S`, `Hz`, `Deg`, `Dps`. A query MUST be a
  noun, such as `angle()` or `frequencyHz()`, and a setter MUST start with `set`.
- A driver MUST take the pins and buses it uses as pointers, so the sketch that creates them shows the wiring, and
  MUST set its members in the constructor's initializer list. A member name MUST NOT end in `_`.
