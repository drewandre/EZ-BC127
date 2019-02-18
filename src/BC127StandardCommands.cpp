#include "BC127.h"

BC127::opResult BC127::enableConnectableAndDiscoverable()
{
  return stdCmd("BT_STATE ON ON");
}

BC127::opResult BC127::resetPIO4()
{
  return stdCmd("PIO 4 OFF");
}

BC127::opResult BC127::write()
{
  return stdCmd("WRITE");
}

BC127::opResult BC127::status()
{
  static unsigned long startingMillis;
  static String EOL = "OK\r";
  static String buffer;
  buffer = "";

  enterCommandMode();

  _serialPort->print("STATUS\r");
  _serialPort->flush();

  startingMillis = millis();
  while (!_serialPort->available())
  {
    Particle.process();

    if (millis() - startingMillis > COMMAND_TIMEOUT_DELAY)
    {
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
      enterDataMode();
      return TIMEOUT_ERROR;
    }
  }

  if (buffer.startsWith("OK"))
  {
#ifdef DEBUG_BC127
    Serial.println(buffer);
#endif
    buffer = "";
    enterDataMode();
    return SUCCESS;
  }

#ifdef DEBUG_BC127
  Serial.println("Error: " + buffer);
#endif
  buffer = "";
  enterDataMode();
  return MODULE_ERROR;
}
