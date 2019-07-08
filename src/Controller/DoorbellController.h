#ifndef __DOORBELLCONTROLLER_H__
#define __DOORBELLCONTROLLER_H__

#define DOORBELL_EVENT "DoorbellPress"

class DoorbellController {
public:
  bool raiseEvent(JsonDocument& jsonEvent);
  void enableDoorbell() { _enabled = true; }
  void disableDoorbell() { _enabled = false; }
  bool isDoorbellEnabled() { return _enabled; }
private:
  bool _enabled = false;
};

bool DoorbellController::raiseEvent(JsonDocument& jsonEvent) {
  if (_enabled) {
    jsonEvent["value"]["state"] = "pressed";
    return true;
  } else {
    return false;
  }
}

#endif
