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
  void handleColorController(SinricProCommand& cmd);

  void setColorState(colorState& state) { _colorState = state; }
  colorState getColorState() { return _colorState; }
private:
  ColorCallback _colorCb;

  colorState _colorState;
};

void ColorController::handleColorController(SinricProCommand& cmd){
  if (cmd.isHandled()) return;
  colorState tmpState = _colorState;

  if (strcmp(cmd.getActionName(), JSON_CMD_SET_COLOR)==0 && _colorCb) {
    tmpState.r = cmd.getAction()[JSON_PARAMETERS][JSON_PARAM_COLOR][JSON_PARAM_COLOR_R];
    tmpState.g = cmd.getAction()[JSON_PARAMETERS][JSON_PARAM_COLOR][JSON_PARAM_COLOR_G];
    tmpState.b = cmd.getAction()[JSON_PARAMETERS][JSON_PARAM_COLOR][JSON_PARAM_COLOR_B];
    cmd.setSuccess(_colorCb(cmd.getDeviceId(), tmpState));
    cmd.setHandled(true);
  }

  if (cmd.getSuccess()) _colorState = tmpState;

  if (cmd.isHandled()) {
    JsonObject& response = cmd.getResponse();
    response[JSON_DEVICE][JSON_RESULT_COLOR][JSON_RESULT_COLOR_R] = _colorState.r;
    response[JSON_DEVICE][JSON_RESULT_COLOR][JSON_RESULT_COLOR_G] = _colorState.g;
    response[JSON_DEVICE][JSON_RESULT_COLOR][JSON_RESULT_COLOR_B] = _colorState.b;
  }
}

#endif
