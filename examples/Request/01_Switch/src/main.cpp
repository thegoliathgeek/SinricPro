#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <SinricPro.h>
#include <Devices/SinricProSwitch.h>

#define SSID "YOUR WIFI-SSID HERE"
#define PASS "YOUR WIFI-PASS HERE"
#define API_KEY "YOUR-SINRIC-PRO-API-KEY-HERE"
#define SWITCH_ID "YOUR-SWITCH-DEVICE-ID-HERE"

// Instantiate SinricProSwitch
SinricProSwitch mySwitch = SinricPro.add<SinricProSwitch>(SWITCH_ID);

// Callback-Routine
bool onPowerState(const char* deviceId, powerState state) {
  Serial.printf("Device %s turned %s\r\n", deviceId, state.state?"ON":"OFF");
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
  mySwitch.onPowerState(onPowerState);    // set callback routine
  SinricPro.begin(API_KEY);               // start SinricPro
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
