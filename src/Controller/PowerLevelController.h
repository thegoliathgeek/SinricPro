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
  void handlePowerLevelController(SinricProCommand& cmd);

  void setPowerLevelState(powerLevelState& state) { _powerLevelState = state; }
  powerLevelState getPowerLevelState() { return _powerLevelState; }
private:
  PowerLevelCallback _powerLevelCb;
  PowerLevelCallback _powerLevelAdjustCb;

  powerLevelState _powerLevelState;
};

void PowerLevelController::handlePowerLevelController(SinricProCommand& cmd) {
  if (cmd.isHandled()) return;
  DEBUG_SINRIC("handlePowerLevelController()\r\n");

  powerLevelState tmpState = _powerLevelState;

  if (strcmp(cmd.getActionName(), JSON_CMD_SET_POWERLEVEL) == 0 && _powerLevelCb) {
    tmpState.powerLevel = cmd.getAction()[JSON_PARAMETERS][JSON_PARAM_VALUE];
    cmd.setSuccess(_powerLevelCb(cmd.getDeviceId(), tmpState));
    cmd.setHandled(true);
  }

  if (strcmp(cmd.getActionName(), JSON_CMD_ADJUST_POWERLEVEL) == 0 && _powerLevelAdjustCb) {
    int adjust = cmd.getAction()[JSON_PARAMETERS][JSON_PARAM_POWERLEVELDELTA];
    tmpState.powerLevel+= adjust;
    if (tmpState.powerLevel > 100) tmpState.powerLevel = 100;
    if (tmpState.powerLevel < 0) tmpState.powerLevel = 0;
    cmd.setSuccess(_powerLevelAdjustCb(cmd.getDeviceId(), tmpState));
    cmd.setHandled(true);
  }

  if (cmd.getSuccess()) _powerLevelState = tmpState;
  if (cmd.isHandled()) cmd.getResponse()[JSON_DEVICE][JSON_RESULT_POWERLEVEL] = _powerLevelState.powerLevel;
}

#endif
