#ifndef __SINRICPRO_H__
#define __SINRICPRO_H__

#include <WebSocketsClient.h>

#include <ArduinoJson.h>
#include <vector>

#include <SinricProDebug.h>
#include <SinricProConfig.h>
#include "Devices/SinricProDevice.h"
#include "Communication/SinricProQueue.h"
#include "Communication/SinricProWebsocket.h"
#include "Request/SinricProRequest.h"
#include "Request/JsonRequests.h"
#include "Events/SinricProEvent.h"

class SinricPro {
  public:
    SinricPro();
    ~SinricPro();

    void begin(const char* api_key);
    void begin(const String& api_key);

    void handle();
    void handleRequest();
    void handleResponse();

    void stop();
    bool isConnected();

    template <typename DeviceType>
    DeviceType& add(const char* deviceId);

    template <typename DeviceType>
    DeviceType& add(const String& deviceId);

    boolean remove(const char* deviceId);
    boolean remove(const String& deviceId);

    unsigned long getTimestamp() { return _baseTS + (millis() / 1000); }
    void syncTimestamp(unsigned long ts) { _baseTS = ts-(millis()/1000); }

    void raiseEvent(SinricProEvent& event);
    SinricProDevice& operator[] (int index)  { return *devices[index]; }
    SinricProDevice* operator[] (const char* deviceId);
    std::vector<SinricProDevice*>::iterator begin() { return devices.begin(); }
    std::vector<SinricProDevice*>::iterator end() { return devices.end(); }
    int size() { return devices.size(); }

  private:
    const char* _api_key;

    websocketListener _websocketListener;
    std::vector<SinricProDevice*> devices;
    unsigned long _baseTS;
};

SinricPro::SinricPro() {};

SinricPro::~SinricPro() {
  stop();

  for (auto& device : devices) {
    delete device;
    device = nullptr;
  }
  devices.clear();

  if (_api_key) delete _api_key;
}

void SinricPro::begin(const char* api_key) {
  _api_key = strdup(api_key);

  String deviceIds;
  for (auto& device : devices) {
    deviceIds += String(device->getDeviceId()) + ";";
  }
  _websocketListener.begin(api_key, deviceIds.c_str());
}

void SinricPro::begin(const String& api_key) {
  begin(api_key.c_str());
}

void SinricPro::handle() {
  _websocketListener.handle();
  handleRequest();
}

void prepareResponse(const JsonObject& jsonRequest, JsonObject& jsonResponse, unsigned long ts) {
  jsonResponse["payloadVersion"] = 1;
  jsonResponse["success"] = false;
  jsonResponse["message"] = "OK";
  jsonResponse["ts"] = ts;
  jsonResponse["did"] = jsonRequest["did"];
  jsonResponse["type"] = "response";
  jsonResponse["action"] = jsonRequest["action"];
  jsonResponse["value"] = jsonRequest["value"];
}

void SinricPro::handleRequest() {
  if (requestQueue.count() > 0) {
    DEBUG_SINRIC("[SinricPro.handleRequest()]: %i requests in queue\r\n", requestQueue.count());
    // POP requests and call device.handle() for each related device
    while (requestQueue.count() > 0) {
      SinricProRequestPayload* requestPayload = requestQueue.pop();

      // decode jsonRequest
      DynamicJsonDocument jsonRequestDoc(512);
      DeserializationError err = deserializeJson(jsonRequestDoc, requestPayload->getRequest());

      if (err) {
        DEBUG_SINRIC("[SinricPro.handleRequest()]: Error (%s) while parsing json request\r\n", err.c_str());
        return;
      }

      JsonObject jsonRequest = jsonRequestDoc.as<JsonObject>();

      if (jsonRequest.containsKey("did") && jsonRequest.containsKey("action")) {
        if (jsonRequest.containsKey("ts")) {
          syncTimestamp(jsonRequest["ts"].as<unsigned long int>());
        }

        for (auto& device : devices) {
          if (strcmp(device->getDeviceId(), jsonRequest["did"]) == 0) {
              DynamicJsonDocument jsonResponseDoc(512);
              JsonObject jsonResponse = jsonResponseDoc.as<JsonObject>();

              prepareResponse(jsonRequest, jsonResponse, getTimestamp());
              device->handle(jsonRequest, jsonResponse);

              if (jsonResponse["success"]) {
                String responseString;
                serializeJsonPretty(jsonResponse, responseString);

                switch (requestPayload->getRequestSource()) {
                  case CS_WEBSOCKET:
                    _websocketListener.sendResponse(responseString);
                    break;
                  default:
                    break;
                }
              }
          }
        }
      }
      delete requestPayload;
    }
  }
}

void SinricPro::stop() {
  _websocketListener.stop();
}

bool SinricPro::isConnected() {
  return _websocketListener.isConnected();
};

template <typename DeviceType>
DeviceType& SinricPro::add(const char* deviceId) {
  DeviceType* newDevice = new DeviceType(deviceId);
  DEBUG_SINRIC("[SinricPro]: Add device %s (%s)\r\n", deviceId, newDevice->getDeviceName());

  devices.push_back(newDevice);
  return *newDevice;
}

template <typename DeviceType>
DeviceType& SinricPro::add(const String& deviceId) {
  return add<DeviceType>(deviceId.c_str());
}

boolean SinricPro::remove(const char* deviceId) {
  boolean success = false;

// To do: Change this for removing device...
  for (auto& device : devices) {
    if (strcmp(device->getDeviceId(), deviceId) == 0) {
      DEBUG_SINRIC("[SinricPro:remove]: Removing device %s\r\n", deviceId);
      success = true;
    }
  }
// To do: end of To do
  if (!success) DEBUG_SINRIC("[SinricPro:remove]: No corresponding device found (id:%s)\r\n", deviceId);
  return success;
}

boolean SinricPro::remove(const String& deviceId) {
  return remove(deviceId.c_str());
}

void SinricPro::raiseEvent(SinricProEvent& event) {
  event.setTS(getTimestamp());
  String tmpString = event.getJsonEventString();
  DEBUG_SINRIC("[SinricPro:raiseEvent]: \r\n%s\r\n", tmpString.c_str());
  _websocketListener.sendEvent(tmpString);
}

SinricProDevice* SinricPro::operator[] (const char* deviceId) {
  for (auto& device : devices) {
    if (strcmp(device->getDeviceId(), deviceId) == 0) return device;
  }
  return nullptr;
}


#ifndef SINRIC_NOINSTANCE
SinricPro SinricPro;
#endif

#endif
