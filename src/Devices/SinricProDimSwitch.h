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
    bool handle(JsonObject& jsonRequest, JsonObject& jsonResponse) override;
  private:
};

SinricProDimSwitch::SinricProDimSwitch(const char* deviceId ) :
  SinricProDevice(deviceId, __func__),
  PowerController(),
  PowerLevelController() {
}

bool SinricProDimSwitch::handle(JsonObject& jsonRequest, JsonObject& jsonResponse) {
  handlePowerController(jsonRequest, jsonResponse);
  handlePowerLevelController(jsonRequest, jsonResponse);

  return jsonResponse["success"];
}

#endif
