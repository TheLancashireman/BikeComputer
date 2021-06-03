/*	BikeComputer.h - header file for all global functions
 *
 *	(c) David Haworth
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
#ifndef BIKECOMPUTER_H
#define BIKECOMPUTER_H	1

#ifndef DBG
#define DBG		0
#endif

// GP sentence types
#define GP_NONE	0
#define GP_RMC	1
#define GP_VTG	2
#define GP_GGA	3
#define GP_GSA	4
#define GP_GLL	5
#define GP_TXT	6
#define GP_ERR	255

// Arduino pins
#define ICP1	8	// Input capture 1 is on pin 8/PB0
#define BTN1	2	// Pushbutton on pin 2
#define BTN2	3	// Pushbutton on pin 3
#define LED1	13	// On-board LED on pin 13

// Operating modes. Bit-fields define modes
extern uint8_t modes;
#define MODE_LOGGING	0x80	// True or false
#define MODE_CHANGE		0x10	// Display mode has changed
#define MODE_DISPLAY	0x07	// What to display?
#define MODE_POSN		0x00	// Display lat/long coordinates
#define MODE_SPEED		0x01	// Display GPS speed and heading
#define MODE_DATE		0x02	// Display date
#define MODE_NMODE		0x03	// No. of modes

// File handling
extern uint8_t fm_init(void);
extern uint8_t fm_open(void);
extern uint8_t fm_close(void);
extern uint8_t fm_write(const char *line);

// GPS handling
extern uint8_t gps_decode(const char *s);
extern uint8_t find_next(const char *s, uint8_t i, uint8_t n, char c);

// Debugging info
extern void blip(char c);

// Time measurement
extern uint8_t tm_elapsed(void);
extern void tm_init(void);

// Button handling
extern void btn_init(void);
extern uint8_t btn_read(uint8_t elapsed);

// Display handling
void display_time(const char *b);
void display_degrees(const char *b);
void display_speed(const char *b);
void display_heading(const char *b);
void display_date(const char *b);

static inline uint8_t digit_to_num(uint8_t d)
{
	return d - '0';
}

// Host debugging
#if DBG
void debug_print(const char *s);
#define DBG_MSG(x)	debug_print(x)
#else
#define DBG_MSG(x)	do {} while(0)
#endif

#endif
