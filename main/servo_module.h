#ifndef SERVO_MODULE_H
#define SERVO_MODULE_H

#include <ESP32Servo.h>

extern Servo myServo;

void setupServo();
void openDoor();
void closeDoor();

#endif