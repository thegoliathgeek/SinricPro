#ifndef __COLORCONTROLLER_H__
#define __COLORCONTROLLER_H__

#include <ArduinoJson.h>
#include "Request/SinricProRequest.h"

struct colorState {
  int r;
  int g;
  int b;
  colorState(int r=0, int b=0, int g=0) : r(r), g(g), b(b) {}
  void setRGB(int newR, int newG, int newB) { r=newR; g=newG, b=newB; }
  void getRGB(int &otherR, int &otherG, int &otherB) { otherR=r; otherG=g, otherB=b; }
  int getR() { return r; }
  int getG() { return g; }
  int getB() { return b; }
};

class ColorController {
public:
  ColorController() : _colorCb(nullptr) {}
  typedef std::function<bool(const char*, colorState&)> ColorCallback;
  void onSetColor(ColorCallback callback) { _colorCb = callback; }

  bool handle(JsonDocument& jsonRequest, JsonDocument& jsonResponse);
  bool raiseEvent(JsonDocument& jsonEvent);

  void setColorState(colorState& state) { _colorState = state; }
  colorState& getColorState() { return _colorState; }
private:
  ColorCallback _colorCb;

  colorState _colorState;
};

bool ColorController::handle(JsonDocument& jsonRequest, JsonDocument& jsonResponse) {
  bool success(false);
  colorState tempState = _colorState;

  const char* deviceId = jsonRequest["deviceId"];
  const char* action = jsonRequest["action"];

  // setColor request
  if (strcmp(action, "setColor")== 0) {
    tempState.r = jsonRequest["value"]["color"]["r"];
    tempState.g = jsonRequest["value"]["color"]["g"];
    tempState.b = jsonRequest["value"]["color"]["b"];

    if (_colorCb) success = _colorCb(deviceId, tempState);
  }

  if (success) _colorState = tempState;
  jsonResponse["value"].createNestedObject("color");
  jsonResponse["value"]["color"]["r"] = _colorState.r;
  jsonResponse["value"]["color"]["g"] = _colorState.g;
  jsonResponse["value"]["color"]["b"] = _colorState.b;
  return success;
}

bool ColorController::raiseEvent(JsonDocument& jsonEvent) {
  jsonEvent["value"].createNestedObject("color");
  jsonEvent["value"]["color"]["r"] = _colorState.r;
  jsonEvent["value"]["color"]["g"] = _colorState.g;
  jsonEvent["value"]["color"]["b"] = _colorState.b;
  return true;
}

#endif
