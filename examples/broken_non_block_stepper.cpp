/*   
 *   Basic example code for controlling a stepper without library
 *      
 *   by Dejan, https://howtomechatronics.com
 */
#include <Arduino.h>

// defines pins
#define POT_PIN A0
#define STEP_PIN 10
#define DIR_PIN 11
#define STEPS_PER_ROTATION 200*8 // 1/8 microstepping

int potVal = 0;
int previousPotVal = 0;

unsigned long period = 100;
unsigned long t = 0;
unsigned long last = 0;
unsigned long delta = 0;

void setup() {
  // Sets the two pins as Outputs
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  digitalWrite(DIR_PIN, HIGH);
}
void loop() {
  t = millis();
  delta += t - last;

  if (delta > period) {
    digitalWrite(STEP_PIN, HIGH); 
    digitalWrite(STEP_PIN, LOW); 
    delta = 0;
  }

  last = t;
}