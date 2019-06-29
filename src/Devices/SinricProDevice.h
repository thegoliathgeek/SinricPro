#ifndef __SINRICPRODEVICE_H__
#define __SINRICPRODEVICE_H__

#include <ArduinoJson.h>
#include "Request/SinricProRequest.h"

class SinricProDevice {
  public:
    SinricProDevice(const char* deviceId, const char* deviceName);
    virtual ~SinricProDevice();
    virtual bool handle(JsonObject& jsonRequest, JsonObject& jsonResponse) { return false; }
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

#endif
