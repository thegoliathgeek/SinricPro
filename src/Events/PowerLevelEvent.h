#ifndef __POWERLEVELEVENT_H__
#define __POWERLEVELEVENT_H__

#include "SinricProEvent.h"
#include "Devices/SinricProDevice.h"
#include "Controller/PowerLevelController.h"

class PowerLevelEvent : public SinricProEvent {
public:
  PowerLevelEvent(SinricProDevice* device, const powerLevelState& state) : SinricProEvent(device), _state(state) {}
  String getJsonEventString() override;
private:
  const powerLevelState _state;
};

String PowerLevelEvent::getJsonEventString() {
  const size_t capacity = 2*JSON_ARRAY_SIZE(1) + 2*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(5);
  DynamicJsonDocument doc(capacity);

  doc[JSON_EVENT_SUCCESS] = true;
  doc[JSON_EVENT_TIMESTAMP] = _timeStamp;
  doc[JSON_EVENT_DEVICEID] = _device.getDeviceId();
  doc[JSON_EVENT_TYPE] = JSON_EVENT_TYPE_EVENT;

  JsonArray event = doc.createNestedArray(JSON_EVENT_TYPE_EVENT);

  JsonObject event_0 = event.createNestedObject();
  event_0[JSON_EVENT_TYPE] = JSON_EVENT_TYPE_POWERLEVELEVENT;

  JsonArray event_0_values = event_0.createNestedArray(JSON_EVENT_VALUES);

  JsonObject event_0_values_0 = event_0_values.createNestedObject();
  event_0_values_0[JSON_EVENT_VALUE_NAME] = JSON_EVENT_VALUE_POWERLEVEL;
  event_0_values_0[JSON_EVENT_VALUE] = _state.powerLevel;

  String tmpString;
  serializeJson(doc, tmpString);

  return tmpString;
}



#endif
