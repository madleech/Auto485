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

#ifndef Auto485_h
#define Auto485_h
	
#include <Arduino.h>

class Auto485 : public Stream
{
	public:
#if defined(USBCON)
		/* need to use Serial1 on Uno */
		Auto485(int DE_pin, int RE_pin = -1, HardwareSerial &serial_port = Serial1);
#else		
		Auto485(int DE_pin, int RE_pin = -1, HardwareSerial &serial_port = Serial);
#endif
		enum    Mode {TX, RX};
		
		void    set_mode(enum Mode);
		size_t  write(uint8_t);
		void    flush(void);
		void    begin(unsigned long);
		void    begin(unsigned long, uint8_t);
		void    end(void);
		int     available(void);
		int     peek(void);
		int     read(void);
		size_t println(const __FlashStringHelper *);
    		size_t println(const String &s);
    		size_t println(const char[]);
    		size_t println(char);
    		size_t println(unsigned char, int = DEC);
    		size_t println(int, int = DEC);
    		size_t println(unsigned int, int = DEC);
    		size_t println(long, int = DEC);
    		size_t println(unsigned long, int = DEC);
    		size_t println(double, int = 2);
    		size_t println(const Printable&);
    		size_t println(void);
		using   Print::println;
		
private:
		int     _DE_pin;
		int     _RE_pin;
		enum    Mode _mode;
		HardwareSerial& _serial;
};

#endif
