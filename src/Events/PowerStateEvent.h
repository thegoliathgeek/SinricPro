#ifndef __POWERSTATEEVENT_H__
#define __POWERSTATEEVENT_H__

#include "SinricProEvent.h"
#include "Devices/SinricProDevice.h"

class PowerStateEvent : public SinricProEvent {
public:
  PowerStateEvent(SinricProDevice* device, const powerState& state) : SinricProEvent(device), _state(state) {}
  String getJsonEventString() override;
private:
  const powerState _state;
};

String PowerStateEvent::getJsonEventString() {
  DEBUG_SINRIC("[PowerStateEvent]:(%s, %s, %lu)\r\n", _device.getDeviceId(), _state.state?"On":"Off", _timeStamp);

  const size_t capacity = 2*JSON_ARRAY_SIZE(1) + 2*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(5);
  DynamicJsonDocument doc(capacity);

  doc[JSON_EVENT_SUCCESS] = true;
  doc[JSON_EVENT_TIMESTAMP] = _timeStamp;
  doc[JSON_EVENT_DEVICEID] = _device.getDeviceId();
  doc[JSON_EVENT_TYPE] = JSON_EVENT_TYPE_EVENT;

  JsonArray event = doc.createNestedArray(JSON_EVENT_TYPE_EVENT);

  JsonObject event_0 = event.createNestedObject();
  event_0[JSON_EVENT_TYPE] = JSON_EVENT_TYPE_POWEREVENT;

  JsonArray event_0_values = event_0.createNestedArray(JSON_EVENT_VALUES);

  JsonObject event_0_values_0 = event_0_values.createNestedObject();
  event_0_values_0[JSON_EVENT_VALUE_NAME] = JSON_EVENT_VALUE_POWERSTATE;
  event_0_values_0[JSON_EVENT_VALUE] = _state.state?JSON_EVENT_VALUE_POWERSTATE_ON:JSON_EVENT_VALUE_POWERSTATE_OFF;

  String tmpString;
  serializeJson(doc, tmpString);

  return tmpString;
}

#endif
