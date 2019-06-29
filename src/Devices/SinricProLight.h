#ifndef __SINRICPROLIGHT_H__
#define __SINRICPROLIGHT_H__

#include "SinricProDevice.h"
#include "Request/SinricProRequest.h"
#include "Controller/PowerController.h"
#include "Controller/BrightnessController.h"
#include "Controller/ColorController.h"
#include "Controller/ColorTemperatureController.h"

class SinricProLight : public SinricProDevice,
                       public PowerController,
                       public BrightnessController,
                       public ColorController,
                       public ColorTemperatureController {
  public:
	  SinricProLight(const char* deviceId);
    bool handle(JsonObject& jsonRequest, JsonObject& jsonResponse) override;
  private:
};

SinricProLight::SinricProLight(const char* deviceId ) :
  SinricProDevice(deviceId, __func__),
  PowerController(),
  BrightnessController(),
  ColorController(),
  ColorTemperatureController() {
}

bool SinricProLight::handle(JsonObject& jsonRequest, JsonObject& jsonResponse) {
  DEBUG_SINRIC("[SinricProDimSwitch:handle] deviceId=%s, action=%s\r\n", cmd.getDeviceId(), cmd.getActionName());

  handlePowerController(jsonRequest, jsonResponse);
  handleBrightnessController(jsonRequest, jsonResponse);
  handleColorController(jsonRequest, jsonResponse);
  handleColorTemperatureController(jsonRequest, jsonResponse);

  return jsonResponse["success"];
}

#endif
