#ifndef __POWERCONTROLLER_H__
#define __POWERCONTROLLER_H__

#include <ArduinoJson.h>
#include "Command/SinricProCommand.h"

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
  bool handle(SinricProCommand& cmd);

  void setPowerState(powerState& state) { _powerState = state; }
  powerState getPowerState() { return _powerState; }
private:
  PowerStateCallback _powerStateCb;

  powerState _powerState;
};

// To do: deliver SinricProResponse object
// fill response object

bool PowerController::handle(SinricProCommand& cmd) {
  DEBUG_SINRIC("handlePowerController()\r\n");
  powerState tmpState = _powerState;

  if (strcmp(cmd.getActionName(), JSON_CMD_OFF)==0 && _powerStateCb) {
    tmpState.state = false;
    cmd.setSuccess(_powerStateCb(cmd.getDeviceId(), tmpState));
    cmd.setHandled(true);
  }

  if (strcmp(cmd.getActionName(), JSON_CMD_ON)==0 && _powerStateCb) {
    tmpState.state = true;
    cmd.setSuccess(_powerStateCb(cmd.getDeviceId(), tmpState));
    cmd.setHandled(true);
  }

  if (cmd.getSuccess()) _powerState = tmpState;
  if (cmd.isHandled()) cmd.getResponse()[JSON_DEVICE][JSON_RESULT_POWERSTATE] = _powerState.state?"On":"Off";
  return cmd.isHandled();
}

#endif
