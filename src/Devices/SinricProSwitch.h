#ifndef __SINRICPROSWITCH_H__
#define __SINRICPROSWITCH_H__

#include "SinricProDevice.h"
#include "Request/SinricProRequest.h"
#include "Controller/PowerController.h"

class SinricProSwitch : public SinricProDevice,
                        public PowerController {
  public:
    SinricProSwitch(const char* deviceId );
    bool handle(JsonObject& jsonRequest, JsonObject& jsonResponse) override;
};

SinricProSwitch::SinricProSwitch(const char* deviceId ) :
  SinricProDevice(deviceId, __func__),
  PowerController() {
}

bool SinricProSwitch::handle(JsonObject& jsonRequest, JsonObject& jsonResponse) {
  handlePowerController(jsonRequest, jsonResponse);
  return jsonResponse["success"];
}

#endif
