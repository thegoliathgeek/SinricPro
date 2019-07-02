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
  typedef std::function<bool(const char*, colorTemperatureState)> ColorTemperatureCallback;

  void onSetColorTemperature(ColorTemperatureCallback callback) { _colorTempCb = callback; }
  void onIncreaseColorTemperature(ColorTemperatureCallback callback) { _incColorTempCb = callback; }
  void onDecreaseColorTemperature(ColorTemperatureCallback callback) { _decColorTempCb = callback; }

  bool handle(JsonDocument& jsonRequest, JsonDocument& jsonResponse);

  void setColorTemperatureState(colorTemperatureState& state) { _colorTemperatureState = state; }
  colorTemperatureState getColorTemperatureState() { return _colorTemperatureState; }
private:
  ColorTemperatureCallback _colorTempCb;
  ColorTemperatureCallback _incColorTempCb;
  ColorTemperatureCallback _decColorTempCb;

  colorTemperatureState _colorTemperatureState;
};

bool ColorTemperatureController::handle(JsonDocument& jsonRequest, JsonDocument& jsonResponse) {
  bool success(false);
  return success;
}

#endif
