/* FileManager - management of files on the BikeComputer SD card
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
#include <SPI.h>
#include "SdFat.h"
#include "sdios.h"
#include "BikeComputer.h"

/* See SdFat/examples/QuickStart/QuickStart.ino for full descriptions. This is a cut-down version
*/
#define SD_FAT_TYPE		1
#define SPI_SPEED		SD_SCK_MHZ(4)
#define SD_CS_PIN		SS
#define SD_CONFIG		SdSpiConfig(SD_CS_PIN, DEDICATED_SPI)

SdFat32 sd;
File32 file;
uint16_t byte_count;

const int8_t DISABLE_CHIP_SELECT = -1;

/* My stuff
*/
char fn[14] = "NMEA0000.log";
//             0123456789012

// Increment the file name. Assumes file name starts at /xxxx0000.eee
uint8_t incFn(void)
{
	fn[7]++;
	if ( fn[7] > '9' )
	{
		fn[7] = '0';
		fn[6]++;
		if ( fn[6] > '9' )
		{
			fn[6] = '0';
			fn[5]++;
			if ( fn[5] > '9' )
			{
				fn[5] = '0';
				fn[4]++;
				if ( fn[4] > '9' )
				{
					fn[4] = '0';		// Wrap around after 10000 files - "should be enough for anyone(tm)"
					return 1;
				}
			}
		}
	}
	return 0;
}

// Initialise the SdFat classes and return with the file open for writing.
// Error code return means no file open.
uint8_t fm_init(void)
{
	if ( !sd.begin(-1, SPI_SPEED) )
	{
		return 101;
	}

	if ( sd.vol()->fatType() == 0 )
	{
		return 102;
	}

	uint32_t size = sd.card()->sectorCount();
	if ( size == 0 )
	{
		return 103;
	}

	uint32_t sizeMB = 0.000512 * size + 0.5;	// WTF?

	if ( (sizeMB > 1100 && sd.vol()->sectorsPerCluster() < 64) || (sizeMB < 2200 && sd.vol()->fatType() == 32) )
	{
		return 104;
	}
}

uint8_t fm_close(void)
{
	file.sync();
	delay(100);
	file.close();
}

uint8_t fm_open(void)
{
	while ( sd.exists(fn) )
	{
		if ( incFn() )
			return 105;		// Unlikely; SD card contains at least 10000 files.
	}

	if ( !file.open(fn, O_WRONLY | O_CREAT | O_AT_END) )
	{
		return 106;
	}

	byte_count = 0;
	delay(100);

	return 0;
}

// fm_write() - writes to the open file
// Assumption: line has non-zero length
uint8_t fm_write(const char *line)
{
	if ( line[0] == '\0' )
		return 255;

	size_t s = file.write(line);

	if ( s == -1 )
	{
		return 108;
	}

	byte_count += s;
	if ( byte_count >= 1024 )
	{
		file.sync();
		byte_count &= 1023;
	}
	else
	{
	}

	return (uint8_t)s;
}
