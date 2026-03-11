#include "servo_module.h"
#include "keypad_module.h"
#include <Keypad.h>
#include <WiFi.h>   
#include <WiFi.h>
// #include "servo_module.h"
#include "firebase_module.h"

// Keypad setup
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {32,33,25,26};
byte colPins[COLS] = {27,14,12,13};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

String correctPassword = "1234";
String inputPassword = "";

void setupKeypad() {
  Serial.println("Enter Password:");
}

void handleKeypad() {
  char key = keypad.getKey();
  if (!key) return;

  if (key == '#') {
    Serial.println();
    if (inputPassword == correctPassword) {
      Serial.println("Password Correct ✅");
      sendFirebase("Authorized","Opened","MANUAL");
      openDoor();
    } else {
      Serial.println("Wrong Password ❌");
      sendFirebase("Unauthorized","Closed","0");
      closeDoor();
    }
    inputPassword="";
    Serial.println("Enter Password:");
  }
  else if (key == '*') {
    inputPassword="";
    Serial.println("\nPassword Cleared");
  }
  else {
    inputPassword += key;
    Serial.print("*");
  }
}