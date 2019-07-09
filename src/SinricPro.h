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
#include "Communication/SinricProUDP.h"
#include "Communication/NTP.h"
#include "Request/SinricProRequest.h"
//#include "Events/SinricProEvent.h"

class SinricPro {
  public:
    typedef std::vector<SinricProDevice*> SinricProDeviceList;
    SinricPro();
    ~SinricPro();

    void begin(const char* api_key);
    void begin(const String& api_key);

    void handle();
    void handleRequest();
    void handleResponse();

    void stop();
    bool isConnected();

    SinricProDevice& add(const char* deviceId);
    SinricProDevice& add(const String& deviceId);

    SinricProDevice* getDevice(const char* deviceId, bool autoCreate = true);
    SinricProDevice* getDevice(const String& deviceId, bool autoCreate = true);

    boolean remove(const char* deviceId);
    boolean remove(const String& deviceId);

//    unsigned long getTimestamp() { return _baseTS + (millis() / 1000); }
    unsigned long getTimestamp() { return _NTP.getTimestamp(); }

//    void syncTimestamp(unsigned long ts) { _baseTS = ts-(millis()/1000); }

    void raiseEvent(const char* deviceId, const char* action, const char* cause);
//    SinricProDevice& operator[] (int index)  { return *devices[index]; } removed for security reasons
    SinricProDevice& operator[] (const char* deviceId);
    SinricProDevice& operator[] (const String& deviceId);

    SinricProDeviceList::iterator begin() { return devices.begin(); }
    SinricProDeviceList::iterator end() { return devices.end(); }
    int size() { return devices.size(); }

  private:
    void prepareResponse(const JsonDocument& jsonRequest, JsonDocument& jsonResponse, unsigned long createdAt);
    const char* _api_key;

    websocketListener _websocketListener;
    udpListener _udpListener;
    SinricProDeviceList devices;
    unsigned long _baseTS;
    myNTP _NTP;
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
  _udpListener.begin();
  _NTP.begin();
}

void SinricPro::begin(const String& api_key) {
  begin(api_key.c_str());
}

void SinricPro::handle() {
  _websocketListener.handle();
  _udpListener.handle();
  handleRequest();
  _NTP.update();
}

void SinricPro::prepareResponse(const JsonDocument& jsonRequest, JsonDocument& jsonResponse, unsigned long createdAt) {
  jsonResponse["payloadVersion"] = 1;
  jsonResponse["success"] = false;
  jsonResponse["message"] = "OK";
  jsonResponse["createdAt"] = createdAt;
  jsonResponse["clientId"] = jsonRequest["clientId"];
  jsonResponse["messageId"] = jsonRequest["messageId"];
  jsonResponse["deviceId"] = jsonRequest["deviceId"];
  jsonResponse["type"] = "response";
  jsonResponse["action"] = jsonRequest["action"];
  jsonResponse.createNestedObject("value");
}

void SinricPro::handleRequest() {

  if (requestQueue.count() > 0) {
    DEBUG_SINRIC("[SinricPro.handleRequest()]: %i requests in queue\r\n", requestQueue.count());
    // POP requests and call device.handle() for each related device
    while (requestQueue.count() > 0) {
      SinricProRequestPayload* requestPayload = requestQueue.pop();
      DynamicJsonDocument jsonRequest(512);
      DeserializationError err = deserializeJson(jsonRequest, requestPayload->getRequest());


      if (err) {
        DEBUG_SINRIC("[SinricPro.handleRequest()]: Error (%s) while parsing json request\r\n", err.c_str());
        return;
      }

      String jsonRequestPretty;
      serializeJsonPretty(jsonRequest, jsonRequestPretty);
      DEBUG_SINRIC("[SinricPro.handleRequest()]: request:\r\n%s\r\n",jsonRequestPretty.c_str());

      long createdAt = jsonRequest["createdAt"]; // 1562001822
      const char* deviceId = jsonRequest["deviceId"]; // "5d12df23eb7e894a699e0ae8"

//      syncTimestamp(createdAt);

      SinricProDevice* device = getDevice(deviceId, false); // get device

      if (device) {
        DynamicJsonDocument jsonResponse(512);
        prepareResponse(jsonRequest, jsonResponse, getTimestamp());
        jsonResponse["success"] = device->handle(jsonRequest, jsonResponse);

        String responseString;
        serializeJsonPretty(jsonResponse, responseString);
        DEBUG_SINRIC("[SinricPro.handleRequest()]: response:\r\n%s\r\n", responseString.c_str());

        switch (requestPayload->getRequestSource()) {
          case CS_WEBSOCKET: _websocketListener.sendResponse(responseString); break;
          case CS_UDP:       _udpListener.sendResponse(responseString); break;
          default:           break;
        }
      } else {
        DEBUG_SINRIC("[SinricPro.handleCommand()]: Device \"%s\" doesn't exist!\r\n", deviceId);
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

SinricProDevice& SinricPro::add(const char* deviceId) {
  DEBUG_SINRIC("[SinricPro]: Add device %s\r\n", deviceId);

  SinricProDevice* newDevice = new SinricProDevice(deviceId);
  devices.push_back(newDevice);
  return *newDevice;
}

SinricProDevice& SinricPro::add(const String& deviceId) {
  return add(deviceId.c_str());
}

SinricProDevice* SinricPro::getDevice(const char* deviceId, bool autoCreate) {
  for (auto& device : devices) {
    if (strcmp(device->getDeviceId(), deviceId) == 0) return device;
  }

  if (autoCreate) {
    DEBUG_SINRIC("[SinricPro]: Add device %s\r\n", deviceId);
    SinricProDevice* newDevice = new SinricProDevice(deviceId);
    devices.push_back(newDevice);
    return newDevice;
  } else return nullptr;

  // if there is no device with deviceId, create a new one
}

SinricProDevice* SinricPro::getDevice(const String& deviceId, bool autoCreate) {
  return getDevice(deviceId.c_str(), autoCreate);
}

SinricProDevice& SinricPro::operator[] (const char* deviceId) {
  return *getDevice(deviceId);
}

SinricProDevice& SinricPro::operator[](const String& deviceId) {
  return *getDevice(deviceId);
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

void SinricPro::raiseEvent(const char* deviceId, const char* action, const char* type = "PHYSICAL_INTERACTION") {
  DynamicJsonDocument jsonEvent(512);
  jsonEvent["payloadVersion"] = 1;
  jsonEvent["createdAt"] = getTimestamp();
  jsonEvent["deviceId"] = deviceId;
  jsonEvent["type"] = "event";
  jsonEvent["action"] = action;
  jsonEvent.createNestedObject("value");
  jsonEvent.createNestedObject("cause");
  jsonEvent["cause"]["type"] = type;

  SinricProDevice* device = getDevice(deviceId, false);
  if (device) {
    if (device->raiseEvent(jsonEvent)) {
      String jsonEventStr;
      serializeJsonPretty(jsonEvent, jsonEventStr);
      DEBUG_SINRIC("[SinricPro.raiseEvent()]:\r\n%s\r\n", jsonEventStr.c_str());
      _websocketListener.sendEvent(jsonEventStr);
    } else {
      DEBUG_SINRIC("Event %s not allowed!\r\n", action);
    }
  } else {
    DEBUG_SINRIC("Device %s doesn't exist!\r\n", deviceId);
  }
}

#ifndef SINRIC_NOINSTANCE
SinricPro SinricPro;
#endif

#endif
