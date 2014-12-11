void doingFirstSmallRadiusTurn(){  
  digitalWrite(ledPinC, HIGH);
  
  static int onCount = 0;
  
  static int totalCount = 0;
  ++totalCount;
  
  if(totalCount < 150){
    BEEPER_ON(); 
  }else{
    BEEPER_OFF();
  }
  
  // TODO - heuristics to end it?
  if(left){
    ++onCount;
    if(onCount > 120){
      totalCount = 0;
      error(3);
    }
    
    // if we hit the inside wall, go straight(ish)
    MOVE_FWD(RIGHT, 255);
    MOVE_FWD(LEFT, 150);
  } else {
    onCount = 0;
    MOVE_FWD(RIGHT, 255);
    MOVE_FWD(LEFT, 10);
  }
  delay(1);
}

