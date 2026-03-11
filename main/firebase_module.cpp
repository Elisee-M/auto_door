#include "firebase_module.h"
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <WiFi.h>

const char* FIREBASE_HOST = "https://home-d9fb3-default-rtdb.firebaseio.com";
const char* FIREBASE_AUTH = "f3ed4KN4k5AbmsByOeBFN1igF7DPBgsExZbXoNuw";

String getNameFromFirebase(String uid) {
  if(WiFi.status() != WL_CONNECTED) return "";

  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;

  String url = String(FIREBASE_HOST) + "/users/" + uid + "/name.json?auth=" + FIREBASE_AUTH;
  http.begin(client, url);
  int httpCode = http.GET();

  String payload = "";
  if(httpCode == 200) {
    payload = http.getString();
    payload.replace("\"",""); 
  }

  http.end();
  return payload;
}

void sendFirebase(String access,String door_state,String userID) {
  if(WiFi.status() != WL_CONNECTED) return;

  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;

  String url = String(FIREBASE_HOST) + "/main_door.json?auth=" + FIREBASE_AUTH;

  String payload="{";
  payload += "\"access\":\""+access+"\",";  
  payload += "\"door_state\":\""+door_state+"\",";  
  payload += "\"user_id\":\""+userID+"\"";
  payload += "}";

  http.begin(client,url);
  http.addHeader("Content-Type","application/json");
  int httpResponseCode=http.PUT(payload);
  Serial.print("Firebase HTTP Code: ");
  Serial.println(httpResponseCode);
  http.end();
}