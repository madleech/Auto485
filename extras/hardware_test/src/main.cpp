/*
    Hardware smoke-test for Auto485.

    Drives a half-duplex RS485 transceiver (e.g. a MAX485) over the hardware
    Serial port, exercising both directions of the automatic DE/RE switching:

      - every second it transmits a heartbeat line (write + println), which
        flips the driver to TX and back to RX automatically
      - any bytes received are echoed straight back, and the trailing println()
        returns the bus to RX

    Wiring: Arduino pin 2 -> MAX485 DE and RE (tied together), Serial TX/RX ->
    the MAX485 DI/RO. Connect A/B to a second RS485 node or a USB RS485 adapter
    to see the traffic; without a peer this simply confirms the sketch builds,
    uploads and boots against the local source.

    Upload + monitor:
      pio run -d extras/hardware_test -e uno -t upload
      pio device monitor -b 9600
*/

#include <Arduino.h>
#include <Auto485.h>

const int DE_PIN = 2;

Auto485 bus(DE_PIN); // pin 2 -> MAX485 DE and RE

void setup()
{
	bus.begin(9600); // must match the speed of the other node
}

unsigned long last_beat = 0;

void loop()
{
	// Echo anything received back onto the bus.
	if (bus.available() > 0)
	{
		while (bus.available() > 0)
			bus.write(bus.read()); // switches to TX
		bus.println();             // completes the line and returns to RX
	}

	// Transmit a heartbeat once a second.
	if (millis() - last_beat >= 1000)
	{
		last_beat = millis();
		bus.print("beat ");
		bus.println(last_beat / 1000); // returns to RX when done
	}
}
