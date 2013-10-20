#include <Auto485.h>

/*
 * Periodically writes some characters to the hardware serial port,
 * which should be hooked up to the MAX485.
 * 
 * Uses the "transparent" mode of Auto485, which automatically toggles between TX and RX mode.
 */

#define DE_PIN 2
#define RE_PIN 2 /* connect DE and RE on the MAX485 together */

Auto485 bus(DE_PIN, RE_PIN); // new Auto485 wrapper using DE_PIN & RE_PIN to toggle read/write mode on the MAX485

void setup() {
  bus.begin(9600); // start talking at 9600bps
}

int i = 0;
char x[4] = {'A', 'B', 'C', 'D'};

void loop() {
  tx(x[i++ % 4]);
  delay(500);
}

void tx(char c) {
  bus.write(c);
  bus.flush(); // since hardware serial is asynchronous. This ends the transmit cycle and returns to receive mode
}
