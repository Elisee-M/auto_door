#include "servo_module.h"
#include "rfid_module.h"
#include <MFRC522.h>
#include <SPI.h>
#include <WiFi.h>
// #include "servo_module.h"
#include "firebase_module.h"

#define SS_PIN 4
#define RST_PIN 22
MFRC522 rfid(SS_PIN,RST_PIN);

void setupRFID() {
  SPI.begin(18,19,23,4);
  rfid.PCD_Init();
  Serial.println("RFID Ready ✅");
}

void checkRFID() {
  if (!rfid.PICC_IsNewCardPresent()) return;
  if (!rfid.PICC_ReadCardSerial()) return;

  String uidString = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) uidString += "0";
    uidString += String(rfid.uid.uidByte[i], HEX);
  }
  uidString.toUpperCase();

  Serial.println("Scanned UID: " + uidString);

  String name = getNameFromFirebase(uidString);

  if(name != "null" && name != "") {
    Serial.println("User Known ✅ Name: " + name);
    sendFirebase("Authorized","Opened",uidString);
    openDoor();
  } else {
    Serial.println("Unknown Card ❌ Access Denied");
    sendFirebase("Unauthorized","Closed","0");
    closeDoor();
  }

  rfid.PICC_HaltA();
}