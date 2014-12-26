void runTests() {
  TestShootingBags();  
  TestFullShootingBags();
  TestOpenServos();
  TestCloseServos();
  TestIRSensor();
}

void TestShootingBags() {
  Serial.println("====Testing proper time shooting====");
  while(holePeriod == 0) {
    calculateHolePeriod();
  }
  while(woodPeriod == 0) {
    calculateWoodPeriod();  
  }
  
  timeBagShooting();
}

void TestFullShootingBags() {
  Serial.println("====Testing bag drop and shooting====");

  bag_count = 0;
  
  // Place all bean bags  
  for(int servoNum = 1; servoNum < 9; ++servoNum){
    Serial.println(servoNum);
    // open each servo
    servos.setPWM(servoNum, 0, OPEN);
    delay(500);
  }
  
  for(int servoNum = 0; servoNum < 8; ++servoNum){
    Serial.println(servoNum);
    // close each servo
    servos.setPWM(servoOrder[servoNum], 0, CLOSE);
    bag_count++;
    delay(2000);
  }

  while(bag_count >0) {
    shootingTarget();  
  }
}

void TestOpenServos() {
  Serial.println("====Testing opening all servos====");

  for(int servoNum = 1; servoNum < 9; ++servoNum){
    Serial.println(servoNum);
    // open each servo
    servos.setPWM(servoNum, 0, OPEN);
    delay(500);
  }  
}

void TestCloseServos() {
  Serial.println("====Testing closing all servos====");
  for(int servoNum = 1; servoNum < 9; ++servoNum){
    Serial.println(servoNum);
    // close each servo
    servos.setPWM(servoNum, 0, CLOSE);
    delay(500);
  }    
}

void TestIRSensor() {
  Serial.println("====Testing IR sensor (period calculation)====");

  calculatePeriod();
  calculateHolePeriod();
  calculateWoodPeriod();
}
