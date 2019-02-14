// Example usage for BC127 library by Drew André.

#include "BC127.h"

// Initialize objects from the lib
BC127 bC127;

void setup() {
  // Call functions on initialized library objects that require hardware
  bC127.begin();
}

void loop() {
  // Use the library's initialized objects and functions
  bC127.process();
}
