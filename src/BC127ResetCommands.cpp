#include "BC127.h"

BC127::opResult BC127::restore()
{
#if DEBUG_BC127
  Serial.println("BC127 | CMD RESTORE");
#endif

  static unsigned long startingMillis;
  static String EOL = "Ready\r";
  static String buffer;
  buffer = "";

  enterCommandMode();

  // long previousBaud;
  // EEPROM.get(10, previousBaud);
  // if ((previousBaud == 0xFFFF) || (previousBaud == -1))
  // {
  //   previousBaud = 9600;
  // }

  // Serial.println("previousBaud " + previousBaud);
  // EEPROM.put(10, 9600);

  // long newBaud;
  // EEPROM.get(10, newBaud);
  // if ((newBaud == 0xFFFF) || (newBaud == -1))
  // {
  //   newBaud = 9600;
  // }
  // Serial.println("newBaud " + newBaud);

  _serialPort->print("RESTORE\r");
  _serialPort->flush();
  // _serialPort->end();
  // _serialPort->begin(9600);

  startingMillis = millis();
  while (!_serialPort->available())
  {
    Particle.process();

    if (millis() - startingMillis > COMMAND_TIMEOUT_DELAY)
    {
#if DEBUG_BC127
      Serial.println(convertCommandResultToString(TIMEOUT_ERROR));
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
#if DEBUG_BC127
      Serial.println(convertCommandResultToString(error));
#endif
      return error;
    }

    Particle.process();

    if (millis() - startingMillis > COMMAND_TIMEOUT_DELAY)
    {
#if DEBUG_BC127
      Serial.println(convertCommandResultToString(TIMEOUT_ERROR));
#endif
      return TIMEOUT_ERROR;
    }
  }

  if (buffer.startsWith("Sierra"))
  {
#if DEBUG_BC127
    buffer.replace("\r", "\n");
    Serial.print(buffer);
#endif
    buffer = "";
    return SUCCESS;
  }

  buffer = "";
#if DEBUG_BC127
  Serial.println(convertCommandResultToString(MODULE_ERROR));
#endif
  return MODULE_ERROR;
}

BC127::opResult BC127::reset()
{
#if DEBUG_BC127
  Serial.println("BC127 | CMD RESET");
#endif

  static unsigned long startingMillis;
  static String EOL = "Ready\r";
  static String buffer;
  buffer = "";

  enterCommandMode();

  _serialPort->print("RESET\r");
  _serialPort->flush();

  startingMillis = millis();
  while (!_serialPort->available())
  {
    Particle.process();

    if (millis() - startingMillis > COMMAND_TIMEOUT_DELAY)
    {
#if DEBUG_BC127
      Serial.println(convertCommandResultToString(TIMEOUT_ERROR));
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
#if DEBUG_BC127
      Serial.println(convertCommandResultToString(error));
#endif
      return error;
    }

    Particle.process();

    if (millis() - startingMillis > COMMAND_TIMEOUT_DELAY)
    {
#if DEBUG_BC127
      Serial.println(convertCommandResultToString(TIMEOUT_ERROR));
#endif
      return TIMEOUT_ERROR;
    }
  }

  if (buffer.startsWith("Sierra"))
  {
#if DEBUG_BC127
    buffer.replace("\r", "\n");
    Serial.print(buffer);
#endif
    buffer = "";
    return SUCCESS;
  }

  buffer = "";
#if DEBUG_BC127
  Serial.println(convertCommandResultToString(MODULE_ERROR));
#endif
  return MODULE_ERROR;
}