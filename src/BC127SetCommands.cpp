#include "BC127.h"

BC127::opResult BC127::enableGPIOControl()
{
  static opResult result = stdSetParam("GPIO_CONFIG", "ON 0");
#if DEBUG_BC127
  Serial.print("Enabling GPIO control: GPIO_CONFIG ON 0 -> " + commandResult(result) + "\r");
#endif
  return result;
}

BC127::opResult BC127::disableGPIOControl()
{
  static opResult result = stdSetParam("GPIO_CONFIG", "OFF 8");
#if DEBUG_BC127
  Serial.print("Disabling GPIO control..." + commandResult(result) + "\r");
#endif
  return result;
  // return stdSetParam("GPIO_CONFIG", "OFF 080");
  // return stdCmd("SET GPIO_CONFIG=OFF 8");
}

BC127::opResult BC127::enableAutoData(bool shouldReboot)
{
  static opResult result = stdSetParam("AUTO_DATA", "ON ON");

#if DEBUG_BC127
  Serial.print("Enabling auto data mode on SPP and BLE connections: AUTO_DATA ON ON -> " + commandResult(result) + "\r");
#endif

  if (shouldReboot)
    reset();

  return result;
}

BC127::opResult BC127::disableAutoData(bool shouldReboot)
{
  static opResult result = stdSetParam("AUTO_DATA", "ON ON");

#if DEBUG_BC127
  Serial.print("Disabling auto data mode on SPP and BLE connections: AUTO_DATA OFF OFF -> " + commandResult(result) + "\r");
#endif

  if (shouldReboot)
    reset();

  return result;
}

BC127::opResult BC127::enableAutoConn(bool shouldReboot)
{
  static opResult result = stdSetParam("AUTOCONN", "ON ON");

#if DEBUG_BC127
  Serial.print("Enabling auto connection on startup: AUTOCONN ON ON -> " + commandResult(result) + "\r");
#endif

  if (shouldReboot)
    reset();

  return result;
}

BC127::opResult BC127::disableAutoConn(bool shouldReboot)
{
  static opResult result = stdSetParam("AUTOCONN", "OFF OFF");

#if DEBUG_BC127
  Serial.print("Disabling auto connection on startup: AUTOCONN OFF OFF -> " + commandResult(result) + "\r");
#endif

  if (shouldReboot)
    reset();

  return result;
}

BC127::opResult BC127::enableHDAudio()
{
  static opResult result = stdSetParam("CODEC", "3");

#if DEBUG_BC127
  Serial.print("Enabling HD audio streaming: CODEC 3 -> " + commandResult(result) + "\r");
#endif
  return result;
}

BC127::opResult BC127::disableAdvertisingOnStartup()
{
  static opResult result = stdSetParam("BT_STATE_CONFIG", "0 0");

#if DEBUG_BC127
  Serial.print("Disabling advertising on startup: BT_STATE_CONFIG 0 0 -> " + commandResult(result) + "\r");
#endif
  return result;
}

BC127::opResult BC127::disableiOSBatteryIndicator()
{
  static opResult result = stdSetParam("ENABLE_BATT_IND", "OFF");
#if DEBUG_BC127
  Serial.print("Disabling iOS battery indicator: ENABLE_BATT_IND OFF -> " + commandResult(result) + "\r");
#endif
  return result;
}

BC127::opResult BC127::disableAdvertising()
{
  static opResult result = stdSetParam("BLE_CONFIG", "0 OFF 40 ON");

#if DEBUG_BC127
  Serial.print("Disabling advertising BLE_CONFIG 0 OFF 40 ON -> " + commandResult(result) + "\r");
#endif
  return result;
}

BC127::opResult BC127::setName(String name, bool shouldReboot)
{
  // enterCommandMode();

  static opResult result = stdSetParam("NAME", name);

#if DEBUG_BC127
  Serial.print("Setting device name to " + String(name) + ": NAME" + name + " -> " + commandResult(result) + "\r");
#endif

  if (shouldReboot)
  {
    reset();
  }
  else
  {
    // stdCmd("ENTER_DATA_MODE 14");
    // exitCommandMode();
  }

  return result;
}

BC127::opResult BC127::setShortName(String name, bool shouldReboot)
{
  static opResult result = stdSetParam("NAME_SHORT", name);

#if DEBUG_BC127
  Serial.print("Setting device short name to " + String(name) + ": NAME_SHORT " + name + " -> " + commandResult(result) + "\r");
#endif

  if (shouldReboot)
    reset();

  return result;
}

BC127::opResult BC127::setMaxNumOfReconnectionAttempts(int maxNum, bool shouldReboot)
{
  static opResult result = stdSetParam("MAX_REC", String(maxNum));

#if DEBUG_BC127
  Serial.print("Setting max number of reconnect attempts to " + String(maxNum) + ": MAX_REC " + String(maxNum) + " -> " + commandResult(result) + "\r");
#endif

  if (shouldReboot)
    reset();

  return result;
}

BC127::opResult BC127::setUARTConfig(int baudRate)
{
  static String uartConfig = String(baudRate) + " OFF 0";
  static opResult result = stdSetParam("UART_CONFIG", uartConfig);

#if DEBUG_BC127
  Serial.print("Setting UART baudrate to " + String(baudRate) + " and enabling flow control: UART_CONFIG " + uartConfig + " " + commandResult(result) + "\r");
#endif
  return result;
}
