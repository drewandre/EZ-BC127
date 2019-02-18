#include "BC127.h"

String BC127::getName()
{
  return stdGetParam("NAME");
}

String BC127::getNameShort()
{
  return stdGetParam("NAME_SHORT");
}

String BC127::getGPIOConfig()
{
  return stdGetParam("GPIO_CONFIG");
}

String BC127::getUARTConfig()
{
  return stdGetParam("UART_CONFIG");
}

String BC127::getBLEConfig()
{
  return stdGetParam("BLE_CONFIG");
}
