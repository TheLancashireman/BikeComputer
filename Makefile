ARDUINO_BASE			= /data1/projects/arduino
ARDUINO_DIR				= $(ARDUINO_BASE)/arduino-1.8.13
ARDUINO_VERSION			= 10813
TARGET					= BikeComputer
MCU						= atmega328p
F_CPU					= 16000000
ARDUINO_LIBS			= SPI
ARDUINO_PORT			= /dev/ttyUSB0
AVRDUDE_ARD_BAUDRATE	= 57600
AVRDUDE_ARD_PROGRAMMER	= arduino
BOARD_TAG				= nano
ARDUINO_TOOLS_PATH		= $(ARDUINO_DIR)/hardware/tools
ARDUINO_ETC_PATH		= $(ARDUINO_TOOLS_PATH)/avr/etc
AVR_TOOLS_PATH			= $(ARDUINO_TOOLS_PATH)/avr/bin

PRJ_INCLUDES            += -I../arduino/libraries/SdFat/src
PRJ_INCLUDES            += -I../arduino/libraries/hd44780
PRJ_INCLUDES            += -I../arduino/libraries/hd44780/hd44780ioClass
PRJ_INCLUDES			+= -I../arduino/arduino-1.8.13/hardware/arduino/avr/libraries/Wire/src
PRJ_INCLUDES			+= -I.

# Display driver
OTHER_OBJS				+= $(OBJDIR)/Wire.o
OTHER_OBJS				+= $(OBJDIR)/hd44780.o
OTHER_OBJS				+= $(OBJDIR)/pcf2119r.o
OTHER_OBJS				+= $(OBJDIR)/twi.o

# SD card library
OTHER_OBJS				+= $(OBJDIR)/FatFileSFN.o
OTHER_OBJS				+= $(OBJDIR)/FatFileLFN.o
OTHER_OBJS				+= $(OBJDIR)/FatFile.o
OTHER_OBJS				+= $(OBJDIR)/SdSpiCard.o
OTHER_OBJS				+= $(OBJDIR)/FsCache.o
OTHER_OBJS				+= $(OBJDIR)/FatPartition.o
OTHER_OBJS				+= $(OBJDIR)/FsDateTime.o
OTHER_OBJS				+= $(OBJDIR)/SdSpiChipSelect.o
OTHER_OBJS				+= $(OBJDIR)/FatVolume.o

include $(ARDUINO_BASE)/Arduino.make

$(OBJDIR)/Wire.o:		../arduino/arduino-1.8.13/hardware/arduino/avr/libraries/Wire/src/Wire.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

$(OBJDIR)/hd44780.o:	../arduino/libraries/hd44780/hd44780.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

$(OBJDIR)/pcf2119r.o:	../arduino/libraries/hd44780/hd44780ioClass/pcf2119r.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

$(OBJDIR)/twi.o:		../arduino/arduino-1.8.13/hardware/arduino/avr/libraries/Wire/src/utility/twi.c
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $< -o $@

$(OBJDIR)/FatFileSFN.o:	../arduino/libraries/SdFat/src/FatLib/FatFileSFN.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

$(OBJDIR)/FatFileLFN.o:	../arduino/libraries/SdFat/src/FatLib/FatFileLFN.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

$(OBJDIR)/FatFile.o:	../arduino/libraries/SdFat/src/FatLib/FatFile.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

$(OBJDIR)/SdSpiCard.o:	../arduino/libraries/SdFat/src/SdCard/SdSpiCard.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

$(OBJDIR)/FsCache.o:	../arduino/libraries/SdFat/src/common/FsCache.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

$(OBJDIR)/FatPartition.o:	../arduino/libraries/SdFat/src/FatLib/FatPartition.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

$(OBJDIR)/FsDateTime.o:	../arduino/libraries/SdFat/src/common/FsDateTime.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

$(OBJDIR)/SdSpiChipSelect.o:	../arduino/libraries/SdFat/src/SpiDriver/SdSpiChipSelect.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

$(OBJDIR)/FatVolume.o:		../arduino/libraries/SdFat/src/FatLib/FatVolume.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@
