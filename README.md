# EZ-BC127

An Arduino library for controlling the [BC127](https://www.sierrawireless.com/products-and-solutions/embedded-solutions/products/bc127/) dual-mode Bluetooth Classic and Bluetooth Low Energy (BLE) module running the latest Melody 7.x firmware.

This library is under development and has only been tested on the Arduino-compatible [Particle Photon](https://www.particle.io/wifi/). I will continue to ensure Arduino compatibility in future commits. PRs are welcome!

The purpose of this library is to create an easy-to-use and performant interface for controlling the module, and for communicating with central devices. Some of the logic is borrowed from SparkFun's BC127 library, but this library is designed for Melody 5.0 which had a bug that prevented the device from exiting command mode. The only BC127 modules that you can buy that run on Melody 5.0 are ones embedded on the now-retired [SparkFun Bluetooth Audio Breakout](https://www.sparkfun.com/products/retired/11927). For that reason, it is strongly recommended that you use the official [BC127 Discovery Board](https://www.digikey.com/catalog/en/partgroup/bc127-discovery-board/44458?mpart=BC127-DISKIT001_6001098&vendor=1645) from Sierra Wireless. Otherwise, if you are able to secure one of these retired breakout board, you can try [upgrading the firmware](https://source.sierrawireless.com/resources/airprime/software/bc127-firmware-upgrade-tool/).

## Usage

Connect hardware as described below, add the BC127 library to your project and follow this simple example:```

```
#include “BC127.h”

#define BC127_GPIO_0 D2 // connect to PIO 0 on discovery board
#define BC127_GPIO_4 D3 // connect to PIO 4 on discovery board
#define BC127_GPIO_5 D4 // connect to PIO 5 on discovery board
#define BC127_SERIAL Serial1
#define BC127_SERIAL_BAUD 9600
#define DEVICE_NAME "MyBC127"

BC127 bc127(BC127_GPIO_5, // used for entering and exiting data mode
           BC127_GPIO_0, // alerts microntroller of a connection event 
           BC127_GPIO_4, // alerts microcontroller that data mode has been exited
           &BC127_SERIAL, // defines serial port used to control BC127
           BC127_SERIAL_BAUD, // serial baud rate for BC127 serial communication
           DEVICE_NAME, // device name as it appears on central device
           handleBC127Serial); // callback for alerting main program of data from data
           					// received from BC127 data mode

void setup() {
bC127.enable();
}

void loop() {
bC127.runTask();
}

void handleBC127Serial(String command) {
	// do something with data received while BC127
	// is in data mode
}

```

Here's how you can make changes to this library and eventually contribute those changes back.

To get started, [clone the library from GitHub to your local machine](https://help.github.com/articles/cloning-a-repository/).

Change the name of the library in `library.properties` to something different. You can add your name at then end.

Modify the sources in /src and /examples with the new behavior.

To compile an example, use `particle compile examples/usage` command in [Particle CLI](https://docs.particle.io/guide/tools-and-features/cli#update-your-device-remotely) or use our [Desktop IDE](https://docs.particle.io/guide/tools-and-features/dev/#compiling-code).

After your changes are done you can upload them with `particle library upload` or `Upload` command in the IDE. This will create a private (only visible by you) library that you can use in other projects. Do `particle library add BC127_myname` to add the library to a project on your machine or add the BC127_myname library to a project on the Web IDE or Desktop IDE.

At this point, you can create a [GitHub pull request](https://help.github.com/articles/about-pull-requests/) with your changes to the original library. 

If you wish to make your library public, use `particle library publish` or `Publish` command.

## LICENSE
Copyright 2019 Drew André
