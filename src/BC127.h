#pragma once

// This will load the definition for common Particle variable types
#include "Particle.h"

#define COMMAND_TIMEOUT_DELAY 2000

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

class BC127 {
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

  BC127(int          commandPin,
        int          connectionIndicatorPin,
        int          eventBitmaskPin,
        USARTSerial *serialPort,
        int          serialBaud,
        String       deviceName);

  ~BC127();

  void     enable(void);

  void     enterCommandMode(void);
  void     exitCommandMode(void);

  void     handleBC127ConnectionEvent();
  void     handleBC127DisconnectionEvent();
  void     handleBC127EventBitmaskEvent();

  // Command helpers
  String   stdGetParam(String command);
  opResult stdSetParam(String command,
                       String param);
  opResult stdCmd(String command);
  String   convertCommandResultToString(int result);
  opResult evaluateError(String errorCode);

  // Get commands
  String   getName(void);
  String   getNameShort(void);
  String   getGPIOConfig(void);
  String   getUARTConfig(void);
  String   getBLEConfig(void);

  // Standard commands
  opResult enableConnectableAndDiscoverable(void);
  opResult resetPIO4(void);
  opResult write(void);
  opResult reset(void);
  opResult restore(void);
  opResult status(void);

  // Set commands
  opResult setVolConfig(void);
  opResult setCodType(void);
  opResult enableGPIOControl(void);
  opResult disableGPIOControl(void);
  opResult enableAutoData(bool shouldReboot);
  opResult disableAutoData(bool shouldReboot); // TODO: combine this logic
  opResult enableAutoConn(bool shouldReboot);
  opResult disableAutoConn(bool shouldReboot);
  opResult enableHDAudio(void);
  opResult disableAdvertisingOnStartup(void);
  opResult enableBLEAdvertising(void);
  opResult disableiOSBatteryIndicator(void);
  opResult disableAdvertising(void);
  opResult setName(String name,
                   bool   shouldReboot);
  opResult setShortName(String name,
                        bool   shouldReboot);
  opResult setMaxNumOfReconnectionAttempts(int  maxNum,
                                           bool shouldReboot);
  opResult setUARTConfig(void);

private:

  String _deviceName = BC127_NAME;

  bool _inDataMode = true; // Start as true since immediately set to false in
                           // enable()

  uint16_t _cmdTo = DEFAULT_CMD_TO_VALUE * 20;
  String _codType = DEFAULT_COD_TYPE;

  int _connectionIndicatorPin = DEFAULT_CONNECTION_INDICATOR_PIN;
  int _eventBitmaskPin        = DEFAULT_EVENT_BITMASK_PIN;
  int _commandPin             = DEFAULT_COMMAND_PIN;

  USARTSerial *_serialPort = DEFAULT_SERIAL_PORT;
  int _serialBaud          = DEFAULT_SERIAL_BAUD_RATE;
};
