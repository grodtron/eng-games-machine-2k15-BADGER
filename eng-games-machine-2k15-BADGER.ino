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

#define BADGER_ADDRESS 0x20
#define LOADER_ADDRESS 0x21

const int N_MOTORS = 4;

/*const int eStopPin = 7;
const int leftMotorErrorPin = 1;
const int rightMotorErrorPin = 0;*/

const int tiltSensePin = 13;
const int tiltSensePin2 = A2;
const int rightSensePin = 4;
const int leftSensePin = 2;
const int topLeftSensePin = 3;
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
const int servoOrder[MAX_BAG_COUNT] = {1, 5, 2, 4, 3, 7, 12, 6}; // Order to close and open flaps, alternating each tower

int pwmPins [N_MOTORS] = {9,  10, 11, 6};
int digPins [N_MOTORS] = {12, 5, 8, 7};

const int RIGHT_FRONT_PWM  = 9;
const int RIGHT_REAR_PWM  = 11;
const int LEFT_FRONT_PWM = 10;
const int LEFT_REAR_PWM = 6;

const int RIGHT_FRONT_DIR  = 12;
const int RIGHT_REAR_DIR  = 8;
const int LEFT_FRONT_DIR = 5;
const int LEFT_REAR_DIR = 7;
                
int bag_count = 0;

#define MOVE_WHEEL_FWD(SIDE, FRONT_REAR, AMOUNT) do { \
  analogWrite (SIDE ## _ ## FRONT_REAR ## _PWM, 255 - (AMOUNT)); \
  digitalWrite(SIDE ## _ ## FRONT_REAR ## _DIR, HIGH); \
} while(0)

#define MOVE_WHEEL_BAK(SIDE, FRONT_REAR, AMOUNT) do { \
  analogWrite (SIDE ## _ ## FRONT_REAR ## _PWM, (AMOUNT)); \
  digitalWrite(SIDE ## _ ## FRONT_REAR ## _DIR, LOW); \
} while(0)


#define MOVE_SIDE_FWD(SIDE, AMOUNT) do {  \
  MOVE_WHEEL_FWD(SIDE, FRONT, AMOUNT); \
  MOVE_WHEEL_FWD(SIDE, REAR, AMOUNT);  \
} while(0)

#define MOVE_SIDE_BAK(SIDE, AMOUNT) do {          \
  MOVE_WHEEL_BAK(SIDE, FRONT, AMOUNT); \
  MOVE_WHEEL_BAK(SIDE, REAR, AMOUNT);  \
} while(0)

boolean left, topLeft, right, tilt, tilt2;
void (*currentState)();

void softStarting();
void onOffStyle(); // Race mode
void doingTrackOnRightSide(); // Tower mode
void error(int code);

#define RUN_TESTS 0

void setup(){
  Serial.begin(9600);
  Wire.begin(BADGER_ADDRESS);
  Wire.onReceive(receiveEvent);

  for(int i = 0; i < N_MOTORS; ++i){
    pinMode(pwmPins[i], OUTPUT);
    pinMode(digPins[i], OUTPUT);
    
    digitalWrite(pwmPins[i], LOW);
    digitalWrite(digPins[i], LOW);
  }
  
//  MOVE_SIDE_FWD(LEFT, 0);
//  MOVE_SIDE_FWD(RIGHT, 0);

  pinMode(leftSensePin, INPUT);
  pinMode(topLeftSensePin, INPUT);
  pinMode(rightSensePin, INPUT);
  pinMode(tiltSensePin, INPUT);
  pinMode(tiltSensePin2, INPUT);
  pinMode(leftBeltSensePin, INPUT);
  pinMode(rightBeltSensePin, INPUT);

  pinMode(irSensorPin, INPUT);

  digitalWrite(tiltSensePin, HIGH);
  digitalWrite(tiltSensePin2, HIGH);

  /*
  pinMode(eStopPin, INPUT);  
  pinMode(leftMotorErrorPin, INPUT);
  pinMode(rightMotorErrorPin, INPUT);
  pinMode(ledPinA, OUTPUT);
  pinMode(ledPinB, OUTPUT);
  pinMode(ledPinC, OUTPUT);*/

  servos.begin();
  servos.setPWMFreq(60);

  for(int i = 0; i < 10; ++i) {
    servos.setPWM(9, 0, 4095); // Belt motor off
  }
  currentState = waitingForBags;

  // open all flaps
  for(int i = 2; i < MAX_BAG_COUNT; ++i) {
    servos.setPWM(servoOrder[i], 0, OPEN);
    delay(500);
  }

  servos.setPWM(servoOrder[0], 0, CLOSE+50);
  delay(100);
  servos.setPWM(servoOrder[1], 0, CLOSE);

  if (tcs.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1);
  }

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
  topLeft = digitalRead(topLeftSensePin);
  right = digitalRead(rightSensePin);
  tilt  = !digitalRead(tiltSensePin);  
  tilt2 = !digitalRead(tiltSensePin2);

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
