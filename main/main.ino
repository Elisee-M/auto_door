#include <Arduino.h>
#include "wifi_module.h"
#include "fingerprint_module.h"
#include "keypad_module.h"
#include "rfid_module.h"
#include "servo_module.h"
#include "firebase_module.h"

void setup() {
  Serial.begin(115200);
  delay(1000);

  setupServo();
  setupFingerprint();
  setupWiFi();
  setupRFID();
  setupKeypad();

  Serial.println("System Ready 🔐");
}

void loop() {
  handleKeypad();
  checkFingerprint();
  checkRFID();
}