#include "count_true.h"

void doingLargeRadiusTurn(){
  
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
    // if we hit the inside wall, go straight
    MOVE_FWD(RIGHT, 255);
    MOVE_FWD(LEFT, 255);
  } else {
    onCount = 0;

    if (right){
      // If we hit the outside wall, turn super hard away
      MOVE_FWD(RIGHT, 255);
      MOVE_FWD(LEFT,  0);
    } else {
      // turning ratio is ~ 5:3
      // but also I think it's not exactly linear...
      // definitely not quite linear...
      MOVE_FWD(RIGHT, 255);
      MOVE_FWD(LEFT, 78);
    }
  }
  
  
  // TODO - another tilt sensor?
  if(2 == COUNT_TRUE(
       onCount > 100,
       totalCount > 850
  )){
      onCount = 0;
      totalCount = 0;
      currentState = doingSpeedBump;
      return;
    }

  
  delay(1);
}

