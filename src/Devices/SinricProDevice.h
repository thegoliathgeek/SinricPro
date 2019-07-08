#ifndef __SINRICPRODEVICE_H__
#define __SINRICPRODEVICE_H__

#include <ArduinoJson.h>
#include "Request/SinricProRequest.h"
#include "Events/SinricProEvent.h"
#include "Controller/ColorController.h"
#include "Controller/ColorTemperatureController.h"
#include "Controller/BrightnessController.h"
#include "Controller/PowerController.h"
#include "Controller/PowerLevelController.h"
#include "Controller/DoorbellController.h"

class SinricProDevice : public PowerController,
                        public PowerLevelController,
                        public BrightnessController,
                        public ColorController,
                        public ColorTemperatureController,
                        public DoorbellController {
  public:
    SinricProDevice(const char* deviceId);
    ~SinricProDevice();
    bool handle(JsonDocument& jsonRequest, JsonDocument& jsonResponse);
    bool raiseEvent(JsonDocument& jsonEvent);
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
  if (compare(actionName, POWER_STATE_REQUEST)) return PowerController::handle(jsonRequest, jsonResponse);
  // PowerLevelController
  if (compare(actionName, POWER_LEVEL_REQUEST)) return PowerLevelController::handle(jsonRequest, jsonResponse);
  if (compare(actionName, ADJUST_POWERLEVEL_REQUEST)) return PowerLevelController::handle(jsonRequest, jsonResponse);
  // BrightnessController
  if (compare(actionName, BRIGHTNESS_REQUEST)) return BrightnessController::handle(jsonRequest, jsonResponse);
  if (compare(actionName, ADJUST_BRIGHTNESS_REQUEST)) return BrightnessController::handle(jsonRequest, jsonResponse);
  // ColorController
  if (compare(actionName, COLOR_REQUEST)) return ColorController::handle(jsonRequest, jsonResponse);
  // ColorTemperatureController
  if (compare(actionName, COLORTEMPERATURE_REQUEST)) return ColorTemperatureController::handle(jsonRequest, jsonResponse);
  if (compare(actionName, INCREASE_COLORTEMPERATURE_REQUEST)) return ColorTemperatureController::handle(jsonRequest, jsonResponse);
  if (compare(actionName, DECREASE_COLORTEMPERATURE_REQUEST)) return ColorTemperatureController::handle(jsonRequest, jsonResponse);
  return false; // should only happen if no controller can handle request.
}

bool SinricProDevice::raiseEvent(JsonDocument& jsonEvent) {
  const char* actionName = jsonEvent["action"];

  if (compare(actionName, POWER_STATE_EVENT)) return PowerController::raiseEvent(jsonEvent);
  if (compare(actionName, POWER_LEVEL_EVENT)) return PowerLevelController::raiseEvent(jsonEvent);
  if (compare(actionName, BRIGHTNESS_EVENT)) return BrightnessController::raiseEvent(jsonEvent);
  if (compare(actionName, COLOR_EVENT)) return ColorController::raiseEvent(jsonEvent);
  if (compare(actionName, COLORTEMPERATURE_EVENT)) return ColorTemperatureController::raiseEvent(jsonEvent);
  if (compare(actionName, DOORBELL_EVENT)) return DoorbellController::raiseEvent(jsonEvent);

  return false; // should only happen if controller cant handle event.
}

SinricProDevice::SinricProDevice(const char* deviceId) {
  _deviceId = strdup(deviceId);
}

SinricProDevice::~SinricProDevice() {
  if (_deviceId) free(_deviceId);
}

#endif
