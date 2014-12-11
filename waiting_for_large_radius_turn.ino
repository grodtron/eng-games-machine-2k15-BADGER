#include "count_true.h"

void waitingForLargeRadiusTurn(){
  
  digitalWrite(ledPinC, LOW);
  
  static boolean haveTouched = false;
  static int awayCount  = 0;
  static int totalCount = 0;
  
  ++totalCount;
  
  if(totalCount < 150){
    BEEPER_ON(); 
  }else{
    BEEPER_OFF();
  }
  
  if(left){
    awayCount = 0;
    haveTouched = true;
    MOVE_FWD(RIGHT, 200);
    MOVE_FWD(LEFT, 255);
  }else{
    ++awayCount;
    MOVE_FWD(RIGHT, 255);
    MOVE_FWD(LEFT, 200);
  }
  
  // we will determine when we're done heuristically.
  //
  // If any two conditions are true, then we're probly at the end of the straightaway
  if(1 < COUNT_TRUE(
    (awayCount > 50 && haveTouched), // We have touched the side once and it's been 50ms since we touched it
    totalCount > 850,                // we've been running for at least a second
    right                             // we have touched the right wall
  )){
    currentState = doingLargeRadiusTurn;
    // reset so that the next time we do this function it will be correct
    haveTouched = false;
    awayCount  = 0;
    totalCount = 0;
    return;
  }
  
  delay(1);
}
