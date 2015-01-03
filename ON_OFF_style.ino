void recover(){
  MOVE_SIDE_BAK(LEFT, 100);
  MOVE_SIDE_FWD(RIGHT, 50);

  delay(500);  
}

SingleMotorTweener left_front(LEFT_FRONT), left_rear(LEFT_REAR), right_front(RIGHT_FRONT), right_rear(RIGHT_REAR); 

void onOffStyle(){
  
  static long sameCount = 0;
  static boolean previousWasLeft = false;
  
  if(left){
    if(previousWasLeft){
      ++sameCount;
    }else{
      previousWasLeft = true;
      sameCount = 0;
    }

    MOVE_SIDE_FWD(RIGHT, 245);
    MOVE_SIDE_FWD(LEFT, 255);
  }else{
    if( ! previousWasLeft){
      ++sameCount;
    }else{
      previousWasLeft = false;
      sameCount = 0;
    }

    //MOVE_SIDE_FWD(RIGHT, 235);
    //MOVE_SIDE_FWD(LEFT, 40);
        
    MOVE_WHEEL_FWD(RIGHT, FRONT, 235);
    MOVE_WHEEL_FWD(RIGHT, REAR, 220);
    MOVE_WHEEL_FWD(LEFT, FRONT, 30);
    MOVE_WHEEL_FWD(LEFT, REAR, 0);    
  }

  if(sameCount > 20000L){
    recover();
    sameCount = 10000L;
  }
}

void startRaceOnRightSide() {
  static long sameCount = 0;
  static boolean previousWasRight = false;
  
  if(right){
    if(previousWasRight){
      ++sameCount;
    }else{
      previousWasRight = true;
      sameCount = 0;
    }
    MOVE_SIDE_FWD(RIGHT, 245);
    MOVE_SIDE_FWD(LEFT, 65);
  }else{
    if( ! previousWasRight){
      ++sameCount;
    }else{
      previousWasRight = false;
      sameCount = 0;
    }

    MOVE_SIDE_FWD(RIGHT, 40);
    MOVE_SIDE_FWD(LEFT, 235);
  } 
  
  if( (millis() - raceStartTime) > 2500 ){
    Serial.println("going on off style");
    currentState = onOffStyle;
  }
}
