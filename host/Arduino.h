#ifndef ARDUINO_H
#define ARDUINO_H	1

#include <stdio.h>

#define LOW		0
#define HIGH	1
#define OUTPUT	0

#define F(x)	x

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;

static inline void pinMode(uint8_t pin, uint8_t mode)
{
	// Dummy
}

static inline void digitalWrite(uint8_t pin, uint8_t val)
{
	// Dummy
}

static inline void delay(unsigned millis)
{
	// Dummy
}

class arduino_serial
{
public:
	static inline void begin(int baud)
	{
	}
	static inline void write(char c)
	{
		putc(c, stdout);
	}
	static inline int read(void)
	{
		return -1;
	}
};

extern arduino_serial Serial;


#endif
