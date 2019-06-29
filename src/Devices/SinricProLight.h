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
    bool handle(SinricProCommand& cmd) override;
  private:
};

SinricProLight::SinricProLight(const char* deviceId ) :
  SinricProDevice(deviceId, __func__),
  PowerController(),
  BrightnessController(),
  ColorController(),
  ColorTemperatureController() {
}

bool SinricProLight::handle(SinricProCommand& cmd) {
  DEBUG_SINRIC("[SinricProDimSwitch:handle] deviceId=%s, action=%s\r\n", cmd.getDeviceId(), cmd.getActionName());

  handlePowerController(cmd);
  handleBrightnessController(cmd);
  handleColorController(cmd);
  handleColorTemperatureController(cmd);

  return cmd.getSuccess();
}

#endif
