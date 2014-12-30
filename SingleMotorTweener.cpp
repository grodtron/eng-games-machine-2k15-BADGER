// IN PROGRESS
#include "SingleMotorTweener.h"

SingleMotorTweener::SingleMotorTweener() : current(0), target(0), lastUpdateTime(millis() - DELAY_TIME)
{}

void SingleMotorTweener::setTargetSpeed(int newTarget){
  if(target != newTarget){
    target  = newTarget;
    Serial.print("New Target: ");Serial.println(newTarget);
  }
}

// TODO: think about how to use macros inside class in a clean way
void SingleMotorTweener::update(){
  long now = millis();
  if(now - DELAY_TIME > lastUpdateTime){
    lastUpdateTime = now;
    
    int delta = target - current;
    if(delta){
      current += (delta/2);
      Serial.print("Update current : "); Serial.println(current);
    }
    
    if(target > 0){
//      MOVE_WHEEL_FWD(SIDE, FRONT_REAR, target);
    }else{
//      MOVE_WHEEL_BAK(SIDE, FRONT_REAR, target);
    }
  }
}
