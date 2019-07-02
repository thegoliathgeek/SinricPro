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
  typedef std::function<bool(const char*, colorState)> ColorCallback;
  void onSetColor(ColorCallback callback) { _colorCb = callback; }

  bool handle(JsonDocument& jsonRequest, JsonDocument& jsonResponse);

  void setColorState(colorState& state) { _colorState = state; }
  colorState getColorState() { return _colorState; }
private:
  ColorCallback _colorCb;

  colorState _colorState;
};

bool ColorController::handle(JsonDocument& jsonRequest, JsonDocument& jsonResponse) {
  bool success(false);
  return success;
}

#endif
