#include "BC127.h"

String BC127::convertCommandResultToString(int result)
{
  switch (result)
  {
  case SUCCESS:
    return "SUCCESS";

  case MODULE_ERROR:
    return "MODULE_ERROR";

  case REMOTE_ERROR:
    return "REMOTE_ERROR";

  case CONNECT_ERROR:
    return "CONNECT_ERROR";

  case TIMEOUT_ERROR:
    return "TIMEOUT_ERROR";

  case DEFAULT_ERR:
    return "DEFAULT_ERR";

  case UNKNOWN_ERROR:
    return "UNKNOWN_ERROR";

  case COMMAND_NOT_ALLOWED_WITH_THE_CURRENT_CONFIGURATION:
    return "COMMAND_NOT_ALLOWED_WITH_THE_CURRENT_CONFIGURATION";

  case COMMAND_NOT_FOUND:
    return "COMMAND_NOT_FOUND";

  case WRONG_PARAMETER:
    return "WRONG_PARAMETER";

  case WRONG_NUMBER_OF_PARAMETERS:
    return "WRONG_NUMBER_OF_PARAMETERS";

  case COMMAND_NOT_ALLOWED_IN_THE_CURRENT_STATE:
    return "COMMAND_NOT_ALLOWED_IN_THE_CURRENT_STATE";

  case DEVICE_ALREADY_CONNECTED:
    return "DEVICE_ALREADY_CONNECTED";

  case DEVICE_NOT_CONNECTED:
    return "DEVICE_NOT_CONNECTED";

  case COMMAND_IS_TOO_LONG:
    return "COMMAND_IS_TOO_LONG";

  case NAME_NOT_FOUND:
    return "NAME_NOT_FOUND";

  case CONFIGURATION_NOT_FOUND:
    return "CONFIGURATION_NOT_FOUND";

  case FAILED_TO_READ_BATTERY_VOLTAGE:
    return "FAILED_TO_READ_BATTERY_VOLTAGE";

  case FAILED_TO_COMMUNICATE_WITH_THE_APPLE_MFI_CO_PROCESSOR:
    return "FAILED_TO_COMMUNICATE_WITH_THE_APPLE_MFI_CO_PROCESSOR";

  case FAILED_TO_REGISTER_UNREGISTER_DEVICE:
    return "FAILED_TO_REGISTER_UNREGISTER_DEVICE";

  case BLE_REQUEST_FAILED:
    return "BLE_REQUEST_FAILED";

  case INSUFFICIENT_ENCRYPTION:
    return "INSUFFICIENT_ENCRYPTION";

  case INSUFFICIENT_AUTHENTICATION:
    return "INSUFFICIENT_AUTHENTICATION";

  case OPERATION_NOT_PERMITTED:
    return "OPERATION_NOT_PERMITTED";

  case INVALID_HANDLE:
    return "INVALID_HANDLE";

  case CRITICAL_ERROR:
    return "CRITICAL_ERROR";

  case MELODY_LICENSE_KEY_IS_MISSING:
    return "MELODY_LICENSE_KEY_IS_MISSING";

  case MELODY_LICENSE_KEY_IS_INVALID:
    return "MELODY_LICENSE_KEY_IS_INVALID";

  case UNKNOWN_ERROR_CODE:
    return "UNKNOWN_ERROR_CODE";

  default:
    return "UNKNOWN_ERROR_CODE";
  }
}

BC127::opResult BC127::evaluateError(String errorCode)
{
  errorCode.trim();

  if (errorCode == "0x0003")
  {
    return UNKNOWN_ERROR;
  }
  else if (errorCode == "0x0011")
  {
    return COMMAND_NOT_ALLOWED_WITH_THE_CURRENT_CONFIGURATION;
  }
  else if (errorCode == "0x0012")
  {
    return COMMAND_NOT_FOUND;
  }
  else if (errorCode == "0x0013")
  {
    return WRONG_PARAMETER;
  }
  else if (errorCode == "0x0014")
  {
    return WRONG_NUMBER_OF_PARAMETERS;
  }
  else if (errorCode == "0x0015")
  {
    return COMMAND_NOT_ALLOWED_IN_THE_CURRENT_STATE;
  }
  else if (errorCode == "0x0016")
  {
    return DEVICE_ALREADY_CONNECTED;
  }
  else if (errorCode == "0x0017")
  {
    return DEVICE_NOT_CONNECTED;
  }
  else if (errorCode == "0x0018")
  {
    return COMMAND_IS_TOO_LONG;
  }
  else if (errorCode == "0x0019")
  {
    return NAME_NOT_FOUND;
  }
  else if (errorCode == "0x001A")
  {
    return CONFIGURATION_NOT_FOUND;
  }
  else if (errorCode == "0x0100")
  {
    return FAILED_TO_READ_BATTERY_VOLTAGE;
  }
  else if (errorCode == "0x1002")
  {
    return FAILED_TO_COMMUNICATE_WITH_THE_APPLE_MFI_CO_PROCESSOR;
  }
  else if (errorCode == "0x1004")
  {
    return FAILED_TO_REGISTER_UNREGISTER_DEVICE;
  }
  else if (errorCode == "0x1005")
  {
    return BLE_REQUEST_FAILED;
  }
  else if (errorCode == "0x1006")
  {
    return INSUFFICIENT_ENCRYPTION;
  }
  else if (errorCode == "0x1007")
  {
    return INSUFFICIENT_AUTHENTICATION;
  }
  else if (errorCode == "0x1008")
  {
    return OPERATION_NOT_PERMITTED;
  }
  else if (errorCode == "0x1009")
  {
    return INVALID_HANDLE;
  }
  else if (errorCode == "0xF00x")
  {
    return CRITICAL_ERROR;
  }
  else if (errorCode == "0xFF01")
  {
    return MELODY_LICENSE_KEY_IS_MISSING;
  }
  else if (errorCode == "0xFF02")
  {
    return MELODY_LICENSE_KEY_IS_INVALID;
  }
  else
  {
    return UNKNOWN_ERROR_CODE;
  }
}

void BC127::enterCommandMode()
{
  // if (_inDataMode)
  // {
  // Serial.println("Entering command mode...");
  delay(420);
  digitalWrite(_commandPin, LOW); // LOW: Enter command mode
  delay(420);

  // }
}

void BC127::exitCommandMode()
{
  // Serial.println("Exiting command mode...");
  digitalWrite(_commandPin, HIGH); // HIGH: Enter data mode
  // _inDataMode = true;
}

BC127::opResult BC127::stdCmd(String command)
{
  static String buffer;
  static String EOL = String("OK\r");

  buffer = "";

  enterCommandMode();

  _serialPort->print(command);
  _serialPort->print(   "\r");
  _serialPort->flush();

  static unsigned long startingMillis = millis();

  while (!_serialPort->available())
  {
    Particle.process();

    if (millis() - startingMillis > COMMAND_TIMEOUT_DELAY)
    {
      exitCommandMode();
      return TIMEOUT_ERROR;
    }
  }
  startingMillis = millis();

  buffer.concat(char(_serialPort->read()));

  while (!buffer.endsWith(EOL))
  {
    Particle.process();

    if (millis() - startingMillis > COMMAND_TIMEOUT_DELAY)
    {
      exitCommandMode();
      return TIMEOUT_ERROR;
    }

    if (_serialPort->available())
    {
      startingMillis = millis();
      buffer.concat(char(_serialPort->read()));
    }
  }

  if (buffer.startsWith("OK"))
  {
    buffer = "";
    exitCommandMode();
    return SUCCESS;
  }

  if (buffer.startsWith("ER"))
  {
    static opResult error = evaluateError(buffer.substring(6));
    buffer = "";
    exitCommandMode();
    return error;
  }
#ifdef DEBUG_BC127
  Serial.println("Error: " + buffer);
#endif
  buffer = "";
  exitCommandMode();
  return MODULE_ERROR;
}

BC127::opResult BC127::stdSetParam(String command, String param)
{
  static String buffer = "";
  static String EOL    = String("OK\r");

  buffer = "";

  enterCommandMode();

  _serialPort->print( "SET ");
  _serialPort->print(command);
  _serialPort->print(    "=");
  _serialPort->print(  param);
  _serialPort->print(   "\r");
  _serialPort->flush();

  static unsigned long startingMillis = millis();

  while (!_serialPort->available())
  {
    Particle.process();

    if (millis() - startingMillis > COMMAND_TIMEOUT_DELAY)
    {
      exitCommandMode();
      return TIMEOUT_ERROR;
    }
  }
  startingMillis = millis();

  buffer.concat(char(_serialPort->read()));

  while (!buffer.endsWith(EOL))
  {
    Particle.process();

    if (millis() - startingMillis > COMMAND_TIMEOUT_DELAY)
    {
      exitCommandMode();
      return TIMEOUT_ERROR;
    }

    if (_serialPort->available())
    {
      startingMillis = millis();
      buffer.concat(char(_serialPort->read()));
    }
  }

  if (buffer.startsWith("OK"))
  {
    buffer = "";
    exitCommandMode();
    return SUCCESS;
  }

  if (buffer.startsWith("ER"))
  {
    static opResult error = evaluateError(buffer.substring(6));
    buffer = "";
    exitCommandMode();
    return error;
  }
#ifdef DEBUG_BC127
  Serial.println("Error entering command mode: " + buffer);
#endif
  buffer = "";
  exitCommandMode();
  return MODULE_ERROR;
}

String BC127::stdGetParam(String command)
{
  static String buffer = "";
  static String param  = "";
  static String EOL    = String("OK\r");

  buffer = "";
  param  = "";

  enterCommandMode();

  _serialPort->print( "GET ");
  _serialPort->print(command);
  _serialPort->print(   "\r");
  _serialPort->flush();

  static unsigned long startingMillis = millis();

  while (!_serialPort->available())
  {
    Particle.process();

    if (millis() - startingMillis > COMMAND_TIMEOUT_DELAY)
    {
      exitCommandMode();
      return convertCommandResultToString(TIMEOUT_ERROR);
    }
  }
  startingMillis = millis();

  buffer.concat(char(_serialPort->read()));

  while (!buffer.endsWith(EOL))
  {
    Particle.process();

    if (millis() - startingMillis > COMMAND_TIMEOUT_DELAY)
    {
      exitCommandMode();
      return convertCommandResultToString(TIMEOUT_ERROR);
    }

    if (_serialPort->available())
    {
      startingMillis = millis();
      buffer.concat(char(_serialPort->read()));
    }
  }

  param = buffer.substring(command.length() + 1);
  param.trim();
  param.remove(param.length() - 3);

  exitCommandMode();
  return param;
}
