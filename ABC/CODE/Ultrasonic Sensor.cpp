#include <Arduino.h>

// --- ฟังก์ชันวัดระยะ Ultrasonic ---
long readUltrasonic(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 30000); // timeout 30ms
  long distance = duration * 0.034 / 2;
  return distance;
}

// --- กำหนดขา Trigger และ Echo ---
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

void setup() {
  Serial.begin(115200);

  pinMode(trig_lf, OUTPUT);    pinMode(echo_lf, INPUT);
  pinMode(trig_lb, OUTPUT);    pinMode(echo_lb, INPUT);
  pinMode(trig_rf, OUTPUT);    pinMode(echo_rf, INPUT);
  pinMode(trig_rb, OUTPUT);    pinMode(echo_rb, INPUT);
  pinMode(trig_front, OUTPUT); pinMode(echo_front, INPUT);
}

void loop() {
  long dist_lf = readUltrasonic(trig_lf, echo_lf);
  long dist_lb = readUltrasonic(trig_lb, echo_lb);
  long dist_rf = readUltrasonic(trig_rf, echo_rf);
  long dist_rb = readUltrasonic(trig_rb, echo_rb);
  long dist_front = readUltrasonic(trig_front, echo_front);

  Serial.println("📏 Ultrasonic Readings (cm)");
  Serial.print("  ➤ Left Front  (LF) : "); Serial.println(dist_lf);
  Serial.print("  ➤ Left Back   (LB) : "); Serial.println(dist_lb);
  Serial.print("  ➤ Right Front (RF) : "); Serial.println(dist_rf);
  Serial.print("  ➤ Right Back  (RB) : "); Serial.println(dist_rb);
  Serial.print("  ➤ Front             : "); Serial.println(dist_front);
  Serial.println("--------------------------------");

  //delay(100);
}
