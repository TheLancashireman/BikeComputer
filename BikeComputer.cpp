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
#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Clcd.h>

#define ICP1	8	// Input capture 1 is on pin 8/PB0

#define LED1	13	// On-board LED on pin 13

hd44780_I2Clcd lcd;

void LcdStatus(char *str, int status)
{
	Serial.print(str);
	Serial.print(" status = ");
	Serial.println(status);

	if ( status != 0 )
	{
		hd44780::fatalError(status);
	}
	delay(2000);
}

void setup(void)
{
	pinMode(LED1, OUTPUT);
    digitalWrite(LED1, LOW);

	Serial.begin(9600);
	Serial.println("Hello, World!");

	int status = lcd.begin(16, 2);
	LcdStatus("begin()", status);

	status = lcd.command(0x35);		// ???
	LcdStatus("command(0x35)", status);

	status = lcd.command(0x80+31);	// Set Va = 4.3v
	LcdStatus("SetVa", status);

	lcd.command(0xc0+31);	// Set Vb = 4.3v
	LcdStatus("SetVb", status);

	status = lcd.command(0x34);		// ???
	LcdStatus("command(0x34)", status);

	status = lcd.clear();
	LcdStatus("clear()", status);

	status = lcd.display();
	LcdStatus("display()", status);

	Serial.println("delay ...");
	delay(10000);
	Serial.println("end delay ...");

	lcd.setCursor(0,0);
	for ( int i = 0; i < 16; i++ )
	{
		lcd.write((char)0xa0);
	}
	lcd.setCursor(0,1);
	for ( int i = 0; i < 16; i++ )
	{
		lcd.write((char)0xa0);
	}

	lcd.setCursor(0,0);
	Serial.println("Before lcd.print()");
	status = lcd.write("Hello, World!");
	Serial.print("After lcd.print() status = ");
	Serial.println(status);

	for (;;)
	{
	}
}

void loop() {}
