#include <Arduino.h>

//ทีมสีน้ำเงิน
int  delay_team_1 = 1800;    // สไลคขวา - ซ้าย
int  delay_team_2 = 2500;    // เดินหน้า

// ============================
// 🔧 PIN DEFINITIONS
// ============================
#define EN_LF 4
#define EN_RF 5
#define EN_LB 3
#define EN_RB 2

#define DIR1_LF 18
#define DIR2_LF 19
#define DIR1_RF 22
#define DIR2_RF 23
#define DIR1_LB 16
#define DIR2_LB 17
#define DIR1_RB 14
#define DIR2_RB 15

#define PUSH_PIN    41
#define LIFT_PIN    42
#define MAGNET_PIN  40

#define STEP_PIN 26
#define DIR_PIN  27
#define LIMIT_SWITCH_PIN 50

// 🆕 SW1–SW5
#define SW1_PIN 35
#define SW2_PIN 34
#define SW3_PIN 33
#define SW4_PIN 32
#define SW5_PIN 31

const int trig_lf = 9;
const int echo_lf = 8;

const int trig_lb = 6;
const int echo_lb = 7;

const int trig_rf = 25;
const int echo_rf = 24;

const int trig_rb = 53;
const int echo_rb = 52;

const int trig_front = 10;
const int echo_front = 11;

// ============================
// 🔧 CONFIGURATION
// ============================
const int SPEED_FORWARD      = 100;
const int SPEED_BACKWARD     = 100;
const int SPEED_SLIDE_LEFT   = 100;
const int SPEED_SLIDE_RIGHT  = 50;
const int SPEED_TURN_LEFT    = 100;
const int SPEED_TURN_RIGHT   = 100;
/// @brief //////////////////////////////////////////////
float factor_forward_lf = 1.00;
float factor_forward_rf = 1.00;
float factor_forward_lb = 1.00;
float factor_forward_rb = 1.00;



/// @brief //////////////////////////////////////////////
float factor_backward_lf = 1.00;  //ล้อซ้ายหน้า
float factor_backward_rf = 1.00;  // ล้อขวาหน้า   







float factor_backward_lb = 1.00;
float factor_backward_rb = 1.00;

////////////////////////////////////////////////////////////





float factor_slide_left_lf = 1.00;

float factor_slide_left_rf = 1.00;
float factor_slide_left_lb = 1.00;
float factor_slide_left_rb = 1.00;

float factor_slide_right_lf = 2.00;
float factor_slide_right_rf = 1.00;
float factor_slide_right_lb = 1.00;
float factor_slide_right_rb = 1.00;

float factor_turn_lf = 1.10;
float factor_turn_rf = 1.00;
float factor_turn_lb = 1.00;
float factor_turn_rb = 1.00;

int stepsPerRevolution = 800;
int delayMicros        = 300;

// ============================
// 🔧 FUNCTION PROTOTYPES
// ============================
void rotateUntilSwitchAndThenReverse();
void step1();
bool isLimitSwitchStableLow();





// ============================
// 🔧 MOTOR CONTROL FUNCTIONS
// ============================
void controlWheel(int en, int in1, int in2, int speed, bool forward) {
  digitalWrite(in1, forward ? HIGH : LOW);
  digitalWrite(in2, forward ? LOW : HIGH);
  analogWrite(en, constrain(abs(speed), 0, 255));
}

void moveForward() {
  controlWheel(EN_LF, DIR1_LF, DIR2_LF, SPEED_FORWARD * factor_forward_lf, true);
  controlWheel(EN_RF, DIR1_RF, DIR2_RF, SPEED_FORWARD * factor_forward_rf, true);
  controlWheel(EN_LB, DIR1_LB, DIR2_LB, SPEED_FORWARD * factor_forward_lb, true);
  controlWheel(EN_RB, DIR1_RB, DIR2_RB, SPEED_FORWARD * factor_forward_rb, true);
}

void moveBackward() {
  controlWheel(EN_LF, DIR1_LF, DIR2_LF, SPEED_BACKWARD * factor_backward_lf, false);
  controlWheel(EN_RF, DIR1_RF, DIR2_RF, SPEED_BACKWARD * factor_backward_rf, false);
  controlWheel(EN_LB, DIR1_LB, DIR2_LB, SPEED_BACKWARD * factor_backward_lb, false);
  controlWheel(EN_RB, DIR1_RB, DIR2_RB, SPEED_BACKWARD * factor_backward_rb, false);
}

void slideLeft() {
  controlWheel(EN_LF, DIR1_LF, DIR2_LF, SPEED_SLIDE_LEFT * factor_slide_left_lf, false);
  controlWheel(EN_RF, DIR1_RF, DIR2_RF, SPEED_SLIDE_LEFT * factor_slide_left_rf, true);
  controlWheel(EN_LB, DIR1_LB, DIR2_LB, SPEED_SLIDE_LEFT * factor_slide_left_lb, true);
  controlWheel(EN_RB, DIR1_RB, DIR2_RB, SPEED_SLIDE_LEFT * factor_slide_left_rb, false);
}

void slideRight() {
  controlWheel(EN_LF, DIR1_LF, DIR2_LF, SPEED_SLIDE_RIGHT * factor_slide_right_lf, true);
  controlWheel(EN_RF, DIR1_RF, DIR2_RF, SPEED_SLIDE_RIGHT * factor_slide_right_rf, false);
  controlWheel(EN_LB, DIR1_LB, DIR2_LB, SPEED_SLIDE_RIGHT * factor_slide_right_lb, false);
  controlWheel(EN_RB, DIR1_RB, DIR2_RB, SPEED_SLIDE_RIGHT * factor_slide_right_rb, true);
}

void turnLeft() {
  controlWheel(EN_LF, DIR1_LF, DIR2_LF, SPEED_TURN_LEFT * factor_turn_lf, false);
  controlWheel(EN_RF, DIR1_RF, DIR2_RF, SPEED_TURN_LEFT * factor_turn_rf, true);
  controlWheel(EN_LB, DIR1_LB, DIR2_LB, SPEED_TURN_LEFT * factor_turn_lb, false);
  controlWheel(EN_RB, DIR1_RB, DIR2_RB, SPEED_TURN_LEFT * factor_turn_rb, true);
}

void turnRight() {
  controlWheel(EN_LF, DIR1_LF, DIR2_LF, SPEED_TURN_RIGHT * factor_turn_lf, true);
  controlWheel(EN_RF, DIR1_RF, DIR2_RF, SPEED_TURN_RIGHT * factor_turn_rf, false);
  controlWheel(EN_LB, DIR1_LB, DIR2_LB, SPEED_TURN_RIGHT * factor_turn_lb, true);
  controlWheel(EN_RB, DIR1_RB, DIR2_RB, SPEED_TURN_RIGHT * factor_turn_rb, false);
}

void stopAllWheels() {
  controlWheel(EN_LF, DIR1_LF, DIR2_LF, 0, true);
  controlWheel(EN_RF, DIR1_RF, DIR2_RF, 0, true);
  controlWheel(EN_LB, DIR1_LB, DIR2_LB, 0, true);
  controlWheel(EN_RB, DIR1_RB, DIR2_RB, 0, true);
}

void softBrakeForward(int ms = 100) {
  moveBackward();
  delay(ms);
  stopAllWheels();
}

void softBrakeSlideRight(int ms = 100) {
  slideLeft();
  delay(ms);
  stopAllWheels();
}

void softBrakeSlideLeft(int ms = 100) {
  slideRight();
  delay(ms);
  stopAllWheels();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

// #ทีมสีน้ำเงิน

// #define delay_blue_team_1  = ;
// #define delay_blue_team_2 = ;

// #ทีมสีแดง

// #define delay_gren_team_1  = ;
// #define delay_gren_team_2 = ;


void step1(){     // ทีมน้ำเงิน
  slideRight(); 
  delay(delay_team_1); //สไลค์ขวา
  softBrakeSlideRight();
  
  moveForward(); 
  delay(delay_team_2); //   เดินหน้า
  softBrakeForward();

  // moveBackward(); 
  // delay(500); //ถอยหลัง
  // stopAllWheels();
  
  rotateUntilSwitchAndThenReverse();

  moveBackward(); 
  delay(1800); //ถอยหลัง
  stopAllWheels();

}

void step2() {    // ทีมเขียว
  slideLeft(); 
  delay(delay_team_1); // สไลค ซ้าย
  softBrakeSlideLeft();

  moveForward(); 
  delay(delay_team_2); // เดินหน้า
  softBrakeForward();

  // moveBackward(); 
  // delay(500); //ถอยหลัง
  // stopAllWheels();  


  rotateUntilSwitchAndThenReverse();

  moveBackward(); 
  delay(2000); //ถอยหลัง
  stopAllWheels();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////



// ============================
// 🔧 STEPPER CONTROL FUNCTIONS
// ============================
void stepMotor(int steps) {
  for (int i = 0; i < steps; i++) {
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(delayMicros);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(delayMicros);
  }
}

void rotateOneRev(bool dir) {
  digitalWrite(DIR_PIN, dir ? HIGH : LOW);
  stepMotor(stepsPerRevolution);
}

bool isLimitSwitchStableLow() {
  int count = 0;
  for (int i = 0; i < 3; i++) {
    if (digitalRead(LIMIT_SWITCH_PIN) == LOW) {
      count++;
    }
    delay(10);
  }
  return (count == 3);
}

void rotateUntilSwitchAndThenReverse() {
  Serial.println("🔄 กำลังหมุน Stepper ไปทางซ้ายจนกว่าจะเจอสวิตช์...");
  // digitalWrite(DIR_PIN, LOW);
  // while (digitalRead(LIMIT_SWITCH_PIN) == LOW) {
  //   digitalWrite(STEP_PIN, HIGH);
  //   delayMicroseconds(delayMicros);
  //   digitalWrite(STEP_PIN, LOW);
  //   delayMicroseconds(delayMicros);
  // }

  // Serial.println("🛑 เจอสวิตช์แล้ว → หยุดมอเตอร์");

  // for (int i = 0; i < 2; i++) {
  //   rotateOneRev(true);
  // }

  digitalWrite(LIFT_PIN, HIGH); delay(3000);
  digitalWrite(PUSH_PIN, HIGH); digitalWrite(MAGNET_PIN, HIGH); 
  delay(5000);
  digitalWrite(PUSH_PIN, LOW); delay(3000);
  digitalWrite(LIFT_PIN, LOW); digitalWrite(MAGNET_PIN, LOW);
}

// ============================
// 🔧 SETUP & LOOP
// ============================
void setup() {
  // PWM + Direction Pins
  pinMode(EN_LF, OUTPUT); pinMode(DIR1_LF, OUTPUT); pinMode(DIR2_LF, OUTPUT);
  pinMode(EN_RF, OUTPUT); pinMode(DIR1_RF, OUTPUT); pinMode(DIR2_RF, OUTPUT);
  pinMode(EN_LB, OUTPUT); pinMode(DIR1_LB, OUTPUT); pinMode(DIR2_LB, OUTPUT);
  pinMode(EN_RB, OUTPUT); pinMode(DIR1_RB, OUTPUT); pinMode(DIR2_RB, OUTPUT);

  // Pneumatic + Magnet
  pinMode(PUSH_PIN, OUTPUT); pinMode(LIFT_PIN, OUTPUT); pinMode(MAGNET_PIN, OUTPUT);
  digitalWrite(PUSH_PIN, LOW); digitalWrite(LIFT_PIN, LOW); digitalWrite(MAGNET_PIN, LOW);

  // Stepper + Limit
  pinMode(STEP_PIN, OUTPUT); pinMode(DIR_PIN, OUTPUT); pinMode(LIMIT_SWITCH_PIN, INPUT_PULLUP);

  // Ultrasonic
  pinMode(trig_lf, OUTPUT); pinMode(echo_lf, INPUT);
  pinMode(trig_lb, OUTPUT); pinMode(echo_lb, INPUT);
  pinMode(trig_rf, OUTPUT); pinMode(echo_rf, INPUT);
  pinMode(trig_rb, OUTPUT); pinMode(echo_rb, INPUT);
  pinMode(trig_front, OUTPUT); pinMode(echo_front, INPUT);

  // 🆕 Switch Inputs
  pinMode(SW1_PIN, INPUT_PULLUP);
  pinMode(SW2_PIN, INPUT_PULLUP);
  pinMode(SW3_PIN, INPUT_PULLUP);
  pinMode(SW4_PIN, INPUT_PULLUP);
  pinMode(SW5_PIN, INPUT_PULLUP);

  Serial.begin(115200);
  Serial.println("🚗 เริ่มระบบหุ่นยนต์พร้อม stepper + ล้อโอมนิ");
  stopAllWheels();
}

void loop() {
  // 🆕 ตรวจสอบ Switch
  digitalWrite(MAGNET_PIN, HIGH); 
  if (digitalRead(SW1_PIN) == LOW) step1();
   else if (digitalRead(SW2_PIN) == LOW) step2();
  // else if (digitalRead(SW3_PIN) == LOW) rotateOneRev(true);
  // else if (digitalRead(SW4_PIN) == LOW) stopAllWheels();
   else if (digitalRead(SW5_PIN) == LOW) stopAllWheels();

  // Serial command control
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim(); cmd.toUpperCase();

    Serial.print("📥 คำสั่ง: "); Serial.println(cmd);

    if (cmd == "F") moveForward();
    else if (cmd == "B") moveBackward();
    else if (cmd == "L") slideLeft();
    else if (cmd == "R") slideRight();
    else if (cmd == "TL") turnLeft();
    else if (cmd == "TR") turnRight();
    else if (cmd == "S") stopAllWheels();


    else if (cmd == "1") {
     step1();
      Serial.println("✅ จบลำดับพิเศษ 1");
    }



    else if (cmd == "W") {
      Serial.println("🔁 หมุน Stepper 3 รอบ...");
      for (int i = 0; i < 3; i++) rotateOneRev(true);
    }
    else if (cmd == "2") {
      rotateUntilSwitchAndThenReverse();
    }



    else {
      Serial.println("❌ คำสั่งไม่ถูกต้อง");
    }
  }
}
