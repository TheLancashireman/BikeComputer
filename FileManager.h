/*	FileManager.h - management of files on the BikeComputer SD card
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
#ifndef FILEMANAGER_H
#define FILEMANAGER_H	1

extern uint8_t fm_init(void);
extern uint8_t fm_open(void);
extern uint8_t fm_close(void);
extern uint8_t fm_write(const char *line);

#endif
