#include <Arduino.h>
#include "motors.h"
  
// IN PROGRESS
class SingleMotorTweener {
  public:
    static const int DELAY_TIME = 1;
  
    SingleMotorTweener();
  
    void setTargetSpeed(int newTarget);    
    void update();
  private:
    int current, target;
    long lastUpdateTime;
};
