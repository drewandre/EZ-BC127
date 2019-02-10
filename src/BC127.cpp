#include "BC127.h"

BC127::BC127(int commandPin,
             int gpioZero,
             USARTSerial *serial,
             int serialBaud,
             String deviceName)
{
    _commandPin = commandPin;
    _gpioZero = gpioZero;
    _deviceName = deviceName;
    _serialPort = serial;
    _serialBaud = serialBaud;
}

BC127::~BC127() {}

void BC127::enable()
{
    Serial.print("Setting command pin (" + String(_commandPin) + ") to OUTPUT\r");
    pinMode(_commandPin, OUTPUT);
    Serial.println("Setting GPIO 0 pin (" + String(_gpioZero) + ") to OUTPUT");
    pinMode(_gpioZero, OUTPUT);
    // digitalWrite(_gpioZero, HIGH); // HIGH: RESTORE BC127 ON STARTUP

    Serial.println("Initializing " + String(_deviceName) + " serial at " + String(_serialBaud));
    _serialPort->begin(_serialBaud);

    reset();

    disableGPIOControl();
    getGPIOConfig();
    resetPIO4();

    getName();

    setName(_deviceName, false);
    setShortName(_deviceName, false);

    setMaxNumOfReconnectionAttempts(255, false);
    // setUARTConfig();
    // getUARTConfig();

    disableiOSBatteryIndicator();
    // disableAdvertisingOnStartup();

    enableConnectableAndDiscoverable();

    enableBLEAdvertising();
    enableAutoConn(false);
    enableAutoData(false);
    getBLEConfig();
    enableHDAudio();

    write();
}

BC127::opResult BC127::restore()
{
#if DEBUG_BC127
    Serial.println("Restoring BC127...\n");
#endif

    String buffer = "";
    String EOL = String("Ready\r");

    _serialPort->print("RESTORE");
    _serialPort->print("\r");
    _serialPort->flush();

    static unsigned long startingMillis = millis();
    while (!_serialPort->available())
    {
        Particle.process();
        if (millis() - startingMillis > TIMEOUT_DELAY)
            return TIMEOUT_ERROR;
    }

    buffer.concat(char(_serialPort->read()));

    startingMillis = millis();
    while (!buffer.endsWith(EOL))
    {
        Particle.process();
        if (millis() - startingMillis > TIMEOUT_DELAY)
            return TIMEOUT_ERROR;

        if (_serialPort->available())
        {
            startingMillis = millis();
            buffer.concat(char(_serialPort->read()));
        }
    }

    if (buffer.startsWith("Sierra"))
    {
        buffer = "";
        return SUCCESS;
    }

    if (buffer.startsWith("ER"))
    {
        static opResult error = evaluateError(buffer.substring(6));
        buffer = "";
        return error;
    }
    buffer = "";
    return MODULE_ERROR;
}

BC127::opResult BC127::reset()
{
#if DEBUG_BC127
    Serial.println("Resetting BC127...\n");
#endif

    String buffer = "";
    String EOL = String("Ready\r");

    _serialPort->print("RESET");
    _serialPort->print("\r");
    _serialPort->flush();

    static unsigned long startingMillis = millis();
    while (!_serialPort->available())
    {
        Particle.process();
        if (millis() - startingMillis > TIMEOUT_DELAY)
            return TIMEOUT_ERROR;
    }

    buffer.concat(char(_serialPort->read()));

    startingMillis = millis();
    while (!buffer.endsWith(EOL))
    {
        Particle.process();
        if (millis() - startingMillis > TIMEOUT_DELAY)
            return TIMEOUT_ERROR;

        if (_serialPort->available())
        {
            startingMillis = millis();
            buffer.concat(char(_serialPort->read()));
        }
    }

    if (buffer.startsWith("Sierra"))
    {
        buffer = "";
        return SUCCESS;
    }

    if (buffer.startsWith("ER"))
    {
        static opResult error = evaluateError(buffer.substring(6));
        buffer = "";
        return error;
    }
    buffer = "";
    return MODULE_ERROR;
}