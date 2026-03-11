#include "servo_module.h"
#include <ESP32Servo.h>
#include <Arduino.h>

Servo myServo;
const int SERVO_PIN = 21;

void setupServo() {
  myServo.attach(SERVO_PIN);
  myServo.write(0);
}

void openDoor() {
  Serial.println("Opening door");
  myServo.write(180);
  delay(5000);
  myServo.write(0);
}

void closeDoor() {
  myServo.write(0);
}