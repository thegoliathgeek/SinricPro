#ifndef __COLORTEMPERATURECONTROLLER_H__
#define __COLORTEMPERATURECONTROLLER_H__

#include <ArduinoJson.h>
#include "Command/SinricProCommand.h"

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

  bool handle(SinricProCommand& cmd);

  void setColorTemperatureState(colorTemperatureState& state) { _colorTemperatureState = state; }
  colorTemperatureState getColorTemperatureState() { return _colorTemperatureState; }
private:
  ColorTemperatureCallback _colorTempCb;
  ColorTemperatureCallback _incColorTempCb;
  ColorTemperatureCallback _decColorTempCb;

  colorTemperatureState _colorTemperatureState;
};

bool ColorTemperatureController::handle(SinricProCommand& cmd) {
  DEBUG_SINRIC("ColorTemperatureController::handle()\r\n");

  colorTemperatureState tmpState = _colorTemperatureState;

  if (strcmp(cmd.getActionName(), JSON_CMD_SET_COLOR_TEMPERATURE) == 0 && _colorTempCb) {
    tmpState.colorTemperature = cmd.getAction()[JSON_PARAMETERS][JSON_PARAM_COLORTEMPERATURE];
    cmd.setSuccess(_colorTempCb(cmd.getDeviceId(), tmpState));
    cmd.setHandled(true);
  }

  if (strcmp(cmd.getActionName(), JSON_CMD_INCREASE_COLOR_TEMPERATURE) == 0 && _incColorTempCb) {
    cmd.setSuccess(_incColorTempCb(cmd.getDeviceId(), tmpState));
    cmd.setHandled(true);
  }

  if (strcmp(cmd.getActionName(), JSON_CMD_DECREASE_COLOR_TEMPERATURE) == 0 && _decColorTempCb) {
    cmd.setSuccess(_decColorTempCb(cmd.getDeviceId(), tmpState));
    cmd.setHandled(true);
  }

  if (cmd.getSuccess()) _colorTemperatureState = tmpState;
  if (cmd.isHandled()) cmd.getResponse()[JSON_DEVICE][JSON_RESULT_COLORTEMPERATURE] = _colorTemperatureState.colorTemperature;

  return cmd.isHandled();
}

#endif
