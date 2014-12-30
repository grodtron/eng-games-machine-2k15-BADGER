void doingTrackOnRightSide(){
   
  static boolean tilting = false;
  static int tiltStartTime = 0;
  
  if(right){
    motors.setTargetSpeed(20, 120);
  }else{
    motors.setTargetSpeed(70, 40);
  }

  motors.update();
  
  if(tilt && tilt2){ 
    if(! tilting){
      tiltStartTime = millis();
      tilting = true;
    }else{
      if( millis() - tiltStartTime > 250 ){
        currentState = goingDownRamp;
      }    
    }
  }else{
    tilting = false;
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

void stopEverything(){
  motors.setTargetSpeed(0, 0);
  motors.update();
}

void goingDownRamp(){
  static boolean nonTilting = false;
  static int nonTiltStartTime = 0;
  
  if( (!tilt) && (!tilt2) ){
    if(! nonTilting){
      nonTilting = true;
      nonTiltStartTime = millis();
    }else{
      if(millis() - nonTiltStartTime > 300){
        Serial.println("Waiting for orange line");
        currentState = waitingForFinalOrangeLine
        ;
      }
    }
  }else{
    nonTilting = false;
    if(right){
      motors.setTargetSpeed(25 , 30);
    }else{
      motors.setTargetSpeed(30, 25);
    }   
  }
  motors.update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////


void waitingForFinalOrangeLine(){
  
  static boolean haveInitialized = false;
  if(!haveInitialized){
    haveInitialized = true;
    
    // turn on or off the LED to show the status of the color sensor
    if(tcs.begin()){
      servos.setPWM(15, 0, 4095 );
    }else{
      servos.setPWM(15, 0, 0);
    }
    
  }
  
  if(left){
    motors.setTargetSpeed(45, 40);
  }else{
    
    /*
    
    FR: 100
    FL: 75
    RR: 83
    RL: 42
    
    */
    
    motors.setTargetSpeed(-60, 80);
  }

  motors.update();

  uint16_t r, g, b, c;
  tcs.getRawData(&r, &g, &b, &c);
  
  if( r > 8 ){
    MOVE_SIDE_FWD(RIGHT, 0);
    MOVE_SIDE_FWD(LEFT,  0);

    tcs.setIntegrationTime(TCS34725_INTEGRATIONTIME_24MS);

    // there is one wasted read to re-acquire the integration time
    // (stupid library!)
    tcs.getRawData(&r, &g, &b, &c);
    tcs.getRawData(&r, &g, &b, &c);
    
    lowerBelt();
    currentState = shootingTarget;
    if(r > 90 && g > 35  ){ // remove this to speed things up?
      // Blink the LED
      for(int i = 0 ; i < 4; ++i){
        servos.setPWM(15, 0, 0 );
        delay(500);
         delay(500);
      }
    }
  }
}
