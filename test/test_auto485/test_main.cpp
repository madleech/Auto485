/*
    Native unit tests for Auto485.

    These run off-device via PlatformIO's `native` platform. The Arduino
    runtime is mocked (see test/mock/Arduino.h): pinMode/digitalWrite record pin
    state and HardwareSerial is a concrete buffer. Auto485's whole job is to
    drive a half-duplex RS485 transceiver's DE/RE pins - HIGH to transmit, LOW
    to receive - so the tests write/flush through the wrapper and assert on the
    recorded pin levels and the bytes handed to the underlying serial port.

    Pin convention: DE (and RE, if separate) is HIGH in TX mode, LOW in RX.
*/

#include <unity.h>

#include "Arduino.h"
#include "Auto485.h"

// Referenced by the Auto485 constructor's default argument. The tests always
// pass an explicit serial port, but the symbol still has to exist.
HardwareSerial Serial;

// Storage for the pin state the mock records.
std::map<int, int> pin_modes;
std::map<int, int> pin_states;

const int DE_PIN = 2;
const int RE_PIN = 3;

void setUp(void)
{
	pin_modes.clear();
	pin_states.clear();
}

void tearDown(void)
{
}

// --- setup / teardown ------------------------------------------------------

// begin() configures the DE pin as an output and starts the serial port.
void test_begin_configures_pin_and_serial(void)
{
	HardwareSerial serial;
	Auto485 bus(DE_PIN, -1, serial);

	bus.begin(9600);

	TEST_ASSERT_EQUAL_INT(OUTPUT, pin_modes[DE_PIN]);
	TEST_ASSERT_TRUE(serial.started);
	TEST_ASSERT_EQUAL_UINT32(9600, serial.baud);
}

// end() returns the transceiver to receive mode and stops the serial port.
void test_end_returns_to_receive(void)
{
	HardwareSerial serial;
	Auto485 bus(DE_PIN, -1, serial);

	bus.begin(9600);
	bus.write('A'); // now transmitting (DE HIGH)
	bus.end();

	TEST_ASSERT_EQUAL_INT(LOW, pin_states[DE_PIN]);
	TEST_ASSERT_TRUE(serial.ended);
}

// --- transmit / receive switching ------------------------------------------

// write() flips into transmit mode and forwards the byte to the serial port.
void test_write_enables_transmit(void)
{
	HardwareSerial serial;
	Auto485 bus(DE_PIN, -1, serial);
	bus.begin(9600);

	bus.write('A');

	TEST_ASSERT_EQUAL_INT(HIGH, pin_states[DE_PIN]);
	TEST_ASSERT_EQUAL_UINT(1u, serial.tx.size());
	TEST_ASSERT_EQUAL_UINT8('A', serial.tx[0]);
}

// flush() drains the serial port and drops back to receive mode.
void test_flush_returns_to_receive(void)
{
	HardwareSerial serial;
	Auto485 bus(DE_PIN, -1, serial);
	bus.begin(9600);

	bus.write('A');
	bus.flush();

	TEST_ASSERT_EQUAL_INT(LOW, pin_states[DE_PIN]);
	TEST_ASSERT_TRUE(serial.flush_count > 0);
}

// A trailing println() sends its bytes then returns the bus to receive mode,
// so the line is complete before anyone else can drive the bus.
void test_println_returns_to_receive(void)
{
	HardwareSerial serial;
	Auto485 bus(DE_PIN, -1, serial);
	bus.begin(9600);

	bus.println("hi");

	TEST_ASSERT_EQUAL_INT(LOW, pin_states[DE_PIN]);
	// 'h', 'i', '\r', '\n'
	TEST_ASSERT_EQUAL_UINT(4u, serial.tx.size());
	TEST_ASSERT_EQUAL_UINT8('h', serial.tx[0]);
	TEST_ASSERT_EQUAL_UINT8('i', serial.tx[1]);
}

// Manual set_mode(): a TX -> RX transition flushes the serial port so the last
// bytes clear the shift register before the driver is disabled.
void test_set_mode_tx_to_rx_flushes(void)
{
	HardwareSerial serial;
	Auto485 bus(DE_PIN, -1, serial);
	bus.begin(9600);

	bus.set_mode(Auto485::TX);
	TEST_ASSERT_EQUAL_INT(HIGH, pin_states[DE_PIN]);
	int before = serial.flush_count;

	bus.set_mode(Auto485::RX);
	TEST_ASSERT_EQUAL_INT(LOW, pin_states[DE_PIN]);
	TEST_ASSERT_TRUE(serial.flush_count > before);
}

// --- separate DE / RE pins -------------------------------------------------

// With distinct DE and RE pins, both are configured as outputs and toggle
// together with the mode.
void test_separate_de_re_pins_toggle_together(void)
{
	HardwareSerial serial;
	Auto485 bus(DE_PIN, RE_PIN, serial);
	bus.begin(9600);

	TEST_ASSERT_EQUAL_INT(OUTPUT, pin_modes[DE_PIN]);
	TEST_ASSERT_EQUAL_INT(OUTPUT, pin_modes[RE_PIN]);

	bus.write('A');
	TEST_ASSERT_EQUAL_INT(HIGH, pin_states[DE_PIN]);
	TEST_ASSERT_EQUAL_INT(HIGH, pin_states[RE_PIN]);

	bus.flush();
	TEST_ASSERT_EQUAL_INT(LOW, pin_states[DE_PIN]);
	TEST_ASSERT_EQUAL_INT(LOW, pin_states[RE_PIN]);
}

// --- receive path ----------------------------------------------------------

// Reads pass straight through to the underlying serial port.
void test_read_passthrough(void)
{
	HardwareSerial serial;
	Auto485 bus(DE_PIN, -1, serial);
	bus.begin(9600);

	serial.feed('X');
	serial.feed('Y');

	TEST_ASSERT_EQUAL_INT(2, bus.available());
	TEST_ASSERT_EQUAL_INT('X', bus.peek());
	TEST_ASSERT_EQUAL_INT('X', bus.read());
	TEST_ASSERT_EQUAL_INT('Y', bus.read());
	TEST_ASSERT_EQUAL_INT(-1, bus.read());
}

int main(int, char **)
{
	UNITY_BEGIN();
	RUN_TEST(test_begin_configures_pin_and_serial);
	RUN_TEST(test_end_returns_to_receive);
	RUN_TEST(test_write_enables_transmit);
	RUN_TEST(test_flush_returns_to_receive);
	RUN_TEST(test_println_returns_to_receive);
	RUN_TEST(test_set_mode_tx_to_rx_flushes);
	RUN_TEST(test_separate_de_re_pins_toggle_together);
	RUN_TEST(test_read_passthrough);
	return UNITY_END();
}
