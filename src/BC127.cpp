#include "BC127.h"

BC127::BC127(int          commandPin,
             int          connectionIndicatorPin,
             int          eventBitmaskPin,
             USARTSerial *serialPort,
             int          serialBaud,
             String       deviceName)
{
  _commandPin             = commandPin;
  _connectionIndicatorPin = connectionIndicatorPin;
  _eventBitmaskPin        = eventBitmaskPin;
  _serialPort             = serialPort;
  _serialBaud             = serialBaud;
  _deviceName             = deviceName;

  _serialPort->begin(_serialBaud);

  pinMode(_commandPin,             OUTPUT);
  pinMode(_connectionIndicatorPin, INPUT_PULLDOWN);
  pinMode(_eventBitmaskPin,        INPUT);

  digitalWrite(_commandPin, LOW);

  // Photon interrupt docs:
  // https://docs.particle.io/reference/device-os/firmware/photon/#interrupts
  // attachInterrupt(_connectionIndicatorPin,
  // &BC127::handleBC127DisconnectionEvent, this, RISING); // FALLING
  attachInterrupt(_connectionIndicatorPin,
                  &BC127::handleBC127ConnectionEvent,
                  this,
                  CHANGE); // RISING
  attachInterrupt(_eventBitmaskPin,
                  &BC127::handleBC127EventBitmaskEvent,
                  this,
                  CHANGE);
}

BC127::~BC127() {}

void BC127::enable()
{
  enterCommandMode();

  restore();

  setUARTConfig();
  disableGPIOControl();

  setName(_deviceName, false);
  setShortName(_deviceName, false);

  getBLEConfig();

  disableiOSBatteryIndicator();
  resetPIO4();
  enableHDAudio();
  setVolConfig();
  setCodType();

  write();
  reset();

  getName();

  enableConnectableAndDiscoverable();
  enableBLEAdvertising();

  // enableAutoConn(false);
  enableAutoData(false);

#ifdef DEBUG_BC127
  Serial.println("\rBC127 ready!");
#endif
}

BC127::opResult BC127::restore()
{
#if DEBUG_BC127
  Serial.println("BC127 | CMD RESTORE");
#endif

  static String buffer = "";
  static String EOL    = String("Ready\r");

  enterCommandMode();

  _serialPort->print("RESTORE\r");
  _serialPort->flush();

  static unsigned long startingMillis = millis();

  while (!_serialPort->available())
  {
    Particle.process();

    if (millis() - startingMillis > COMMAND_TIMEOUT_DELAY)
    {
#if DEBUG_BC127
      Serial.print(convertCommandResultToString(TIMEOUT_ERROR));
#endif
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
#if DEBUG_BC127
      Serial.print(convertCommandResultToString(TIMEOUT_ERROR));
#endif
      return TIMEOUT_ERROR;
    }

    if (_serialPort->available())
    {
      startingMillis = millis();
      buffer.concat(char(_serialPort->read()));
    }
  }

  if (buffer.startsWith("Sierra"))
  {
#if DEBUG_BC127
    Serial.print(buffer + '\r');
#endif
    buffer = "";

    // _inDataMode = false;
    return SUCCESS;
  }

  if (buffer.startsWith("ER"))
  {
    static opResult error = evaluateError(buffer.substring(6));
    buffer = "";
#if DEBUG_BC127
    Serial.print(convertCommandResultToString(error));
#endif
    return error;
  }
  buffer = "";
#if DEBUG_BC127
  Serial.print(convertCommandResultToString(MODULE_ERROR));
#endif
  return MODULE_ERROR;
}

BC127::opResult BC127::reset()
{
#if DEBUG_BC127
  Serial.println("BC127 | CMD RESET");
#endif

  static String buffer = "";
  static String EOL    = String("Ready\r");

  enterCommandMode();

  _serialPort->print("RESET\r");
  _serialPort->flush();

  static unsigned long startingMillis = millis();

  while (!_serialPort->available())
  {
    Particle.process();

    if (millis() - startingMillis > COMMAND_TIMEOUT_DELAY)
    {
#if DEBUG_BC127
      Serial.print(convertCommandResultToString(TIMEOUT_ERROR));
#endif
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
#if DEBUG_BC127
      Serial.print(convertCommandResultToString(TIMEOUT_ERROR));
#endif
      return TIMEOUT_ERROR;
    }

    if (_serialPort->available())
    {
      startingMillis = millis();
      buffer.concat(char(_serialPort->read()));
    }
  }

  if (buffer.startsWith("Sierra"))
  {
#if DEBUG_BC127
    Serial.print(buffer + '\r');
#endif
    buffer = "";

    // _inDataMode = false;
    return SUCCESS;
  }

  if (buffer.startsWith("ER"))
  {
    static opResult error = evaluateError(buffer.substring(6));
    buffer = "";
#if DEBUG_BC127
    Serial.print(convertCommandResultToString(error));
#endif
    return error;
  }
  buffer = "";
#if DEBUG_BC127
  Serial.print(convertCommandResultToString(MODULE_ERROR));
#endif
  return MODULE_ERROR;
}

void BC127::handleBC127ConnectionEvent()
{
  // Serial.println("BC127 | Connection event!");
  // enterCommandMode();
  // Serial.println("BC127 | Connnection event: " +
  // convertCommandResultToString(status()));
}

void BC127::handleBC127DisconnectionEvent()
{
  // Serial.println("BC127 | Disconnnection event!");
  // enterCommandMode();
  // Serial.println("BC127 | Discnnnection event: " +
  // convertCommandResultToString(status()));
}

void BC127::handleBC127EventBitmaskEvent()
{
  // Serial.println("BC127 | Event bitmask event occured");
}
