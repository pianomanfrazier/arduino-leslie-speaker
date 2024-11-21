#include <Arduino.h>

#define LED_PIN 7
#define POT_PIN A0

int timer = 1000;
unsigned long t = 0;
unsigned long last = 0;
unsigned long delta = 0;
bool led_state = false;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  t = millis();
}

void loop() {
  t = millis();
  delta += t - last;

  timer = map(analogRead(POT_PIN), 0, 1023, 25, 500);

  if (delta > timer) {
    led_state = !led_state;
    delta = 0;
  }

  if (led_state) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }

  last = t;
}