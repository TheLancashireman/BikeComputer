#include <Arduino.h>
#include "BikeComputer.h"
#include <string.h>
#include <stdio.h>

extern void setup(void);
extern void loop(void);

char buf[1025];

uint8_t sim_button;
int have_serial;
int serial_index;

arduino_serial Serial;

int main(int argc, char **argv)
{
	setup();
	for (;;)
	{
		// Read a line of input
		int n = scanf("%s", buf);
		printf("length %d, \"%s\"\n", n, buf);

		if ( buf[0] == '1' )
		{
			sim_button = 1;
		}
		else
		if ( buf[0] == '2' )
		{
			sim_button = 2;
		}
		else
		{
			have_serial = 1;
			serial_index = 0;
		}

		while ( have_serial || (sim_button != 0) )
			loop();
	}
}

int arduino_serial::read(void)
{
	int xx = -1;
	if ( have_serial )
	{
		if ( buf[serial_index] == '\0' )
		{
			xx = '\n';
			have_serial = 0;
		}
		else
		{
			xx = buf[serial_index++];
		}
	}

	return xx;
}

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
	uint8_t x = sim_button;
	sim_button = 0;
	return x;
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
	printf("\nfm_write(\"%s\")\n", s);
	return strlen(s);
}
