#pragma once

// This will load the definition for common Particle variable types
#include "Particle.h"

#define COMMAND_TIMEOUT_DELAY 3000

#define BC127_NAME "Palette"
#define DEFAULT_CONNECTION_INDICATOR_PIN D2
#define DEFAULT_EVENT_BITMASK_PIN D3
#define DEFAULT_COMMAND_PIN D4
#define CONNECTION_INDICATOR_PIN D4
#define DEFAULT_SERIAL_PORT &Serial1
#define DEFAULT_SERIAL_BAUD_RATE 9600
#define DEFAULT_CMD_TO_VALUE 21
#define DEFAULT_COD_TYPE "200428"

#define DEBUG_BC127 true

class BC127
{
public:
  enum connType
  {
    SPP,
    BLE,
    A2DP,
    HFP,
    AVRCP,
    PBAP,
    ANY
  };
  enum opResult
  {
    SUCCESS,
    MODULE_ERROR,
    REMOTE_ERROR,
    CONNECT_ERROR,
    TIMEOUT_ERROR,
    DEFAULT_ERR,
    UNKNOWN_ERROR,
    COMMAND_NOT_ALLOWED_WITH_THE_CURRENT_CONFIGURATION,
    COMMAND_NOT_FOUND,
    WRONG_PARAMETER,
    WRONG_NUMBER_OF_PARAMETERS,
    COMMAND_NOT_ALLOWED_IN_THE_CURRENT_STATE,
    DEVICE_ALREADY_CONNECTED,
    DEVICE_NOT_CONNECTED,
    COMMAND_IS_TOO_LONG,
    NAME_NOT_FOUND,
    CONFIGURATION_NOT_FOUND,
    FAILED_TO_READ_BATTERY_VOLTAGE,
    FAILED_TO_COMMUNICATE_WITH_THE_APPLE_MFI_CO_PROCESSOR,
    FAILED_TO_REGISTER_UNREGISTER_DEVICE,
    BLE_REQUEST_FAILED,
    INSUFFICIENT_ENCRYPTION,
    INSUFFICIENT_AUTHENTICATION,
    OPERATION_NOT_PERMITTED,
    INVALID_HANDLE,
    CRITICAL_ERROR,
    MELODY_LICENSE_KEY_IS_MISSING,
    MELODY_LICENSE_KEY_IS_INVALID,
    UNKNOWN_ERROR_CODE
  };
  enum audioCmds
  {
    PLAY,
    PAUSE,
    FORWARD,
    BACK,
    UP,
    DOWN,
    STOP
  };

  BC127(int commandPin,
        int connectionIndicatorPin,
        int eventBitmaskPin,
        USARTSerial *serialPort,
        int serialBaud,
        String deviceName,
        void (*serialCallbackPointer)(String command));

  ~BC127();

  void handleBC127Serial(String command)
  {
    funct(command);
  };

  void enable();
  void runTask();

  void listenAndHandleSPPData();

  // Command helpers
  String stdGetParam(String command);
  opResult stdSetParam(String command,
                       String param,
                       bool shouldRestart);
  opResult stdCmd(String command);

  // Get commands
  String getName();
  String getNameShort();
  String getGPIOConfig();
  String getUARTConfig();
  String getBLEConfig();

  // Standard commands
  opResult enableConnectableAndDiscoverable();
  opResult resetPIO4();
  opResult reset();
  opResult restore();
  opResult status();

  // Set commands
  opResult setVolConfig();
  opResult setCodType();
  opResult enableGPIOControl();
  opResult disableGPIOControl();
  opResult enableAutoData(bool shouldRestart);
  opResult disableAutoData(bool shouldRestart); // TODO: combine this logic
  opResult enableAutoConn(bool shouldRestart);
  opResult disableAutoConn(bool shouldRestart);
  opResult enableHDAudio();
  opResult disableAdvertisingOnStartup();
  opResult enableBLEAdvertising();
  opResult disableiOSBatteryIndicator();
  opResult disableAdvertising();
  opResult setName(String name,
                   bool shouldRestart);
  opResult setShortName(String name,
                        bool shouldRestart);
  opResult setMaxNumOfReconnectionAttempts(int maxNum,
                                           bool shouldRestart);
  opResult setUARTConfig();

private:
  String _deviceName = BC127_NAME;

  bool _inDataMode = false;
  bool _connected = false;

  uint16_t _cmdTo = DEFAULT_CMD_TO_VALUE * 20;
  String _codType = DEFAULT_COD_TYPE;

  int _connectionIndicatorPin = DEFAULT_CONNECTION_INDICATOR_PIN;
  int _eventBitmaskPin = DEFAULT_EVENT_BITMASK_PIN;
  int _commandPin = DEFAULT_COMMAND_PIN;

  USARTSerial *_serialPort = DEFAULT_SERIAL_PORT;
  int _serialBaud = DEFAULT_SERIAL_BAUD_RATE;

  void (*funct)(String command);

  void enableInterrupts();
  void disableInterrupts();

  opResult enterCommandModeUART();
  void enterCommandMode();
  void enterDataMode();

  void handleBC127ConnectionEvent();
  void handleBC127EventBitmaskEvent();

  opResult write();

  opResult evaluateError(String errorCode);
  String convertCommandResultToString(int result);

  String handleConnectionEvent();
};
