/*   
 *   Basic example code for controlling a stepper without library
 *      
 *   by Dejan, https://howtomechatronics.com
 */
#include <Arduino.h>
#include <TimerOne.h>

// defines pins
#define LED_PIN 7
#define POT_PIN A0
#define STEP_PIN 10
#define DIR_PIN 11
#define STEPS_PER_ROTATION 200*16 // 1/16 microstepping

int potVal = 0;
int previousPotVal = 0;
int period = 300;
int dutyTime = 30;
void setup() {
  // Sets the two pins as Outputs
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  digitalWrite(DIR_PIN, HIGH);
  // Timer1.initialize(period);
  // Timer1.pwm(STEP_PIN, 1023 / 10);
}
void loop() {
  // potVal = analogRead(POT_PIN);
  // if (abs(potVal - previousPotVal) > 20) {
  //   previousPotVal = potVal;
  //   period = map(potVal, 0, 1023, 90, 900);
  //   Timer1.setPeriod(period);
  // }
  // Makes 200 pulses for making one full cycle rotation
  for(int x = 0; x < (STEPS_PER_ROTATION); x++) {
    digitalWrite(STEP_PIN, HIGH); 
    digitalWrite(LED_PIN, HIGH);
    // delayMicroseconds(dutyTime);    // by changing this time delay between the steps we can change the rotation speed
    digitalWrite(STEP_PIN, LOW); 
    digitalWrite(LED_PIN, LOW);
    delayMicroseconds(dutyTime*9);
    if (dutyTime > 5 && x%100==0) {
      dutyTime-=1;
    } 
  }
}