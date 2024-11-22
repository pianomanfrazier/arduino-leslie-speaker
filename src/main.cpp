/*   
 *   Basic example code for controlling a stepper without library
 *      
 *   by Dejan, https://howtomechatronics.com
 */
#include <Arduino.h>

#define FAST_POT_PIN A0
#define SLOW_POT_PIN A1
#define ACC_POT_PIN A2
#define DEC_POT_PIN A3
#define LED_GREEN_PIN 8
#define LED_YELLOW_PIN 9

#define FAST_INPUT_PIN 7
#define SLOW_INPUT_PIN 6

#define STEP_PIN 10
#define DIR_PIN 11
#define EN_PIN 12
#define STEPS_PER_ROTATION 200*16 // 1/8 microstepping

/**
 * Gear ratio of 3 to 1
 * need rpms fast 380rpm to slow 30rpm
 * 1200 -> 7.5RPM * 3 = 22.5
 * 900 -> 10RPM * 3 = 30
 * 600 -> 15RPM * 3 = 45
 * 300 -> 30RPM
 * 150 -> 60RPM
 * 75 -> 108RPM * 3 = 324
 * 60 -> 125RPM * 3 = 375
 * 46 -> 133RPM * 3 = 400
 * 30 -> 227RPM
*/

#define STOP_SPEED 2400
#define FAST_SPEED_MAX 46
#define FAST_SPEED_MIN 75
#define SLOW_SPEED_MAX 600
#define SLOW_SPEED_MIN 1000

#define DEFAULT_FAST_SPEED 40
#define DEFAULT_SLOW_SPEED 900

// 20 -> 2 s from FAST to STOP
// 40 -> 3 s
// 60 -> 4 s
// 100 -> 6 s
#define MAX_FACTOR 2 
#define MIN_FACTOR 100

#define DEFAULT_ACC_FACTOR 40
#define DEFAULT_DEC_FACTOR 100 

#define FAST_INDEX 0
#define SLOW_INDEX 1
#define ACC_INDEX 2
#define DEC_INDEX 3

int potPins[] = {FAST_POT_PIN, SLOW_POT_PIN, ACC_POT_PIN, DEC_POT_PIN};
int potVals[] = {0, 0, 0, 0};
int previousPotVals[] = {0, 0, 0, 0};
int settings[] = {DEFAULT_FAST_SPEED, DEFAULT_SLOW_SPEED, DEFAULT_ACC_FACTOR, DEFAULT_DEC_FACTOR};
const int SETTINGS_RANGES[4][2] = {{FAST_SPEED_MAX, FAST_SPEED_MIN}, {SLOW_SPEED_MAX, SLOW_SPEED_MIN}, {MAX_FACTOR, MIN_FACTOR}, {MAX_FACTOR, MIN_FACTOR}};

enum State {
  STOP,
  FAST,
  SLOW
};

int potVal = 0;
int previousPotVal = 0;

unsigned long t = 0;
unsigned long last = 0;
unsigned long delta = 0;
unsigned long acc_delta = 0;
unsigned long input_delta = 0;

State currentState = State::STOP;
unsigned long stopPoint = STOP_SPEED;
unsigned long period = STOP_SPEED;

State readState();
void readPots();
void readPotsTimed();
void outputLED(State);
void stepMotor();
void updateStopPoint();
void updatePeriod();

void setup() {

  /**
   * analogRead is really slow and interrupts the stepper motor
   * speeding up the ADC (Analog to Digital Converter) settings will reduce the clunking noise
   * if reading analog pins every cycle then speed up the ADC settings
   * the downside is that you sacrifice accuracy
   * see http://gammon.com.au/adc
   */
  // ADCSRA &= ~(bit (ADPS0) | bit (ADPS1) | bit (ADPS2)); // clear prescaler bits

  // uncomment as required
  //  ADCSRA |= bit (ADPS0);                               //   2  
  //  ADCSRA |= bit (ADPS1);                               //   4  
  //  ADCSRA |= bit (ADPS0) | bit (ADPS1);                 //   8  
  //  ADCSRA |= bit (ADPS2);                               //  16 
  //  ADCSRA |= bit (ADPS0) | bit (ADPS2);                 //  32 
  //  ADCSRA |= bit (ADPS1) | bit (ADPS2);                 //  64 
  //  ADCSRA |= bit (ADPS0) | bit (ADPS1) | bit (ADPS2);   // 128

  pinMode(STEP_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);
  pinMode(LED_YELLOW_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(EN_PIN, OUTPUT);

  pinMode(SLOW_INPUT_PIN, INPUT_PULLUP);
  pinMode(FAST_INPUT_PIN, INPUT_PULLUP);

  digitalWrite(DIR_PIN, HIGH);
  digitalWrite(EN_PIN, HIGH);

  /**
   * Will need to reset the arduino to read settings
   */
  readPots();
}
void loop() {
  t = micros();
  delta += t - last;
  acc_delta += t - last;
  input_delta += t - last;

  currentState = readState();
  outputLED(currentState);
  
  /**
   * analogRead is really slow and interrupts
   * the stepper motor
   */
  // readPotsTimed();
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
    stopPoint = settings[FAST_INDEX];
  }
  if (currentState == State::SLOW) {
    stopPoint = settings[SLOW_INDEX];
  }
  if (currentState == State::STOP) {
    stopPoint = STOP_SPEED;
  }
}

void updatePeriod() {
  if (acc_delta > 10000) {

    // DECELERATE
    if (period < stopPoint) {
      unsigned int val = int(trunc(period/settings[3]));
      period += (1 + val);
      // This can overshoot the STOP_SPEED and not stop
      if (period > STOP_SPEED - val - 2) {
        period = STOP_SPEED;
      }
    }
    // ACCELERATE
    if (period > stopPoint) {
      period -= (1 + int(trunc(period/settings[2])));
    }

    if (period == STOP_SPEED) {
      digitalWrite(EN_PIN, HIGH);
    } else {
      digitalWrite(EN_PIN, LOW);
    }

    acc_delta = 0;
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

void readPots() {
    potVals[0] = analogRead(FAST_POT_PIN);
    potVals[1] = analogRead(SLOW_POT_PIN);
    potVals[2] = analogRead(ACC_POT_PIN);
    potVals[3] = analogRead(DEC_POT_PIN);
    for (byte i = 0; i < 4; i++) {
      if (potVals[i] - previousPotVals[i] > 2 || potVals[i] - previousPotVals[i] < -2) {
        settings[i] = map(potVals[i], 0, 1023, SETTINGS_RANGES[i][0], SETTINGS_RANGES[i][1]);
        previousPotVals[i] = potVals[i];
      }
    }
}

void readPotsTimed() {
  if (input_delta > 1000000) {
    readPots();

    input_delta = 0;
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
