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

  funct = serialCallbackPointer;

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

  getName();

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

void BC127::listenAndHandleSPPData()
{
  static String buffer;
  if (_serialPort->available())
  {
    buffer.concat(char(_serialPort->read()));

    if (buffer.endsWith("\r"))
    {
      funct(buffer);
      buffer = "";
    }
  }
}