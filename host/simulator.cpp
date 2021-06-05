/* simulator.cpp - host-based test framework for some aspects of the BikeComputer
 *
 *  (c) David Haworth
 *
 *  This file is part of BikeComputer.
 *
 *  BikeComputer is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  BikeComputer is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with BikeComputer.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <Arduino.h>
#include "BikeComputer.h"
#include <string.h>
#include <stdio.h>

/* This is not an arduino simulator. It's a test framework for testing some aspects
 * of BikeComputer (mainly GPS decoding and display/mode management
 *
 * Principle of operation
 *	- The simulation reads a line from stdin. Until that line arrives, time stands still
 *	- If the line contains no characters, the loop() function is called and tm_elapsed() returns 100;
 *	- When the line has been read, the loop() function is called until the line has been processed.
 *	- If the line starts with 1 or 2 a button press is simulated. tm_elapsed() returns 100 and
 *		btn_read() returns 1 or 2
 *	- Otherwise, the line is fed to the application one character at a time when the application calls
 *		Serial.read(). During this time, each call to tm_elapsed() returns 1.
*/

extern void setup(void);
extern void loop(void);

char buf[1025];

uint8_t sim_button;
int have_serial;
int serial_index;
uint8_t elapsed;

arduino_serial Serial;

int main(int argc, char **argv)
{
	setup();
	for (;;)
	{
		// Read a line of input
		buf[0] = '\0';
		int n = scanf("%s", buf);

		if ( buf[0] == '\0' )
		{
			sim_button = 0;	
			elapsed = 100;
		}
		else
		if ( buf[0] == '1' )
		{
			sim_button = 1;
			elapsed = 100;
		}
		else
		if ( buf[0] == '2' )
		{
			sim_button = 2;
			elapsed = 100;
		}
		else
		{
			have_serial = 1;
			serial_index = 0;
			elapsed = 1;
		}

		do {						// loop() called at least once
			loop();
		} while ( have_serial || (sim_button != 0) );
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
	return elapsed;
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
