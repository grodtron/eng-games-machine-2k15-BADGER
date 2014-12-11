

void leaveStartingZone(){
  
  if(left){
    MOVE_FWD(RIGHT, 190);
    MOVE_FWD(LEFT, 200);
  }else{
    MOVE_FWD(RIGHT, 200);
    MOVE_FWD(LEFT, 40);
  }
}
