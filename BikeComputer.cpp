/* BikeComputer - a bike computer with GPS receiver and route logger
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
uint8_t gpscnt;
pcf2119r lcd;
uint8_t modes;
uint16_t start_time;

// Display/log SD card problems
void FmStatus(uint8_t err)
{
	if ( err != 0 )
	{
		modes = modes | MODE_ERROR;		// All bits set.
		lcd.setCursor(0,0);
		lcd.print(F("File error "));
		lcd.print(err);
	}
}

// The real main()
void setup(void)
{
	start_time = 0xffff;

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
	delay(1000);

	status = fm_init();				// Initialise SD card interface
	FmStatus(status);
	if ( status == 0 )
	{
		delay(100);
		status = fm_open();
		FmStatus(status);
		if ( status == 0 )
		{
			modes |= MODE_LOGGING;
		}
	}

	tm_init();
	btn_init();
}

void loop()
{
	uint8_t elapsed = tm_elapsed();
	uint8_t btn = btn_read(elapsed);

	if ( btn != 0 )
		mode_control(btn);

	int ch = Serial.read();
	if ( ch >= 0 )
	{
		if ( ch == '$' )
		{
			gpsbuf[0] = '$';
			gpscnt = 1;
		}
		else
		if ( gpscnt > 0 )
		{
			if ( ch == '\0' || ch == '\r' || ch == '\n' )
			{
				gpsbuf[gpscnt++] = '\n';
				gpsbuf[gpscnt] = '\0';

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

				gpscnt = 0;		// Wait for next sentence
			}
			else
			if ( gpscnt < 80 )
			{
				gpsbuf[gpscnt] = ch;
				gpscnt++;
			}
			else
			{	// Over-length line
			}
		}
	}
}

void mode_control(uint8_t btn)
{
	if ( btn == 1 )
	{
		if ( (modes & MODE_DISPLAY_0) < MODE_MAX_0 )
		{
			modes += MODE_INC_0;
		}
		else
		{
			modes = modes & ~MODE_DISPLAY_0;
		}

		lcd.clear_row(0);
		if ( (modes & MODE_DISPLAY_0) == MODE_LOGGING_CTRL )
		{
			display_logging();
		}
		else
		if ( (modes & MODE_DISPLAY_0) == MODE_ENTER_MENU )
		{
			display_menu();
		}
		else
		{
			lcd.setCursor(0,0);
			lcd.print(F("---"));
		}
	}
	else
	if ( btn == 2 )
	{
		if ( (modes & MODE_DISPLAY_0) == MODE_LOGGING_CTRL )
		{
			modes ^= MODE_LOGGING;
			display_logging();
			if ( modes & MODE_LOGGING )
			{	// Logging turned on
				int status = fm_open();
				FmStatus(status);
				if ( status != 0 )
					modes &= ~MODE_LOGGING;
			}
			else
			{	// Logging turned off
				fm_close();
			}
		}
		else
		if ( (modes & MODE_DISPLAY_0) == MODE_ENTER_MENU )
		{	// To do: state machine
		}
		else
		{
			if ( (modes & MODE_DISPLAY_1) < MODE_MAX_1 )
			{
				modes += MODE_INC_1;
			}
			else
			{
				modes = modes & ~MODE_DISPLAY_1;
			}
			lcd.clear_row(1);
			lcd.setCursor(0,1);
			lcd.print(F("---"));
		}
	}
}


// Display the time in a GPRMC record
void display_time(const char *b)
{
	uint8_t err = 0;
	uint8_t i;

	for ( i = 0; i < 6; i++ )
	{
		if ( !isdigit(b[i]) )
		{
			err = 1;
			break;
		}
	}

	lcd.setCursor(0,1);
	if ( err )
	{
		lcd.print(F("--:--:--"));
	}
	else
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
	f = d2n(b[dp-2]) * 10 + d2n(b[dp-1]);
	f = f * 10;
	if ( isdigit(b[dp+1]) )
		f += d2n(b[dp+1]);
	f = f * 10;
	if ( isdigit(b[dp+2]) )
		f += d2n(b[dp+2]);				// f is now in 100ths of minutes
	f = f/6;							// f is now in 1000ths of degrees

	lcd.write('.');
	x = f/100;
	lcd.write(x + '0');
	x = (f/10) - 10*x;
	lcd.write(x + '0');
	lcd.write(f % 10 + '0');
}

// Display GPS speed
// Speed is given in knots (dd.ff)
// Have to convert to km/h
void display_gpsspeed(const char *b, uint8_t col, uint8_t row)
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
		kn = kn * 10 + d2n(b[i]);
	}
	i++;								// Skip over dp
	for ( ; b[i] != ','; i++ )			// Fractional part
	{
		if ( !isdigit(b[i]) )
			return;						// Not-a-digit found; bad data
		f_kn = f_kn * 10 + d2n(b[i]);
		div = div * 10;
	}

	double speed = (double)kn + (double)f_kn / (double)div;		// knots
	speed = speed * 1.852001;									// km/h

	lcd.setCursor(col,row);					// Cursor to start of output field
	lcd.print(speed, 2);
	lcd.print(F(" km/h "));
}

// Display wheel speed
// Wheel speed not implemented yet
void display_wheelspeed(void)
{
	lcd.setCursor(9,1);
	lcd.print(F("--.-- k"));
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

// Display date and time
// Format of input is DDMMYY  and HHMMSS
// Show in ISO form YYYY-MM-DD HH:MM (assume 20xx) ==> exactly 16 characters
void display_date_time(const char *d, const char *t)
{
	uint8_t i;
	uint8_t err = 0;

	for ( i = 0; i < 6; i++ )
	{
		if ( !isdigit(d[i]) )
		{
			err = 1;
			break;
		}
	}
	lcd.setCursor(0,0);					// Cursor to start of output field
	if ( err )
	{
		lcd.print(F("----------"));
//					 YYYY-MM-DD
	}
	else
	{
		lcd.write('2');
		lcd.write('0');
		lcd.write(d[4]);
		lcd.write(d[5]);
		lcd.write('-');
		lcd.write(d[2]);
		lcd.write(d[3]);
		lcd.write('-');
		lcd.write(d[0]);
		lcd.write(d[1]);
	}
	lcd.write(' ');

	err = 0;
	for ( i = 0; i < 6; i++ )
	{
		if ( !isdigit(t[i]) )
		{
			err = 1;
			break;
		}
	}

	if ( err )
	{
		lcd.print(F("--:--"));
	}
	else
	{
		lcd.write(t[0]);
		lcd.write(t[1]);
		lcd.write(':');
		lcd.write(t[2]);
		lcd.write(t[3]);
	}
}

// Convert ascii time to 16-bit decimal (scaled in units of 2 seconds)
// A time of day in seconds is 86400; too big for 16 bits
uint16_t time_to_t16(const char *t)
{
	uint16_t t16;

	for ( int i = 0; i < 6; i++ )
	{
		if ( !isdigit(t[i]) )
			return 0xffff;
	}

	t16 = d2n(t[0]) * 10 + d2n(t[1]);					// Hours
	t16 = t16 * 60 + d2n(t[2]) * 10 + d2n(t[3]);		// Convert hours to minutes and add minutes
	t16 = t16 * 30 + d2n(t[4]) * 10 + d2n(t[5])/2;		// Convert minutes to 2secs and add seconds/2

	return t16;
}

// Calculate and display the elapsed time
void display_elapsed_time(const char *t)
{
	uint16_t t16 = time_to_t16(t);

	if ( t16 == 0xffff )
	{
		t16 = 0;
	}
	else
	if ( t16 < start_time )
	{	// Gone over midnight
		t16 = t16 - start_time + 43200;		// Overflows intentional
	}
	else
	{
		t16 = t16 - start_time;
	}

	display_t16(t16);
}

void display_t16(uint16_t t)
{
	uint8_t s, m, h;

	s = t%30;
	t = t/30;
	m = t%60;
	h = t/60;

	lcd.setCursor(0,1);
	lcd.write((char)(h/10) + '0');
	lcd.write((char)(h%10) + '0');
	lcd.write(':');
	lcd.write((char)(m/10) + '0');
	lcd.write((char)(m%10) + '0');
	lcd.write(':');
	lcd.write((char)(s/10) + '0');
	lcd.write((char)(s%10) + '0');
	lcd.write(' ');
}

// Display logging status
void display_logging(void)
{
	lcd.setCursor(0,0);
	lcd.print(F("Logging: "));

	if ( (modes & MODE_LOGGING) == 0 )
		lcd.print(F("off"));
	else
		lcd.print(F("on"));
}

// Display menu entry
void display_menu(void)
{
	lcd.setCursor(0,0);
	lcd.print(F("Settings"));
}

#if DBG
// Host debugging
#include <stdio.h>
void debug_print(const char *s)
{
	printf("\nDBG: %s\n", s);
}
#endif
