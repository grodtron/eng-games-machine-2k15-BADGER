void recover(){
  BEEPER_ON();

  MOVE_BAK(RIGHT, 100);
  MOVE_FWD(LEFT, 50);

  delay(250);
  
  BEEPER_OFF();
}

void onOffStyle(){
  
  static long sameCount = 0;
  static boolean previousWasLeft = false;
  
  if(left){
    if(previousWasLeft){
      ++sameCount;
    }else{
      previousWasLeft = true;
      sameCount = 0;
    }
    BEEPER_OFF();
    MOVE_FWD(RIGHT, 245);
    MOVE_FWD(LEFT, 255);
  }else{
    if( ! previousWasLeft){
      ++sameCount;
    }else{
      previousWasLeft = false;
      sameCount = 0;
    }

    BEEPER_ON();
    MOVE_FWD(RIGHT, 235);
    MOVE_FWD(LEFT, 22);
  }
  
  if(sameCount > 20000L){
    recover();
    sameCount = 12500L;
  }
}
