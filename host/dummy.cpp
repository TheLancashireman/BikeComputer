#include <Arduino.h>
#include "BikeComputer.h"
#include <string.h>

void tm_init(void)
{
}

uint8_t tm_elapsed(void)
{
	return 10;
}

void btn_init(void)
{
}

uint8_t btn_read(uint8_t tm)
{
	return 0;
}

uint8_t fm_init(void)
{
	return 0;
}

uint8_t fm_open(void)
{
	return 0;
}

uint8_t fm_close(void)
{
	return 0;
}

uint8_t fm_write(char const *s)
{
	return strlen(s);
}
