int holePeriod;
int woodPeriod;
unsigned long initialTime = 0;
unsigned long finishTime = 0;
const int ShootingSpeed = 1325; // Time to reach the target after turning motors on

void shootingTarget() {
  Serial.print("Bean bags left: ");
  Serial.println(bag_count);
  holePeriod = 0;
  woodPeriod = 0;
  while(holePeriod == 0 || woodPeriod == 0) {
    calculatePeriod();
  }
  static int bottomServo; // Bottom servo that will release the bean bang

  // Alternate bottom servo from which bean bags will be dropped
  bottomServo = servoOrder[bag_count % 2];
  
  Serial.print("Toggling bottom servo: ");
  Serial.println(bottomServo);
  servos.setPWM(bottomServo, 0, CLOSE);
  
  delay(1000);   
  openNextFlap();
  delay(1250);
  
  servos.setPWM(bottomServo, 0, OPEN);
  timeBagShooting();

  bag_count--;

  if(bag_count == 0) {
    // current_state =
    while(1); // we done son
  }   
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* 1 when hole, 0 when wood
	typical values: wood period ~ 890, hole period ~850. 
	measurement time: BCET ~2.4s, WCET ~4s
*/
void calculatePeriod() {
  if (!digitalRead(irSensorPin)) { // if we start at wood, wait for it to finish, measure hole period first, then wood period
    while(!digitalRead(irSensorPin));
    
    initialTime = millis();
    while(digitalRead(irSensorPin));
    finishTime = millis();
    if (finishTime-initialTime>50) {
      holePeriod = finishTime-initialTime;
    }
    
    initialTime = millis();
    while(!digitalRead(irSensorPin));
    finishTime = millis();
    if (finishTime-initialTime>50) {
      woodPeriod = finishTime-initialTime;
    }
  } else { // we started at hole, wait for finish, measure wood period first, then hole
    while(digitalRead(irSensorPin));
    
    initialTime = millis();
    while(!digitalRead(irSensorPin));
    finishTime = millis();
    if (finishTime-initialTime>50) {
      woodPeriod = finishTime-initialTime;
    }
    
    initialTime = millis();
    while(digitalRead(irSensorPin));
    finishTime = millis();
    if (finishTime-initialTime>50) {
      holePeriod = finishTime-initialTime;
    }
  }
  Serial.print("hole period: ");
  Serial.print(holePeriod);

  Serial.print(", wood period: ");
  Serial.println(woodPeriod); 
}

void calculateWoodPeriod() {
  if (!digitalRead(irSensorPin)) { // detecting object
    while(!digitalRead(irSensorPin));
    while(digitalRead(irSensorPin));
  } else { // if it's one of the Halls
    while(digitalRead(irSensorPin));
    while(!digitalRead(irSensorPin));
    while(digitalRead(irSensorPin));
  }
  initialTime = millis();
  while(!digitalRead(irSensorPin));
  finishTime = millis();
  if (finishTime-initialTime>50) {
    woodPeriod = finishTime-initialTime;
    Serial.print("wood period: ");
    Serial.println(woodPeriod);
  }
}

void calculateHolePeriod() {
  if (!digitalRead(irSensorPin)) { // detecting object
    while(!digitalRead(irSensorPin));
    while(digitalRead(irSensorPin));
    while(!digitalRead(irSensorPin));
  } else { // if it's one of the Halls
    while(digitalRead(irSensorPin));
    while(!digitalRead(irSensorPin));
  }
  initialTime = millis();
  while(digitalRead(irSensorPin));
  finishTime = millis();
  if (finishTime-initialTime>50) {
    holePeriod=finishTime-initialTime;
    Serial.print("hole period: ");
    Serial.println(holePeriod);
  }
}

// Signal to start spinning conveyor belt
void shootBag() {     // for ~1300 msecs
  Serial.print("Starting to shoot bags...");
  for(int i = 0; i < 140; i += 11){
    servos.setPWM(8, 0, map( i, 0, 1667, 0, 4096 ));
    delay(100);
  }
  servos.setPWM(8, 0, map( 0, 0, 1000, 0, 4096 ));
  Serial.println("done");

}

// Waiting for optimal time to shoot
void timeBagShooting() {
  Serial.print("Waiting for best time to shoot bag...");
  unsigned long time_to_wait = holePeriod + woodPeriod;
  while (time_to_wait < 2*ShootingSpeed) {
    time_to_wait = time_to_wait + holePeriod + woodPeriod;
  }
  time_to_wait -= (holePeriod/2 + ShootingSpeed);
  Serial.print("waiting time before shoting: ");
  Serial.println(time_to_wait);
  if (!digitalRead(irSensorPin)) { // detecting object
    while(!digitalRead(irSensorPin));
    while(digitalRead(irSensorPin));
  } else { // if it's one of the Halls
    while(digitalRead(irSensorPin));
    while(!digitalRead(irSensorPin));
    while(digitalRead(irSensorPin));
  }
  long start_Time = millis();
  while( (millis() - start_Time) < time_to_wait);
  shootBag();
}

void lowerBelt() {
  Serial.print("Lowering belt...");
  // lower the belt a bit
  servos.setPWM(9, 0, 0);
  while(!digitalRead(leftBeltSensePin) && !digitalRead(rightBeltSensePin));
  delay(50);
  servos.setPWM(9, 0, 4095);
  
  if(digitalRead(rightBeltSensePin) != 1) {
    MOVE_FWD(RIGHT, 40);
    MOVE_FWD(LEFT,  40);
    Serial.println("right belt switch, going forward");
  } else if(digitalRead(leftBeltSensePin) != 1) {
      MOVE_BAK(RIGHT, 40);
      MOVE_BAK(LEFT, 40);
    Serial.println("left belt switch, going backward");
  }

  while(digitalRead(leftBeltSensePin) != 1 || digitalRead(rightBeltSensePin) != 1);
  MOVE_FWD(RIGHT, 0);
  MOVE_FWD(LEFT,  0);
  Serial.println("done");
}
