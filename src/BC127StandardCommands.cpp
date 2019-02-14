#include "BC127.h"

BC127::opResult BC127::enableConnectableAndDiscoverable()
{
  static opResult result = stdCmd("BT_STATE ON ON");

#if DEBUG_BC127
  Serial.print("BC127 | CMD BT_STATE ON ON..." + convertCommandResultToString(result) + "\r");
#endif
  return result;
}

BC127::opResult BC127::resetPIO4()
{
  static opResult result = stdCmd("PIO 4 OFF");

#if DEBUG_BC127
  Serial.print("BC127 | CMD PIO 4 OFF..." + convertCommandResultToString(result) + "\r");
#endif
  return result;
}

BC127::opResult BC127::write()
{
  static opResult result = stdCmd("WRITE");

#if DEBUG_BC127
  Serial.println("BC127 | CMD WRITE..." + convertCommandResultToString(result));
#endif
  return result;
}

BC127::opResult BC127::status()
{
  static String buffer;
  static String EOL = String("OK\r");

  buffer = "";

  enterCommandMode();

  _serialPort->print("STATUS\r");
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
#ifdef DEBUG_BC127
    Serial.println(buffer);
#endif
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
