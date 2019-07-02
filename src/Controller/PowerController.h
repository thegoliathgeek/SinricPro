#ifndef __POWERCONTROLLER_H__
#define __POWERCONTROLLER_H__

#include <ArduinoJson.h>
#include "Request/SinricProRequest.h"
#include "SinricProDebug.h"

struct powerState {
  bool state;
  powerState(bool state=false) : state(state) {}
  void turnOn() { state = true; }
  void turnOff() { state = false; }
  bool switchState() { state = !state; return state; }
  bool isOn() { return state == true; }
  bool isOff() { return state == false; }
};

class PowerController {
public:
  PowerController() : _powerStateCb(nullptr) {};
  typedef std::function<bool(const char*, powerState& state)> PowerStateCallback;
  void onPowerState(PowerStateCallback callback) { _powerStateCb = callback; }

  bool handle(JsonDocument& jsonRequest, JsonDocument& jsonResponse);

  void setPowerState(powerState& state) { _powerState = state; }
  powerState getPowerState() { return _powerState; }
private:
  PowerStateCallback _powerStateCb;

  powerState _powerState;
};

bool PowerController::handle(JsonDocument& jsonRequest, JsonDocument& jsonResponse) {
  bool success(false);
  powerState tempState = _powerState;

  const char* deviceId = jsonRequest["deviceId"];
  const char* action = jsonRequest["action"];

  // setPowerState request
  if (strcmp(action, "setPowerState")==0) {
    const char* value_state = jsonRequest["value"]["state"];

    if (strcmp(value_state,"On")==0) tempState.state = true;
    if (strcmp(value_state,"Off")==0) tempState.state = false;

    if (_powerStateCb) success = _powerStateCb(deviceId, tempState);
  }

  if (success) _powerState = tempState;
  jsonResponse["value"]["state"] = _powerState.state?"On":"Off";
  return success;
}

#endif
