#include "wifi_module.h"
#include <WiFi.h>

// const char* WIFI_SSID = "EdNet";
// const char* WIFI_PASSWORD = "Huawei@123";

const char* WIFI_SSID = "Mango_F315E3";
const char* WIFI_PASSWORD = "5HB84387CR";

void setupWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("\nWiFi Connected ✅");
}