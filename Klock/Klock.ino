#include <RTClib.h>
#include <Wire.h>
#include "U8glib.h"
#include <Servo.h>

const int tmpPin = A1;
int servoPosition = 0;
int seconds = 0;
int minutes = 0;
int hours = 0;
unsigned long lastUpdateTime = 0;

RTC_DS3231 rtc;
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);
Servo myservo;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  pinMode(tmpPin, INPUT);
  rtc.begin();
  myservo.attach(9);
  u8g.setFont(u8g_font_unifont);
  myservo.write(servoPosition);
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

void loop() {
  DateTime now = rtc.now();

  /
  updateTimer();
  String countUpTime = formatTime(hours, minutes, seconds);

  
  String realTimeClock = formatTime(now.hour(), now.minute(), now.second());

  
  u8g.firstPage();
  do {
    u8g.drawStr(10, 20, countUpTime.c_str()); 
    u8g.drawStr(10, 40, realTimeClock.c_str()); 
  } while (u8g.nextPage());

  
  float currentTemp = getTemp();
  if (currentTemp > 20.0) {
    Serial.println("System paused due to high temperature.");
    oledWrite(10, 20, "Paused: Temp > 20C");
    delay(500);
    return;
  }

  
  servoPosition = map(seconds, 0, 59, 0, 180);
  myservo.write(servoPosition);

  delay(1000);
}

void updateTimer() {
  if (millis() - lastUpdateTime >= 1000) {
    lastUpdateTime = millis();
    seconds++;
    if (seconds >= 60) {
      seconds = 0;
      minutes++;
      if (minutes >= 60) {
        minutes = 0;
        hours++;
      }
    }
  }
}

String formatTime(int h, int m, int s) {
  return (String(h) + ":" + (m < 10 ? "0" : "") + String(m) + ":" + (s < 10 ? "0" : "") + String(s));
}

float getTemp() {
  int Vo = analogRead(tmpPin);
  float R1 = 10000;
  float R2 = R1 * (1023.0 / (float)Vo - 1.0);
  float logR2 = log(R2);
  float T = (1.0 / (0.001129148 + 0.000234125 * logR2 + 0.0000000876741 * logR2 * logR2 * logR2));
  T = T - 273.15;

  Serial.println(T);
  return T;
}

void oledWrite(int x, int y, String text) {
  u8g.firstPage();
  do {
    u8g.drawStr(x, y, text.c_str());
  } while (u8g.nextPage());
}
