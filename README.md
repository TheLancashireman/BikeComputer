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
