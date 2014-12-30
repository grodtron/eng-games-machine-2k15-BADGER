volatile boolean received = false; // flag set when loader arduino has sent a msg
int tower_to_close = -1;

void waitingForBags() {
  if(start && !started && isBatteryReady() == true) { // check right switch?
    started = true;
    total_bags = bag_count;
    for(int i = 2; i < MAX_BAG_COUNT; ++i){ // Close servos before we gtfo
      servos.setPWM(servoOrder[i], 0, CLOSE);
      delay(500);
    }    
    currentState = doingTrackOnRightSide;  
  }
  
  if(received) {
    delay(250);
    closeNextFlap();
    received = false;
    bag_count++;
    Serial.print("Total bags: ");
    Serial.println(bag_count);
  }
    
  if (bag_count == 8) {
     currentState = doingTrackOnRightSide;
     Serial.println("Got all bags dawg");
  }
}

///////////////////////////////////////////////////////////////////////////////////////////

boolean isBatteryReady() {
  float shuntvoltage, busvoltage, loadvoltage = 0;

  shuntvoltage = ina219.getShuntVoltage_mV();
  busvoltage = ina219.getBusVoltage_V();
  loadvoltage = busvoltage + (shuntvoltage / 1000);
  Serial.print("Bus Voltage:   "); Serial.print(busvoltage); Serial.println(" V");
  Serial.print("Shunt Voltage: "); Serial.print(shuntvoltage); Serial.println(" mV");
  Serial.print("Load Voltage:  "); Serial.print(loadvoltage); Serial.println(" V");
  return (loadvoltage > BATTERY_THRESHOLD);
}

// Opens a flap in order
void openNextFlap()
{
  if(bag_count < 0) {
    Serial.println("wtf got negative bags");
    bag_count = MAX_BAG_COUNT;
  }
  
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
  while(Wire.available())
  {
    tower_to_close = Wire.read();
  }  
}
