#include <Arduino.h>

// 🔧 PWM ของมอเตอร์แต่ละล้อ
#define EN_LF 4
#define EN_RF 5
#define EN_LB 3
#define EN_RB 2

// 🔧 ทิศทางของมอเตอร์ (IN1, IN2)
#define DIR1_LF 18
#define DIR2_LF 19
#define DIR1_RF 22
#define DIR2_RF 23
#define DIR1_LB 16
#define DIR2_LB 17
#define DIR1_RB 14
#define DIR2_RB 15

// 🔧 ความเร็วพื้นฐานแต่ละทิศทาง
const int SPEED_FORWARD      = 100;
const int SPEED_BACKWARD     = 100;
const int SPEED_SLIDE_LEFT   = 100;
const int SPEED_SLIDE_RIGHT  = 50;
const int SPEED_TURN_LEFT    = 100;
const int SPEED_TURN_RIGHT   = 100;
const int SPEED_BRAKE_SOFT   = 100;

// 🔧 factor แยกแต่ละคำสั่ง

// เดินหน้า
float factor_forward_lf = 1.00;
float factor_forward_rf = 1.20;
float factor_forward_lb = 1.00;
float factor_forward_rb = 1.00;

// ถอยหลัง
float factor_backward_lf = 1.00;
float factor_backward_rf = 1.00;
float factor_backward_lb = 1.00;
float factor_backward_rb = 1.00;

// สไลด์ซ้าย
float factor_slide_left_lf = 1.00;
float factor_slide_left_rf = 1.00;
float factor_slide_left_lb = 1.00;
float factor_slide_left_rb = 1.00;

// สไลด์ขวา
float factor_slide_right_lf = 2.00;
float factor_slide_right_rf = 1.00;
float factor_slide_right_lb = 1.00;
float factor_slide_right_rb = 1.00;

// หมุน
float factor_turn_lf = 1.10;
float factor_turn_rf = 1.00;
float factor_turn_lb = 1.00;
float factor_turn_rb = 1.00;

// 🛠 ควบคุมล้อเดี่ยว
void controlWheel(int en, int in1, int in2, int speed, bool forward) {
  digitalWrite(in1, forward ? HIGH : LOW);
  digitalWrite(in2, forward ? LOW : HIGH);
  analogWrite(en, constrain(abs(speed), 0, 255));
}

// ✅ ฟังก์ชันเคลื่อนที่พื้นฐาน
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

// 🛑 ฟังก์ชันเบรกนุ่มนวล
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

// ✅ setup
void setup() {
  pinMode(EN_LF, OUTPUT); pinMode(DIR1_LF, OUTPUT); pinMode(DIR2_LF, OUTPUT);
  pinMode(EN_RF, OUTPUT); pinMode(DIR1_RF, OUTPUT); pinMode(DIR2_RF, OUTPUT);
  pinMode(EN_LB, OUTPUT); pinMode(DIR1_LB, OUTPUT); pinMode(DIR2_LB, OUTPUT);
  pinMode(EN_RB, OUTPUT); pinMode(DIR1_RB, OUTPUT); pinMode(DIR2_RB, OUTPUT);

  Serial.begin(115200);
  Serial.println("🚗 เริ่มระบบล้อโอมนิ พร้อม factor แยกทิศทาง");
  stopAllWheels();
}

// 🔁 loop
void loop() {
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

    // 🔄 ลำดับพิเศษ
    else if (cmd == "1") {
      Serial.println("➡ สไลด์ขวา 1 วิ...");
      slideRight();
      delay(1100);
      Serial.println("⏹ เบรกสไลด์ขวา...");
      softBrakeSlideRight(100);    

      Serial.println("▶ เดินหน้า 2 วิ...");
      moveForward();
      delay(2700);
      Serial.println("⏹ เบรกกลับทางเดินหน้า...");
      softBrakeForward(100);

      Serial.println("⏳ หน่วง 5 วินาที...");
      delay(5000);

      Serial.println("🔄 ถอยหลัง 2 วิ...");
      moveBackward();
      delay(1000);
      softBrakeForward(100);  // ใช้เดินหน้าช้า ๆ หยุดถอยหลัง


      Serial.println("⬅ สไลด์ซ้าย 2 วิ...");
      slideLeft();
      delay(2000);
      Serial.println("⏹ เบรกสไลด์ซ้าย...");
      slideRight();  // ใช้ slideRight() เบรกแบบนุ่มนวล
      delay(100);

      
      Serial.println("➡ สไลด์ขวา 2 วิ...");
      slideRight();
      delay(8500);
      softBrakeSlideRight(100);


      //stopAllWheels();

      Serial.println("✅ จบลำดับทั้งหมด");
    }
    else {
      Serial.println("❌ คำสั่งไม่ถูกต้อง");
    }
  }
}
