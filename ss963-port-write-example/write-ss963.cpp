/******************************************************************************
write-ss963.cpp
Raspberry Pi SPI interface demo with SS963 Serial Power Driver
iletisim@izlencebilisim.com 25/2/2017
https://github.com/....

A brief demonstration of the Raspberry Pi SPI interface, using the Izlence's SS963 Serial Power Driver

Resources:

This example makes use of the Wiring Pi library, which streamlines the interface
to the the I/O pins on the Raspberry Pi, providing an API that is similar to the Arduino.  

You can learn about installing Wiring Pi here:
http://wiringpi.com/download-and-install/

The wiringPi SPI API is documented here:
https://projects.drogon.net/raspberry-pi/wiringpi/spi-library/

The init call returns a standard file descriptor.  More detailed configuration
of the interface can be performed using ioctl calls on that descriptor.
See the wiringPi SPI implementation (wiringPi/wiringPiSPI.c) for some examples.
Parameters configurable with ioctl are documented here:
http://git.kernel.org/cgit/linux/kernel/git/torvalds/linux.git/tree/Documentation/spi/spidev

Hardware connections:

This file interfaces with the SS963 Serial Driver card:
http://www.izlencebilisim.com/urun/ss963-seri-surucu-karti/2/

The board was connected as follows:
(Raspberry Pi)(SS963)
GND  	-> GND
GPIO.2	-> RESET
GPIO.2	-> LATCH
SCK  	-> CLOCK
MOSI 	-> DATA

To build this file, I use the command:
>  g++ write-ss963.cpp -lwiringPi -o write-ss963.out -Ofast

Then to run it, first the spi kernel module needs to be loaded.  This can be 
done using the GPIO utility in old Raspbian versions unfortunately you need
to run the raspi-config program (as root) and select the module SPI
that you wish to load/unload there and reboot.

> sudo ./write-ss963.out

This test uses the single SS963 Serial Power Driver Card.  It shifts a 
bit through the card.

Development environment specifics:
Tested on Raspberry Pi V3 hardware, running Raspbian (4.4.38-v7+)
Building with gcc version 4.9.2 (Raspbian 4.9.2-10)

Distributed as-is; no warranty is given.
******************************************************************************/
#include <iostream>
#include <errno.h>
#include <wiringPiSPI.h>
#include <wiringPi.h>
#include <unistd.h>

using namespace std;

// Channel is the wiringPi name for the chip select (or chip enable) pin.
// Set this to 0 or 1, depending on how it's connected.
static const int CHANNEL = 0;
static const int LATCH_PIN = 3;
static const int RESET_PIN = 2;

int main()
{
	int fd, i;
	unsigned char buffer[12];
	
	cout << "Initializing..." << endl ;
	
	// Open the GPIO devices and allows our program to access it
	wiringPiSetup();	
	
	// Set pin modes
	pinMode (RESET_PIN, OUTPUT);
	pinMode (LATCH_PIN, OUTPUT);
	
	// Disable Low-Active reset pin of SS963	
	digitalWrite(RESET_PIN, HIGH);
	digitalWrite(LATCH_PIN, LOW);
	
	// Configure the interface.
	// CHANNEL indicates chip select,
	// 500000 indicates bus speed. 
	// According to Gordon the other speeds are: 0.5,1,2,4,8,16,32 MHz 
	// Otherwise the SS963 Serial Power Driver has support 100MHz transmission
	if (fd = wiringPiSPISetup(CHANNEL, 1*1000*1000) < 0)
		cout << "SPI could not be initialized. Init result: " << fd << endl;

	// Fill buffer array with any value
	for (i=0; i < 12; i++)
		buffer[i] = i;

	// 96 bits data will be sent and be replied back into the buffer
	wiringPiSPIDataRW(CHANNEL, buffer, 12);
	
	// Latch data to outputs (Qn) from SS963 registers (RQn)	
	digitalWrite(LATCH_PIN, HIGH);
	usleep(100);
	digitalWrite(LATCH_PIN, LOW);
		
	cout << "96 bits written to SS963." << endl;
}