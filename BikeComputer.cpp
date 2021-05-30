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
#if 0
#include <MicroNMEA.h>
#endif
#include "BikeComputer.h"

char nmeabuf[85];
#if 0
MicroNMEA nmea(nmeabuf, sizeof(nmeabuf));
#endif

pcf2119r lcd;

static void display_time(char *b);
static void display_degrees(uint8_t col, char dir, long angle);

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

void setup(void)
{
	uint8_t mode_change = 1;
	uint8_t logging = 0;
	uint8_t char_cnt = 0xff;
	uint8_t gp_type = GP_NONE;
	
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
			logging = 1;
		}
	}

	tm_init();
	btn_init();

	lcd.setCursor(15,1);
	lcd.write(logging ? '+' : '-');

	for (;;)
	{
		uint8_t elapsed = tm_elapsed();
		uint8_t btn = btn_read(elapsed);

		if ( btn == 1 )
		{
			blip('1');
			if ( logging )
			{
				logging = 0;
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
					logging = 1;
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
				nmeabuf[0] = '$';
				char_cnt = 1;
				gp_type = GP_NONE;
			}
			else
			if ( ch == '\0' || ch == '\r' || ch == '\n' )
			{
				nmeabuf[char_cnt++] = '\n';
				nmeabuf[char_cnt] = '\0';

				if ( gp_type == GP_RMC )
				{
					if ( logging )
					{
						uint8_t q = fm_write(nmeabuf);
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

				// Process the sentence ... (todo)
				char_cnt = 0xff;		// Wait for next sentence
			}
			else
			if ( char_cnt < 80 )
			{
				nmeabuf[char_cnt] = ch;
				char_cnt++;
				if ( char_cnt == 15 )
				{
					if ( nmeabuf[1] == 'G' && 	// Unrolled strncmp() ;-)
						 nmeabuf[2] == 'P' && 
						 nmeabuf[3] == 'R' && 
						 nmeabuf[4] == 'M' && 
						 nmeabuf[5] == 'C' && 
						 nmeabuf[6] == ',')
					{
						gp_type = GP_RMC;
						display_time(&nmeabuf[7]);
					}
				}
			}
			else
			if ( char_cnt != 0xff )
			{
				blip('!');
			}
#if 0
			// Forward the character to the NMEA processor
			if ( nmea.process(ch) )
			{
				// Sentence received: what is it?
				if ( nmea.isValid() )
				{
					if ( mode_change )
					{
						lcd.clear_row(0);
						mode_change = 0;
					}
					long p = nmea.getLatitude();
					if ( p < 0 )
					{
						display_degrees(0, 'S', -p);
					}
					else
					{
						display_degrees(0, 'N', p);
					}
					p = nmea.getLongitude();
					if ( p < 0 )
					{
						display_degrees(8, 'W', -p);
					}
					else
					{
						display_degrees(8, 'E', p);
					}
				}
				if ( logging )
				{
					const char *s = nmea.getSentence();
					uint8_t q = fm_write(s);
					if ( q > 250 )
					{
						FmStatus(q);
					}
				}
		    }
#endif
		}
	}
}

void loop() {}

static void display_time(char *b)
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

static void display_degrees(uint8_t col, char dir, long angle)
{
	lcd.setCursor(col,0);
	lcd.write(dir);
	angle = (angle+500L)/1000L;				// Round to nearest. Do this as integer to avoid loss of accuracy
	double deg = ((double)angle/1000.0);
	lcd.print(deg, 3);
	lcd.write(' ');
}
