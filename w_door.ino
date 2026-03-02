#include <Keypad.h>
#include <Adafruit_Fingerprint.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <SPI.h>
#include <MFRC522.h>

// ---------------- WIFI ------------------
const char* ssid = "Mango_F315E3";
const char* password = "5HB84387CR";

// ---------------- FIREBASE ------------------
const char* firebaseHost = "https://home-d9fb3-default-rtdb.firebaseio.com";
const char* firebaseAuth = "f3ed4KN4k5AbmsByOeBFN1igF7DPBgsExZbXoNuw";

// ---------------- FINGERPRINT ----------------
HardwareSerial mySerial(2);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

// ---------------- KEYPAD --------------------
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {13, 12, 14, 27};
byte colPins[COLS] = {26, 25, 33, 32};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ---------------- PASSWORD ------------------
String correctPassword = "1234";
String inputPassword = "";

// ---------------- SERVO ---------------------
Servo myServo;
const int servoPin = 5;

// ---------------- RFID ---------------------
#define SS_PIN 4
#define RST_PIN 22

MFRC522 mfrc522(SS_PIN, RST_PIN);
String allowedUID = "2BC21B06";

// ---------------- SCAN CONTROL -------------
unsigned long lastScanTime = 0;
unsigned long scanDelay = 3000;

// ---------------- SETUP --------------------
void setup() {

  Serial.begin(115200);
  delay(1000);

  myServo.attach(servoPin);
  myServo.write(0);

  mySerial.begin(57600, SERIAL_8N1, 16, 17);

  if (finger.verifyPassword()) {
    Serial.println("Fingerprint sensor ready ✅");
  } else {
    Serial.println("Fingerprint sensor NOT found ❌");
    while (1);
  }

  WiFi.begin(ssid, password);
  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected ✅");
  Serial.println("System Ready 🔐");
  Serial.println("Enter Password:");

  SPI.begin(18, 19, 23, 4);
  mfrc522.PCD_Init();
  Serial.println("RFID Reader Ready ✅");
}

// ---------------- LOOP ---------------------
void loop() {

  handleKeypad();

  if (millis() - lastScanTime > scanDelay) {
    if (scanFingerprint()) {
      lastScanTime = millis();

      Serial.println("Fingerprint Authorized ✅");
      sendFirebase("Authorized", "Opened", finger.fingerID);
      openDoor();
    }
  }

  checkRFID();
}

// ---------------- KEYPAD -------------------
void handleKeypad() {

  char key = keypad.getKey();
  if (!key) return;

  if (key == '#') {

    Serial.println();

    if (inputPassword == correctPassword) {
      Serial.println("Password Correct ✅");
      sendFirebase("Authorized", "Opened", -1);
      openDoor();
    } else {
      Serial.println("Wrong Password ❌");
      sendFirebase("Unauthorized", "Closed", 0);
      closeDoor();
    }

    inputPassword = "";
    Serial.println("Enter Password:");
  }
  else if (key == '*') {
    inputPassword = "";
    Serial.println("\nPassword Cleared");
  }
  else {
    inputPassword += key;
    Serial.print("*");
  }
}

// ---------------- FINGERPRINT ----------------
bool scanFingerprint() {

  if (finger.getImage() != FINGERPRINT_OK) return false;
  if (finger.image2Tz() != FINGERPRINT_OK) return false;
  if (finger.fingerSearch() != FINGERPRINT_OK) {
    Serial.println("Fingerprint not recognized ❌");
    sendFirebase("Unauthorized", "Closed", 0);
    return false;
  }

  Serial.print("Fingerprint ID #");
  Serial.print(finger.fingerID);
  Serial.println(" recognized ✅");

  return true;
}

// ---------------- RFID ----------------
void checkRFID() {

  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;

  String uidString = "";

  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 0x10) uidString += "0";
    uidString += String(mfrc522.uid.uidByte[i], HEX);
  }

  uidString.toUpperCase();
  Serial.println("Scanned UID: " + uidString);

  if (uidString == allowedUID) {
    Serial.println("RFID Authorized ✅");
    sendFirebase("Authorized", "Opened", -2);
    openDoor();
  } else {
    Serial.println("RFID Unauthorized ❌");
    sendFirebase("Unauthorized", "Closed", 0);
    closeDoor();
  }

  mfrc522.PICC_HaltA();
}

// ---------------- SERVO ----------------
void openDoor() {

  Serial.println("Opening servo 180° for 5 seconds");
  myServo.write(180);
  delay(5000);
  myServo.write(0);
  Serial.println("Door Closed 🔒");
}

void closeDoor() {
  myServo.write(0);
}

// ---------------- FIREBASE ----------------
void sendFirebase(String access, String door_state, int userID) {

  if (WiFi.status() != WL_CONNECTED) return;

  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;

  String url = String(firebaseHost) + "/main_door.json?auth=" + firebaseAuth;

  String payload = "{";
  payload += "\"access\":\"" + access + "\",";
  payload += "\"door_state\":\"" + door_state + "\",";

  if (userID >= 0)
    payload += "\"user_id\":" + String(userID);
  else if (userID == -1)
    payload += "\"user_id\":\"MANUAL\"";
  else if (userID == -2)
    payload += "\"user_id\":\"RFID\"";

  payload += "}";

  http.begin(client, url);
  http.addHeader("Content-Type", "application/json");

  int httpResponseCode = http.PUT(payload);

  Serial.print("Firebase HTTP Code: ");
  Serial.println(httpResponseCode);

  http.end();
}