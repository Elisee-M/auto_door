#include "servo_module.h"
#include "fingerprint_module.h"
#include <HardwareSerial.h>
#include "servo_module.h"
#include "firebase_module.h"

HardwareSerial fingerSerial(2);
Adafruit_Fingerprint finger(&fingerSerial);

unsigned long lastScanTime = 0;
unsigned long scanDelay = 3000;

void setupFingerprint() {
  fingerSerial.begin(57600, SERIAL_8N1, 16, 17);
  if(finger.verifyPassword())
    Serial.println("Fingerprint sensor ready ✅");
  else
    Serial.println("Fingerprint NOT found ❌");
}

void checkFingerprint() {
  if (millis() - lastScanTime < scanDelay) return;
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) return;

  if (finger.image2Tz() == FINGERPRINT_OK &&
      finger.fingerSearch() == FINGERPRINT_OK) {
    Serial.print("Fingerprint ID #");
    Serial.print(finger.fingerID);
    Serial.println(" recognized ✅");
    sendFirebase("Authorized","Opened",String(finger.fingerID));
    openDoor();
  } else {
    Serial.println("Fingerprint not recognized ❌");
    sendFirebase("Unauthorized","Closed","0");
    closeDoor();
  }
  lastScanTime = millis();
}