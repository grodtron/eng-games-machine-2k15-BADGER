// IN PROGRESS
#include <Arduino.h>
#include "motors.h"

typedef enum wheel_e {
  RIGHT_FRONT,
  RIGHT_REAR,
  LEFT_FRONT,
  LEFT_REAR
};
  
class SingleMotorTweener {
  public:
    static const int DELAY_TIME = 1;
  
    SingleMotorTweener(wheel_e wheel);
  
    void setTargetSpeed(int newTarget);    
    void update();
  private:
    int current, target;
    long lastUpdateTime;
    wheel_e _wheel;
};
