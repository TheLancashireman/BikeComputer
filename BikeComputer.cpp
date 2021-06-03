/* BikeComputer - a bike computer with GPS receiver and route loggergps_decode(gpsbuf);
 *
 * (c) David Haworth
 *
 *	This file is part of BikeComputer.
 *
 *	BikeComputer is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	BikeComputer is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with BikeComputer.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <Arduino.h>
#include <ctype.h>
#include <pcf2119r.h>
#include "BikeComputer.h"

char gpsbuf[85];
pcf2119r lcd;
uint8_t modes;

// Low-overhead debugging
uint8_t nblip;
void blip(char c)
{
	Serial.write(c);
	nblip++;
	if ( nblip >= 80 )
	{
		nblip = 0;
		Serial.write('\r');
		Serial.write('\n');
	}
	lcd.setCursor(14,1);
	lcd.write(c);
}

// Display/log SD card problems
void FmStatus(uint8_t err)
{
	lcd.setCursor(11,1);
	if ( err != 0 )
	{
		lcd.print(err);
	}
	else
	{
		lcd.write(' ');
		lcd.write(' ');
		lcd.write(' ');
	}
}

// The real main()
void setup(void)
{
	modes |= MODE_CHANGE;
	uint8_t char_cnt = 0xff;
	
	pinMode(LED1, OUTPUT);
    digitalWrite(LED1, LOW);

	Serial.begin(9600);

	int status = lcd.begin(16, 2);
	status = lcd.command(0x35);		// Switch to extended command mode
	status = lcd.command(0x80+31);	// Set Va = 4.3v
	lcd.command(0xc0+31);			// Set Vb = 4.3v
	status = lcd.command(0x34);		// Switch to normal command mode
	status = lcd.display();			// Turn on display
	lcd.clear_row(0);				// Clear display. clear() doesn't work in this hardware
	lcd.clear_row(1);
	lcd.setCursor(0,0);				// Boring welcome message
	lcd.print(F("Bike Computer!"));

	status = fm_init();				// Initialise SD card interface
	FmStatus(status);
	if ( status == 0 )
	{
		delay(1000);
		status = fm_open();
		FmStatus(status);
		if ( status == 0 )
		{
			modes |= MODE_LOGGING;
		}
	}

	tm_init();
	btn_init();

	lcd.setCursor(15,1);
	lcd.write((modes & MODE_LOGGING) == 0 ? '-' : '+');

	for (;;)
	{
		uint8_t elapsed = tm_elapsed();
		uint8_t btn = btn_read(elapsed);

		if ( btn == 1 )
		{
			blip('1');
			if ( modes & MODE_LOGGING )
			{
				modes &= ~MODE_LOGGING;
				fm_close();
				lcd.setCursor(15,1);
				lcd.write('-');
			}
			else
			{
				status = fm_open();
				FmStatus(status);
				if ( status == 0 )
				{
					modes |= MODE_LOGGING;
					lcd.setCursor(15,1);
					lcd.write('+');
				}
			}
		}

		int ch = Serial.read();
		if ( ch >= 0 )
		{
			if ( ch == '$' )
			{
				gpsbuf[0] = '$';
				char_cnt = 1;
			}
			else
			if ( ch == '\0' || ch == '\r' || ch == '\n' )
			{
				gpsbuf[char_cnt++] = '\n';
				gpsbuf[char_cnt] = '\0';

				uint8_t gp_type = gps_decode(gpsbuf);

				if ( gp_type == GP_RMC )
				{
					if ( modes & MODE_LOGGING )
					{
						uint8_t q = fm_write(gpsbuf);
						if ( q > 250 )
						{
							FmStatus(q);
						}
						else
						{
							FmStatus(0);
						}
					}
				}

				char_cnt = 255;		// Wait for next sentence
			}
			else
			if ( char_cnt < 80 )
			{
				gpsbuf[char_cnt] = ch;
				char_cnt++;
			}
			else
			if ( char_cnt != 255 )
			{	// Over-length line
				blip('!');
			}
		}
	}
}

void loop() {}

// Display the time in a GPRMC record
void display_time(const char *b)
{
	lcd.setCursor(0,1);
	if ( isdigit(b[0]) && isdigit(b[1]) && isdigit(b[2]) && isdigit(b[3]) && isdigit(b[4]) && isdigit(b[5]) )
	{
		lcd.write(b[0]);
		lcd.write(b[1]);
		lcd.write(':');
		lcd.write(b[2]);
		lcd.write(b[3]);
		lcd.write(':');
		lcd.write(b[4]);
		lcd.write(b[5]);
	}
	else
	{
		lcd.write("--:--:--");
	}
}

// Disply part of co-ordinates
// Format is dddd.pppp,N
// dddd = degrees and minutes; minutes must be 2 digits, degrees could be 1 to 3 digits.
// After the comma, N, S, E or W
void display_degrees(const char *b)
{
	uint8_t i, dp, comma, col;
	char x;
	uint16_t f;

	for ( i = 0; b[i] != '.'; i++ )		// Find the decimal point
	{
		if ( !isdigit(b[i]) )
			return;						// Not-a-digit found before dp; bad data
	}

	if ( i < 3 )
		return;							// Not enough digits; bad data

	dp = i;								// Index of dp.
	i++;								// Skip the dp

	for ( ; b[i] != ','; i++ )			// Find the comma
	{
		if ( !isdigit(b[i]) )
			return;						// Not-a-digit found before comma; bad data
	}

	comma = i;							// Index of comma
	i++;								// Skip the comma

	if ( b[i] == 'N' || b[i] == 'S' )	// Latitude?
		col = 0;						// Display latitude at column 0
	else
	if ( b[i] == 'E' || b[i] == 'W' )	// Longitude?
		col = 8;						// Display longitude at column 8
	else
		return;							// Unrecognised direction; bad data
	
	lcd.setCursor(col,0);				// Cursor to start of output field
	for ( uint8_t j = 0; j < 8; j++ )	// Clear output field (half of top line)
		lcd.write(' ');

	lcd.setCursor(col,0);				// Cursor to start of output field
	lcd.write(b[i]);					// Direction
	for ( i = 0; i < (dp - 2); i++ )	// Degrees
		lcd.write(b[i]);

	// Convert minutes and fractional minutes to 1000ths of degrees
	// Need 1000ths of minutes divided by 60
	// Equivalent to 100ths of minutes dvided by 6
	// Need whole minutes and first 2 digits after dp
	f = digit_to_num(b[dp-2]) * 10 + digit_to_num(b[dp-1]);
	f = f * 10;
	if ( isdigit(b[dp+1]) )
		f += digit_to_num(b[dp+1]);
	f = f * 10;
	if ( isdigit(b[dp+2]) )
		f += digit_to_num(b[dp+2]);		// f is now in 100ths of minutes
	f = f/6;							// f is now in 1000ths of degrees

	lcd.write('.');
	x = f/100;
	lcd.write(x + '0');
	x = (f/10) - 10*x;
	lcd.write(x + '0');
	lcd.write(f % 10 + '0');
}

// Display speed
// Speed is given in knots (dd.ff)
// Have to convert to km/h
void display_speed(const char *b)
{
	uint8_t kn = 0;
	uint16_t f_kn = 0;
	uint16_t div = 1;
	uint8_t i;

	// Work in integers for the whole number and fractional parts
	for ( i = 0; b[i] != '.'; i++ )		// Whole number part
	{
		if ( !isdigit(b[i]) )
			return;						// Not-a-digit found; bad data
		kn = kn * 10 + digit_to_num(b[i]);
	}
	i++;								// Skip over dp
	for ( ; b[i] != ','; i++ )			// Fractional part
	{
		if ( !isdigit(b[i]) )
			return;						// Not-a-digit found; bad data
		f_kn = f_kn * 10 + digit_to_num(b[i]);
		div = div * 10;
	}

	double speed = (double)kn + (double)f_kn / (double)div;		// knots
	speed = speed * 1.852001;									// km/h

	lcd.setCursor(0,0);					// Cursor to start of output field
	lcd.print(speed, 2);
	lcd.print(F(" km/h "));
}

// Display heading
// Copy field to display. At most 5 characters, stop when not a digit or dp.
void display_heading(const char *b)
{
	uint8_t i;
	lcd.setCursor(11,0);				// Cursor to start of output field
	for ( i = 0; i < 5; i++ )			// At most 5 characters
	{
		if ( b[i] == '.' || isdigit(b[i]) )
			lcd.write(b[i]);
		else
			return;
	}
}

// Display date
// Format of input is DDMMYY
// Write in ISO form YYYY-MM-DD (assume 20xx)
void display_date(const char *b)
{
	uint8_t i;

	for ( i = 0; i < 6; i++ )
	{
		if ( !isdigit(b[i]) )
			return;
	}
	lcd.setCursor(0,0);					// Cursor to start of output field
	lcd.write('2');
	lcd.write('0');
	lcd.write(b[4]);
	lcd.write(b[5]);
	lcd.write('-');
	lcd.write(b[2]);
	lcd.write(b[3]);
	lcd.write('-');
	lcd.write(b[0]);
	lcd.write(b[1]);
}

#if DBG
// Host debugging
#include <stdio.h>
void debug_print(const char *s)
{
	printf("\nDBG: %s\n", s);
}
#endif
