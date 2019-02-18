#include "BC127.h"

BC127::opResult BC127::enableGPIOControl()
{
  return stdSetParam("GPIO_CONFIG", "ON 0", false);
}

BC127::opResult BC127::disableGPIOControl()
{
  return stdSetParam("GPIO_CONFIG", "OFF 80", false);
}

BC127::opResult BC127::enableAutoData(bool shouldRestart)
{
  return stdSetParam("AUTO_DATA", "ON ON", shouldRestart);
}

BC127::opResult BC127::disableAutoData(bool shouldRestart)
{
  return stdSetParam("AUTO_DATA", "ON ON", shouldRestart);
}

BC127::opResult BC127::enableAutoConn(bool shouldRestart)
{
  return stdSetParam("AUTOCONN", "1", shouldRestart);
}

BC127::opResult BC127::disableAutoConn(bool shouldRestart)
{
  return stdSetParam("AUTOCONN", "0", shouldRestart);
}

BC127::opResult BC127::enableHDAudio()
{
  return stdSetParam("CODEC", "3", false);
}

BC127::opResult BC127::disableAdvertisingOnStartup()
{
  return stdSetParam("BT_STATE_CONFIG", "0 0", false);
}

BC127::opResult BC127::disableiOSBatteryIndicator()
{
  return stdSetParam("ENABLE_BATT_IND", "OFF", false);
}

BC127::opResult BC127::enableBLEAdvertising()
{
  return stdSetParam("BLE_CONFIG", "0 ON 100 ON", false);
}

BC127::opResult BC127::disableAdvertising()
{
  return stdSetParam("BLE_CONFIG", "0 OFF 100 ON", false);
}

BC127::opResult BC127::setName(String name, bool shouldRestart)
{
  return stdSetParam("NAME", name, shouldRestart);
}

BC127::opResult BC127::setShortName(String name, bool shouldRestart)
{
  return stdSetParam("NAME_SHORT", name, shouldRestart);
}

BC127::opResult BC127::setMaxNumOfReconnectionAttempts(int maxNum,
                                                       bool shouldRestart)
{
  return stdSetParam("MAX_REC", String(maxNum), shouldRestart);
}

BC127::opResult BC127::setUARTConfig()
{
  static String uartConfig = "SET UART_CONFIG=" + String(_serialBaud) + " OFF 0\r";

  enterCommandMode();

  Serial.println("BC127 | " + uartConfig);
  _serialPort->print(uartConfig);
  _serialPort->flush();
  // delay(500);
  // _serialPort->end();
  // _serialPort->begin(_serialBaud);

  // EEPROM.put(10, _serialBaud);

  write();
  reset();
  return SUCCESS;

  // if (stdSetParam("UART_CONFIG", uartConfig, false) == SUCCESS)
  // {
  //   _serialPort->end();
  //   _serialPort->begin(_serialBaud);
  //   write();
  //   reset();
  // }
}

BC127::opResult BC127::setVolConfig()
{
  static String volConfig = "A 60 10 0";
  return stdSetParam("BT_VOL_CONFIG", volConfig, false);
}

BC127::opResult BC127::setCodType()
{
  return stdSetParam("COD", _codType, false);
}
