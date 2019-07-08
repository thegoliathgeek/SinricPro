#ifndef _SINRICPRO_REQUEST_H
#define _SINRICPRO_REQUEST_H

//#define JSON_ACTION_NAME "name"
#define POWER_STATE_REQUEST "setPowerState"
#define POWER_LEVEL_REQUEST "setPowerLevel"
#define ADJUST_POWERLEVEL_REQUEST "adjustPowerLevel"

#define BRIGHTNESS_REQUEST "setBrightness"
#define ADJUST_BRIGHTNESS_REQUEST "adjustBrightness"

#define COLOR_REQUEST "setColor"
#define COLORTEMPERATURE_REQUEST "setColorTemperature"
#define INCREASE_COLORTEMPERATURE_REQUEST "increaseColorTemperature"
#define DECREASE_COLORTEMPERATURE_REQUEST "decreaseColorTemperature"


typedef enum {
  CS_UNKNOWN    = 0,
  CS_WEBSOCKET  = 1,
  CS_UDP        = 2
} sp_request_source_t;

class SinricProRequestPayload {
public:
  SinricProRequestPayload(sp_request_source_t source, char* request) : _source(source) { _request = strdup(request); };
  ~SinricProRequestPayload() { if (_request) free(_request); };
  const char* getRequest() { return _request; };
  const sp_request_source_t getRequestSource() { return _source; }
  sp_request_source_t getSource() { return _source; };
private:
  sp_request_source_t _source;
  char* _request;
};

#endif
