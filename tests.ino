const int switchPins[7] = {2, 3, 4, A0, A2, A3, 13};
/* Switch pins
    4  = S1
    3  = S2 top left
    2  = S3 
    A2 = S4
    A3 = S5
    A0 = S6
    13 = S7 
*/

/* Flaps servos to Pins
front  -   back tower
    X - 6
    3 - 7
    2 - 4
    1 - 5
*/

void runTests() {
  TestOpenServos();
  TestCloseServos();
//  TestBagPlacing();
//  TestFullShootingBags();
/*TestIRSensor();*/
// for (int i=0; i < 30; ++i)
//     TestShootingBags();
//  TestTargetMovementCalculations();
//  TestIRSensor();
//  TestShootingBags();
//  TestBeltLowering();
//  TestMeasureShootingTime();
//  while(1) TestSwitchPins();
//while(1)  TestSlowServoOpen();
//  TestI2CServo();
//  TestBagDrops();
//TestCollectBagAndShoot();
}

void TestCollectBagAndShoot() {
bag_count = 0;
  while(bag_count < 8) {
    waitingForBags();
  }
  delay(1000);
  while(bag_count > 0) {
     shootingTarget(); 
  }
}

void TestMeasureShootingTime() {

  Serial.println("====Testing measuring shooting time====");
  shootBag();
  while(!Serial.available()) ; Serial.read();
/*  unsigned long startTime = millis();
  shootBag();
  while(digitalRead(targetPin));
  unsigned long endTime = millis();
  unsigned long shootingTime = endTime - startTime;
  Serial.print("Shooting time: ");
  Serial.println(shootingTime);
  while(!Serial.available()) ; Serial.read();  */
}

void TestTargetMovementCalculations() {
  int num_iterations = 10;
  Serial.println("====Testing target time movement calculations====");
  long initial_time, final_time;
  for (int i = 0; i < num_iterations; ++i) {
      Serial.println(i);
      initial_time = millis();
      calculatePeriod();
      final_time = millis();
      Serial.print("calculate period execution time: ");
      Serial.println(final_time - initial_time);
  }
}

void TestShootingBags() {
  Serial.println("====Testing proper time shooting====");
  while (holePeriod < 800 || holePeriod > 900 
          || woodPeriod < 800 || woodPeriod > 900) {
      calculatePeriod();
  }
  timeBagShooting();
  while(!Serial.available()) ; Serial.read();
}

void TestBagPlacing() {
  Serial.println("====Testing proper time shooting====");
  bag_count = 0;
  // Place all bean bags  
  for(int servoNum = 2; servoNum < 8; ++servoNum){
    Serial.println(servoNum);
    // open each servo
    servos.setPWM(servoOrder[servoNum], 0, OPEN);
    delay(500);
  }
  
  for(int servoNum = 2; servoNum < 8; ++servoNum){
    while(!Serial.available()) ; Serial.read(); 
    Serial.println(servoNum);
    // close each servo
    servos.setPWM(servoOrder[servoNum], 0, CLOSE);
    bag_count++;
    delay(500);
  }  
}

void TestFullShootingBags() {
  Serial.println("====Testing bag drop and shooting====");

  bag_count = 8;
  
  while(bag_count >0) {
    shootingTarget();  
  }
}

void TestOpenServos() {
  Serial.println("====Testing opening all servos====");

  for(int servoNum = 0; servoNum < 8; ++servoNum){
    while(!Serial.available()) ; Serial.read(); 
    Serial.println(servoNum);
    // open each servo
    servos.setPWM(servoNum, 0, OPEN);
    delay(500);
  }  
}

void TestCloseServos() {
  Serial.println("====Testing closing all servos====");
  for(int servoNum = 0; servoNum < 8; ++servoNum){
    while(!Serial.available()) ; Serial.read(); 
    Serial.println(servoNum);
    // close each servo
    servos.setPWM(servoNum, 0, CLOSE);
    delay(500);
  }    
}

void TestIRSensor() {
  Serial.println("====Testing IR sensor (period calculation)====");

  while(holePeriod == 0 || woodPeriod == 0) {
    calculatePeriod();
  }

  while(holePeriod == 0 || woodPeriod == 0) {
    calculatePeriod();    
  }
}

void TestMotor() {
  static int i = 0;
  
  Serial.println(i);
  analogWrite(pwmPins[i], 128);
  Serial.println("  fwd");
  
  while(!Serial.available()) ; Serial.read();
  
  digitalWrite(digPins[i], HIGH);
  Serial.println("  rev");
  
  while(!Serial.available()) ; Serial.read();
 
  digitalWrite(digPins[i], LOW);
  digitalWrite(pwmPins[i], LOW);
 
  while(!Serial.available()) ; Serial.read();
  
  ++i;
  i = i % 4;  
}

void TestBeltLowering() {
  lowerBelt();  
}

void TestSwitchPins() {
  /*for(int i = 0; i < 7; ++i) {
    Serial.print(switchPins[i]);
    Serial.print("=");
    Serial.println(digitalRead(switchPins[i])); 
  }*/
  Serial.print("left(");
  Serial.print(leftSensePin);  
  Serial.print(")=");
  Serial.println(digitalRead(leftSensePin));
  Serial.print("startButton(");
  Serial.print(startButton);  
  Serial.print(")=");
  Serial.println(digitalRead(startButton));
  Serial.print("right(");
  Serial.print(rightSensePin);
  Serial.print(")=");
  Serial.println(digitalRead(rightSensePin));
  Serial.print("tilt_1(");
  Serial.print(tiltSensePin);  
  Serial.print(")=");
  Serial.println(digitalRead(tiltSensePin));
  Serial.print("tilt_2(");
  Serial.print(tiltSensePin2);  
  Serial.print(")=");
  Serial.println(digitalRead(tiltSensePin2));
  Serial.print("leftBelt(");
  Serial.print(leftBeltSensePin);  
  Serial.print(")=");
  Serial.println(digitalRead(leftBeltSensePin));
  Serial.print("rightBelt(");
  Serial.print(rightBeltSensePin);  
  Serial.print(")=");  
  Serial.println(digitalRead(rightBeltSensePin));
  
  Serial.println("===");
  delay(1000);  
}

void TestSlowServoOpen() {
  Serial.println("====Testing bottom servos slow open====");

  servos.setPWM(servoOrder[0], 0, CLOSE+50); 
  delay(250);
  servos.setPWM(servoOrder[1], 0, CLOSE); 

  while(!Serial.available()) ; Serial.read();

  for(int i = CLOSE; i >= OPEN; --i) {
    servos.setPWM(servoOrder[0], 0, i); 
    delay(2);    
  }
  while(!Serial.available()) ; Serial.read();
  
  for(int i = CLOSE; i >= OPEN; --i) {
    servos.setPWM(servoOrder[1], 0, i);
//    delay(2);    
  }  
  while(!Serial.available()) ; Serial.read();  
}

void TestI2CServo() {
  Serial.println("====Testing i2c and servo closing====");

  bag_count = 0; 
  
  // open all flaps
  for(int i = 2; i < MAX_BAG_COUNT; ++i) {
    servos.setPWM(servoOrder[i], 0, OPEN);
    delay(500);
  }

  servos.setPWM(servoOrder[0], 0, CLOSE+50);
  delay(100);
  servos.setPWM(servoOrder[1], 0, CLOSE);
  
  while(bag_count < 8) {
    waitingForBags();  
  }
  Serial.println("Got all bags, woohoo");
}

void TestBagDrops() {
  Serial.println("====Testing bag drops====");
   bag_count = 8;
   while(bag_count > 0) {
    Serial.print("Bean bags left: ");
    Serial.println(bag_count);
    while(!Serial.available()) ; Serial.read();  
   
    static int bottomServo; // Bottom servo that will release the bean bang
  
    // Alternate bottom servo from which bean bags will be dropped
    bottomServo = servoOrder[bag_count % 2];
    
    Serial.print("Toggling bottom servo: ");
    Serial.println(bottomServo);
    servos.setPWM(bottomServo, 0, CLOSE);
    
    // TODO: Find best time for these delays
    delay(500);   
    openNextFlap();
    delay(250);

      servos.setPWM(bottomServo, 0, OPEN); 
    
    if(bottomServo == servoOrder[0]) {
      for(int i = CLOSE; i >= OPEN; --i) {
        servos.setPWM(servoOrder[0], 0, i); 
        delay(2);    
      }  
    } else if (bottomServo == servoOrder[1]) {
      servos.setPWM(bottomServo, 0, OPEN); 
    }
    
    bag_count--;
  }  
}
