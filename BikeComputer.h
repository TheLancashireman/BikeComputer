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
#define MODE_LOGGING			0x80	// True or false

#define MODE_DISPLAY_0			0x07	// What to display on top line?
#define MODE_POSITION			0x00	// Display lat/long coordinates
#define MODE_GPSSPEED_HEADING	0x01	// Display GPS speed and heading
#define MODE_DATE_TIME			0x02	// Display date and time
#define MODE_LOGGING_CTRL		0x03	// Display "Logging on/off"
#define MODE_ENTER_MENU			0x04	// Menu
#define MODE_ERROR				0x07	// Error code. Cannot step to this value
#define MODE_MAX_0				0x04	// Max value of display mode 0
#define MODE_INC_0				0x01	// Increment value for display mode 0

#define MODE_DISPLAY_1			0x38	// What to display on bottom line?
#define	MODE_TIME_WHEELSPEED	0x00	// Time and wheel speed
#define	MODE_ELAPSED_WHEELSPEED	0x08	// Elapsed time and wheel speed
#define	MODE_TIME_GPSSPEED		0x10	// Time and wheel speed
#define	MODE_ELAPSED_GPSSPEED	0x18	// Elapsed time and wheel speed
#define MODE_MAX_1				0x18	// Max value of display mode 1
#define MODE_INC_1				0x08	// Increment value for display mode 1

//
extern uint16_t start_time;

// File handling
extern uint8_t fm_init(void);
extern uint8_t fm_open(void);
extern uint8_t fm_close(void);
extern uint8_t fm_write(const char *line);

// GPS handling
extern uint8_t gps_decode(const char *s);
extern uint8_t find_next(const char *s, uint8_t i, uint8_t n, char c);

// Time measurement
extern uint8_t tm_elapsed(void);
extern void tm_init(void);

// Button handling
extern void btn_init(void);
extern uint8_t btn_read(uint8_t elapsed);
extern void mode_control(uint8_t btn);

// Display handling
void display_time(const char *b);
void display_date_time(const char *d, const char *t);
void display_degrees(const char *b);
void display_gpsspeed(const char *b, uint8_t col, uint8_t row);
void display_heading(const char *b);
void display_date(const char *b);
void display_logging(void);
void display_menu(void);
void display_elapsed_time(const char *t);
void display_t16(uint16_t t);
uint16_t time_to_t16(const char *t);
void display_wheelspeed(void);

static inline void display_gpsspeed_l0(const char *b)
{
	display_gpsspeed(b, 0, 0);
}

static inline void display_gpsspeed_l1(const char *b)
{
	display_gpsspeed(b, 9, 1);
}


static inline uint8_t d2n(uint8_t d)
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
