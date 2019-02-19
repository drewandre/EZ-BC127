#include "BC127.h"

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

void BC127::enterDataMode()
{
  digitalWrite(_commandPin, HIGH); // HIGH: Enter data mode
  _inDataMode = true;
#ifdef DEBUG_BC127
  Serial.println("BC127 | Entered data mode");
#endif
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