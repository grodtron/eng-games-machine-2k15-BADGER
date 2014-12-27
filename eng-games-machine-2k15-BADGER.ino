/* TODO: Open bottom flaps slower for best drop
         Measure period when waiting for next shot to save time         
*/
#include <Adafruit_PWMServoDriver.h>
#include <Adafruit_TCS34725.h>
#include <Wire.h>
#include <avr/io.h>

#include <avr/interrupt.h>

#define BADGER_ADDRESS 0x20
#define LOADER_ADDRESS 0x21

const int N_MOTORS = 4;

const int eStopPin = 7;

const int leftMotorErrorPin = 1;
const int rightMotorErrorPin = 0;

const int tiltSensePin = 13;
const int rightSensePin = 4;
const int leftSensePin = 2;

const int targetPin = 3;

const int colorSenseLedPin = A3;

const int irSensorPin = A1;      

const int leftBeltSensePin = A0;
const int rightBeltSensePin = A3;

const int switchPins[7] = {2, 3, 4, A0, A2, A3, 13};

/*
    4  = S1
    3  = S2
    2  = S3 
    A2 = S4
    A3 = S5
    A0 = S6
    13 = S7 
*/

// TODO
const int ledPinA   = 12;
const int ledPinB   = 12;
const int ledPinC   = 12;

/*const int LEFT_FWD = 5;
const int LEFT_BAK = 3;

const int RIGHT_FWD = 6;
const int RIGHT_BAK = 4;*/

Adafruit_PWMServoDriver servos = Adafruit_PWMServoDriver();
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_1X);

#define CLOSE 520
#define OPEN 200
#define MAX_BAG_COUNT 8
const int servoOrder[MAX_BAG_COUNT] = {1, 5, 2, 4, 3, 7, 12, 6}; // Order to cloe and open flaps, alternating each tower
/* Servos to Pins
front  -   back tower
    X - 6
    3 - 7
    2 - 4
    1 - 5
*/

int pwmPins [N_MOTORS] = {9,  10, 11, 6};
int digPins [N_MOTORS] = {12, 5, 8, 7};

const int RIGHT_FWD_PWM  = 9;
const int RIGHT_BAK_PWM  = 11;
const int LEFT_FWD_PWM = 10;
const int LEFT_BAK_PWM = 6;

const int RIGHT_FWD_DIR  = 12;
const int RIGHT_BAK_DIR  = 8;
const int LEFT_FWD_DIR = 5;
const int LEFT_BAK_DIR = 7;
                
int bag_count = 0;

#define MOVE_FWD(SIDE, amount) do {  \
  analogWrite(SIDE ## _FWD_PWM, 255 - (amount)); \
  analogWrite(SIDE ## _BAK_PWM, 255 - (amount)); \
  digitalWrite(SIDE ## _FWD_DIR, HIGH);   \
  digitalWrite(SIDE ## _BAK_DIR, HIGH);   \
} while(0)

#define MOVE_BAK(SIDE, amount) do {          \
  analogWrite(SIDE ## _FWD_PWM, amount); \
  analogWrite(SIDE ## _BAK_PWM, amount); \
  digitalWrite(SIDE ## _FWD_DIR, LOW);   \
  digitalWrite(SIDE ## _BAK_DIR, LOW);   \
} while(0)
  
#define BEEPER_ON() do{ \
} while(0)

#define BEEPER_OFF() do{ \
} while(0)

boolean left, right, tilt;
void (*currentState)();

void softStarting();
void waitingForLargeRadiusTurn();
void doingLargeRadiusTurn();
void doingSpeedBump();
void doingFirstSmallRadiusTurn();
void waitingForSecondSmallRadiusTurn();
void error(int code);

void waitingForFinalOrangeLine();

void motorsOff() {
   for(int i = 0; i < N_MOTORS; ++i){
    digitalWrite(pwmPins[i], LOW);
    digitalWrite(digPins[i], LOW);
  }  
}

volatile boolean flag = false;

void setup(){
  Serial.begin(9600);
//  Wire.begin(BADGER_ADDRESS);
//  Wire.onReceive(receiveEvent);

  BEEPER_ON();
  for(int i = 0; i < N_MOTORS; ++i){
    pinMode(pwmPins[i], OUTPUT);
    pinMode(digPins[i], OUTPUT);
    
    digitalWrite(pwmPins[i], LOW);
    digitalWrite(digPins[i], LOW);
  }
//  MOVE_FWD(LEFT, 0);
//  MOVE_FWD(RIGHT, 0);

  pinMode(leftSensePin, INPUT);
  pinMode(rightSensePin, INPUT);
  pinMode(tiltSensePin, INPUT);
  pinMode(irSensorPin, INPUT);
  pinMode(targetPin, INPUT);
  pinMode(leftBeltSensePin, INPUT);
  pinMode(rightBeltSensePin, INPUT);

  for(int i = 0; i < 7; ++i) {
    pinMode(switchPins[i], INPUT);  
  }
  digitalWrite(tiltSensePin, HIGH);
  /*
  pinMode(eStopPin, INPUT);  
  pinMode(leftMotorErrorPin, INPUT);
  pinMode(rightMotorErrorPin, INPUT);
  pinMode(ledPinA, OUTPUT);
  pinMode(ledPinB, OUTPUT);
  pinMode(ledPinC, OUTPUT);
  pinMode(colorSenseLedPin, OUTPUT);
  digitalWrite(colorSenseLedPin, LOW);*/
  if (tcs.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1);
  }

  servos.begin();
  servos.setPWMFreq(60);

  servos.setPWM(9, 0, 4095);
  currentState = doingTrackOnRightSide;

  // open all flaps
  /*for(int i = 0; i < MAX_BAG_COUNT; ++i) {
    servos.setPWM(servoOrder[i], 0, OPEN);
    delay(500);
  }*/

  servos.setPWM(servoOrder[0], 0, CLOSE);
  servos.setPWM(servoOrder[1], 0, CLOSE);

  delay(100);
  BEEPER_OFF();

 // TestBagPlacing();
 //  bag_count = 8;
 // while(1);
 
 // Blink color sensor LED
  for(int i = 0 ; i < 4; ++i) {
    servos.setPWM(15, 0, 0 );
    delay(500);
    servos.setPWM(15, 0, 4095 );
    delay(500);
  }
}

void loop(){
  left  = digitalRead(leftSensePin);
  right = digitalRead(rightSensePin);
  tilt = digitalRead(tiltSensePin);  

  if(currentState){
    currentState();
  }
}

void softStarting(){
 
  static int count = 0;
  
  if(++count < 75){
    MOVE_FWD(RIGHT, count);
    MOVE_FWD(LEFT,  count);
    delay(1);
  }else{
    currentState = onOffStyle;
  }
}

void error(int code){
  MOVE_FWD(RIGHT, 0);
  MOVE_FWD(LEFT,  0);

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
