const int N_MOTORS = 4;

const int RIGHT_FRONT_PWM  = 9;
const int RIGHT_REAR_PWM  = 11;
const int LEFT_FRONT_PWM = 10;
const int LEFT_REAR_PWM = 6;

const int RIGHT_FRONT_DIR  = 12;
const int RIGHT_REAR_DIR  = 8;
const int LEFT_FRONT_DIR = 5;
const int LEFT_REAR_DIR = 7;

#define MOVE_WHEEL_FWD(SIDE, FRONT_REAR, AMOUNT) do { \
  analogWrite (SIDE ## _ ## FRONT_REAR ## _PWM, 255 - (AMOUNT)); \
  digitalWrite(SIDE ## _ ## FRONT_REAR ## _DIR, HIGH); \
} while(0)

#define MOVE_WHEEL_BAK(SIDE, FRONT_REAR, AMOUNT) do { \
  analogWrite (SIDE ## _ ## FRONT_REAR ## _PWM, (AMOUNT)); \
  digitalWrite(SIDE ## _ ## FRONT_REAR ## _DIR, LOW); \
} while(0)


#define MOVE_SIDE_FWD(SIDE, AMOUNT) do {  \
  MOVE_WHEEL_FWD(SIDE, FRONT, AMOUNT); \
  MOVE_WHEEL_FWD(SIDE, REAR, AMOUNT);  \
} while(0)

#define MOVE_SIDE_BAK(SIDE, AMOUNT) do {  \
  MOVE_WHEEL_BAK(SIDE, FRONT, AMOUNT); \
  MOVE_WHEEL_BAK(SIDE, REAR, AMOUNT);  \
} while(0) 
