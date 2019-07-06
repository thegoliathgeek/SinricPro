#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <SinricPro.h>

#define SSID "YOUR WIFI-SSID HERE"
#define PASS "YOUR WIFI-PASS HERE"
#define API_KEY "YOUR-SINRIC-PRO-API-KEY-HERE"
#define DIMSWITCH_ID "YOUR-DIM-SWITCH-DEVICE-ID-HERE"

// Callback-Routine for on/off request
bool onPowerState(const char* deviceId, powerState& state) {
  Serial.printf("Device %s turned %s\r\n", deviceId, state.state?"ON":"OFF");
  return true;
}

// Callback-Routine for power-level request
bool onPowerLevel(const char* deviceId, powerLevelState& state) {
  Serial.printf("Device %s set power-level to %i\r\n", deviceId, state.powerLevel);
  return true;
}

// Callback-Routine for adjustPowerLevel request
bool onAdjustPowerLevel(const char* deviceId, powerLevelState& state) {
  Serial.printf("Device %s set power-level to %i\r\n", deviceId, state.powerLevel);
  return true;
}

// Setup Wifi
void setupWiFi() {
  Serial.print("Connecting WiFi");
  WiFi.begin(SSID, PASS);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("connected!");
}

// Setup SinricPro
void setupSinricPro() {
  SinricPro[DIMSWITCH_ID].onPowerState(onPowerState);  // automatic create device and set callback routine for on/off request
  SinricPro[DIMSWITCH_ID].onPowerLevel(onPowerLevel);  // set callback for power-level request
  SinricPro[DIMSWITCH_ID].onAdjustPowerLevel(onAdjustPowerLevel);  // set callback for adjust-power-level request
  SinricPro.begin(API_KEY);                // start SinricPro
}

// Setup
void setup() {
  Serial.begin(115200);
  setupWiFi();
  setupSinricPro();
}

void loop() {
  SinricPro.handle();
}
