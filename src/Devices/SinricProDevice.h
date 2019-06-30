#ifndef __SINRICPRODEVICE_H__
#define __SINRICPRODEVICE_H__

#include <ArduinoJson.h>
#include "Command/SinricProCommand.h"
#include "Controller/ColorController.h"
#include "Controller/ColorTemperatureController.h"
#include "Controller/BrightnessController.h"
#include "Controller/PowerController.h"
#include "Controller/PowerLevelController.h"
/*
class SinricProDevice {
  public:
    SinricProDevice(const char* deviceId, const char* deviceName);
    virtual ~SinricProDevice();
    virtual bool handle(SinricProCommand& cmd) { return false; }
    const char* getDeviceId() { return _deviceId; }
    const char* getDeviceName() { return _deviceName; }
  protected:
    char* _deviceId;
    const char* _deviceName;
};

SinricProDevice::SinricProDevice(const char* deviceId, const char* deviceName) :
  _deviceName(deviceName) {
  _deviceId = strdup(deviceId);
}

SinricProDevice::~SinricProDevice() {
  if (_deviceId) free(_deviceId);
}
*/

class SinricProDevice : public PowerController,
                        public PowerLevelController,
                        public BrightnessController,
                        public ColorController,
                        public ColorTemperatureController{
  public:
    SinricProDevice(const char* deviceId);
    ~SinricProDevice();
    bool handle(SinricProCommand& cmd);
    const char* getDeviceId() { return _deviceId; }
  protected:
    char* _deviceId;
};

SinricProDevice::SinricProDevice(const char* deviceId) : PowerController(),
                                                         PowerLevelController(),
                                                         BrightnessController(),
                                                         ColorController(),
                                                         ColorTemperatureController() {
  _deviceId = strdup(deviceId);
}

bool compare(const char* a, const char *b) {
  return strcmp(a, b) == 0;
}

bool SinricProDevice::handle(SinricProCommand& cmd) {
  const char* actionName = cmd.getActionName();
  if (compare(actionName, JSON_CMD_ON)) return PowerController::handle(cmd);
  if (compare(actionName, JSON_CMD_OFF)) return PowerController::handle(cmd);
  if (compare(actionName, JSON_CMD_SET_POWERLEVEL)) return PowerLevelController::handle(cmd);
  if (compare(actionName, JSON_CMD_ADJUST_POWERLEVEL)) return PowerLevelController::handle(cmd);
  if (compare(actionName, JSON_CMD_SET_BRIGHTNESS)) return BrightnessController::handle(cmd);
  if (compare(actionName, JSON_CMD_ADJUST_BRIGHTNESS)) return BrightnessController::handle(cmd);
  if (compare(actionName, JSON_CMD_SET_COLOR)) return ColorController::handle(cmd);
  if (compare(actionName, JSON_CMD_SET_COLOR_TEMPERATURE)) return ColorTemperatureController::handle(cmd);
  if (compare(actionName, JSON_CMD_INCREASE_COLOR_TEMPERATURE)) return ColorTemperatureController::handle(cmd);
  if (compare(actionName, JSON_CMD_DECREASE_COLOR_TEMPERATURE)) return ColorTemperatureController::handle(cmd);
  return false; // should only happen if no controller can handle it...
}

SinricProDevice::~SinricProDevice() {
  if (_deviceId) free(_deviceId);
}
#endif
