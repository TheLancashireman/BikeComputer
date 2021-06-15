/* Gps.cpp - decoding GPS sentences
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
#include <ctype.h>

/* GPS sentence decoding: see GPS-OEM7_Commands_Logs_Manual.pdf
 *
 * Why not MicroNMEA?
 *	Uses a lot of memory to do that.
 *	For arduino with limited RAM, better just to decode what we want when we want it.
*/

static uint8_t decode_gprmc(const char *s);
static uint8_t decode_gpvtg(const char *s);
static uint8_t decode_gpgga(const char *s);
static uint8_t decode_gpgsa(const char *s);
static uint8_t decode_gpgll(const char *s);
static uint8_t decode_gptxt(const char *s);

/* gp_decoder() - decode a GPS sentemce
 *
 * Returns sentence type
 *
 * Sentences start with $GP
 * Caller ensures that 1st character is $ (used as sync)
*/
uint8_t gps_decode(const char *s)
{
	if ( s[1] == 'G' && s[2] == 'P' && s[6] == ',' )
	{
		if ( s[3] == 'R' && s[4] == 'M' && s[5] == 'C' )
		{
			return decode_gprmc(s);
		}
		else
		if ( s[3] == 'V' && s[4] == 'T' && s[5] == 'G' )
		{
			return decode_gpvtg(s);
		}
		else
		if ( s[3] == 'G' && s[4] == 'G' && s[5] == 'A' )
		{
			return decode_gpgga(s);
		}
		else
		if ( s[3] == 'G' && s[4] == 'G' && s[5] == 'A' )
		{
			return decode_gpgsa(s);
		}
		else
		if ( s[3] == 'G' && s[4] == 'L' && s[5] == 'L' )
		{
			return decode_gpgll(s);
		}
		else
		if ( s[3] == 'T' && s[4] == 'X' && s[5] == 'T' )
		{
			return decode_gptxt(s);
		}
	}
	return GP_NONE;		// Unrecognised sentence
}

// find_next()
//
// Find the next (next-but-one, next-but-2, ...) occurrence of a character in a string.
// Return index of the character.
// Stop at '\n' or '\0'
// n = 0 as parameter ==> find 256th occurrence. So don't do it ;-)
uint8_t find_next(const char *s, uint8_t i, uint8_t n, char c)
{
	while ( s[i] != '\n' && s[i] != '\0' )
	{
		if ( s[i] == c )
		{
			n--;
			if ( n == 0 )
				return i;	// Return index of nth instance of c
		}
		i++;				// Go to next character
	}
	return i;				// Return index of newline or terminator
}

// GPRMC sentence:
// Fields are comma-separated... check that s[6] is a comma
// Special case: time (UTC) is 1st field...
//		time format hhmmss.dd  (if valid) - get this out as soon as possible (display_time() copes with junk)
// For the rest: fields are comma-separated fields of variable length
static uint8_t decode_gprmc(const char *s)
{
	uint8_t i;
	uint8_t gpsspeed_line1 = 0;

	if ( start_time == 0xffff )
		start_time = time_to_t16(&s[7]);

	if ( (modes & MODE_DISPLAY_0) == MODE_TRIP )
	{
		display_elapsed_time(&s[7], 0);
		display_distance();
	}

	if ( (modes & MODE_DISPLAY_1) == MODE_TIME_WHEELSPEED )
	{
		display_time(&s[7]);
		display_wheelspeed();
	}
	else
	if ( (modes & MODE_DISPLAY_1) == MODE_TIME_GPSSPEED )
	{
		display_time(&s[7]);
		gpsspeed_line1 = 1;
	}
	else
	if ( (modes & MODE_DISPLAY_1) == MODE_ELAPSED_WHEELSPEED )
	{
		display_elapsed_time(&s[7], 1);
		display_wheelspeed();
	}
	else
	if ( (modes & MODE_DISPLAY_1) == MODE_ELAPSED_GPSSPEED )
	{
		display_elapsed_time(&s[7], 1);
		gpsspeed_line1 = 1;
	}
	

	i = find_next(s, 7, 1, ',');
	if ( s[i] != ',' )	return GP_ERR;
	i++;

	// i should now be the "position valid" flag
	if ( (modes & MODE_DISPLAY_0) == MODE_POSITION )
	{
		// Check validity flag
		if ( s[i] == 'A' && s[i+1] == ',' )
		{
			// Valid. Skip over flag and display the latitude fields
			i += 2;
			display_degrees(&s[i]);

			// Skip 2 comma-separated fields and display the longitude fields
			i = find_next(s, i, 2, ',');
			if ( s[i] == ',' )
			{
				i++;
				display_degrees(&s[i]);
			}
		}
	}

	// Skip over the lat/long fields
	i = find_next(s, i, 5, ',');
	if ( s[i] != ',' )	return GP_ERR;
	i++;

	if ( gpsspeed_line1 )
	{
		display_gpsspeed_l1(&s[i]);
	}

	if ( (modes & MODE_DISPLAY_0) == MODE_HEADING_GPSSPEED )
	{
		display_gpsspeed_l0(&s[i]);
		i = find_next(s, i, 1, ',');
		i++;
		display_heading(&s[i]);
	}

	// Skip over the speed/heading fields
	i = find_next(s, i, 2, ',');
	if ( s[i] != ',' )	return GP_ERR;
	i++;

	if ( (modes & MODE_DISPLAY_0) == MODE_DATE_TIME )
	{
		display_date_time(&s[i], &s[7]);
	}
	return GP_RMC;
}

static uint8_t decode_gpvtg(const char *s)
{
	return GP_VTG;
}

static uint8_t decode_gpgga(const char *s)
{
	return GP_GGA;
}

static uint8_t decode_gpgsa(const char *s)
{
	return GP_GSA;
}

static uint8_t decode_gpgll(const char *s)
{
	return GP_GLL;
}

static uint8_t decode_gptxt(const char *s)
{
	return GP_TXT;
}
