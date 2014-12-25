#include "servo_control.h"

volatile boolean received = false;

void waitingForBags() {
  if(received) {
    delay(250);
    closeNextFlap();
    received = false;
  }
  
  if (bag_count == 8) {
     //current_state =  
  }
}

///////////////////////////////////////////////////////////////////////////////////////////

// Opens a flap in order
void openNextFlap()
{
  if(bag_count < 0) {
    Serial.println("wtf got negative bags");
    bag_count = MAX_BAG_COUNT;
  }
  
  Serial.print("Opening servo ");
  Serial.println(servoOrder[MAX_BAG_COUNT - bag_count]);
  servos.setPWM(servoOrder[MAX_BAG_COUNT - bag_count--], 0, OPEN);
  delay(500);
}

// Cycles through all flaps to close in order, closes 1 flap at a time
void closeNextFlap()
{
  if(bag_count >= MAX_BAG_COUNT) {
     Serial.println("wtf got more than 8 bags"); 
     return;
  }
  
  Serial.print("Closing servo ");
  Serial.println(servoOrder[bag_count]);
  servos.setPWM(servoOrder[bag_count++], 0, 1100);
  
  // Open back flaps for testing
  if(bag_count == 5) {
    bag_count = 1;
    for(int i = 1; i <= MAX_BAG_COUNT; ++i) {
      servos.setPWM(servoOrder[i], 0, 400);
      delay(500);
    }    
  }
  delay(500);
}

// function that executes when data is received from loader
void receiveEvent(int bytes)
{
  int towerToClose;
  received = true;
  while(Wire.available())
  {
    towerToClose = Wire.read();
  }  
}
