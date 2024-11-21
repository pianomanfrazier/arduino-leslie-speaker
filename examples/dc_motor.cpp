#include <Arduino.h>

#define LED_PIN 11
#define POT_PIN A0
// PWM pins 3, 5, 6, 9, 10, 11, 13
// L298N PINS
#define ENA_PIN 9
#define IN1_PIN 8
#define IN2_PIN 7 

unsigned long timer = 1000;
unsigned long t = 0;
unsigned long last = 0;
unsigned long delta = 0;
bool led_state = false;
int motor_speed = 255;
int pot_output = 0;

void setup() {
  pinMode(ENA_PIN, OUTPUT);
  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  t = millis();

  digitalWrite(IN1_PIN, HIGH);
  digitalWrite(IN2_PIN, LOW);
}

void loop() {
  t = millis();
  delta += t - last;

  pot_output = analogRead(POT_PIN);

  timer = map(pot_output, 0, 1023, 500, 25);

  if (delta > timer) {
    led_state = !led_state;
    delta = 0;
  }

  if (led_state) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }

  // 255 => 550 RPM
  // spin motor at 72% is 400RPM
  // 11% is 30RPM
  motor_speed = map(pot_output, 0, 1023, 100, 185);
  analogWrite(ENA_PIN, motor_speed);

  last = t;
}