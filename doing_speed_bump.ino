void doingSpeedBump(){
  
  digitalWrite(ledPinC, LOW);

  static int count = 0;

  static int tiltCount = 0;

  if(tilt){
    ++tiltCount;
  }else{
    if(tiltCount > 0){
      --tiltCount;
    } 
  }
  
  if(tiltCount > 290){
    tiltCount = 0;
    currentState = doingFirstSmallRadiusTurn;
    return;
  }
  
  if(left){
    MOVE_FWD(LEFT, 255);
    MOVE_FWD(RIGHT, 200);
  }else{
    MOVE_FWD(LEFT, 255);
    MOVE_FWD(RIGHT, 200);
  }
  delay(1);
}

