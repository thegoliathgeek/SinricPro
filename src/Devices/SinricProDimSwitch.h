#ifndef __SINRICPRODIMWITCH_H__
#define __SINRICPRODIMWITCH_H__

#include "SinricProDevice.h"
#include "Request/SinricProRequest.h"
#include "Controller/PowerController.h"
#include "Controller/PowerLevelController.h"

class SinricProDimSwitch : public SinricProDevice,
                           public PowerController,
                           public PowerLevelController {
  public:
    SinricProDimSwitch(const char* deviceId );
    bool handle(SinricProCommand& cmd) override;
  private:
};

SinricProDimSwitch::SinricProDimSwitch(const char* deviceId ) :
  SinricProDevice(deviceId, __func__),
  PowerController(),
  PowerLevelController() {
}

bool SinricProDimSwitch::handle(SinricProCommand& cmd) {
  DEBUG_SINRIC("[SinricProDimSwitch:handle] deviceId=%s, action=%s\r\n", cmd.getDeviceId(), cmd.getActionName());

  handlePowerController(cmd);
  handlePowerLevelController(cmd);

  return cmd.getSuccess();
}

#endif
