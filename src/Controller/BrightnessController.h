#ifndef __BRIGHTNESSCONTROLLER_H__
#define __BRIGHTNESSCONTROLLER_H__

#include <ArduinoJson.h>
#include "Request/SinricProRequest.h"

struct brightnessState {
  int brightness;
  brightnessState(int brightness=0) : brightness(brightness) {}
  void setBrightness(int newBrightness) { brightness = newBrightness; }
  int getBrightness() { return brightness; }
};

class BrightnessController {
public:
  BrightnessController() : _brightnessLevelCb(nullptr) {}
  typedef std::function<bool(const char*, brightnessState)> BrightnessLevelCallback;
  void onSetBrightness(BrightnessLevelCallback callback) { _brightnessLevelCb = callback; }
  void onAdjustBrightness(BrightnessLevelCallback callback) { _brightnessAdjustCb = callback; }

  bool handle(JsonDocument& jsonRequest, JsonDocument& jsonResponse);

  void setBrightnessState(brightnessState& state) { _brightnessState = state; }
  brightnessState getBrightnessState() { return _brightnessState; }
private:
  BrightnessLevelCallback _brightnessLevelCb;
  BrightnessLevelCallback _brightnessAdjustCb;

  brightnessState _brightnessState;
};

bool BrightnessController::handle(JsonDocument& jsonRequest, JsonDocument& jsonResponse) {
  bool success(false);
  brightnessState tempState = _brightnessState;

  const char* deviceId = jsonRequest["deviceId"];
  const char* action = jsonRequest["action"];

  // setBrightness request
  if (strcmp(action, "setBrightness")== 0) {
    tempState.brightness = jsonRequest["value"]["brightness"];

    if (_brightnessLevelCb) success = _brightnessLevelCb(deviceId, tempState);
  }

  // adjustBrightness request
  if (strcmp(action, "adjustBrightness") == 0) {
    int brightnessDelta = jsonRequest["value"]["brightnessDelta"];
    DEBUG_SINRIC("delta:%i\r\n", brightnessDelta);
    tempState.brightness += brightnessDelta;
    if (tempState.brightness > 100) tempState.brightness = 100;
    if (tempState.brightness < 0) tempState.brightness = 0;

    if (_brightnessAdjustCb) success = _brightnessAdjustCb(deviceId, tempState);
  }

  if (success) _brightnessState = tempState;
  jsonResponse["value"]["brightness"] = _brightnessState.brightness;
  return success;
}

#endif
