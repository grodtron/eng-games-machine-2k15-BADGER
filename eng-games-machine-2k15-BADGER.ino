/* TODO:  
   
* Need to fully test complete bag collection state and transition to start moving (includes i2c communication test)
- Measure hole & wood period every shot, compare it to some expected value?
      Figure out when is the best time to calculate it too
- Need to find a way to measure accurately the shooting time of the target
- Explore other motor speed for optimal trajectory (needs a better shooting success rate)
x Opening servos slower when releasing flaps for shooting (TO BE TESTED)

*/
#include <Adafruit_PWMServoDriver.h>
#include <Adafruit_TCS34725.h>
#include <Wire.h>
#include "SingleMotorTweener.h"
//#include "motors.h"

#include <Adafruit_INA219.h>
Adafruit_INA219 ina219;

#define BADGER_ADDRESS 0x20
#define LOADER_ADDRESS 0x21

/*const int eStopPin = 7;
const int leftMotorErrorPin = 1;
const int rightMotorErrorPin = 0;*/

const int tiltSensePin = 13;
const int tiltSensePin2 = A2;
const int rightSensePin = 4;
const int leftSensePin = 2;
const int startButton = 3;
const int leftBeltSensePin = A0;
const int rightBeltSensePin = A3;
//const int targetPin = 3;
const int irSensorPin = A1;      

// TODO
const int ledPinA   = 12;
const int ledPinB   = 12;
const int ledPinC   = 12;

Adafruit_PWMServoDriver servos = Adafruit_PWMServoDriver();
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_1X);

#define CLOSE 510
#define OPEN 215
#define MAX_BAG_COUNT 8
const int servoOrder[MAX_BAG_COUNT] = {1, 5, 2, 4, 3, 7, 0, 6}; // Order to close and open flaps, alternating each tower

int pwmPins [N_MOTORS] = {9,  10, 11, 6};
int digPins [N_MOTORS] = {12, 5, 8, 7};
                
int bag_count = 0;

int total_bags = MAX_BAG_COUNT;
boolean left, topLeft, right, tilt, tilt2, start, started = false;

void (*currentState)();

void softStarting();
void onOffStyle(); // Race mode
void doingTrackOnRightSide(); // Tower mode
void error(int code);

#define RUN_TESTS 1

void setup(){
  Serial.begin(9600);

  Serial.println("testing");

  Wire.begin(BADGER_ADDRESS);
  Wire.onReceive(receiveEvent);

  servos.begin();
  servos.setPWMFreq(60);

  Serial.print("Stopping pulley ");
  for(int i = 0; i < 10; ++i) {
    servos.setPWM(9, 0, 4095); // Belt motor off
    Serial.print(".");
  }
  Serial.println(" done");
  
  Serial.println("opening voltage sensor");
  ina219.begin();


  Serial.println("init'ing motors");
  for(int i = 0; i < N_MOTORS; ++i){
    pinMode(pwmPins[i], OUTPUT);
    pinMode(digPins[i], OUTPUT);
    
    digitalWrite(pwmPins[i], LOW);
    digitalWrite(digPins[i], LOW);
  }
  
  pinMode(leftSensePin, INPUT);
  pinMode(startButton, INPUT);
  pinMode(rightSensePin, INPUT);
  pinMode(tiltSensePin, INPUT);
  pinMode(tiltSensePin2, INPUT);
  pinMode(leftBeltSensePin, INPUT);
  pinMode(rightBeltSensePin, INPUT);

  pinMode(irSensorPin, INPUT);

  digitalWrite(tiltSensePin, HIGH);
  digitalWrite(tiltSensePin2, HIGH);

  currentState = waitingForBags;

  Serial.println("open all flaps");
  // open all flaps
  for(int i = 2; i < MAX_BAG_COUNT; ++i) {
    servos.setPWM(servoOrder[i], 0, OPEN);
    delay(500);
  }

  servos.setPWM(servoOrder[0], 0, CLOSE+50);
  delay(100);
  servos.setPWM(servoOrder[1], 0, CLOSE);

  if (tcs.begin()) {
    Serial.println("Found color sensor, gonna shoot sum bags");
    currentState = waitingForBags;
  } else if (digitalRead(leftSensePin)) {
    Serial.println("No color sensor, we gon run today");
    while (1);
  }

#if RUN_TESTS  
  runTests();
  while(1);
#endif

   // Blink color sensor LED
  for(int i = 0 ; i < 4; ++i) {
    servos.setPWM(15, 0, 0 );
    delay(150);
    servos.setPWM(15, 0, 4095 );
    delay(150);
  }
}

void loop(){
  left  = digitalRead(leftSensePin);
  topLeft = digitalRead(startButton);
  right = digitalRead(rightSensePin);
  tilt  = !digitalRead(tiltSensePin);  
  tilt2 = !digitalRead(tiltSensePin2);
  start = digitalRead(startButton);  

  if(currentState){
    currentState();
  }
}

void softStarting(){
 
  static int count = 0;
  
  if(++count < 75){
    MOVE_SIDE_FWD(RIGHT, count);
    MOVE_SIDE_FWD(LEFT,  count);
    delay(1);
  }else{
    currentState = onOffStyle;
  }
}

void error(int code){
  MOVE_SIDE_FWD(RIGHT, 0);
  MOVE_SIDE_FWD(LEFT,  0);

  Serial.print("ERROR: ");
  Serial.println(code);

  while(1){
    for(int i = 0; i < 4; ++i){
      digitalWrite(ledPinA, HIGH);
      digitalWrite(ledPinB, HIGH);
      digitalWrite(ledPinC, HIGH);
      delay(125);
      digitalWrite(ledPinA, LOW);
      digitalWrite(ledPinB, LOW);
      digitalWrite(ledPinC, LOW);
      delay(125);
    }
    digitalWrite(ledPinA, (code >> 0) & 1);
    digitalWrite(ledPinB, (code >> 1) & 1);
    digitalWrite(ledPinC, (code >> 2) & 1);
    delay(1000);
  }
}
