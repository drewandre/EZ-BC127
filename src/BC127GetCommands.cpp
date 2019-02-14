#include "BC127.h"

String BC127::getName()
{
  static String result = stdGetParam("NAME");

#if DEBUG_BC127
  Serial.print("BC127 | GET NAME..." + result + "\r");
#endif

  return result;
}

String BC127::getNameShort()
{
  static String result = stdGetParam("NAME_SHORT");

#if DEBUG_BC127
  Serial.print("BC127 | GET NAME_SHORT..." + result + "\r");
#endif

  return result;
}

String BC127::getGPIOConfig()
{
  static String result = stdGetParam("GPIO_CONFIG");

#if DEBUG_BC127
  Serial.print("BC127 | GET GPIO_CONFIG..." + result + "\r");
#endif

  return result;
}

String BC127::getUARTConfig()
{
  static String result = stdGetParam("UART_CONFIG");

#if DEBUG_BC127
  Serial.print("BC127 | GET UART_CONFIG..." + result + "\r");
#endif

  return result;
}

String BC127::getBLEConfig()
{
  static String result = stdGetParam("BLE_CONFIG");

#if DEBUG_BC127
  Serial.print("BC127 | GET BLE_CONFIG..." + result + "\r");
#endif

  return result;
}
