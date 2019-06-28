#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <SinricPro.h>
#include <Devices/SinricProLight.h>

#define SSID "YOUR WIFI-SSID HERE"
#define PASS "YOUR WIFI-PASS HERE"
#define API_KEY "YOUR-SINRIC-PRO-API-KEY-HERE"
#define LIGHT_ID "YOUR-LIGHT-DEVICE-ID-HERE"

// Instantiate SinricProDimSwitch (dimmable Switch)
SinricProLight myLight = SinricPro.add<SinricProLight>(LIGHT_ID);

// Callback-Routine for on/off request
bool onPowerState(const char* deviceId, powerState state) {
  Serial.printf("Device %s turned %s\r\n", deviceId, state.state?"ON":"OFF");
  return true;
}

// Callback-Routine for brightness-level request
bool onSetBrightness(const char* deviceId, brightnessState state) {
  Serial.printf("Device %s set power-level to %i\r\n", deviceId, state.brightness);
  return true;
}

// Callback-Routine for brightness-adjustment request
bool onAdjustBrightness(const char* deviceId, brightnessState state) {
  Serial.printf("Device %s set power-level to %i\r\n", deviceId, state.brightness);
  return true;
}

// Callback-Routine for color request
bool onSetColor(const char* deviceId, colorState state) {
  Serial.printf("Device %s sets color to r:%i, g:%i, b:%i\r\n", deviceId, state.r, state.g, state.b);
  return true;
}

// Callback-Routine for color-temperature request
bool onSetColorTemperature(const char* deviceId, colorTemperatureState state) {
  Serial.printf("Device %s sets color-temperature to %i\r\n", deviceId, state.colorTemperature);
  return true;
}

// Callback-Routine for color-temperature-increase request
bool onIncreaseColorTemperature(const char* deviceId, colorTemperatureState state) {
  Serial.printf("Device %s increase color-temperature to %i\r\n", deviceId, state.colorTemperature);
  return true;
}

// Callback-Routine for color-temperature-decrease request
bool onDecreaseColorTemperature(const char* deviceId, colorTemperatureState state) {
  Serial.printf("Device %s decrease color-temperature to %i\r\n", deviceId, state.colorTemperature);
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
  myLight.onPowerState(onPowerState);             // set callback routine for on/off request
  myLight.onSetBrightness(onSetBrightness);       // set callback for power-level request
  myLight.onAdjustBrightness(onAdjustBrightness); // set callback for adjust-brightness request
  myLight.onSetColor(onSetColor);                 // set callback for set-color request
  myLight.onSetColorTemperature(onSetColorTemperature); // set callback for set-color-temperature request
  myLight.onIncreaseColorTemperature(onIncreaseColorTemperature); // set callback for increase-color-temperature request
  myLight.onDecreaseColorTemperature(onDecreaseColorTemperature); // set callback for decrease-color-temperature request
  SinricPro.begin(API_KEY);                   // start SinricPro
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
