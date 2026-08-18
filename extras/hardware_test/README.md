# Test on Real Hardware

This is a tiny sketch to test the current library code on a real device. It
drives a half-duplex RS485 transceiver over the hardware Serial port: once a
second it transmits a heartbeat line, and it echoes any received bytes back onto
the bus. Both directions rely on Auto485 flipping the DE/RE pins automatically.

Wiring: Arduino pin 2 -> MAX485 DE and RE (tied together); Serial TX/RX -> the
MAX485 DI/RO.

Build + upload + monitor:
```
$ pio run -d extras/hardware_test -e uno -t upload
$ pio device monitor -b 9600
```

Meaningfully exercising the bus needs a second RS485 node (or a USB RS485
adapter) to talk to. Without one, this still serves as a compile/upload/boot
smoke test against the local library source.
```
$ pio run -d extras/hardware_test -e uno
...
SUCCESS
```
