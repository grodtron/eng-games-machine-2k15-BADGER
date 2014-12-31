// IN PROGRESS
<<<<<<< HEAD

=======
/*
>>>>>>> oli-in-progress
class MotorTweener {
  public:
    static const int DELAY_TIME = 1;
  
    MotorTweener() : 
    current(0), target(0),
    lastUpdateTime(millis() - DELAY_TIME)
    {}
  
    void setTargetSpeed(int newTarget, int newRightTarget){
      if(target != newTarget){
        target  = newLeftTarget;
        Serial.print("New Target: ");Serial.println(leftTarget);
      }
    }
    
    void update(){
      long now = millis();
      if(now - DELAY_TIME > lastUpdateTime){
        lastUpdateTime = now;
        
        int leftDelta = leftTarget - left;
        if(leftDelta){
          left += (leftDelta/2);
          Serial.print("Update left : "); Serial.println(left);
        }
        
        int rightDelta = rightTarget - right;
        if(rightDelta){
          right += (rightDelta/2);
          Serial.print("Update right: "); Serial.println(right);
        }
        
        if(left > 0){
          MOVE_SIDE_FWD(LEFT, left);
        }else{
          MOVE_SIDE_BAK(LEFT, abs(left));
        }
        if(right > 0){
          MOVE_SIDE_FWD(RIGHT, right);
        }else{
          MOVE_SIDE_BAK(RIGHT, abs(right));
        }
      }
    }
  private:
    int left,  leftTarget;
    int right, rightTarget;
    long lastUpdateTime;
  
<<<<<<< HEAD
};
=======
};*/
>>>>>>> oli-in-progress
