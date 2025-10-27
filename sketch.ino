#include <Wire.h>
#include <ESP32Servo.h>
#include <MPU6050_tockn.h>

Servo servo1, servo2, servo3, servo4, servo5;

#define PIN_SERVO1 23
#define PIN_SERVO2 22
#define PIN_SERVO3 14
#define PIN_SERVO4 12
#define PIN_SERVO5 13
#define PIN_PIR 17
#define INITIAL_ANGLE 0.0

MPU6050 mpu(Wire);

bool deteksi = false;
bool inPIRMode = false;
bool tahanYaw = false;

long deteksi_waktu = 0;
long yawTime = 0;
long timer = 0;

const long PIR_FREE_DURATION = 1500;
const long SERVO_5_HOLD = 1000;

float angleX = 0, angleY = 0, angleZ = 0;
float roll = 0, pitch = 0, yaw = 0;
float target1 = INITIAL_ANGLE, target2 = INITIAL_ANGLE;
float target3 = INITIAL_ANGLE, target4 = INITIAL_ANGLE;
float target5 = INITIAL_ANGLE;

void pir() {
  deteksi = true;
  deteksi_waktu = millis();
}

void ResetGerakan() {
  target1 = target2 = target3 = target4 = target5 = INITIAL_ANGLE;
}

void setup() {
  Serial.begin(115200);
  delay(200);

  pinMode(PIN_PIR, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_PIR), pir, CHANGE);

  servo1.attach(PIN_SERVO1);
  servo2.attach(PIN_SERVO2);
  servo3.attach(PIN_SERVO3);
  servo4.attach(PIN_SERVO4);
  servo5.attach(PIN_SERVO5);

  int initialPos = map(INITIAL_ANGLE, -90, 90, 0, 180);
  servo1.write(initialPos);
  servo2.write(initialPos);
  servo3.write(initialPos);
  servo4.write(initialPos);
  servo5.write(initialPos);

  Wire.begin(21, 22);  // default SDA=21, SCL=22
  mpu.begin();
  mpu.calcGyroOffsets(true);
  timer = millis();
}

void loop() {
  mpu.update();
  angleX = mpu.getAngleX();
  angleY = mpu.getAngleY();
  angleZ = mpu.getAngleZ();

  if (deteksi) {
    deteksi = false;
    inPIRMode = true;
    deteksi_waktu = millis();
    target1 = 60; target2 = -60;
    target3 = 45; target4 = -45;
    target5 = 60;
  }

  if (inPIRMode) {
    if (millis() - deteksi_waktu >= PIR_FREE_DURATION) {
      inPIRMode = false;
      ResetGerakan();
    }
  }

  roll = constrain(angleX, -90.0, 90.0);
  target1 = roll;
  target2 = roll;

  pitch = constrain(angleY, -90.0, 90.0);
  target3 = pitch;
  target4 = pitch;

  if (tahanYaw) {
    target5 = constrain(angleZ, -90.0, 90.0);
    if (millis() - yawTime >= SERVO_5_HOLD) {
      tahanYaw = false;
      target5 = INITIAL_ANGLE;
    }
  } else {
    target5 = INITIAL_ANGLE;
  }

  servo1.write(map(target1, -90, 90, 0, 180));
  servo2.write(map(target2, -90, 90, 0, 180));
  servo3.write(map(target3, -90, 90, 0, 180));
  servo4.write(map(target4, -90, 90, 0, 180));
  servo5.write(map(target5, -90, 90, 0, 180));
}
