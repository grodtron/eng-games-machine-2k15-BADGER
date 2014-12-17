
void waitingForFirstTurn(){
  if(left){
    MOVE_FWD(RIGHT, 190);
    MOVE_FWD(LEFT, 200);
  }else{
    currentState = doingFirstTurn;
  }
}

void doingFirstTurn(){
  if( ! left ){
    MOVE_FWD(RIGHT, 200);
    MOVE_FWD(LEFT, 22);
  }else{
    currentState = waitingForInitialGap;
  }
}

void waitingForInitialGap(){
  if(left){
    MOVE_FWD(RIGHT, 190);
    MOVE_FWD(LEFT, 200);    
  }else{
    currentState = doingInitialGap;
  }
}

void doingInitialGap(){
  if( ! left){
    // move ever so slightly veering left
    MOVE_FWD(RIGHT, 205);
    MOVE_FWD(LEFT, 200);    
  }else{
    currentState = onOffStyle;
  }
}
