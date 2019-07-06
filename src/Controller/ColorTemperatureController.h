#ifndef __COLORTEMPERATURECONTROLLER_H__
#define __COLORTEMPERATURECONTROLLER_H__

#include <ArduinoJson.h>
#include "Request/SinricProRequest.h"

struct colorTemperatureState {
  int colorTemperature;
  colorTemperatureState(int colorTemperature=0) :colorTemperature(colorTemperature) {}
  void setColorTemperature(int newColorTemperature) { colorTemperature = newColorTemperature; }
  int getColorTemperature() { return colorTemperature; }
};

class ColorTemperatureController {
public:
  ColorTemperatureController() :
    _colorTempCb(nullptr),
    _incColorTempCb(nullptr),
    _decColorTempCb(nullptr) {
  };
  typedef std::function<bool(const char*, colorTemperatureState&)> ColorTemperatureCallback;

  void onSetColorTemperature(ColorTemperatureCallback callback) { _colorTempCb = callback; }
  void onIncreaseColorTemperature(ColorTemperatureCallback callback) { _incColorTempCb = callback; }
  void onDecreaseColorTemperature(ColorTemperatureCallback callback) { _decColorTempCb = callback; }

  bool handle(JsonDocument& jsonRequest, JsonDocument& jsonResponse);
  bool raiseEvent(JsonDocument& jsonEvent);

  void setColorTemperatureState(colorTemperatureState& state) { _colorTemperatureState = state; }
  colorTemperatureState& getColorTemperatureState() { return _colorTemperatureState; }
private:
  ColorTemperatureCallback _colorTempCb;
  ColorTemperatureCallback _incColorTempCb;
  ColorTemperatureCallback _decColorTempCb;

  colorTemperatureState _colorTemperatureState;
};

bool ColorTemperatureController::handle(JsonDocument& jsonRequest, JsonDocument& jsonResponse) {
  bool success(false);
  colorTemperatureState tempState = _colorTemperatureState;

  const char* deviceId = jsonRequest["deviceId"];
  const char* action = jsonRequest["action"];

  // setcolorTemperature request
  if (strcmp(action, "setColorTemperature")==0) {
    tempState.colorTemperature = jsonRequest["value"]["colorTemperature"];

    if (_colorTempCb) success = _colorTempCb(deviceId, tempState);
  }

  // increaseColorTemperature request
  if (strcmp(action, "increaseColorTemperature")==0) {
    if (_incColorTempCb) success = _incColorTempCb(deviceId, tempState);
  }

  // decreaseColorTemperature request
  if (strcmp(action, "decreaseColorTemperature")==0) {
    if (_incColorTempCb) success = _decColorTempCb(deviceId, tempState);
  }

  if (success) _colorTemperatureState = tempState;
  jsonResponse["value"]["colorTemperature"] = _colorTemperatureState.colorTemperature;
  return success;
}

bool ColorTemperatureController::raiseEvent(JsonDocument& jsonEvent) {
  jsonEvent["value"]["colorTemperature"] = _colorTemperatureState.colorTemperature;
  return true;
}

#endif
