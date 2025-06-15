#include <Arduino.h>

// Pin Definitions
#define STEP_PIN 26  // D26 → PUL+
#define DIR_PIN  27  // D27 → DIR+

// Motor Config
int stepsPerRevolution = 800;  // ตั้ง DIP = 800 steps/rev
int delayMicros        = 300;  // หน่วงระหว่างสเต็ป (น้อย = เร็ว)

// ประกาศล่วงหน้าก่อนใช้งาน
void stepMotor(int steps);
void rotateOneRev(bool dir);

void setup() {
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  Serial.begin(115200);

  Serial.println("🟢 Ready to receive command:");
  Serial.println("  F = Forward 1 rev");
  Serial.println("  B = Backward 1 rev");
  Serial.println("  S = Stop");
  Serial.println("  1–9 = Speed (1=ช้า, 9=เร็ว)");
}

void loop() {
  if (Serial.available()) {
    char cmd = Serial.read();

    switch (cmd) {
      case 'F':
      case 'f':
        Serial.println("➡️ Forward 1 revolution");
        rotateOneRev(true);
        break;

      case 'B':
      case 'b':
        Serial.println("⬅️ Backward 1 revolution");
        rotateOneRev(false);
        break;

      case 'S':
      case 's':
        Serial.println("⏹️ Stop (no action)");
        break;

      // ปรับความเร็ว (1 = ช้า, 9 = เร็ว)
      case '1' ... '9':
        // map ค่า '1'–'9' ให้เป็น delayMicroseconds ระหว่าง 1000–100
        delayMicros = map(cmd - '0', 1, 9, 1000, 100);
        Serial.print("⚙️ Speed set (delayMicros): ");
        Serial.println(delayMicros);
        break;

      default:
        Serial.println("❓ Unknown command");
        break;
    }
  }
}

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
