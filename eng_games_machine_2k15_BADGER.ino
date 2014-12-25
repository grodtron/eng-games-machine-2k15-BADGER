#include <Adafruit_PWMServoDriver.h>
#include <Adafruit_TCS34725.h>
#include <Wire.h>

#define BADGER_ADDRESS 0x20
#define LOADER_ADDRESS 0x21

const int rightSensePin = A0;
const int leftSensePin = A1;
const int eStopPin = 7;

const int leftMotorErrorPin = 1;
const int rightMotorErrorPin = 0;

const int tiltSensePin = 13;

const int colorSenseLedPin = A3;

// TODO
const int ledPinA   = 12;
const int ledPinB   = 12;
const int ledPinC   = 12;

const int LEFT_FWD = 5;
const int LEFT_BAK = 3;

const int RIGHT_FWD = 6;
const int RIGHT_BAK = 4;

#define CLOSE 1025
#define OPEN 400
#define MAX_BAG_COUNT 8
const int servoOrder[MAX_BAG_COUNT] = {11, 4, 10, 3, 9, 2, 8, 1}; // Order to cloe and open flaps, alternating each tower
/* Servos to Pins
front  -   back tower
     8 - 1
     9 - 2
    10 - 3
    11 - 4
*/
int bag_count = 0;

#define MOVE_FWD(SIDE, amount) do {  \
  analogWrite(SIDE ## _FWD, amount); \
  digitalWrite(SIDE ## _BAK, LOW);   \
} while(0)

#define MOVE_BAK(SIDE, amount) do {          \
  analogWrite(SIDE ## _FWD, 255 - (amount)); \
  digitalWrite(SIDE ## _BAK, HIGH);          \
} while(0)

#define MOTORS_OFF() do {     \
  digitalWrite(LEFT_FWD, LOW); \
  digitalWrite(LEFT_BAK, LOW);   \
  digitalWrite(RIGHT_BAK, LOW);   \
  digitalWrite(RIGHT_BAK, LOW);   \
} while(0)
  
#define BEEPER_ON() do{ \
  analogWrite(11, 200); \
} while(0)

#define BEEPER_OFF() do{ \
  analogWrite(11, 0); \
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

void setup(){
  Serial.begin(9600);
  
  BEEPER_ON();
  pinMode(LEFT_FWD, OUTPUT);
  pinMode(LEFT_BAK, OUTPUT);
  pinMode(RIGHT_FWD, OUTPUT);
  pinMode(RIGHT_BAK, OUTPUT);

  pinMode(leftSensePin, INPUT);
  pinMode(rightSensePin, INPUT);
  pinMode(eStopPin, INPUT);
  
  pinMode(tiltSensePin, INPUT);
  
  pinMode(leftMotorErrorPin, INPUT);
  pinMode(rightMotorErrorPin, INPUT);

  pinMode(ledPinA, OUTPUT);
  pinMode(ledPinB, OUTPUT);
  pinMode(ledPinC, OUTPUT);
  
  pinMode(colorSenseLedPin, OUTPUT);
  digitalWrite(colorSenseLedPin, LOW);
  
  MOVE_FWD(LEFT, 0);
  MOVE_FWD(RIGHT, 0);
  
  currentState = doingTrackOnRightSide;
  delay(100);
  BEEPER_OFF();
  
  Wire.begin(BADGER_ADDRESS);
  Wire.onReceive(receiveEvent);  
}

void loop(){
  left  = digitalRead(leftSensePin);
  right = digitalRead(rightSensePin);
  tilt = digitalRead(tiltSensePin);
  
  digitalWrite(ledPinA, tilt);
  digitalWrite(ledPinA, tilt);
  
  if(! digitalRead(leftMotorErrorPin)){
     // TODO error(6); 
  }
  if(! digitalRead(rightMotorErrorPin)){
     // TODO error(7); 
  }
  
  if(digitalRead(eStopPin)){
    error(5);
  } else {
    if(currentState){
      currentState();
    }
  }
}

void softStarting(){
 
  static int count = 0;
  
  if(++count < 75){
    MOVE_FWD(RIGHT, count);
    MOVE_FWD(LEFT,  count);
    delay(1);
  }else{
    currentState = doingTrackOnRightSide;
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
