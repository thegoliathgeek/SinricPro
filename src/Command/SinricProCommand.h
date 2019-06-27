/*
 * (c) 2019 Sinric
 * License: MIT License.
 * Author: Aruna Tennakoon
 *
 * holds a remote command sent via offline/online protocol
 */

#ifndef _SINRICPRO_REMOTE_COMMAND_H
#define _SINRICPRO_REMOTE_COMMAND_H

#define JSON_ACTION_NAME "name"

typedef enum {
  CS_UNKNOWN    = 0,
  CS_WEBSOCKET  = 1,
  CS_UDP        = 2
} sp_command_source_t;

class SinricProCommand {
  public:
    SinricProCommand(const char* deviceId, JsonObject& action, JsonObject& response) : _deviceId(deviceId), _action(action), _response(response) { _handled = false; }
    const char* getDeviceId() { return _deviceId; }
    JsonObject& getAction() { return _action; }
    const char* getActionName() { return _action[JSON_ACTION_NAME]; }
    JsonObject& getResponse() { return _response; }
    void setSuccess(bool success) { _success = success; }
    bool getSuccess() { return _success; }
    void setHandled(bool handled) { _handled = handled; }
    bool isHandled() { return _handled; }
    bool isNotHandled() { return !_handled; }
  private:
    const char* _deviceId;
    JsonObject& _action;
    JsonObject& _response;
    bool _success;
    bool _handled;
};

class SinricProReceiveCommand {
public:
  SinricProReceiveCommand(sp_command_source_t source, char* command) : _source(source) { _command = strdup(command); };
  ~SinricProReceiveCommand() { if (_command) free(_command); };
  const char* getCommand() { return _command; };
  const sp_command_source_t getCommandSource() { return _source; }
  sp_command_source_t getSource() { return _source; };
private:
  sp_command_source_t _source;
  char* _command;
};

#endif
