void doingFirstSmallRadiusTurn(){  
  digitalWrite(ledPinC, HIGH);
  
  static int onCount = 0;
  
  // TODO - heuristics to end it?
  if(left){
    ++onCount;
    if(onCount > 120){
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

