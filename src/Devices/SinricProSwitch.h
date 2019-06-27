#ifndef __SINRICPROSWITCH_H__
#define __SINRICPROSWITCH_H__

#include "SinricProDevice.h"
#include "Command/SinricProCommand.h"
#include "Controller/PowerController.h"

class SinricProSwitch : public SinricProDevice,
                        public PowerController {
  public:
    SinricProSwitch(const char* deviceId );
    bool handle(SinricProCommand& cmd) override;
};

SinricProSwitch::SinricProSwitch(const char* deviceId ) :
  SinricProDevice(deviceId, __func__),
  PowerController() {
}

bool SinricProSwitch::handle(SinricProCommand& cmd) {
  DEBUG_SINRIC("[SinricProSwitch:handle] deviceId=%s, action=%s\r\n", cmd.getDeviceId(), cmd.getActionName());

  handlePowerController(cmd);

  return cmd.getSuccess();
}

#endif
