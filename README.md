Auto485 - Automatic RS485 Wrapper Class
=======================================

* Author: Michael Adams (<http://www.michael.net.nz>)
* Copyright (C) 2012 Michael D K Adams.
* Released under the MIT license.

Auto485 is a small helper library that takes some of the tedium out of RS485 communication. It presents an identical interface to the standard `Serial` object, so you can `.println(...)`, `.available()`, `.read()` and so on, while under the hood, Auto485 will handle toggling between receive and transmit mode on the MAX485 or compatible chip.

If you prefer to do things manually, there is an explicit `.set_mode(..)` function that lets you toggle between tranmit and receive mode.

For documentation and examples please see the [project blog][1].

[1]: http://utrainia.michael.net.nz/46-simulating-railroad-crossing-lights

Requirements
------------
* An Arduino -- http://arduino.cc/
* A RS485 transceiver
* (optional) An RS485 to USB converter on your PC

Installation
------------
Download the ZIP archive (https://github.com/madleech/RS485/zipball/master) and extract it to your Arduino folder under `libraries/RS485`.

Restart the Arduino IDE and you should see in File > Examples > RS485 entires for 'hello world' or so on.


Code Examples
-------------
Here is the 'echo' example program, included in the download:

```Arduino
#include <Auto485.h>

#define DE_PIN 2
#define RE_PIN 3

Auto485 bus(DE_PIN, RE_PIN); // new Auto485 wrapper using DE_PIN & RE_PIN to toggle read/write mode on the MAX485

void setup() {
  bus.begin(9600); // open bus at 9600bps
  bus.println("Hello world, now listening"); // at the end of println, we return to listening
}

void loop() {
  if (bus.available() > 0) {
    while (bus.available() > 0)
      bus.write(bus.read()); // mode -> transmit
    bus.println(); // mode -> receive
  }
  delay(1000);
}
```

We start off by defining the Driver Enable pin (`DE_PIN`), and Receiver Enable pin (`DE_PIN`). Usually these pins will be connected together on the MAX485, so you can just use the same pin here.

Next we initialise the bus at 9600bps, just like you would with `Serial.begin(9600)`. Then we print out some text using `.println(...)`. Println handles the bus state for us, changing to TX mode then back to RX mode.

In our main loop we check if any input is available, and if it is, we set the bus to TX mode, write out the data, and return to RX mode.


Documentation
-------------
**Auto485(int DE_pin)**
Creates a new Auto485 object. The Driver and Receiver Enable pins on the MAX485 are connected together.

**Auto485(int DE_pin, int RE_pin)**
Creates a new Auto485 object. The Driver and Receiver Enable pins on the MAX485 are connected to separate pins on the Arduino.

**Auto485(int DE_pin, int RE_pin, HardwareSerial serial_port)**
Creates a new Auto485 object using the specified serial port. Usually the defaults are fine (Serial on most boards, Serial1 on the Leonardo and other USBCOM boards). If you are using a Mega with multiple serial ports this lets you choose which serial port to use.

**Auto485::TX**, **Auto485::RX**
These are constants to let you toggle the mode of the bus.

**set_mode(Auto485::TX)**, **set_mode(Auto485::RX)**
Manually change to transmit or receive mode. When returning to receive mode, the function will pause until all pending serial data has been sent.

**write(...)**, **print(...)**
When in receive mode, the first call to any output functions will change to transmit mode, then send out the data as expected. It handles all the formatting options of the regular Arduino `print` and `write` functions.

**flush()**
Finish writing data, then switch to receive mode. Usually the serial writing functions happen asynchronously, with no delay while the data is sent out the serial port. When we're operating in half duplex mode though, we need to wait for the data to finish being sent before we change the mode of the bus. By calling `.flush()` we ensure there is no unsent data in the buffer. Once all pending data has been sent, we automatically switch back to receive mode!

**println(...)**
Like the `write(...)` and `print(...)` functions, calling `println(...)` will automatically switch to transmit mode. Unlike the lower-level functions though, println will return to receive mode at the end of the line. This means you can easily print simple messages to the bus and everything operates as expected, but if you're sending data byte-by-byte, that functionality if there too and Auto485 won't toggle between RX and TX for every single byte you send.

License
-------
MIT license, see LICENSE file.