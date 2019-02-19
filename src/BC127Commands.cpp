#include "BC127.h"

void BC127::sendDataToCentral(String data)
{
  _serialPort->print(data + '\r');
  _serialPort->flush();
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