/*   
 *   Basic example code for controlling a stepper without library
 *      
 *   by Dejan, https://howtomechatronics.com
 */
#include <Arduino.h>

#define POT_PIN A0
#define LED_GREEN_PIN 8
#define LED_YELLOW_PIN 9

#define FAST_INPUT_PIN 7
#define SLOW_INPUT_PIN 6

#define STEP_PIN 10
#define DIR_PIN 11
#define EN_PIN 12
#define STEPS_PER_ROTATION 200*16 // 1/8 microstepping

#define FAST_SPEED 30
#define SLOW_SPEED 300
#define STOP_SPEED 600

enum State {
  STOP,
  FAST,
  SLOW
};

int potVal = 0;
int previousPotVal = 0;

unsigned long period = 500;
unsigned long t = 0;
unsigned long last = 0;
unsigned long delta = 0;
unsigned long input_delta = 0;

State currentState = State::STOP;
unsigned long stopPoint = STOP_SPEED;

State readState();
void outputLED(State);
void stepMotor();
void updateStopPoint();
void updatePeriod();

void setup() {
  pinMode(STEP_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);
  pinMode(LED_YELLOW_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(EN_PIN, OUTPUT);

  pinMode(SLOW_INPUT_PIN, INPUT_PULLUP);
  pinMode(FAST_INPUT_PIN, INPUT_PULLUP);

  digitalWrite(EN_PIN, LOW);
  digitalWrite(DIR_PIN, HIGH);
  digitalWrite(EN_PIN, HIGH);
  // digitalWrite(LED_GREEN_PIN, HIGH);
  // digitalWrite(LED_YELLOW_PIN, HIGH);
}
void loop() {
  t = micros();
  delta += t - last;
  input_delta += t - last;

  currentState = readState();
  outputLED(currentState);
  
  updateStopPoint();
  updatePeriod();
  stepMotor();

  last = t;
}

void stepMotor() {
  if (delta > period) {
    digitalWrite(STEP_PIN, HIGH); 
    digitalWrite(STEP_PIN, LOW); 
    delta = 0;
  }
}

void updateStopPoint() {
  if (currentState == State::FAST) {
    stopPoint = FAST_SPEED;
  }
  if (currentState == State::SLOW) {
    stopPoint = SLOW_SPEED;
  }
  if (currentState == State::STOP) {
    stopPoint = STOP_SPEED;
  }
}

void updatePeriod() {
  if (input_delta > 10000) {

    if (period < stopPoint) {
      period += 1;
    }
    if (period > stopPoint) {
      period -= 1;
    }
    if (period == STOP_SPEED) {
      digitalWrite(EN_PIN, HIGH);
    } else {
      digitalWrite(EN_PIN, LOW);
    }

    input_delta = 0;
  }
}

State readState() {
  // PULLUP inputs read low when connected
  if (!digitalRead(SLOW_INPUT_PIN)) {
    return State::SLOW;
  } else if (!digitalRead(FAST_INPUT_PIN)) {
    return State::FAST;
  } else {
    return State::STOP;
  }
}

void outputLED(State state) {
  if (state == State::SLOW) {
    digitalWrite(LED_YELLOW_PIN, HIGH);
  } else {
    digitalWrite(LED_YELLOW_PIN, LOW);
  }

  if (state == State::FAST) {
    digitalWrite(LED_GREEN_PIN, HIGH);
  } else {
    digitalWrite(LED_GREEN_PIN, LOW);
  }
}
