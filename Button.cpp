/* Button.cpp - pushbutton functions
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
#include "BikeComputer.h"

#define DEBOUNCE	200

uint8_t button_time = 0;
uint8_t button = 0;

void btn_init(void)
{
	pinMode(BTN1, INPUT_PULLUP);
	pinMode(BTN2, INPUT_PULLUP);
}

uint8_t btn_read(uint8_t elapsed)
{
	if ( button_time == 0 )
	{
		uint8_t b1 = digitalRead(BTN1);
		uint8_t b2 = digitalRead(BTN2);

		if ( button == 0 )
		{
			// No buttons previously pressed
			if ( b1 == LOW )
			{
				button = 1;
				button_time = DEBOUNCE;
			}
			else
			if ( b2 == LOW )
			{
				button = 2;
				button_time = DEBOUNCE;
			}
			return button;
		}
		else
		{
			// Get current state of last button pressed.
			uint8_t b = (button == 1) ? b1 : b2;

			// Has the button been released?
			if ( b != LOW )
			{
				// Yes. Clear button and don't look again until debounce finished
				button = 0;
				button_time = DEBOUNCE;
			}
		}
	}
	else
	if ( button_time > elapsed )
	{
		button_time -= elapsed;
	}
	else
	{
		button_time = 0;
	}
	return 0;
}
