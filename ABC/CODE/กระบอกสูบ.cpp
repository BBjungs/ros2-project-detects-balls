#include <Arduino.h>

#define PUSH_PIN    41  // กระบอกลมผลักชิ้นงาน
#define LIFT_PIN    42  // กระบอกลมยกถาด
#define MAGNET_PIN  40  // แม่เหล็กดูด

void setup() {
  Serial.begin(115200);

  pinMode(PUSH_PIN, OUTPUT);
  pinMode(LIFT_PIN, OUTPUT);
  pinMode(MAGNET_PIN, OUTPUT);

  digitalWrite(PUSH_PIN, LOW);
  digitalWrite(LIFT_PIN, LOW);
  digitalWrite(MAGNET_PIN, LOW);
}

void loop() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();  // ลบช่องว่างหรือตัวขึ้นบรรทัด

    if (cmd == "push_on") {
      digitalWrite(PUSH_PIN, HIGH);
      Serial.println("✅ Push ON");
    } else if (cmd == "push_off") {
      digitalWrite(PUSH_PIN, LOW);
      Serial.println("⛔ Push OFF");
    } else if (cmd == "lift_on") {
      digitalWrite(LIFT_PIN, HIGH);
      Serial.println("✅ Lift ON");
    } else if (cmd == "lift_off") {
      digitalWrite(LIFT_PIN, LOW);
      Serial.println("⛔ Lift OFF");
    } else if (cmd == "magnet_on") {
      digitalWrite(MAGNET_PIN, HIGH);
      Serial.println("✅ Magnet ON");
    } else if (cmd == "magnet_off") {
      digitalWrite(MAGNET_PIN, LOW);
      Serial.println("⛔ Magnet OFF");
    } else {
      Serial.println("❓ Unknown command: " + cmd);
    }
  }
}
