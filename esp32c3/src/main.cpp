// Earth & Mars clock for ESP32-C3 + 240x240 SPI TFT.
// Time comes from NTP over Wi-Fi.
#include <Arduino.h>
#include <SPI.h>
#include <WiFi.h>
#include <sys/time.h>
#include <time.h>
#include <Adafruit_GFX.h>
#include "mars.h"
#include "secrets.h"

// ---- Wiring: change to match your board ----
const int PIN_SCK  = 4;   // display SCL / CLK
const int PIN_MOSI = 6;   // display SDA / DIN
const int PIN_CS   = 7;   // set to -1 if the module has no CS pin
const int PIN_DC   = 5;
const int PIN_RST  = 10;
const int PIN_BL   = 3;   // backlight; -1 if tied to 3V3

// ---- Local Earth time zone (POSIX TZ string). Default: US Pacific ----
const char *TZ_INFO = "PST8PDT,M3.2.0,M11.1.0";

#ifdef DISPLAY_GC9A01
  #include <Adafruit_GC9A01A.h>
  Adafruit_GC9A01A tft(PIN_CS, PIN_DC, PIN_RST);
#else
  #include <Adafruit_ST7789.h>
  Adafruit_ST7789 tft(PIN_CS, PIN_DC, PIN_RST);
#endif

const uint16_t BLACK = 0x0000, WHITE = 0xFFFF, GREY = 0x8C71, BLUE = 0x5D9F, ORANGE = 0xFCCE, RUST = 0xCC4D;

// Centered text, space-padded to `width` characters so each redraw
// overwrites the previous one without clearing (no flicker).
void line(int y, int size, int width, uint16_t color, const char *text) {
  char buf[41];
  int len = strlen(text);
  int left = len < width ? (width - len) / 2 : 0;
  snprintf(buf, sizeof buf, "%*s%-*s", left, "", width - left, text);
  tft.setTextSize(size);
  tft.setTextColor(color, BLACK);
  tft.setCursor((240 - width * 6 * size) / 2, y);
  tft.print(buf);
}

void draw() {
  struct timeval tv;
  gettimeofday(&tv, nullptr);
  time_t now = tv.tv_sec;
  struct tm local;
  localtime_r(&now, &local);
  char s[41];

  strftime(s, sizeof s, "%H:%M:%S", &local);
  line(42, 3, 8, WHITE, s);
  strftime(s, sizeof s, "%a %d %b %Y", &local);
  line(74, 1, 20, GREY, s);

  MarsTime m = marsTime(tv.tv_sec + tv.tv_usec / 1e6);
  snprintf(s, sizeof s, "%02d:%02d:%02d", m.h, m.m, m.s);
  line(134, 3, 8, ORANGE, s);
  line(166, 2, 12, WHITE, m.solName);
  snprintf(s, sizeof s, "%d %s", m.day, m.moomName);
  line(186, 2, 14, ORANGE, s);
  if (m.mini) snprintf(s, sizeof s, "cycle %ld  mini weem", m.cycle);
  else        snprintf(s, sizeof s, "cycle %ld  weem %d/10", m.cycle, m.weem);
  line(207, 1, 22, GREY, s);
  snprintf(s, sizeof s, "MSD %.3f", m.msd);
  line(218, 1, 14, GREY, s);
}

void setup() {
  Serial.begin(115200);
  if (PIN_BL >= 0) {
    pinMode(PIN_BL, OUTPUT);
    digitalWrite(PIN_BL, HIGH);
  }

  SPI.begin(PIN_SCK, -1, PIN_MOSI, PIN_CS);
#ifdef DISPLAY_GC9A01
  tft.begin();
#else
  tft.init(240, 240, PIN_CS < 0 ? SPI_MODE3 : SPI_MODE0);
#endif
  tft.setRotation(0);
  tft.fillScreen(BLACK);
  line(116, 1, 28, WHITE, "Connecting to Wi-Fi...");

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  configTzTime(TZ_INFO, "pool.ntp.org", "time.nist.gov");  // resyncs by itself

  struct tm t;
  while (!getLocalTime(&t, 500)) {
    Serial.printf("waiting for time, Wi-Fi status %d\n", WiFi.status());
    delay(500);
  }

  tft.fillScreen(BLACK);
  line(28, 1, 5, BLUE, "EARTH");
  tft.drawFastHLine(20, 100, 200, GREY);
  line(116, 1, 10, RUST, "MARS (MTC)");
}

void loop() {
  draw();
  delay(100);
}
