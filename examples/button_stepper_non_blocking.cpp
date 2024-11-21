// testing a stepper motor with a Pololu A4988 driver board or equivalent

// this version uses millis() to manage timing rather than delay()
// and the movement is determined by a pair of momentary push switches
// press one and it turns CW, press the other and it turns CCW
#include <Arduino.h>

// defines pins
#define LED_PIN 7
#define POT_PIN A0
#define STEP_PIN 10
#define DIR_PIN 11
#define STEPS_PER_ROTATION 200*16 // 1/16 microstepping


unsigned long curMicros;
unsigned long prevStepMicros = 0;
unsigned long microsBetweenSteps = 50; // microseconds
boolean led_on = false;
int pot_output = 0;
int prev_pot_output = 0;

void singleStep();

void setup() { 
  pinMode(DIR_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  digitalWrite(DIR_PIN, LOW);
  pot_output = analogRead(POT_PIN);
  prev_pot_output = pot_output;
}

void loop() { 
  curMicros = micros();

  singleStep();
}

void singleStep() {
  if (curMicros - prevStepMicros >= microsBetweenSteps) {
    led_on = !led_on;
    if (led_on) {
      digitalWrite(LED_PIN, HIGH);
    }
    // prevStepMillis += millisBetweenSteps;
    prevStepMicros = curMicros;

    // pot_output = analogRead(POT_PIN);
    // if (abs(prev_pot_output - pot_output) > 5) {
    //   microsBetweenSteps = map(pot_output, 0, 1023, 50, 500);
    //   prev_pot_output = pot_output;
    // }

    // next 2 lines changed 28 Nov 2018
    //prevStepMillis += millisBetweenSteps;
    // prevStepMillis = curMillis;
    digitalWrite(STEP_PIN, HIGH);
  }
  digitalWrite(STEP_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
}