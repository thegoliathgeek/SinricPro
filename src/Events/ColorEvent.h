#ifndef __COLOREVENT_H__
#define __COLOREVENT_H__

#include "SinricProEvent.h"
#include "Devices/SinricProDevice.h"
#include "Controller/ColorController.h"

class ColorEvent : public SinricProEvent {
public:
  ColorEvent(SinricProDevice* device, const colorState& state) : SinricProEvent(device), _state(state) {}
  String getJsonEventString() override;
private:
  const colorState _state;
};

String ColorEvent::getJsonEventString() {
  const size_t capacity = 2*JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(2) + 2*JSON_OBJECT_SIZE(4);
  DynamicJsonDocument doc(capacity);

  doc[JSON_EVENT_SUCCESS] = true;
  doc[JSON_EVENT_TIMESTAMP] = _timeStamp;
  doc[JSON_EVENT_DEVICEID] = _device.getDeviceId();
  doc[JSON_EVENT_TYPE] = JSON_EVENT_TYPE_EVENT;

  JsonArray event = doc.createNestedArray(JSON_EVENT_TYPE_EVENT);

  JsonObject event_0 = event.createNestedObject();
  event_0[JSON_EVENT_TYPE] = JSON_EVENT_TYPE_COLOREVENT;

  JsonArray event_0_values = event_0.createNestedArray(JSON_EVENT_VALUES);
/*
  JsonObject event_0_values_0 = event_0_values.createNestedObject();
  event_0_values_0[JSON_EVENT_VALUE_NAME] = JSON_EVENT_VALUE_BRIGHTNESS;
  event_0_values_0[JSON_EVENT_VALUE] = _state.brightness;
*/
  JsonObject event_0_values_0 = event_0_values.createNestedObject();
  event_0_values_0[JSON_EVENT_VALUE_NAME] = JSON_EVENT_VALUE_COLOR;
  event_0_values_0[JSON_EVENT_VALUE_R] = _state.r;
  event_0_values_0[JSON_EVENT_VALUE_G] = _state.g;
  event_0_values_0[JSON_EVENT_VALUE_B] = _state.b;

  String tmpString;
  serializeJson(doc, tmpString);

  return tmpString;
}



#endif
