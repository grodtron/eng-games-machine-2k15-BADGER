// IN PROGRESS
#include "SingleMotorTweener.h"

SingleMotorTweener::SingleMotorTweener(wheel_e wheel) : current(0), target(0), lastUpdateTime(millis() - DELAY_TIME), _wheel(wheel)
{}

void SingleMotorTweener::setTargetSpeed(int newTarget){
  if(target != newTarget){
    target  = newTarget;
    Serial.print("New Target: ");Serial.println(newTarget);
  }
}

// TODO: make this less retarded 
void SingleMotorTweener::update(){
  long now = millis();
  if(now - DELAY_TIME > lastUpdateTime){
    lastUpdateTime = now;
    
    int delta = target - current;
    if(delta){
      current += (delta/2);
      Serial.print("Update current : "); Serial.println(current);
    }

    switch(_wheel) {
      case RIGHT_FRONT:
        if(current > 0){
          MOVE_WHEEL_FWD(RIGHT, FRONT, target);
        }else{
          MOVE_WHEEL_BAK(RIGHT, FRONT, target);
        }
        break;
      case RIGHT_REAR:
        if(current > 0){
          MOVE_WHEEL_FWD(RIGHT, REAR, target);
        }else{
          MOVE_WHEEL_BAK(RIGHT, REAR, target);
        }
        break;        
      case LEFT_FRONT:
        if(current > 0){
          MOVE_WHEEL_FWD(LEFT, FRONT, target);
        }else{
          MOVE_WHEEL_BAK(LEFT, FRONT, target);
        }
        break;      
      case LEFT_REAR:
        if(current > 0){
          MOVE_WHEEL_FWD(LEFT, REAR, target);
        }else{
          MOVE_WHEEL_BAK(LEFT, REAR, target);
        }
        break;
    }
  }
}
