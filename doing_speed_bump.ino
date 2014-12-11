#include "count_true.h"

void doingSpeedBump(){
  
  digitalWrite(ledPinC, LOW);

  static int totalCount = 0;
  ++totalCount;
  
  if(totalCount < 150){
    BEEPER_ON(); 
  }else{
    BEEPER_OFF();
  }


  static int tiltCount = 0;

  if(tilt){
    ++tiltCount;
  }else{
    if(tiltCount > 0){
      --tiltCount;
    } 
  }
    
  static int offCount = 0;
  
  if(left){
    offCount = 0;
    MOVE_FWD(LEFT, 255);
    MOVE_FWD(RIGHT, 200);
  }else{
    ++offCount;
    MOVE_FWD(LEFT, 200);
    MOVE_FWD(RIGHT, 255);
  }
  
  if(2 == COUNT_TRUE(
    offCount   > 50,
    tiltCount  > 200/*,
    totalCount > 650*/
  )){
    tiltCount = 0;
    totalCount = 0;
    currentState = doingFirstSmallRadiusTurn;
    return;
  }

  delay(1);
}

