
#ifndef PCF2119R_H
#define PCF2119R_H	1

#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Clcd.h>

class pcf2119r : public hd44780
{
	size_t write(uint8_t value);	// Overrides hd44780:write(uint8_t)
};

#endif
