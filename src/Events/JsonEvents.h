#ifndef __JSONEVENTS_H__
#define __JSONEVENTS_H__

#define JSON_EVENT_SUCCESS "success"
#define JSON_EVENT_TIMESTAMP "ts"
#define JSON_EVENT_DEVICEID "did"
#define JSON_EVENT_TYPE "type"
#define JSON_EVENT_TYPE_EVENT "event"
#define JSON_EVENT_TYPE_POWEREVENT "powerEvent"
#define JSON_EVENT_TYPE_POWERLEVELEVENT "powerLevelEvent"
#define JSON_EVENT_TYPE_BRIGHTNESSLEVELEVENT "brightnessLevelEvent"
#define JSON_EVENT_TYPE_COLOREVENT "colorEvent"
#define JSON_EVENT_TYPE_COLORTEMPERATUREEVENT "colorTemperatureEvent"

#define JSON_EVENT_VALUES "values"
#define JSON_EVENT_VALUE "value"
#define JSON_EVENT_VALUE_NAME "name"
#define JSON_EVENT_VALUE_POWERSTATE "powerState"
#define JSON_EVENT_VALUE_POWERSTATE_ON "On"
#define JSON_EVENT_VALUE_POWERSTATE_OFF "Off"
#define JSON_EVENT_VALUE_POWERLEVEL "powerLevel"
#define JSON_EVENT_VALUE_BRIGHTNESS "brightnessLevel"
#define JSON_EVENT_VALUE_COLOR "color"
#define JSON_EVENT_VALUE_R "r"
#define JSON_EVENT_VALUE_G "g"
#define JSON_EVENT_VALUE_B "b"
#define JSON_EVENT_VALUE_COLORTEMPERATURE "colorTemperature"
#endif
