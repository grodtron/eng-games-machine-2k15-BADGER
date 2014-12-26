int holePeriod;
int woodPeriod;
unsigned long initialTime = 0;
unsigned long finishTime = 0;
const int ShootingSpeed = 1300; // Time to reach the target after turning motors on

Adafruit_PWMServoDriver servos = Adafruit_PWMServoDriver();
boolean shooting_initialized = false;

void shootingTarget() {
  if(!shooting_initialized) {
    lowerBelt();
    
    holePeriod = 0;
    woodPeriod = 0;
    while(0 == woodPeriod) {
      calculateWoodPeriod();
    }
    while(0 == holePeriod) {
      calculateHolePeriod();
    }
    Serial.println("Done measuring target periods");     
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
  }   
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 1 when hole, 0 when wood
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
    while(digitalRead(irSensorPin));
    finishTime = millis();
    if (finishTime-initialTime>50) {
      woodPeriod = finishTime-initialTime;
    }
  } else { // we started at hole, wait for finish, measure wood period first, then hole
    while(digitalRead(irSensorPin));
    
    initialTime = millis();
    while(digitalRead(irSensorPin));
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
  Serial.println(holePeriod);

  Serial.print("wood period: ");
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
  for(int i = 0; i < 130; i += 10){
    servos.setPWM(0, 0, map( i, 0, 1000, 0, 4096 ));
    delay(100);
  }
  servos.setPWM(0, 0, map( 0, 0, 1000, 0, 4096 ));
  Serial.println("done");
}

// Waiting for optimal time to shoot
void timeBagShooting() {
  Serial.print("Waiting for best time to shoot bag...");
  long time_to_wait = holePeriod + woodPeriod;
  while (time_to_wait < ShootingSpeed) {
    time_to_wait = time_to_wait + holePeriod + woodPeriod;
  }
  time_to_wait -= (holePeriod/2 + ShootingSpeed);
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
  delay(1000);
  servos.setPWM(9, 0, 4095);  
  Serial.println("done");
}
