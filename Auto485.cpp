/*
	Auto485 - a small helper library that takes some of the tedium out of RS485
	communication.
	Copyright (C) 2012 Michael Adams (www.michael.net.nz)
	All rights reserved.
	
	Permission is hereby granted, free of charge, to any person obtaining a 
	copy of this software and associated documentation files (the "Software"), 
	to deal in the Software without restriction, including without limitation 
	the rights to use, copy, modify, merge, publish, distribute, sublicense, 
	and/or sell copies of the Software, and to permit persons to whom the 
	Software is furnished to do so, subject to the following conditions:
	
	The above copyright notice and this permission notice shall be included 
	in all copies or substantial portions of the Software.
	
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
	OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
	SOFTWARE.
*/

#include "Auto485.h"
#include <Arduino.h>

Auto485::Auto485(int DE_pin, int RE_pin, HardwareSerial &serial_port)
: _DE_pin(DE_pin)
, _RE_pin(RE_pin == -1 ? DE_pin : RE_pin)
, _serial(serial_port)
, _mode(RX)
{
	pinMode(_DE_pin, OUTPUT); // defaults to LOW = read mode
	if (_DE_pin != _RE_pin)
		pinMode(_RE_pin, OUTPUT); // defaults to LOW = read mode
}

void Auto485::set_mode(enum Mode mode)
{
	if (_mode == TX && mode == RX)
		_serial.flush(); // finish writing if we're moving into RX mode
	
	_mode = mode;
	digitalWrite(_DE_pin, _mode == TX); // high if TX, low if RX
	if (_DE_pin != _RE_pin)
		digitalWrite(_RE_pin, _mode == TX); // high if TX, low if RX
}

void Auto485::begin(unsigned long baud)
{
	_serial.begin(baud);
}

void Auto485::begin(unsigned long baud, uint8_t config)
{
	_serial.begin(baud, config);
}

void Auto485::end(void) {
	_serial.end();
}

int Auto485::available(void)
{
	return _serial.available();
}

int Auto485::peek(void)
{
	return _serial.peek();
}

int Auto485::read(void)
{
	return _serial.read();
}


size_t Auto485::write(uint8_t c)
{
	if (_mode != TX) set_mode(TX);
	return _serial.write(c);
}

void Auto485::flush(void)
{
	_serial.flush();
	set_mode(RX);
}


size_t Auto485::println(void)
{
	 size_t n = Print::println();
         set_mode(RX);
	 return n;
}

size_t Auto485::println(const String &s)
{
	size_t n = Print::print(s);
	n += Print::println();
	set_mode(RX);
	return n;
}

size_t Auto485::println(const __FlashStringHelper *ifsh)
{
  size_t n = Print::print(ifsh);
  n += Print::println();
  set_mode(RX);
  return n;
}

size_t Auto485::println(const char c[])
{
  size_t n = Auto485::print(c);
  n += Auto485::println();
  set_mode(RX);
  return n;
}

size_t Auto485::println(char c)
{
  size_t n = Print::print(c);
  n += Print::println();
  set_mode(RX);
  return n;
}


size_t Auto485::println(const Printable& x)
{
  size_t n = Print::print(x);
  n += Print::println();
  set_mode(RX);
  return n;
}

size_t Auto485::println(unsigned char b, int base)
{
  size_t n = Print::print(b, base);
  n += Print::println();
  set_mode(RX);
  return n;
}

size_t Auto485::println(int num, int base)
{
  size_t n = Print::print(num, base);
  n += Print::println();
  set_mode(RX);
  return n;
}

size_t Auto485::println(unsigned int num, int base)
{
  size_t n = Print::print(num, base);
  n += Print::println();
  set_mode(RX);
  return n;
}

size_t Auto485::println(long num, int base)
{
  size_t n = Print::print(num, base);
  n += Print::println();
  set_mode(RX);
  return n;
}

size_t Auto485::println(unsigned long num, int base)
{
  size_t n = Print::print(num, base);
  n += Print::println();
  set_mode(RX);
  return n;
}

size_t Auto485::println(double num, int digits)
{
  size_t n = Print::print(num, digits);
  n += Print::println();
  set_mode(RX);
  return n;
}