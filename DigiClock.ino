#include <TinyWireM.h>

#define DHT_PIN 4
#define BTN_PIN 3
#define RTC_ADDR 0x68
#define OLED_ADDR 0x3C

const unsigned long DHT_INTERVAL = 3000;
const unsigned long CLK_INTERVAL = 1000;

uint8_t h, m, s;
int8_t temp = 0, hum = 0;
int8_t display_temp = 0;
bool colonState = true;

uint8_t last_h = 255, last_m = 255, last_s = 255;
int8_t last_display_temp = -127, last_hum = -1;
bool last_colon = false;

unsigned long prevTime_DHT = 0;
unsigned long prevTime_CLK = 0;

const uint8_t digitsTop[] PROGMEM = {
  0xFC,0xFE,0x06,0x02,0x02,0x02,0x02,0x02,0x06,0xFE,0xFC,0x00,
  0x00,0x08,0x04,0xFE,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x04,0x06,0x02,0x02,0x82,0xC2,0x62,0x36,0x1E,0x0C,0x00,0x00,
  0x04,0x06,0x02,0x22,0x22,0x22,0x36,0x7E,0xDC,0x80,0x00,0x00,
  0x80,0xC0,0x60,0x30,0x18,0x0C,0xFE,0xFE,0x00,0x00,0x00,0x00,
  0x3E,0x3E,0x22,0x22,0x22,0x22,0x62,0xC2,0x82,0x00,0x00,0x00,
  0xF8,0xFC,0x26,0x22,0x22,0x22,0x26,0xE4,0xC0,0x00,0x00,0x00,
  0x02,0x02,0x02,0x82,0xC2,0x62,0x32,0x1A,0x0E,0x06,0x00,0x00,
  0x1C,0xBE,0xE2,0xC2,0xC2,0xC2,0xE2,0xBE,0x1C,0x00,0x00,0x00,
  0x3C,0x7E,0xC2,0x82,0x82,0x82,0xC6,0xFE,0xFC,0x00,0x00,0x00,
};

const uint8_t digitsBot[] PROGMEM = {
  0x3F,0x7F,0x60,0x40,0x40,0x40,0x40,0x40,0x60,0x7F,0x3F,0x00,
  0x00,0x40,0x40,0x7F,0x7F,0x40,0x40,0x00,0x00,0x00,0x00,0x00,
  0x60,0x70,0x58,0x4C,0x46,0x43,0x41,0x40,0x40,0x40,0x00,0x00,
  0x01,0x41,0x40,0x44,0x44,0x44,0x6C,0x7E,0x3B,0x01,0x00,0x00,
  0x01,0x01,0x01,0x01,0x01,0x01,0x7F,0x7F,0x01,0x01,0x00,0x00,
  0x01,0x41,0x40,0x40,0x40,0x40,0x60,0x3F,0x1F,0x00,0x00,0x00,
  0x1F,0x3F,0x60,0x44,0x44,0x44,0x64,0x3C,0x18,0x00,0x00,0x00,
  0x00,0x00,0x00,0x7F,0x7F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x39,0x7D,0x47,0x43,0x43,0x43,0x47,0x7D,0x39,0x00,0x00,0x00,
  0x00,0x00,0x03,0x47,0x44,0x44,0x64,0x3F,0x1F,0x00,0x00,0x00,
};

const uint8_t font5x8[] PROGMEM = {
  0x3E,0x51,0x49,0x45,0x3E,
  0x00,0x42,0x7F,0x40,0x00,
  0x62,0x51,0x49,0x49,0x46,
  0x22,0x41,0x49,0x49,0x36,
  0x18,0x14,0x12,0x7F,0x10,
  0x27,0x45,0x45,0x45,0x39,
  0x3C,0x4A,0x49,0x49,0x30,
  0x01,0x71,0x09,0x05,0x03,
  0x36,0x49,0x49,0x49,0x36,
  0x06,0x49,0x49,0x29,0x1E,
  0x00,0x00,0x00,0x00,0x00,
  0x06,0x09,0x09,0x06,0x00,
  0x3E,0x41,0x41,0x41,0x22,
  0x23,0x13,0x08,0x64,0x62,
};

const uint8_t secFont[] PROGMEM = {
  0x3E,0x51,0x49,0x45,0x3E,
  0x00,0x42,0x7F,0x40,0x00,
  0x62,0x51,0x49,0x49,0x46,
  0x22,0x41,0x49,0x49,0x36,
  0x18,0x14,0x12,0x7F,0x10,
  0x27,0x45,0x45,0x45,0x39,
  0x3C,0x4A,0x49,0x49,0x30,
  0x01,0x71,0x09,0x05,0x03,
  0x36,0x49,0x49,0x49,0x36,
  0x06,0x49,0x49,0x29,0x1E,
};

const uint8_t colonTop[] PROGMEM = {0x00,0x00,0x18,0x18,0x00,0x00,0x00,0x00};
const uint8_t colonBot[] PROGMEM = {0x00,0x00,0x18,0x18,0x00,0x00,0x00,0x00};

const uint8_t iconTemp[] PROGMEM = {0x18,0x24,0x24,0x24,0x24,0x24,0x18,0x00};
const uint8_t iconHum[]  PROGMEM = {0x00,0x0C,0x12,0x12,0x12,0x0C,0x00,0x00};

void oledCmd(uint8_t c) {
  TinyWireM.beginTransmission(OLED_ADDR);
  TinyWireM.send(0x00);
  TinyWireM.send(c);
  TinyWireM.endTransmission();
}

void oledData(uint8_t d) {
  TinyWireM.beginTransmission(OLED_ADDR);
  TinyWireM.send(0x40);
  TinyWireM.send(d);
  TinyWireM.endTransmission();
}

void oledPos(uint8_t x, uint8_t page) {
  oledCmd(0xB0 + (page & 0x07));
  oledCmd(0x00 + (x & 0x0F));
  oledCmd(0x10 + (x >> 4));
}

void oledInit() {
  oledCmd(0xAE);
  oledCmd(0x20); oledCmd(0x00);
  oledCmd(0x40);
  oledCmd(0xA0);
  oledCmd(0xC0);
  oledCmd(0x81); oledCmd(0xCF);
  oledCmd(0xA6);
  oledCmd(0xA8); oledCmd(0x3F);
  oledCmd(0xD3); oledCmd(0x00);
  oledCmd(0xD5); oledCmd(0x80);
  oledCmd(0xD9); oledCmd(0xF1);
  oledCmd(0xDA); oledCmd(0x12);
  oledCmd(0xDB); oledCmd(0x40);
  oledCmd(0x8D); oledCmd(0x14);
  oledCmd(0xAF);

  for (uint8_t p = 0; p < 8; p++) {
    oledPos(0, p);
    for (uint8_t i = 0; i < 128; i++) oledData(0x00);
  }
}

#define DIGIT_WIDTH 12
#define DIGIT_SPACING 2
#define COLON_WIDTH 8

void drawDigit(uint8_t d, uint8_t x, uint8_t p) {
  if (d > 9) return;

  oledPos(x, p);
  TinyWireM.beginTransmission(OLED_ADDR);
  TinyWireM.send(0x40);
  for (uint8_t i = 0; i < DIGIT_WIDTH; i++) {
    TinyWireM.send(pgm_read_byte(&digitsTop[d * DIGIT_WIDTH + i]));
  }
  TinyWireM.endTransmission();

  oledPos(x, p + 1);
  TinyWireM.beginTransmission(OLED_ADDR);
  TinyWireM.send(0x40);
  for (uint8_t i = 0; i < DIGIT_WIDTH; i++) {
    TinyWireM.send(pgm_read_byte(&digitsBot[d * DIGIT_WIDTH + i]));
  }
  TinyWireM.endTransmission();
}

void drawColon(uint8_t x, uint8_t p, bool on) {
  oledPos(x, p);
  TinyWireM.beginTransmission(OLED_ADDR);
  TinyWireM.send(0x40);
  for (uint8_t i = 0; i < 8; i++) {
    TinyWireM.send(on ? pgm_read_byte(&colonTop[i]) : 0x00);
  }
  TinyWireM.endTransmission();

  oledPos(x, p + 1);
  TinyWireM.beginTransmission(OLED_ADDR);
  TinyWireM.send(0x40);
  for (uint8_t i = 0; i < 8; i++) {
    TinyWireM.send(on ? pgm_read_byte(&colonBot[i]) : 0x00);
  }
  TinyWireM.endTransmission();
}

void drawSeconds(uint8_t x, uint8_t p) {
  oledPos(x, p);
  TinyWireM.beginTransmission(OLED_ADDR);
  TinyWireM.send(0x40);
  for (uint8_t i = 0; i < 16; i++) oledData(0x00);
  TinyWireM.endTransmission();

  oledPos(x, p);
  TinyWireM.beginTransmission(OLED_ADDR);
  TinyWireM.send(0x40);
  for (uint8_t i = 0; i < 5; i++) {
    TinyWireM.send(pgm_read_byte(&secFont[(s / 10) * 5 + i]));
  }
  TinyWireM.send(0x00);
  for (uint8_t i = 0; i < 5; i++) {
    TinyWireM.send(pgm_read_byte(&secFont[(s % 10) * 5 + i]));
  }
  TinyWireM.endTransmission();
}

void drawSmallChar(uint8_t idx) {
  TinyWireM.beginTransmission(OLED_ADDR);
  TinyWireM.send(0x40);
  for (uint8_t i = 0; i < 5; i++) {
    TinyWireM.send(pgm_read_byte(&font5x8[idx * 5 + i]));
  }
  TinyWireM.send(0x00);
  TinyWireM.endTransmission();
}

void drawSmallNum(int8_t n) {
  if (n < 0) {
    TinyWireM.beginTransmission(OLED_ADDR);
    TinyWireM.send(0x40);
    TinyWireM.send(0x08);
    TinyWireM.send(0x08);
    TinyWireM.send(0x08);
    TinyWireM.send(0x00);
    TinyWireM.endTransmission();
    n = -n;
  }
  if (n > 99) n = 99;
  drawSmallChar(n / 10);
  drawSmallChar(n % 10);
}

void drawIcon(const uint8_t* icon) {
  TinyWireM.beginTransmission(OLED_ADDR);
  TinyWireM.send(0x40);
  for (uint8_t i = 0; i < 8; i++) {
    TinyWireM.send(pgm_read_byte(&icon[i]));
  }
  TinyWireM.send(0x00);
  TinyWireM.endTransmission();
}

void updateDisplay() {
  const uint8_t TIME_X = 30;
  const uint8_t TIME_Y = 1;

  const uint8_t H1_X = TIME_X;
  const uint8_t H2_X = H1_X + DIGIT_WIDTH + DIGIT_SPACING;
  const uint8_t COLON_X = H2_X + DIGIT_WIDTH + DIGIT_SPACING;
  const uint8_t M1_X = COLON_X + COLON_WIDTH + DIGIT_SPACING;
  const uint8_t M2_X = M1_X + DIGIT_WIDTH + DIGIT_SPACING;

  const uint8_t SEC_X = TIME_X + 28;
  const uint8_t SEC_Y = 4;

  if (h != last_h) {
    drawDigit(h / 10, H1_X, TIME_Y);
    drawDigit(h % 10, H2_X, TIME_Y);
    last_h = h;
  }

  if (m != last_m) {
    drawDigit(m / 10, M1_X, TIME_Y);
    drawDigit(m % 10, M2_X, TIME_Y);
    last_m = m;
  }

  bool targetColon = (s % 2 == 0);
  if (targetColon != last_colon) {
    drawColon(COLON_X, TIME_Y, targetColon);
    last_colon = targetColon;
  }

  if (s != last_s) {
    drawSeconds(SEC_X, SEC_Y);
    last_s = s;
  }

  if (display_temp != last_display_temp || hum != last_hum) {
    oledPos(0, 7);
    for (uint8_t i = 0; i < 128; i++) oledData(0x00);

    oledPos(10, 7);
    drawIcon(iconTemp);
    drawSmallChar(10);
    drawSmallNum(display_temp);
    drawSmallChar(11);
    drawSmallChar(12);

    oledPos(70, 7);
    drawIcon(iconHum);
    drawSmallChar(10);
    drawSmallNum((uint8_t)hum);
    drawSmallChar(13);

    last_display_temp = display_temp;
    last_hum = hum;
  }
}

uint8_t b2d(uint8_t v) { return ((v >> 4) * 10) + (v & 0x0F); }
uint8_t d2b(uint8_t v) { return ((v / 10) << 4) | (v % 10); }

void readRTC() {
  TinyWireM.beginTransmission(RTC_ADDR);
  TinyWireM.send(0x00);
  TinyWireM.endTransmission();
  TinyWireM.requestFrom(RTC_ADDR, 3);
  if (TinyWireM.available() >= 3) {
    s = b2d(TinyWireM.receive() & 0x7F);
    m = b2d(TinyWireM.receive());
    h = b2d(TinyWireM.receive() & 0x3F);
  }
}

void setTime(uint8_t hr, uint8_t mn, uint8_t sc) {
  TinyWireM.beginTransmission(RTC_ADDR);
  TinyWireM.send(0x00);
  TinyWireM.send(d2b(sc));
  TinyWireM.send(d2b(mn));
  TinyWireM.send(d2b(hr));
  TinyWireM.endTransmission();
}

void readDHT() {
  uint8_t bits[5] = {0};
  uint8_t cnt = 7;
  uint8_t idx = 0;

  pinMode(DHT_PIN, OUTPUT);
  digitalWrite(DHT_PIN, LOW);
  delay(18);
  digitalWrite(DHT_PIN, HIGH);
  delayMicroseconds(40);
  pinMode(DHT_PIN, INPUT_PULLUP);

  noInterrupts();

  uint16_t loopCnt = 10000;
  while (digitalRead(DHT_PIN) == LOW) if (--loopCnt == 0) { interrupts(); return; }

  loopCnt = 10000;
  while (digitalRead(DHT_PIN) == HIGH) if (--loopCnt == 0) { interrupts(); return; }

  for (int i = 0; i < 40; i++) {
    loopCnt = 10000;
    while (digitalRead(DHT_PIN) == LOW) if (--loopCnt == 0) { interrupts(); return; }

    unsigned long t = micros();
    loopCnt = 10000;
    while (digitalRead(DHT_PIN) == HIGH) if (--loopCnt == 0) { interrupts(); return; }

    if ((micros() - t) > 40) bits[idx] |= (1 << cnt);
    if (cnt == 0) { cnt = 7; idx++; } else cnt--;
  }

  interrupts();

  uint8_t sum = bits[0] + bits[1] + bits[2] + bits[3];
  if (bits[4] == sum && sum != 0) {
    hum = bits[0];
    temp = bits[2];
    display_temp = (temp * 70) / 100;
  }
}

void setup() {
  delay(500);
  TinyWireM.begin();
  oledInit();
  pinMode(BTN_PIN, INPUT_PULLUP);

  readRTC();
  readDHT();

  last_h = last_m = last_s = 255;
  last_display_temp = -127;
  last_hum = -1;
  last_colon = !(s % 2 == 0);
  updateDisplay();
}

void loop() {
  unsigned long now = millis();

  if (digitalRead(BTN_PIN) == LOW) {
    delay(50);
    if (digitalRead(BTN_PIN) == LOW) {
      m += 10;
      if (m >= 60) {
        m -= 60;
        h++;
        if (h > 23) h = 0;
      }
      s = 0;
      setTime(h, m, s);
      last_h = last_m = last_s = 255;
      updateDisplay();

      unsigned long waitStart = millis();
      while (digitalRead(BTN_PIN) == LOW) {
        if (millis() - waitStart > 2000) break;
      }
      delay(100);
    }
  }

  if (now - prevTime_CLK >= CLK_INTERVAL) {
    prevTime_CLK = now;
    readRTC();
    updateDisplay();
  }

  if (now - prevTime_DHT >= DHT_INTERVAL) {
    prevTime_DHT = now;
    readDHT();
  }
}
