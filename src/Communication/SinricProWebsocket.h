
#ifndef _SINRICPRO_WEBSOCKET_H__
#define _SINRICPRO_WEBSOCKET_H__

#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include "SinricProConfig.h"
#include "Request/SinricProRequest.h"
#include "Communication/SinricProQueue.h"

class websocketListener
{
  public:
    typedef std::function<void(void)> wsConnectedCallback;
    typedef std::function<void(void)> wsDisconnectedCallback;

    websocketListener();
    ~websocketListener();

    void begin(String apikey, String deviceIds);
    void handle();
    void stop();
    bool isConnected() { return _isConnected; }

    void sendResponse(String& response);
    void sendEvent(String& event) { sendResponse(event); }

    void onConnected(wsConnectedCallback callback) { _wsConnectedCb = callback; }
    void onDisconnected(wsDisconnectedCallback callback) { _wsDisconnectedCb = callback; }
  private:
    bool _isConnected = false;

    WebSocketsClient webSocket;

    wsConnectedCallback _wsConnectedCb;
    wsDisconnectedCallback _wsDisconnectedCb;

    void webSocketEvent(WStype_t type, uint8_t * payload, size_t length);
};

websocketListener::websocketListener() : _isConnected(false) {}

websocketListener::~websocketListener() {
  stop();
}

void websocketListener::begin(String apikey, String deviceIds) {

    DEBUG_SINRIC("[SinricPro:Websocket]: Conecting to WebSocket Server\r\n");

    if (_isConnected) {
        stop();
    }

    String headers = "deviceids:" + deviceIds;
    DEBUG_SINRIC("[SinricPro:Websocket]: headers: \"%s\"\r\n", headers.c_str());

    webSocket.setExtraHeaders(headers.c_str());
    webSocket.begin(SERVER_URL, SERVER_PORT, "/"); // server address, port and URL
    webSocket.onEvent([&](WStype_t type, uint8_t * payload, size_t length) { webSocketEvent(type, payload, length); });
    webSocket.setAuthorization(apikey.c_str()); //

    webSocket.setReconnectInterval(WEBSOCKET_CONNECTION_TIMEOUT);
    webSocket.enableHeartbeat(WEBSOCKET_PING_INTERVAL, WEBSOCKET_PING_TIMEOUT, WEBSOCKET_RETRY_COUNT);

}

void websocketListener::handle() {
  webSocket.loop();
}

void websocketListener::stop() {
  if (_isConnected) {
    webSocket.disconnect();
    _isConnected = false;
  }
}

void websocketListener::sendResponse(String& response) {
  webSocket.sendTXT(response);
}


void websocketListener::webSocketEvent(WStype_t type, uint8_t * payload, size_t length)
{
  switch (type) {
    case WStype_DISCONNECTED:
      _isConnected = false;
      DEBUG_SINRIC("[SinricPro:Websocket]: disconnected\r\n");
      if (_wsDisconnectedCb) _wsDisconnectedCb();
      break;
    case WStype_CONNECTED:
      _isConnected = true;
      DEBUG_SINRIC("[SinricPro:Websocket]: connected\r\n");
      if (_wsConnectedCb) _wsConnectedCb();
      break;
    case WStype_TEXT: {
      SinricProRequestPayload* request = new SinricProRequestPayload(CS_WEBSOCKET, (char*)payload);
      DEBUG_SINRIC("[SinricPro:Websocket]: receiving request\r\n");
      requestQueue.push(request);
      break;
    }
    default: break;
  }
}

#endif
