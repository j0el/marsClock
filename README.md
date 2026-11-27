# marsClock

A single-page live clock showing Earth time and Mars time (Coordinated Mars Time, Mars Sol Date) side by side, plus a decimal sol/weem/moom calendar.

Open `index.html` in a browser.

## ESP32-C3 version

`esp32c3/` is a PlatformIO project for an ESP32-C3 with a 240x240 SPI display.

1. Copy `include/secrets.example.h` to `include/secrets.h` and enter your Wi-Fi details.
2. Check the pins and time zone at the top of `src/main.cpp`.

| Display | C3 pin |
|---|---|
| SCL/CLK | GPIO4 |
| SDA/DIN | GPIO6 |
| CS | GPIO7 |
| DC | GPIO5 |
| RST | GPIO10 |
| BL | GPIO3 |
| VCC / GND | 3V3 / GND |

3. From `esp32c3/`, run `pio run -e round -t upload` (GC9A01) or `pio run -e square -t upload` (ST7789).
