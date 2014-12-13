#include "count_true.h"

// TODO: Verify integration time
// Longer integration times can be used for increased sensitivity at low light levels.
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);
static boolean initialized = false;

void waitingForLine(){
  // TODO: More testing with colors on track
  /*  Approximate values from test 
     black: R: 605 G: 437 B: 368 C: 1278        
     orange: R: 1796 G: 909 B: 683 C: 3263
  */  
  const int red_threshold = 1100;
  const int green_threshold = 700;
  const int blue_threshold = 500;
  const int clear_threshold = 2000;
  
  if(!initialized)
  {
    if (tcs.begin()) { // Not sure if bad practice to put color sensor init here
      Serial.println("Found sensor");
    } else {
      Serial.println("No TCS34725 found ... check your connections");
      while (1);
    }
    initialized = true;    
  }
  
  digitalWrite(ledPinC, LOW);

  static int totalCount = 0;
  
  uint16_t r, g, b, c;
  tcs.getRawData(&r, &g, &b, &c);
  
  ++totalCount;
  
  if(totalCount < 150){
    BEEPER_ON(); 
  }else{
    BEEPER_OFF();
  }
   
  static int offCount = 0;
  
  // TODO: Adjust speeds to go really slow to make sure we align with orange line
  if(left){
    offCount = 0;
    MOVE_FWD(LEFT, 100);
    MOVE_FWD(RIGHT, 45);
  }else{
    ++offCount;
    MOVE_FWD(LEFT, 45);
    MOVE_FWD(RIGHT, 100);
  }
  
  if(4 == COUNT_TRUE(
    //offCount   > 50,
    r > red_threshold,
    g > green_threshold,
    b > blue_threshold,
    c > clear_threshold/*,
    totalCount > 650*/
  )){
    totalCount = 0;
    MOTORS_OFF();
//    currentState = doingSecondSmallRadiusTurn;
    return;
  }

  delay(1);
}

