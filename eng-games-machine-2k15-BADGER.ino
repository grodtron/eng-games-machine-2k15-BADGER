/* TODO:  
   
* Need to fully test complete bag collection state and transition to start moving (includes i2c communication test)
- Measure hole & wood period every shot, compare it to some expected value? Figure out when is the best time to calculate it too
- Need to find a way to measure accurately the shooting time of the target
- Explore other motor speed for optimal trajectory (needs a better shooting success rate)
x Opening servos slower when releasing flaps for shooting (TO BE TESTED)

*/
#include <Adafruit_PWMServoDriver.h>
#include <Adafruit_TCS34725.h>
#include <Wire.h>

#define BADGER_ADDRESS 0x20
#define LOADER_ADDRESS 0x21
#define RUN_TESTS 1

const int N_MOTORS = 4;

/*const int eStopPin = 7;
const int leftMotorErrorPin = 1;
const int rightMotorErrorPin = 0;*/

const int tiltSensePin = 13;
const int rightSensePin = 4;
const int leftSensePin = 2;
const int colorSenseLedPin = A3;
const int irSensorPin = A1;      
const int leftBeltSensePin = A0;
const int rightBeltSensePin = A3;

const int targetPin = 3;
const int switchPins[7] = {2, 3, 4, A0, A2, A3, 13};

// TODO
const int ledPinA   = 12;
const int ledPinB   = 12;
const int ledPinC   = 12;

Adafruit_PWMServoDriver servos = Adafruit_PWMServoDriver();
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_1X);

#define CLOSE 520
#define OPEN 200
#define MAX_BAG_COUNT 8
const int servoOrder[MAX_BAG_COUNT] = {1, 5, 2, 4, 3, 7, 12, 6}; // Order to close and open flaps, alternating each tower

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

boolean left, right, tilt;
void (*currentState)();

void softStarting();
void onOffStyle(); // Race mode
void doingTrackOnRightSide(); // Tower mode
void error(int code);

void setup(){
  Serial.begin(9600);
//  Wire.begin(BADGER_ADDRESS);
//  Wire.onReceive(receiveEvent);

  for(int i = 0; i < N_MOTORS; ++i){
    pinMode(pwmPins[i], OUTPUT);
    pinMode(digPins[i], OUTPUT);
    
    digitalWrite(pwmPins[i], LOW);
    digitalWrite(digPins[i], LOW);
  }
  
  for(int i = 0; i < 7; ++i) {
    pinMode(switchPins[i], INPUT);  
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

  servos.setPWM(9, 0, 4095); // Belt motor off
  currentState = doingTrackOnRightSide;

  // open all flaps
  /*for(int i = 2; i < MAX_BAG_COUNT; ++i) {
    servos.setPWM(servoOrder[i], 0, OPEN);
    delay(500);
  }*/

  servos.setPWM(servoOrder[0], 0, CLOSE);
  delay(100);
  servos.setPWM(servoOrder[1], 0, CLOSE);

#if RUN_TESTS  
  runTests();
  while(1);
#endif

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
