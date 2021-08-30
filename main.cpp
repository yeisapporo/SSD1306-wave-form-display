// Sound wave form display on 128*32 OLED(SSD1306).
// Core : ATMEGA1284P(MightyCore).

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_I2CDevice.h> 
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define WAVE_BUFFER_SIZE  (256)

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);

volatile short wavBuff[WAVE_BUFFER_SIZE];
volatile int buffIdx;
volatile bool wavBuffBusy;

// int handler(read sound data into buffer)
ISR (TIMER1_COMPA_vect) {
  wavBuffBusy = true;
  wavBuff[buffIdx] = analogRead(A2);
  ++buffIdx %= WAVE_BUFFER_SIZE;
  wavBuffBusy = false;
}

void setTimerInt() {
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1B |= (1 << WGM12) | (1 << CS12);
  OCR1A = 23;
  TIMSK1 |= (1 << OCIE1A);
}
void setup() {
  memset((void *)wavBuff, 0, WAVE_BUFFER_SIZE * 2);
  wavBuffBusy = false;
  buffIdx = 0;
  // debug LED
  pinMode(A0, OUTPUT);
  // set timer int.
  setTimerInt();

  display.begin(0x3c);
  display.display();
  delay(2000);
  display.clearDisplay();
}

void loop() {
  // use driver's buffer
  static unsigned char *vRam = display.getBuffer();
  // blink LED(debug)
  digitalWrite(A0, !digitalRead(A0));
  // make wave form
  memset(vRam, 0, 640);
  for(int x = 0; x < 128; x++) {
    int y = wavBuff[x] / 5 + 10;
    vRam[x + (y >> 3) * 128] = (1 << (y & 7));
  }
  // frame
  display.drawRect(0, 0, 128, 32, SSD1306_WHITE);

  display.display();
  delay(1);
}
