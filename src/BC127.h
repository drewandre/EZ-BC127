#pragma once

// This will load the definition for common Particle variable types
#include "Particle.h"

#define TIMEOUT_DELAY 2000
#define DEFAULT_SERIAL_PORT &Serial1
#define DEFAULT_SERIAL_BAUD_RATE 9600
// #define AUTOCONN "0"
// #define AUTO_DATA "ON ON" // "ON ON" for auto connection to first BLE profile
// #define BT_STATE "ON ON"  // "ON ON" for auto connection to first BLE profile

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
        int gpioZero,
        USARTSerial *_serialPort,
        int _serialBaud,
        String deviceName);

  ~BC127();

  void enable(void);

  void enterCommandMode(void);
  void exitCommandMode(void);

  String commandResult(int result);
  opResult evaluateError(String errorCode);

  String getGPIOConfig(void);
  String getBLEConfig(void);
  String getUARTConfig(void);
  String getName();

  opResult disableGPIOControl(void);
  opResult resetPIO4(void);
  opResult enableGPIOControl(void);
  opResult disableAdvertising(void);
  opResult disableAdvertisingOnStartup(void);
  opResult enableConnectableAndDiscoverable(void);
  opResult enableBLEAdvertising(void);
  opResult disableiOSBatteryIndicator(void);
  opResult enableAutoData(bool shouldReboot);
  opResult disableAutoData(bool shouldReboot);
  opResult enableAutoConn(bool shouldReboot);
  opResult disableAutoConn(bool shouldReboot);
  opResult enableHDAudio(void);

  opResult setName(String name, bool shouldReboot);
  opResult setShortName(String name, bool shouldReboot);
  opResult setMaxNumOfReconnectionAttempts(int maxNum, bool shouldReboot);
  opResult setUARTConfig(int baudRate);

  opResult reset(void);
  opResult restore(void);
  opResult write(void);
  opResult inquiry(int timeout);
  opResult connect(char index,
                   connType connection);

  // opResult getAddress(char    index,
  //                     String& address);

  opResult BLEScan(int timeout);
  opResult musicCommands(audioCmds command);

  // opResult addressQuery(String& address);
  opResult setClassicSink(void);
  opResult setClassicSource(void);

  String stdGetParam(String command);
  opResult stdSetParam(String command,
                       String param);
  opResult stdCmd(String command);

  String connectionState(void);

  bool factorySettings(String name);

private:
  BC127() {}

  USARTSerial *_serialPort = DEFAULT_SERIAL_PORT;
  int _serialBaud = DEFAULT_SERIAL_BAUD_RATE;

  int _commandPin = 3;
  int _gpioZero = 1;
  String _deviceName = "BC127";

  opResult knownStart();
};
