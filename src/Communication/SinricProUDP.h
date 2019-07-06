#ifndef __SINRIC_PRO_UDP_H__
#define __SINRIC_PRO_UDP_H__

#include <ESP8266WiFi.h>
#include <WiFiUDP.h>
#include <Request/SinricProRequest.h>
#include <Communication/SinricProQueue.h>

//

class udpListener {
public:
  void begin();
  void handle();
  void sendResponse(String response);
  void stop();
private:
  WiFiUDP _udp;
};

void udpListener::begin() {
  _udp.beginMulticast(WiFi.localIP(), UDP_MULTICAST_IP, UDP_MULTICAST_PORT);
}

void udpListener::handle() {
  int len = _udp.parsePacket();
  if (len) {

    char buffer[512];
    int n = _udp.read(buffer, 512);
    buffer[n] = 0;
    SinricProRequestPayload* request = new SinricProRequestPayload(CS_UDP, buffer);
    DEBUG_SINRIC("[SinricPro:UDP]: receiving request\r\n");
    requestQueue.push(request);
  }
}

void udpListener::sendResponse(String response) {
  _udp.beginPacket(_udp.remoteIP(), _udp.remotePort());
  _udp.write(response.c_str());
  _udp.endPacket();
}

void udpListener::stop() {
  _udp.stop();
}

#endif
