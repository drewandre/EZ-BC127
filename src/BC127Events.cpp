#include "BC127.h"

void BC127::enableInterrupts()
{
  // Photon interrupt docs:
  // https://docs.particle.io/reference/device-os/firmware/photon/#interrupts
  // attachInterrupt(_connectionIndicatorPin,
  // &BC127::handleBC127DisconnectionEvent, this, RISING);
  attachInterrupt(_connectionIndicatorPin,
                  &BC127::handleBC127ConnectionEvent,
                  this,
                  CHANGE);
  attachInterrupt(_eventBitmaskPin,
                  &BC127::handleBC127EventBitmaskEvent,
                  this,
                  CHANGE);
}

void BC127::disableInterrupts()
{
  detachInterrupt(_connectionIndicatorPin);
  detachInterrupt(_eventBitmaskPin);
}

void BC127::handleBC127EventBitmaskEvent()
{
  Serial.println("BC127 | PIO 4 -> Exited data mode");
  _inDataMode = false;
}

void BC127::handleBC127ConnectionEvent()
{
#ifdef DEBUG_BC127
  Serial.println("BC127 | Connection event");
#endif

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

  status = splitString(buffer, ' ', 0);
  channel = splitString(buffer, ' ', 1);
  type = splitString(buffer, ' ', 2);
  id = splitString(buffer, ' ', 3);

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