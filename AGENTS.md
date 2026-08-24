# AGENTS.md — drone_engage_rpi_gpio

DroneEngage RPi GPIO module (`de_rpi_gpio`). Controls Raspberry Pi GPIO
ports as on/off outputs (LEDs, buzzer, motors, etc.) from the
DroneEngage bus. C++17, WiringPi. See parent `../AGENTS.md` for
workspace architecture, `de_common` vendoring, and config conventions.

## Build

    ./build.sh                          # DEBUG
    ./build_release.sh                  # RELEASE
    ./build_ddebug.sh                   # DEBUG + DDEBUG + TEST_MODE_NO_WIRINGPI_LINK

Out-of-source in `build/`. Binaries: `bin/de_rpi_gpio`,
`bin/de_rpi_gpio.so`.

### CMake options

- `DDEBUG` — detailed debug.
- `TEST_MODE_NO_WIRINGPI_LINK` — **simulation mode**: stubs the WiringPi
  link so the module builds on non-Pi hosts (x86 dev machines). Use
  `build_ddebug.sh` or pass `-DTEST_MODE_NO_WIRINGPI_LINK=ON` to cmake.
  On a real Pi, leave it OFF.

Version is hard-coded (`1.4.1`), no `.version` auto-increment.

### Dependencies

Threads, WiringPi (only when `TEST_MODE_NO_WIRINGPI_LINK=OFF`).

## Config

- `de_rpi_gpio.config.module.json` — module config (WebClient UI).
- `de_rpi_gpio.config.module2nd.json` — alternate config.
- `de_rpi_gpio.config.local` — instance identity.

## Source layout

`src/` — `main.cpp`, `gpio/` (parser + GPIO logic, e.g.
`gpio_parser.cpp`), `de_common/` (vendored), `3rdparty/`, `defines.hpp`,
`global.hpp`, `version.h`. `resources/` — logos/assets. `examples/` —
usage examples.
