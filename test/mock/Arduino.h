/*
    Minimal Arduino.h mock for native unit tests.

    Provides just enough of the Arduino runtime for Auto485 to build and run
    off-device. Auto485 is a Stream wrapper that flips a transceiver's DE/RE
    pins between transmit and receive, so the mock supplies:

      - pinMode/digitalWrite that record pin state the test can inspect
      - the Print/Stream base classes Auto485 derives from
      - a concrete HardwareSerial backed by an output buffer and an input queue
*/

#ifndef _Auto485_test_Arduino_h
#define _Auto485_test_Arduino_h

#include <stdint.h>
#include <stddef.h>
#include <deque>
#include <map>
#include <string>
#include <vector>

// Pin levels and modes.
#define LOW 0
#define HIGH 1
#define INPUT 0
#define OUTPUT 1
#define INPUT_PULLUP 2

// Number bases for print().
#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2

// Serial config token; the mock never interprets it.
#define SERIAL_8N2 0x0E

// Recorded pin state, inspected by the tests. Defined in the test translation
// unit.
extern std::map<int, int> pin_modes;
extern std::map<int, int> pin_states;

inline void pinMode(int pin, int mode)
{
	pin_modes[pin] = mode;
}

inline void digitalWrite(int pin, int value)
{
	pin_states[pin] = value ? HIGH : LOW;
}

inline unsigned long millis()
{
	return 0;
}

// Forward declarations for the print() overloads Auto485 references.
class __FlashStringHelper;
class Print;

class String
{
	std::string _s;

  public:
	String()
	{
	}
	String(const char *s) : _s(s)
	{
	}
	const char *c_str() const
	{
		return _s.c_str();
	}
};

class Printable
{
  public:
	virtual size_t printTo(Print &) const = 0;
};

// Minimal Print: everything is funnelled through the pure-virtual write(), so
// deriving classes (Auto485) see a write() call for every byte printed - which
// is exactly what drives its TX/RX mode switching.
class Print
{
  public:
	virtual size_t write(uint8_t) = 0;
	virtual void flush()
	{
	}

	size_t print(const char *s)
	{
		size_t n = 0;
		while (*s)
			n += write((uint8_t)*s++);
		return n;
	}
	size_t print(char c)
	{
		return write((uint8_t)c);
	}
	size_t print(const String &s)
	{
		return print(s.c_str());
	}
	size_t print(const __FlashStringHelper *)
	{
		return 0;
	}
	size_t print(unsigned char b, int = DEC)
	{
		return write(b);
	}
	size_t print(int n, int = DEC)
	{
		return print_number(n);
	}
	size_t print(unsigned int n, int = DEC)
	{
		return print_number((long)n);
	}
	size_t print(long n, int = DEC)
	{
		return print_number(n);
	}
	size_t print(unsigned long n, int = DEC)
	{
		return print_number((long)n);
	}
	size_t print(double, int = 2)
	{
		return write('0');
	}
	size_t print(const Printable &x)
	{
		return x.printTo(*this);
	}
	size_t println()
	{
		return write('\r') + write('\n');
	}

  private:
	size_t print_number(long n)
	{
		char buf[16];
		size_t len = 0;
		if (n < 0)
		{
			write('-');
			len++;
			n = -n;
		}
		int i = 0;
		do
		{
			buf[i++] = '0' + (n % 10);
			n /= 10;
		} while (n);
		while (i > 0)
			len += write((uint8_t)buf[--i]);
		return len;
	}
};

class Stream : public Print
{
  public:
	virtual int available() = 0;
	virtual int read() = 0;
	virtual int peek() = 0;
};

// A concrete stand-in for the Arduino HardwareSerial. tx collects everything
// written; rx holds bytes the test feeds in for read().
class HardwareSerial : public Stream
{
  public:
	std::vector<uint8_t> tx;
	std::deque<uint8_t> rx;
	int flush_count = 0;
	bool started = false;
	bool ended = false;
	unsigned long baud = 0;

	void begin(unsigned long b)
	{
		started = true;
		ended = false;
		baud = b;
	}
	void begin(unsigned long b, uint8_t)
	{
		begin(b);
	}
	void end()
	{
		ended = true;
		started = false;
	}

	size_t write(uint8_t c)
	{
		tx.push_back(c);
		return 1;
	}
	void flush()
	{
		flush_count++;
	}
	int available()
	{
		return (int)rx.size();
	}
	int read()
	{
		if (rx.empty())
			return -1;
		uint8_t c = rx.front();
		rx.pop_front();
		return c;
	}
	int peek()
	{
		return rx.empty() ? -1 : rx.front();
	}

	// Test helper: enqueue a byte as if it had arrived over the wire.
	void feed(uint8_t c)
	{
		rx.push_back(c);
	}
};

// The default argument of the Auto485 constructor references Serial; the test
// translation unit defines it.
extern HardwareSerial Serial;

#endif
