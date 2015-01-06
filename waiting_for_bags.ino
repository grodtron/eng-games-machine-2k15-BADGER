
volatile boolean received = false; // flag set when loader arduino has sent a msg
volatile int tower_to_close = 0;

#define BATTERY_THRESHOLD 10.8

void waitingForBags() {

  if(!batteryCharged){
    batteryCharged = isBatteryReady();
  }

  if(received) {
    delay(250);
    closeNextFlap();
    received = false;
    bag_count++;
    Serial.print("Total bags: ");
    Serial.println(bag_count);
  }
  // start
  if (start && batteryCharged) {
     currentState = doingTrackOnRightSide;
     bag_count = MAX_BAG_COUNT;
     total_bags = bag_count;
      for(int i = 2; i < MAX_BAG_COUNT; ++i){ // Close servos before we gtfo
        servos.setPWM(servoOrder[i], 0, CLOSE);
        delay(500);
     }
     Serial.println("Got all bags dawg");
     MOVE_SIDE_FWD(LEFT, 120);
     MOVE_SIDE_FWD(RIGHT, 120);
     delay(300);
     MOVE_SIDE_FWD(LEFT, 0);
     MOVE_SIDE_FWD(RIGHT, 0);

  /*}else{
    Serial.println("no:");
    Serial.print("   st?: "); Serial.println(start);
    Serial.print("   bat?: "); Serial.println(batteryCharged);    */
  }
}

///////////////////////////////////////////////////////////////////////////////////////////

boolean isBatteryReady() {
  
  float busvoltage = 0;

  busvoltage = ina219.getBusVoltage_V();
  Serial.print("Bus Voltage:   "); Serial.print(busvoltage); Serial.println(" V");
  return (busvoltage > BATTERY_THRESHOLD);

}

// Opens a flap in order
void openNextFlap()
{
  
  if(bag_count < 0) {
    Serial.println("wtf got negative bags");
    bag_count = MAX_BAG_COUNT;
  }
  
  if(servoOrder[total_bags - bag_count] == servoOrder[0] || servoOrder[total_bags - bag_count] == servoOrder[1]) return;
  
  Serial.print("Opening servo ");
  Serial.println(servoOrder[total_bags - bag_count]);
  servos.setPWM(servoOrder[total_bags - bag_count], 0, OPEN);
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
  if(bag_count < 6) {
    Serial.println(servoOrder[bag_count+2]);
    servos.setPWM(servoOrder[bag_count+2], 0, CLOSE);    
  }
  
  delay(500);
}

// function that executes when data is received from loader
void receiveEvent(int bytes)
{
  received = true;
  while(Wire.available()) {
    tower_to_close = Wire.read();
  }  
}

void requestEvent() {
  while(Wire.available()) {
    Wire.read(); 
  }

  Wire.write(tower_to_close); 
}
