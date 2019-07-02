#ifndef __POWERLEVELCONTROLLER_H__
#define __POWERLEVELCONTROLLER_H__

#include <ArduinoJson.h>
#include "Request/SinricProRequest.h"

struct powerLevelState {
  int powerLevel;
  powerLevelState(int powerLevel=0) : powerLevel(powerLevel) {}
  void setPowerLevel(int newLevel) { powerLevel = newLevel; }
  int getPowerLevel() {  return powerLevel; }
};

class PowerLevelController {
public:
  PowerLevelController() : _powerLevelCb(nullptr), _powerLevelAdjustCb(nullptr) {};
  typedef std::function<bool(const char*, powerLevelState state)> PowerLevelCallback;
  void onPowerLevel(PowerLevelCallback callback) { _powerLevelCb = callback; }
  void onAdjustPowerLevel(PowerLevelCallback callback) { _powerLevelAdjustCb = callback; }

  bool handle(JsonDocument& jsonRequest, JsonDocument& jsonResponse);

  void setPowerLevelState(powerLevelState& state) { _powerLevelState = state; }
  powerLevelState getPowerLevelState() { return _powerLevelState; }
private:
  PowerLevelCallback _powerLevelCb;
  PowerLevelCallback _powerLevelAdjustCb;
  powerLevelState _powerLevelState;
};

bool PowerLevelController::handle(JsonDocument& jsonRequest, JsonDocument& jsonResponse) {
  bool success(false);
  powerLevelState tempState = _powerLevelState;

  const char* deviceId = jsonRequest["deviceId"];
  const char* action = jsonRequest["action"];

  // setPowerLevel request
  if (strcmp(action, "setPowerLevel")== 0) {
    tempState.powerLevel = jsonRequest["value"]["powerLevel"];

    if (_powerLevelCb) success = _powerLevelCb(deviceId, tempState);
  }

  // adjustPowerLevel request
  if (strcmp(action, "adjustPowerLevel") == 0) {
    int powerLevelDelta = jsonRequest["value"]["powerLevelDelta"];
    tempState.powerLevel += powerLevelDelta;
    if (tempState.powerLevel > 100) tempState.powerLevel = 100;
    if (tempState.powerLevel < 0) tempState.powerLevel = 0;

    if (_powerLevelAdjustCb) success = _powerLevelAdjustCb(deviceId, tempState);
  }

  if (success) _powerLevelState = tempState;
  jsonResponse["value"]["powerLevel"] = _powerLevelState.powerLevel;
  return success;
}

#endif
