# BikeComputer

A bike computer with a GPS receiver and an SD card slot for logging a GPS trace.

## Components

* An Arduino nano
* An LCD display module
* A GPS receiver module
* An SD card module

### SD card module

Use an adaptor that has a level-shifter chip. My first attempt used an adaptor that was intended for
a raspberry pi. It may have contributed to the instability and eventually destroyed the SD card.

### GPS receiver

Most cheap little receivers run at 3.3v. For reliability, use a level shifter between the Tx pin of the receiver
and the Rx pin of the arduino. I use a pair of NPN transistors. If you need to connect the Tx pin of the
arduino to the Rx pin of the receiver, you must use a level shifter there too.

You need to disconnect the Tx pin of the receiver to update program the arduino, so use a switch or jumper
in the circuit.

### Anag Vision AV1623 display

Needs LCD drive/contrast voltage on pin 3: connect a 10k preset potentiometer between 5v and gnd and connect the
wiper to pin 3. Adjust until the display looks right.

Backlight LED current 130 mA at 4.2v

Series resistor: R = V/I ==> R = 0.8 / 0.13 = 6.2 Ohm. Still need experiments to determine good value.

### Batron BTHQ 21605AV-YETF-LED04-I2C-5V LCD  (I2C, character set R)

Backlight LED current 90mA at 4.1v

Series resistor: R = V/I ==> R = 0.9 / 0.09 = 10 Ohm. 47 Ohm gives usable backlight.

This display has an I2C interface. The driver library (whichever you choose) uses the twi library,
which consumes around 300 bytes of RAM. In conjunction with the Sd library this is too much for
the nano and the software behaves erratically, probably because of a stack overflow.

## Display layout, modes and buttons

The top line and the bottom line of the display can be controlled independently.

The information presented on the top line is controlled by the MODE_DISPLAY_0 field of the modes byte:

| Mode                    |0|1|2|3|4|5|6|7|8|9|0|1|2|3|4|5| Function                             | Remark       |
| ------------------------|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|--------------------------------------|--------------|
| MODE_POSITION           |N|1|2|.|3|4|5| |E|0|1|2|.|3|4|5| display_degrees()/display_degrees()  | Called twice |
| MODE_GPSSPEED_HEADING   |1|2|.|3|4|k|m|/|h| | |1|2|3|.|4| display_gpsspeed()/display_heading() |              |
| MODE_DATE_TIME          |Y|Y|Y|Y|-|M|M|-|D|D| |h|h|:|m|m| display_date_time()                  |              |
| MODE_LOGGING_CTRL       |L|o|g|g|i|n|g|:| |o|f|f| | | | | display_logging()                    | off or on    |
| MODE_TRIP_CTRL          |T|r|i|p|:| |c|l|e|a|r|e|d| | | | display_trip()/display_cleared()     |              |
| MODE_ENTER_MENU         |S|e|t|t|i|n|g|s| | | | | | | | | display_menu()                       |              |
| MODE_ERROR              |F|i|l|e| |e|r|r|o|r| |1|2|3| | | FmStatus()                           |              |

Pressing BTN1 steps through the modes from MODE_POSITION to MODE_ENTER_MENU. MODE_ERROR is only used when
a filesystem error is reported. Pressing BTN1 on MODE_ERROR steps to MODE_POSITION.

When the mode is MODE_LOGGING_CTRL, pressing button 2 toggles the logging of data to the SD card. Each time logging
is enabled a new file is opened.

When the mode is MODE_TRIP_CTRL, pressing button 2 clears the trip (elapsed) time and distance. The word "cleared"
appears then.

When the mode is MODE_ENTER_MENU, pressing button 2 enters the settings menu. This changes the display management
completely. Yet to be defined and implemented.

The information presented on the bottom line is controlled by the MODE_DISPLAY_1 field of the modes byte:

| Mode                    |0|1|2|3|4|5|6|7|8|9|0|1|2|3|4|5| Function                                    | Remark       |
| ------------------------|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|---------------------------------------------|--------------|
| MODE_TIME_WHEELSPEED    |h|h|:|m|m|:|s|s| |9|9|.|9|9|k|m| display_time()/display_wheelspeed()         |              |
| MODE_ELAPSED_WHEELSPEED |h|h|:|m|m|:|s|s| |9|9|.|9|9|k|m| display_elapsed_time()/display_wheelspeed() |              |
| MODE_TIME_GPSSPEED      |h|h|:|m|m|:|s|s| |9|9|.|9|9|k|m| display_time()/display_gpsspeed()           |              |
| MODE_ELAPSED_GPSSPEED   |h|h|:|m|m|:|s|s| |9|9|.|9|9|k|m| display_elapsed_time()/display_gpsspeed()   |              |

Pressing BTN2 when MODE_DISPLAY_0 is MODE_POSITION, MODE_GPSSPEED_HEADING or MODE_DATE_TIME steps through the
modes.

Note: wheelspeed and distance yet to be implemented. More display modes will be needed.

