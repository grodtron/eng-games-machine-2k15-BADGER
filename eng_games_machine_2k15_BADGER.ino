const int rightSensePin = A0;
const int leftSensePin = A1;
const int eStopPin = 7;

const int leftMotorErrorPin = 1;
const int rightMotorErrorPin = 0;

const int tiltSensePin = 13;

const int ledPinA   = A3;
const int ledPinB   = A4;
const int ledPinC   = A5;

const int LEFT_FWD = 5;
const int LEFT_BAK = 3;

const int RIGHT_FWD = 6;
const int RIGHT_BAK = 4;

#define MOVE_FWD(SIDE, amount) do {  \
  analogWrite(SIDE ## _FWD, amount); \
  digitalWrite(SIDE ## _BAK, LOW);   \
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

void setup(){
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

  MOVE_FWD(LEFT, 0);
  MOVE_FWD(RIGHT, 0);
  
  currentState = softStarting;
}

void loop(){
  left  = digitalRead(leftSensePin);
  right = digitalRead(rightSensePin);
  tilt = digitalRead(tiltSensePin);
  
  digitalWrite(ledPinA, tilt);
  digitalWrite(ledPinA, tilt);
  
  if(! digitalRead(leftMotorErrorPin)){
     error(6); 
  }
  if(! digitalRead(rightMotorErrorPin)){
     error(7); 
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
    currentState = waitingForLargeRadiusTurn;
  }
}

void error(int code){
  MOVE_FWD(RIGHT, 0);
  MOVE_FWD(LEFT,  0);

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
