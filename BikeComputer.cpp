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
#include <MicroNMEA.h>

#define ICP1	8	// Input capture 1 is on pin 8/PB0

#define LED1	13	// On-board LED on pin 13

char nmeabuf[85];
MicroNMEA nmea(nmeabuf, sizeof(nmeabuf));
pcf2119r lcd;

static void display_time(char *b);
static void display_degrees(uint8_t col, char dir, long angle);

void LcdStatus(char *str, int status)
{
	if ( status != 0 )
	{
		Serial.print(str);
		Serial.print(" status = ");
		Serial.println(status);

		hd44780::fatalError(status);
	}
}

void setup(void)
{
	char buf[16];
	uint8_t cnt;
	uint8_t mode_change;
	pinMode(LED1, OUTPUT);
    digitalWrite(LED1, LOW);

	Serial.begin(9600);
	Serial.println("Hello, World!");

	int status = lcd.begin(16, 2);
	LcdStatus("begin()", status);

	status = lcd.command(0x35);		// Switch to extended command mode
	LcdStatus("command(0x35)", status);

	status = lcd.command(0x80+31);	// Set Va = 4.3v
	LcdStatus("SetVa", status);

	lcd.command(0xc0+31);	// Set Vb = 4.3v
	LcdStatus("SetVb", status);

	status = lcd.command(0x34);		// Switch to normal command mode
	LcdStatus("command(0x34)", status);

	status = lcd.clear();
	LcdStatus("clear()", status);

	status = lcd.display();
	LcdStatus("display()", status);

	lcd.clear_row(0);
	lcd.clear_row(1);
	lcd.setCursor(0,0);
	lcd.write("Hello, World!");
	mode_change = 1;

	cnt = 0xff;

	for (;;)
	{
		int ch = Serial.read();
		if ( ch >= 0 )
		{
//			Serial.write((char)ch);
			if ( ch == '$' )
			{
				cnt = 0;
			}
			else if ( cnt < 15 )	// Looking for GPRMC,hhmmss.pp
			{
				buf[cnt] = ch;
				cnt++;
				if ( cnt == 15 )
				{
					if ( strncmp(buf, "GPRMC,", 6) == 0 )
					{
						display_time(&buf[6]);
					}
					cnt = 0xff;		// Wait for next sentence
				}
			}

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
		    }
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
	angle = angle/1000L;
	double deg = ((double)angle/1000.0);
	lcd.print(deg, 3);
	lcd.write(' ');
}
