#include <Arduino.h>
#include <micro_ros_platformio.h>
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <std_msgs/msg/int32.h>

// —— ขาเซ็นเซอร์ (active-LOW) ——
#define SENSOR_PIN1 32
#define SENSOR_PIN2 33
#define SENSOR_PIN3 25
#define SENSOR_PIN4 26

// —— ขามอเตอร์ (LOW→หมุน HIGH→หยุด) ——
#define MOTOR1_PIN 18
#define MOTOR2_PIN 19
#define MOTOR3_PIN 23
#define LED_PIN    2

#ifndef NODE_NAME
#define NODE_NAME "tray_node"
#endif
int count = 0;
// 🔁 ฟังก์ชัน Error Loop
void rclErrorLoop() {
  while (true) {
    digitalWrite(LED_PIN, HIGH);
    delay(150);
    digitalWrite(LED_PIN, LOW);
    delay(150);
  }
}

#ifndef RCCHECK
#define RCCHECK(fn) { if ((fn) != RCL_RET_OK) rclErrorLoop(); }
#endif

#ifndef EXECUTE_EVERY_N_MS
#define EXECUTE_EVERY_N_MS(MS, X)             \
  do {                                       \
    static int64_t last = -1;                \
    if (last < 0) last = uxr_millis();       \
    if (uxr_millis() - last > (MS)) {        \
      X;                                     \
      last = uxr_millis();                   \
    }                                        \
  } while (0)
#endif

enum States {
  WAITING_AGENT,
  AGENT_AVAILABLE,
  AGENT_CONNECTED,
  AGENT_DISCONNECTED
};
States state = WAITING_AGENT;

bool motor1Running = false;
int motor1SensorCmd = -1;  // เก็บคำสั่งที่รอให้เซ็นเซอร์ทริก
bool pushing = false;
unsigned long pushStart = 0;

rclc_support_t support;
rcl_node_t node;
rclc_executor_t executor;
rcl_allocator_t allocator;

rcl_subscription_t cmd_sub;
std_msgs__msg__Int32 cmd_msg;

rcl_publisher_t feedback_pub;
std_msgs__msg__Int32 feedback_msg;

void cmd_callback(const void * msgin) {
  const std_msgs__msg__Int32 * incoming = (const std_msgs__msg__Int32 *)msgin;
  int cmd = incoming->data;

  switch (cmd) {
    case 0:
      motor1Running = false;
      pushing = false;
      motor1SensorCmd = -1;
      count = 0;
      digitalWrite(MOTOR1_PIN, HIGH);
      digitalWrite(MOTOR2_PIN, HIGH);
      digitalWrite(MOTOR3_PIN, HIGH);
      break;
    case 1: case 2: case 3: case 4:
      motor1SensorCmd = cmd;
      motor1Running = true;
      digitalWrite(MOTOR1_PIN, LOW);  // ให้มอเตอร์ 1 หมุน
      break;
    case 5:
      digitalWrite(MOTOR2_PIN, LOW);
      break;
    case 6:
      digitalWrite(MOTOR2_PIN, HIGH);
      break;
    case 7:
      pushing = true;
      pushStart = millis();
      digitalWrite(MOTOR3_PIN, LOW);
      break;
    default:
      break;
  }
}

bool createEntities() {
  allocator = rcl_get_default_allocator();
  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));
  RCCHECK(rclc_node_init_default(&node, NODE_NAME, "", &support));

  RCCHECK(rclc_subscription_init_default(
    &cmd_sub, &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
    "command"
  ));

  RCCHECK(rclc_publisher_init_default(
    &feedback_pub,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
    "feedback_motor_1"
  ));

  executor = rclc_executor_get_zero_initialized_executor();
  RCCHECK(rclc_executor_init(&executor, &support.context, 1, &allocator));
  RCCHECK(rclc_executor_add_subscription(&executor, &cmd_sub, &cmd_msg, &cmd_callback, ON_NEW_DATA));

  return true;
}

bool destroyEntities() {
  rmw_context_t * ctx = rcl_context_get_rmw_context(&support.context);
  (void)rmw_uros_set_context_entity_destroy_session_timeout(ctx, 0);
  rclc_executor_fini(&executor);
  rcl_publisher_fini(&feedback_pub, &node);
  (void)rcl_node_fini(&node);
  rclc_support_fini(&support);
  return true;
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  set_microros_serial_transports(Serial);

  pinMode(SENSOR_PIN1, INPUT_PULLUP);
  pinMode(SENSOR_PIN2, INPUT_PULLUP);
  pinMode(SENSOR_PIN3, INPUT_PULLUP);
  pinMode(SENSOR_PIN4, INPUT_PULLUP);

  pinMode(MOTOR1_PIN, OUTPUT); digitalWrite(MOTOR1_PIN, HIGH);
  pinMode(MOTOR2_PIN, OUTPUT); digitalWrite(MOTOR2_PIN, HIGH);
  pinMode(MOTOR3_PIN, OUTPUT); digitalWrite(MOTOR3_PIN, HIGH);
}

void loop() {
  switch (state) {
    case WAITING_AGENT:
      EXECUTE_EVERY_N_MS(500,
        state = (rmw_uros_ping_agent(100, 1) == RMW_RET_OK ? AGENT_AVAILABLE : WAITING_AGENT)
      );
      break;
    case AGENT_AVAILABLE:
      state = createEntities() ? AGENT_CONNECTED : WAITING_AGENT;
      if (state == WAITING_AGENT) destroyEntities();
      break;
    case AGENT_CONNECTED:
      EXECUTE_EVERY_N_MS(200,
        state = (rmw_uros_ping_agent(100, 1) == RMW_RET_OK ? AGENT_CONNECTED : AGENT_DISCONNECTED)
      );
      if (state == AGENT_CONNECTED) {
        rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
      }
      break;
    case AGENT_DISCONNECTED:
      destroyEntities();
      state = WAITING_AGENT;
      break;
  }

  if (motor1Running) {
    if ((motor1SensorCmd == 1 && digitalRead(SENSOR_PIN1) == LOW) ||
        (motor1SensorCmd == 2 && digitalRead(SENSOR_PIN2) == LOW) ||
        (motor1SensorCmd == 3 && digitalRead(SENSOR_PIN3) == LOW) ||
        (motor1SensorCmd == 4 && digitalRead(SENSOR_PIN4) == LOW)) {

      motor1Running = false;
      motor1SensorCmd = -1;
      digitalWrite(MOTOR1_PIN, HIGH);     // หยุดมอเตอร์ 1
      digitalWrite(MOTOR2_PIN, HIGH);     // หยุดมอเตอร์ 2 ด้วย
      delay(2000);
      count ++;
      if(count <= 3){
        digitalWrite(MOTOR2_PIN, LOW);
      }
      

      // ส่งค่ากลับไปยัง ROS 2 ว่าเซ็นเซอร์ทริกแล้ว
      feedback_msg.data = 10;
      rcl_publish(&feedback_pub, &feedback_msg, NULL);
    }
  }

  if (pushing && millis() - pushStart >= 1000) {
    pushing = false;
    digitalWrite(MOTOR3_PIN, HIGH);
  }

  delay(10);
}
