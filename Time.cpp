/* Time.cpp - timing functionality
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

unsigned long then;

// Initialise the base time
void tm_init(void)
{
	then = millis();
}

// How much time has elapsed since last time? Should only measure short intervals, so max out at 255/
uint8_t tm_elapsed(void)
{
	unsigned long now = millis();
	unsigned long elapsed = now - then;
	if ( elapsed > 255 )
		return 255;
	return (uint8_t)elapsed;
}
