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
#if DEBUG_BC127
    Serial.print("BC127 | Setting command pin (" + String(_commandPin) + ") to OUTPUT\r");
    Serial.print("BC127 | Setting GPIO 0 pin (" + String(_gpioZero) + ") to OUTPUT\r");
    Serial.print("BC127 | Initializing BC127 serial at " + String(_serialBaud) + '\r');
#endif

    pinMode(_commandPin, OUTPUT);
    pinMode(_gpioZero, OUTPUT);
    _serialPort->begin(_serialBaud);

    restore();
    // setUARTConfig(_serialBaud);

    getName();
    setName(_deviceName, false);
    setShortName(_deviceName, false);

    getBLEConfig();

    disableGPIOControl();
    disableiOSBatteryIndicator();

    resetPIO4();

    enableHDAudio();

    write();
    reset();

    enableConnectableAndDiscoverable();
    enableBLEAdvertising();

    enableAutoConn(false);
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
    static String EOL = String("Ready\r");

    _serialPort->print("RESTORE");
    _serialPort->print("\r");
    _serialPort->flush();

    static unsigned long startingMillis = millis();
    while (!_serialPort->available())
    {
        Particle.process();
        if (millis() - startingMillis > TIMEOUT_DELAY) {
        #if DEBUG_BC127
            Serial.print(commandResult(TIMEOUT_ERROR));
        #endif
            return TIMEOUT_ERROR;
        }
    }
    startingMillis = millis();

    buffer.concat(char(_serialPort->read()));

    while (!buffer.endsWith(EOL))
    {
        Particle.process();
        if (millis() - startingMillis > TIMEOUT_DELAY) {
        #if DEBUG_BC127
            Serial.print(commandResult(TIMEOUT_ERROR));
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
        return SUCCESS;
    }

    if (buffer.startsWith("ER"))
    {
        static opResult error = evaluateError(buffer.substring(6));
        buffer = "";
    #if DEBUG_BC127
        Serial.print(commandResult(error));
    #endif
        return error;
    }
    buffer = "";
#if DEBUG_BC127
    Serial.print(commandResult(MODULE_ERROR));
#endif
    return MODULE_ERROR;
}

BC127::opResult BC127::reset()
{
#if DEBUG_BC127
    Serial.println("BC127 | CMD RESET");
#endif

    static String buffer = "";
    static String EOL = String("Ready\r");

    _serialPort->print("RESET\r");
    _serialPort->flush();

    static unsigned long startingMillis = millis();
    while (!_serialPort->available())
    {
        Particle.process();
        if (millis() - startingMillis > TIMEOUT_DELAY) {
        #if DEBUG_BC127
            Serial.print(commandResult(TIMEOUT_ERROR));
        #endif
            return TIMEOUT_ERROR;
        }

    }
    startingMillis = millis();

    buffer.concat(char(_serialPort->read()));

    while (!buffer.endsWith(EOL))
    {
        Particle.process();
        if (millis() - startingMillis > TIMEOUT_DELAY) {
        #if DEBUG_BC127
            Serial.print(commandResult(TIMEOUT_ERROR));
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
        return SUCCESS;
    }

    if (buffer.startsWith("ER"))
    {
        static opResult error = evaluateError(buffer.substring(6));
        buffer = "";
    #if DEBUG_BC127
        Serial.print(commandResult(error));
    #endif
        return error;
    }
    buffer = "";
#if DEBUG_BC127
    Serial.print(commandResult(MODULE_ERROR));
#endif
    return MODULE_ERROR;
}