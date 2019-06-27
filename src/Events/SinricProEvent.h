#ifndef __EVENT_H__
#define __EVENT_H__

#include "JsonEvents.h"
#include "Devices/SinricProDevice.h"

class SinricProEvent {
public:
  SinricProEvent(SinricProDevice* device) : _device(*device) {}
  virtual ~SinricProEvent() {}

  void setTS(unsigned long timeStamp) { _timeStamp = timeStamp; }
  virtual String getJsonEventString() { return String(""); }
private:
protected:
  SinricProDevice& _device;
  unsigned long _timeStamp;
};

#endif
