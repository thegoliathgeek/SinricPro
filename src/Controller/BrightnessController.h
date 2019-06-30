#ifndef __BRIGHTNESSCONTROLLER_H__
#define __BRIGHTNESSCONTROLLER_H__

#include <ArduinoJson.h>
#include "Command/SinricProCommand.h"

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

  bool handle(SinricProCommand& cmd);

  void setBrightnessState(brightnessState& state) { _brightnessState = state; }
  brightnessState getBrightnessState() { return _brightnessState; }
private:
  BrightnessLevelCallback _brightnessLevelCb;
  BrightnessLevelCallback _brightnessAdjustCb;

  brightnessState _brightnessState;
};

bool BrightnessController::handle(SinricProCommand& cmd) {
  DEBUG_SINRIC("BrightnessController::handle()\r\n");

  brightnessState tmpState = _brightnessState;

  if (strcmp(cmd.getActionName(), JSON_CMD_SET_BRIGHTNESS) == 0 && _brightnessLevelCb) {
    tmpState.brightness = cmd.getAction()[JSON_PARAMETERS][JSON_PARAM_BRIGHTNESS];
    cmd.setSuccess(_brightnessLevelCb(cmd.getDeviceId(), tmpState));
    cmd.setHandled(true);
  }

  if (strcmp(cmd.getActionName(), JSON_CMD_ADJUST_BRIGHTNESS) == 0 && _brightnessAdjustCb) {
    int adjust = cmd.getAction()[JSON_PARAMETERS][JSON_PARAM_BRIGHTNESSDELTA];
    tmpState.brightness+= adjust;
    if (tmpState.brightness > 100) tmpState.brightness = 100;
    if (tmpState.brightness < 0) tmpState.brightness = 0;
    cmd.setSuccess(_brightnessAdjustCb(cmd.getDeviceId(), tmpState));
    cmd.setHandled(true);
  }

  if (cmd.getSuccess()) _brightnessState = tmpState;
  if (cmd.isHandled()) cmd.getResponse()[JSON_DEVICE][JSON_RESULT_BRIGHTNESS] = _brightnessState.brightness;

  return cmd.isHandled();
}

#endif
