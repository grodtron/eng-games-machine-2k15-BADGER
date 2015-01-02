#define WOOD_MAX 1000
#define WOOD_MIN 700

#define HOLE_MAX 1000
#define HOLE_MIN 700

int holePeriod;
int woodPeriod;
unsigned long initialTime = 0;
unsigned long finishTime = 0;
const int ShootingSpeed = 1175; // Time to reach the target after turning motors on

void shootingTarget() {
  Serial.print("Bean bags left: ");
  Serial.println(bag_count);
  holePeriod = 0;
  woodPeriod = 0;
  while (holePeriod < HOLE_MIN || holePeriod > HOLE_MAX 
          || woodPeriod < WOOD_MIN || woodPeriod > WOOD_MAX)
      calculatePeriod();
 
  static int bottomServo; // Bottom servo that will release the bean bang

  // Alternate bottom servo from which bean bags will be dropped
  bottomServo = servoOrder[bag_count % 2];
  
  Serial.print("Toggling bottom servo: ");
  Serial.println(bottomServo);
  
  if(bottomServo == servoOrder[0]) {
    servos.setPWM(bottomServo, 0, CLOSE+50);
    
  } else {
    servos.setPWM(bottomServo, 0, CLOSE);    
  }
  
  // TODO: Find best time for these delays
  delay(500);   
  openNextFlap();
  delay(500);
  
  //servos.setPWM(bottomServo, 0, OPEN); 

  for(int i = CLOSE; i >= OPEN; --i) {
    servos.setPWM(bottomServo, 0, i); 
    delay(2);    
  }

  // shake it off
  const int numShakes = 10;
  const int shakeDelay = 30;
  const int shakeAmount = 50;
  shake(numShakes, shakeDelay, shakeAmount);

  timeBagShooting();

  bag_count--;

  if(bag_count == 0) {
    while(1) { // we done son
      for(int i = 0 ; i < 4; ++i) {
        servos.setPWM(15, 0, 0 );
        delay(500);
        servos.setPWM(15, 0, 4095);
        delay(500);
      }
    }
  }   
}  

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* 1 when hole, 0 when wood
	expected values: wood period ~ 890, hole period ~850. 
	measurement time: BCET ~2.4s, WCET ~4s
*/
void calculatePeriod() {
    holePeriod = woodPeriod = 0;
    while(holePeriod == 0 || woodPeriod == 0) {
        if (!digitalRead(irSensorPin)) {
            /* start at wood, wait till end, must measure hole period 
            then measures wood's period only if not calculated */
            while(!digitalRead(irSensorPin));

            initialTime = millis();
            while(digitalRead(irSensorPin));
            finishTime = millis();
            if (finishTime-initialTime>50) {
                holePeriod = finishTime-initialTime;
            }

            if(holePeriod > 50 && woodPeriod > 50)
                break;

            initialTime = millis();
            while(!digitalRead(irSensorPin));
            finishTime = millis();
            if (finishTime-initialTime>50) {
                woodPeriod = finishTime-initialTime;
            }
        } else { // start at hole, wait till end, measure wood period then hole's
            /* start at hole, wait till end, must measure wood's period 
            then measures hole's period only if not calculated */
            while(digitalRead(irSensorPin));

            initialTime = millis();
            while(!digitalRead(irSensorPin));
            finishTime = millis();
            if (finishTime-initialTime>50) {
                woodPeriod = finishTime-initialTime;
            }
            
            if(holePeriod > 50 && woodPeriod > 50)
                break;
            
            initialTime = millis();
            while(digitalRead(irSensorPin));
            finishTime = millis();
            if (finishTime-initialTime>50) {
                holePeriod = finishTime-initialTime;
            }
        }
  }
  Serial.print("hole period: ");
  Serial.print(holePeriod);
  Serial.print(", wood period: ");
  Serial.println(woodPeriod); 
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
  while(!digitalRead(leftBeltSensePin) && !digitalRead(rightBeltSensePin)); // Drop until we hit a sensor
  delay(50);
  servos.setPWM(9, 0, 4095);
  
  // TODO: Maybe go slower to be sure to never skip the first sensor we hit
  if(digitalRead(rightBeltSensePin) != 1) {
    MOVE_SIDE_FWD(RIGHT, 40);
    MOVE_SIDE_FWD(LEFT,  40);
    Serial.println("right belt switch, going forward");
  } else if(digitalRead(leftBeltSensePin) != 1) {
      MOVE_SIDE_BAK(RIGHT, 40);
      MOVE_SIDE_BAK(LEFT, 40);
    Serial.println("left belt switch, going backward");
  }

  while(digitalRead(leftBeltSensePin) != 1 || digitalRead(rightBeltSensePin) != 1); // Go until both belt sensor pins are touched
  MOVE_SIDE_FWD(RIGHT, 0);
  MOVE_SIDE_FWD(LEFT,  0);
  Serial.println("done");
}

void shake(int numShakes, int shakeDelay, int amount) {
  for(int i = 0; i < numShakes; ++i) {
    MOVE_SIDE_FWD(RIGHT, amount);
    MOVE_SIDE_FWD(LEFT, amount);
    delay(shakeDelay);
    MOVE_SIDE_BAK(RIGHT, amount);
    MOVE_SIDE_BAK(LEFT, amount); 
    delay(shakeDelay); 
    MOVE_SIDE_FWD(RIGHT, 0);
    MOVE_SIDE_FWD(LEFT, 0);    
  }      
}
