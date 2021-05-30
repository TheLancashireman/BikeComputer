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

#define ICP1	8	// Input capture 1 is on pin 8/PB0
#define BTN1	2	// Pushbutton on pin 2
#define BTN2	3	// Pushbutton on pin 3

#define LED1	13	// On-board LED on pin 13

extern uint8_t fm_init(void);
extern uint8_t fm_open(void);
extern uint8_t fm_close(void);
extern uint8_t fm_write(const char *line);

extern void blip(char c);

extern uint8_t tm_elapsed(void);
extern void tm_init(void);

extern void btn_init(void);
extern uint8_t btn_read(uint8_t elapsed);

#endif
