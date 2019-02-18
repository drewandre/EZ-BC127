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
#ifdef DEBUG_BC127
    Serial.println("Unknown BC127 error: " + String(result));
#endif
    return "UNKNOWN_ERROR_CODE";
  default:
#ifdef DEBUG_BC127
    Serial.println("Unknown BC127 error: " + String(result));
#endif
    return "UNKNOWN_ERROR_CODE";
  }
}

BC127::opResult BC127::evaluateError(String errorCode)
{
  errorCode = errorCode.substring(6);
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
  if (_connected && _inDataMode)
  {
    delay(420);
    digitalWrite(_commandPin, LOW); // LOW: Enter command mode
    delay(420);
#ifdef DEBUG_BC127
    Serial.println("BC127 | Entered command mode");
#endif
  }
}

BC127::opResult BC127::enterCommandModeUART()
{
  static unsigned long startingMillis;
  static String EOL = "\r";
  static String buffer;
  buffer = "";

  delay(420);
  _serialPort->print("$$$$");
  _serialPort->flush();

  startingMillis = millis();
  while (!_serialPort->available())
  {
    Particle.process();

    if (millis() - startingMillis > COMMAND_TIMEOUT_DELAY)
    {
#if DEBUG_BC127
      Serial.println("BC127 | $$$$..." + convertCommandResultToString(TIMEOUT_ERROR) + "\r");
#endif
      return TIMEOUT_ERROR;
    }
  }

  startingMillis = millis();
  while (!buffer.endsWith(EOL))
  {
    if (_serialPort->available())
    {
      buffer.concat(char(_serialPort->read()));
      startingMillis = millis();
    }

    if (buffer.startsWith("ER"))
    {
      static opResult error = evaluateError(buffer);
      buffer = "";
      return error;
    }

    Particle.process();

    if (millis() - startingMillis > COMMAND_TIMEOUT_DELAY)
    {
#if DEBUG_BC127
      Serial.println("BC127 | $$$$..." + convertCommandResultToString(TIMEOUT_ERROR) + "\r");
#endif
      return TIMEOUT_ERROR;
    }
  }

  if (buffer.startsWith("OK"))
  {
#if DEBUG_BC127
    Serial.println("BC127 | $$$$..." + buffer + "\r");
#endif
    buffer = "";
    return SUCCESS;
  }

#if DEBUG_BC127
  Serial.println("BC127 | $$$$..." + buffer + "\r");
#endif
  buffer = "";
  return MODULE_ERROR;
}

void BC127::enterDataMode()
{
  digitalWrite(_commandPin, HIGH); // HIGH: Enter data mode
  _inDataMode = true;
#ifdef DEBUG_BC127
  Serial.println("BC127 | Entered data mode");
#endif
}

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++)
  {
    if (data.charAt(i) == separator || i == maxIndex)
    {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

BC127::opResult BC127::stdCmd(String command)
{
  static unsigned long startingMillis;
  static String EOL = "OK\r";
  static String buffer;
  buffer = "";

  enterCommandMode();

  _serialPort->print(command + "\r");
  _serialPort->flush();

  startingMillis = millis();
  while (!_serialPort->available())
  {
    Particle.process();

    if (millis() - startingMillis > COMMAND_TIMEOUT_DELAY)
    {
#if DEBUG_BC127
      Serial.println("BC127 | " + command + "..." + convertCommandResultToString(TIMEOUT_ERROR) + "\r");
#endif
      enterDataMode();
      return TIMEOUT_ERROR;
    }
  }

  startingMillis = millis();
  while (!buffer.endsWith(EOL))
  {
    if (_serialPort->available())
    {
      buffer.concat(char(_serialPort->read()));
      startingMillis = millis();
    }

    if (buffer.startsWith("ER"))
    {
      static opResult error = evaluateError(buffer);
      buffer = "";
      enterDataMode();
      return error;
    }

    Particle.process();

    if (millis() - startingMillis > COMMAND_TIMEOUT_DELAY)
    {
#if DEBUG_BC127
      Serial.println("BC127 | " + command + "..." + convertCommandResultToString(TIMEOUT_ERROR) + "\r");
#endif
      enterDataMode();
      return TIMEOUT_ERROR;
    }
  }

#if DEBUG_BC127
  Serial.println("BC127 | " + command + "..." + buffer + "\r");
#endif
  buffer = "";
  enterDataMode();
  return SUCCESS;
}

BC127::opResult BC127::stdSetParam(String command, String param, bool shouldRestart)
{
  static unsigned long startingMillis;
  static String EOL = "OK\r";
  static String buffer;
  buffer = "";

  enterCommandMode();

  _serialPort->print("SET " + command + "=" + param + "\r");
  _serialPort->flush();

  startingMillis = millis();
  while (!_serialPort->available())
  {
    Particle.process();

    if (millis() - startingMillis > COMMAND_TIMEOUT_DELAY)
    {
#ifdef DEBUG_BC127
      Serial.println("BC127 | SET " + command + "=" + param + "..." + convertCommandResultToString(TIMEOUT_ERROR) + "\r");
#endif
      enterDataMode();
      return TIMEOUT_ERROR;
    }
  }

  startingMillis = millis();
  while (!buffer.endsWith(EOL))
  {
    if (_serialPort->available())
    {
      buffer.concat(char(_serialPort->read()));
      startingMillis = millis();
    }

    if (buffer.startsWith("ER"))
    {
      static opResult error = evaluateError(buffer);
      buffer = "";
#ifdef DEBUG_BC127
      Serial.println("BC127 | SET " + command + "=" + param + "..." + convertCommandResultToString(error) + "\r");
#endif
      enterDataMode();
      return error;
    }

    Particle.process();

    if (millis() - startingMillis > COMMAND_TIMEOUT_DELAY)
    {
#ifdef DEBUG_BC127
      Serial.println("BC127 | SET " + command + "=" + param + "..." + convertCommandResultToString(TIMEOUT_ERROR) + "\r");
#endif
      enterDataMode();
      return TIMEOUT_ERROR;
    }
  }

  if (buffer.startsWith("OK"))
  {
    buffer = "";
#ifdef DEBUG_BC127
    Serial.println("BC127 | SET " + command + "=" + param + "..." + convertCommandResultToString(SUCCESS) + "\r");
#endif
    if (shouldRestart)
    {
      reset();
    }
    enterDataMode();
    return SUCCESS;
  }
#ifdef DEBUG_BC127
  Serial.println("BC127 | SET " + command + "=" + param + "..." + convertCommandResultToString(MODULE_ERROR) + "\r");
#endif
  buffer = "";
  return MODULE_ERROR;
}

String BC127::stdGetParam(String command)
{
  static unsigned long startingMillis;
  static String EOL = "OK\r";
  static String buffer;
  static String param;
  static String result;

  buffer = "";
  param = "";
  result = "";

  enterCommandMode();

  _serialPort->print("GET " + command + "\r");
  _serialPort->flush();

  startingMillis = millis();
  while (!_serialPort->available())
  {
    Particle.process();

    if (millis() - startingMillis > COMMAND_TIMEOUT_DELAY)
    {
      result = convertCommandResultToString(TIMEOUT_ERROR);
#ifdef DEBUG_BC127
      Serial.println("BC127 | GET " + command + "..." + result + "\r");
#endif
      enterDataMode();
      return result;
    }
  }

  startingMillis = millis();
  while (!buffer.endsWith(EOL))
  {
    if (_serialPort->available())
    {
      buffer.concat(char(_serialPort->read()));
      startingMillis = millis();
    }

    if (buffer.startsWith("ER"))
    {
      static String error = convertCommandResultToString(evaluateError(buffer));
      buffer = "";
#ifdef DEBUG_BC127
      Serial.println("BC127 | GET " + command + "..." + error + "\r");
#endif
      enterDataMode();
      return error;
    }

    Particle.process();

    if (millis() - startingMillis > COMMAND_TIMEOUT_DELAY)
    {
      result = convertCommandResultToString(TIMEOUT_ERROR);
#ifdef DEBUG_BC127
      Serial.println("BC127 | GET " + command + "..." + result + "\r");
#endif
      enterDataMode();
      return result;
    }
  }

  param = buffer.substring(command.length() + 1);
  param.trim();
  param.remove(param.length() - 3);

#ifdef DEBUG_BC127
  Serial.println("BC127 | GET " + command + "..." + param + "\r");
#endif
  enterDataMode();
  return param;
}

void BC127::handleBC127ConnectionEvent()
{
  static String buffer;
  static String status;
  static String channel;
  static String type;
  static String id;
  buffer = "";
  status = "";
  channel = "";
  type = "";
  id = "";

  while (!buffer.endsWith("\r"))
  {
    if (_serialPort->available())
    {
      buffer.concat(char(_serialPort->read()));
    }
  }

  status = getValue(buffer, ' ', 0);
  channel = getValue(buffer, ' ', 1);
  type = getValue(buffer, ' ', 2);
  id = getValue(buffer, ' ', 3);

  /*
#ifdef DEBUG_BC127
  Serial.println("BC127 | " + buffer);
  Serial.println("status -> " + status);
  Serial.println("channel -> " + channel);
  Serial.println("type -> " + type);
  Serial.println("id -> " + id);
#endif
  */

  // TODO: Baud rate increase to 115200 so first character is received after interrupt event
  if (status.endsWith("EN_OK")) // OPEN_OK
  {
    _connected = true;
    _inDataMode = false;
  }
  else if (status.endsWith("EN_ERROR")) // OPEN_ERROR
  {
    _connected = false;
    _inDataMode = false;
  }
  else if (status.endsWith("SE_OK")) // CLOSE_OK
  {
    _connected = false;
    _inDataMode = false;
    enableBLEAdvertising();
  }
  else if (status.endsWith("SE_ERROR")) // CLOSE_ERROR
  {
    _connected = true;
  }

  /*
#ifdef DEBUG_BC127
  Serial.println("Opening " + type + " connection on channel " + channel);
#endif
  */

  stdCmd("ENTER_DATA_MODE " + channel);
}