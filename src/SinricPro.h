#ifndef __SINRICPRO_H__
#define __SINRICPRO_H__

#include <WebSocketsClient.h>

#include <ArduinoJson.h>
#include <vector>

#include <SinricProDebug.h>
#include "Command/JsonCommands.h"
#include <SinricProConfig.h>
#include "Devices/SinricProDevice.h"
#include "Communication/SinricProWebsocket.h"
#include "Command/SinricProCommand.h"
#include "Command/SinricProQueue.h"
#include "Events/SinricProEvent.h"

class SinricPro {
  public:
    SinricPro();
    ~SinricPro();

    void begin(const char* api_key);
    void begin(const String& api_key);

    void handle();
    void handleCommand();
    void handleResponse();

    void stop();
    bool isConnected();

    template <typename DeviceType>
    DeviceType& add(const char* deviceId);

    template <typename DeviceType>
    DeviceType& add(const String& deviceId);

    boolean remove(const char* deviceId);
    boolean remove(const String& deviceId);

    unsigned long getTS() { return _baseTS + (millis() / 1000); }
    void setBaseTS(unsigned long ts) { _baseTS = ts-(millis()/1000); }

    void raiseEvent(SinricProEvent& event);
    SinricProDevice& operator[] (int index)  { return *devices[index]; }
    SinricProDevice* operator[] (const char* deviceId);
    std::vector<SinricProDevice*>::iterator begin() { return devices.begin(); }
    std::vector<SinricProDevice*>::iterator end() { return devices.end(); }
    int size() { return devices.size(); }

  private:
    const char* _api_key;

    wsRequestListener _wsCommandListener;
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
  _wsCommandListener.begin(api_key, deviceIds.c_str());
}

void SinricPro::begin(const String& api_key) {
  begin(api_key.c_str());
}

void SinricPro::handle() {
  _wsCommandListener.handle();
  handleCommand();
}

void SinricPro::handleCommand() {
  if (requestQueue.count() > 0) {
    DEBUG_SINRIC("[SinricPro.handleCommand()]: %i commands in queue\r\n", commandQueue.count());
    // POP commands and call device.handle() for each related device
    while (requestQueue.count() > 0) {
      SinricProRequestPayload* request = requestQueue.pop();
      DynamicJsonDocument doc(512);
      DeserializationError err = deserializeJson(doc, request->getRequest());

      if (err) {
        DEBUG_SINRIC("[SinricPro.handleCommand()]: Error (%s) while parsing json command\r\n", err.c_str());
        break;
      }

      if (doc.containsKey(JSON_DEVICEID) && doc.containsKey(JSON_ACTIONS)) {
        if (doc.containsKey(JSON_TIMESTAMP)) {
          unsigned long ts = doc[JSON_TIMESTAMP];
          setBaseTS(ts);
        }

        const char* deviceId = doc[JSON_DEVICEID];
        JsonArray actions = doc[JSON_ACTIONS].as<JsonArray>();

        DynamicJsonDocument response(512);

        // pre-build json response structure
        response[JSON_SUCCESS] = true;
        response[JSON_DEVICEID] = deviceId;
        response[JSON_TIMESTAMP] = getTS();
        response[JSON_RESPONSE_TYPE] = JSON_ACTION_RESPONSE;
        JsonArray actionResults = response.createNestedArray(JSON_RESPONSE_RESULTS);
        // end of pre-build

        for (auto& device : devices) {
          if (strcmp(device->getDeviceId(), deviceId) == 0) {
            for (JsonObject action : actions) {

              JsonObject actionResult = actionResults.createNestedObject();
              actionResult[JSON_ACTION] = action[JSON_ACTION_NAME];
              SinricProCommand jsonCommand(deviceId, action, actionResult);
              DEBUG_SINRIC("[SinricPro.handleCommand()]: Found corresponding device...calling device.handle() routine\r\n");
              // pre-build device response structure
              actionResult[JSON_MESSAGE] = JSON_MESSAGE_OK;
              actionResult[JSON_SUCCESS] = false;
              actionResult.createNestedObject(JSON_DEVICE);
              // pre-build device response structure
              device->handle(jsonCommand);

              if (jsonCommand.getSuccess()) {
                actionResult[JSON_MESSAGE] = JSON_MESSAGE_OK;
                actionResult[JSON_SUCCESS] = true;
              } else {
                DEBUG_SINRIC("[SinricPro.handleCommand()]: device.handle() returned: %s\r\n\r\n", jsonCommand.getSuccess()?"true":"false");
                actionResult[JSON_MESSAGE] = JSON_MESSAGE_OK;
                actionResult[JSON_SUCCESS] = false;
              }
            }
          }
        }

        String responseString;
        serializeJsonPretty(response, responseString);

        switch (request->getRequestSource()) {
          case CS_WEBSOCKET:
            _wsCommandListener.sendResponse(responseString);
            break;
          default:
            break;
        }
        DEBUG_SINRIC("\r\n");
      } else {
        DEBUG_SINRIC("[SinricPro.handleCommand()]: Error! command doesn't contain deviceId and/or action\r\n");
      }
      delete request;
    }
  }
}

void SinricPro::stop() {
  _wsCommandListener.stop();
}

bool SinricPro::isConnected() {
  return _wsCommandListener.isConnected();
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
  event.setTS(getTS());
  String tmpString = event.getJsonEventString();
  DEBUG_SINRIC("[SinricPro:raiseEvent]: \r\n%s\r\n", tmpString.c_str());
  _wsCommandListener.sendEvent(tmpString);
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
