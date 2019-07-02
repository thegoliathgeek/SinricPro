#ifndef __SINRICPRODEVICE_H__
#define __SINRICPRODEVICE_H__

#include <ArduinoJson.h>
#include "Request/SinricProRequest.h"
#include "Controller/ColorController.h"
#include "Controller/ColorTemperatureController.h"
#include "Controller/BrightnessController.h"
#include "Controller/PowerController.h"
#include "Controller/PowerLevelController.h"

class SinricProDevice : public PowerController,
                        public PowerLevelController,
                        public BrightnessController,
                        public ColorController,
                        public ColorTemperatureController{
  public:
    SinricProDevice(const char* deviceId);
    ~SinricProDevice();
    bool handle(JsonDocument& jsonRequest, JsonDocument& jsonResponse);
    const char* getDeviceId() { return _deviceId; }
  protected:
    char* _deviceId;
};

bool compare(const char* a, const char *b) {
  return strcmp(a, b) == 0;
}

bool SinricProDevice::handle(JsonDocument& jsonRequest, JsonDocument& jsonResponse) {
  const char* actionName = jsonRequest["action"];
  // PowerController
  if (compare(actionName, "setPowerState")) return PowerController::handle(jsonRequest, jsonResponse);
  // PowerLevelController
  if (compare(actionName, "setPowerLevel")) return PowerLevelController::handle(jsonRequest, jsonResponse);
  if (compare(actionName, "adjustPowerLevel")) return PowerLevelController::handle(jsonRequest, jsonResponse);
  // BrightnessController
  if (compare(actionName, "setBrightness")) return BrightnessController::handle(jsonRequest, jsonResponse);
  if (compare(actionName, "adjustBrightness")) return BrightnessController::handle(jsonRequest, jsonResponse);
  // ColorController
  if (compare(actionName, "setColor")) return ColorController::handle(jsonRequest, jsonResponse);
  // ColorTemperatureController
  if (compare(actionName, "setColorTemperature")) return ColorTemperatureController::handle(jsonRequest, jsonResponse);
  if (compare(actionName, "increaseColorTemperature")) return ColorTemperatureController::handle(jsonRequest, jsonResponse);
  if (compare(actionName, "decreaseColorTemperature")) return ColorTemperatureController::handle(jsonRequest, jsonResponse);
  return false; // should only happen if no controller can handle it...
}

SinricProDevice::SinricProDevice(const char* deviceId) {
  _deviceId = strdup(deviceId);
}

SinricProDevice::~SinricProDevice() {
  if (_deviceId) free(_deviceId);
}

#endif
