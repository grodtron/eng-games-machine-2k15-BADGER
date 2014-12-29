void recover(){
  MOVE_SIDE_BAK(LEFT, 100);
  MOVE_SIDE_FWD(RIGHT, 50);

  delay(500);  
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
    MOVE_SIDE_FWD(RIGHT, 245);
    MOVE_SIDE_FWD(LEFT, 255);
  }else{
    if( ! previousWasLeft){
      ++sameCount;
    }else{
      previousWasLeft = false;
      sameCount = 0;
    }

    MOVE_SIDE_FWD(RIGHT, 235);
    MOVE_SIDE_FWD(LEFT, 40);
  }
  
  if(sameCount > 20000L){
    recover();
    sameCount = 10000L;
  }
}
