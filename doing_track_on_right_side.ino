class MotorTweener {
  public:
    static const int DELAY_TIME = 1;
  
    MotorTweener() : 
    left(0), leftTarget(0),
    right(0), rightTarget(0),
    lastUpdateTime(millis() - DELAY_TIME)
    {}
  
    void setTargetSpeed(int newLeftTarget, int newRightTarget){
      if(leftTarget != newLeftTarget && rightTarget != newRightTarget){
        leftTarget  = newLeftTarget;
        rightTarget = newRightTarget;
        Serial.println("New Target: ");
        Serial.print("    Left : "); Serial.println(leftTarget);
        Serial.print("    Right: "); Serial.println(rightTarget);
      }
    }
    
    void update(){
      long now = millis();
      if(now - DELAY_TIME > lastUpdateTime){
        lastUpdateTime = now;
        
        int leftDelta = leftTarget - left;
        if(leftDelta){
          left += (leftDelta/2);
          Serial.print("Update left : "); Serial.println(left);
        }
        
        int rightDelta = rightTarget - right;
        if(rightDelta){
          right += (rightDelta/2);
          Serial.print("Update right: "); Serial.println(right);
        }
        
        if(left > 0){
          MOVE_FWD(LEFT, left);
        }else{
          MOVE_BAK(LEFT, abs(left));
        }
        if(right > 0){
          MOVE_FWD(RIGHT, right);
        }else{
          MOVE_BAK(RIGHT, abs(right));
        }
      }
    }
  private:
    int left,  leftTarget;
    int right, rightTarget;
    long lastUpdateTime;
  
};

MotorTweener motors;


void doingTrackOnRightSide(){
   
  static boolean tilting = false;
  static int tiltStartTime = 0;
  
  if(right){
    motors.setTargetSpeed(20, 120);
  }else{
    motors.setTargetSpeed(60, 40);
  }

  motors.update();
  
  if(tilt){
    if(! tilting){
      tiltStartTime = millis();
      tilting = true;
    }else{
      if( millis() - tiltStartTime > 350 ){
        currentState = goingDownRamp;
      }    
    }
  }else{
    tilting = false;
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

void goingDownRamp(){
  static boolean nonTilting = false;
  static int nonTiltStartTime = 0;
  
  if(! tilt ){
    if(! nonTilting){
      nonTilting = true;
      nonTiltStartTime = millis();
    }else{
      if(millis() - nonTiltStartTime > 450){
        currentState = waitingForFinalOrangeLine;
      }
    }
  }else{
    nonTilting = false;
    if(right){
      motors.setTargetSpeed(0, 30);
    }else{
      motors.setTargetSpeed(30, 25);
    }   
  }
  motors.update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_1X);

void waitingForFinalOrangeLine(){
  
  static boolean haveInitialized = false;
  if(!haveInitialized){
    haveInitialized = true;
    
    if(tcs.begin()){
      digitalWrite(colorSenseLedPin, HIGH);
    }else{
      digitalWrite(colorSenseLedPin, LOW);
    }
    
  }
  
  if(left){
    motors.setTargetSpeed(45, 40);
  }else{
    motors.setTargetSpeed(-60, 80);
  }

  motors.update();

  uint16_t r, g, b, c;
  tcs.getRawData(&r, &g, &b, &c);
  
  if( r > 8 ){
    MOVE_FWD(RIGHT, 0);
    MOVE_FWD(LEFT,  0);

    tcs.setIntegrationTime(TCS34725_INTEGRATIONTIME_24MS);

    // there is one wasted read to re-acquire the integration time
    // (stupid library!)
    tcs.getRawData(&r, &g, &b, &c);
    tcs.getRawData(&r, &g, &b, &c);
    
    if(r > 90 && g > 35  ){
      while(true){
        digitalWrite(colorSenseLedPin, LOW);
        delay(100);
        digitalWrite(colorSenseLedPin, HIGH);
        delay(100);
      }      
    }
  }
}