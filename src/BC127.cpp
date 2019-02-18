#include "BC127.h"

BC127::BC127(int commandPin,
             int connectionIndicatorPin,
             int eventBitmaskPin,
             USARTSerial *serialPort,
             int serialBaud,
             String deviceName,
             void (*serialCallbackPointer)(String command))
{
  _commandPin = commandPin;
  _connectionIndicatorPin = connectionIndicatorPin;
  _eventBitmaskPin = eventBitmaskPin;
  _serialPort = serialPort;
  _serialBaud = serialBaud;
  _deviceName = deviceName;

  pinMode(_commandPin, OUTPUT);
  pinMode(_connectionIndicatorPin, INPUT_PULLDOWN);
  pinMode(_eventBitmaskPin, INPUT);

  // Toggle command pin to trigger enter-command-mode PIO event
  digitalWrite(_commandPin, HIGH);
  digitalWrite(_commandPin, LOW);
}

BC127::~BC127()
{
  _serialPort->end();
  disableInterrupts();
}

void BC127::enable()
{
  // long currentBaud;
  // EEPROM.get(10, currentBaud);
  // if ((currentBaud == 0xFFFF) || (currentBaud == -1))
  // {
  //   Serial.println("EEPROM 10 undefined");
  //   currentBaud = 9600;
  // }
  // else
  // {
  //   Serial.println("EEPROM 10 " + String(currentBaud));
  // }
  // Serial.println("GET currentBaud " + String(currentBaud));

  _serialPort->begin(_serialBaud);

  restore();

  // setUARTConfig();

  disableGPIOControl();

  setName(_deviceName, false);
  setShortName(_deviceName, false);

  getBLEConfig();

  disableiOSBatteryIndicator();
  enableHDAudio();
  setVolConfig();
  setCodType();

  write();
  reset();

  enableInterrupts();

  getName();

  enableConnectableAndDiscoverable();
  enableBLEAdvertising();

  getUARTConfig();

#ifdef DEBUG_BC127
  Serial.println("BC127 | Ready");
#endif
}

void BC127::runTask()
{
  listenAndHandleSPPData();
}

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

void BC127::listenAndHandleSPPData()
{
  static String buffer;

  if (_serialPort->available())
  {
    buffer.concat(char(_serialPort->read()));

    if (buffer.endsWith("\r"))
    {
      handleBC127Serial(buffer);
      // if (buffer.startsWith("ANIMATION "))
      // {
      //   stdCmd("SEND 14 ANIMATION CHANGED TO " + buffer.substring(10).trim());
      // }
      // else
      // {
      //   Serial.println(buffer);
      // }

      buffer = "";
    }
  }
}

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

void BC127::handleBC127EventBitmaskEvent()
{
  Serial.println("BC127 | PIO 4 -> Exited data mode");
  _inDataMode = false;
}
