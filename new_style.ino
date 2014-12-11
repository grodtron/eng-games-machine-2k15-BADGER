void newStyle(){
 
  static int onLeftHiSpeed = 255;
  static int onLeftLoSpeed = 200;

  static int offLeftHiSpeed = 255;
  static int offLeftLoSpeed = 200;
  
  static int onLeftCount  = 0;
  static int offLeftCount = 0;
  
  static int lastOnLeftCount  = 0;
  static int lastOffLeftCount = 0;
  
  if(left){
    if (offLeftCount) lastOffLeftCount = offLeftCount;
    offLeftCount = 0;
    ++onLeftCount;
    
    // If we have been on the left side for longer than we were off
    // the left side, then increase the turning angle. (turn away faster)
    if(onLeftCount > lastOffLeftCount){
      onLeftLoSpeed = max(0, onLeftLoSpeed - (onLeftCount - lastOffLeftCount));
    }
    
    MOVE_FWD(LEFT,  onLeftHiSpeed);
    MOVE_FWD(RIGHT, onLeftLoSpeed);
  }else{
    if (onLeftCount){
      lastOnLeftCount = onLeftCount;
      // If the just completed time touching the left was shorter than the most recent
      // time off the left, then 
      if(lastOnLeftCount < lastOffLeftCount){
        
      }
    }
    onLeftCount = 0;
    ++offLeftCount;
    
    // If we have been away from the side for longer than we were last on it, then 
    // we turn towards it harder
    if(offLeftCount > lastOnLeftCount){
      offLeftLoSpeed = max(0, offLeftLoSpeed - (offLeftCount - lastOnLeftCount));
    }
    
    MOVE_FWD(LEFT,  offLeftLoSpeed);
    MOVE_FWD(RIGHT, offLeftHiSpeed);
  }
  
  delayMicroseconds(100);
}
