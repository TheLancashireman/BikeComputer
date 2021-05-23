ARDUINO_BASE			= /data1/projects/arduino
ARDUINO_DIR				= $(ARDUINO_BASE)/arduino-1.8.13
ARDUINO_VERSION			= 10813
TARGET					= BikeComputer
MCU						= atmega328p
F_CPU					= 16000000
#ARDUINO_LIBS			= Wire
ARDUINO_PORT			= /dev/ttyUSB0
AVRDUDE_ARD_BAUDRATE	= 57600
AVRDUDE_ARD_PROGRAMMER	= arduino
BOARD_TAG				= nano
ARDUINO_TOOLS_PATH		= $(ARDUINO_DIR)/hardware/tools
ARDUINO_ETC_PATH		= $(ARDUINO_TOOLS_PATH)/avr/etc
AVR_TOOLS_PATH			= $(ARDUINO_TOOLS_PATH)/avr/bin

PRJ_INCLUDES            += -I../arduino/libraries/hd44780
PRJ_INCLUDES			+= -I../arduino/arduino-1.8.13/hardware/arduino/avr/libraries/Wire/src

OTHER_OBJS				+= $(OBJDIR)/Wire.o
OTHER_OBJS				+= $(OBJDIR)/hd44780.o
OTHER_OBJS				+= $(OBJDIR)/twi.o

include $(ARDUINO_BASE)/Arduino.make

$(OBJDIR)/Wire.o:		../arduino/arduino-1.8.13/hardware/arduino/avr/libraries/Wire/src/Wire.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

$(OBJDIR)/hd44780.o:	../arduino/libraries/hd44780/hd44780.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

$(OBJDIR)/twi.o:		../arduino/arduino-1.8.13/hardware/arduino/avr/libraries/Wire/src/utility/twi.c
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $< -o $@
