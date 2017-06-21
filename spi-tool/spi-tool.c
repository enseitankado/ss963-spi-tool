/*
 * spi-tool v1.0
 * SS963 Serial Power Driver Card Testing Utility
 *
 * Written by: Ensei Tankado, ensei@tankado.com
 *
 * Copyright (c) 2017  Izlence Electronic Software Inc. 
 * www.izlencebilisim.com
 *
 * Referances:
 * Copyright (c) 2007  MontaVista Software, Inc.
 * Copyright (c) 2007  Anton Vorontsov <avorontsov@ru.mvista.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License.
 *
 * This program uses wiringPi library from wiringpi.com. 
 * First you must install it: http://wiringpi.com/download-and-install/
 
 After you can compile the spi-tool as below:
 > gcc spi-tool.c -lwiringPi -o spi -Ofast -s
 
 To see supported parameters use --help:
 > ./spi-tool --help
Usage: ./spi-tool [-DstScrpdblHOLC3h]
  -D --device   device to use (default /dev/spidev0.0)
  -s --speed    max speed (Hz)
  -t --transmit hex coded decimal bytes
  -S --SS963      using SS963 Serial Driver Card. Set ^reset and apply latch signal.
  -c --latch    latch pin GPIO number (specic for SS963). Default 2
  -r --reset    reset pin GPIO number (specic for SS963). Default 3
  -p --ldelay   latch delay as micro seconds (specic for SS963). Default 50uS
  -d --delay    delay (usec)
  -b --bpw      bits per word
  -l --loop     loopback
  -H --cpha     clock phase
  -O --cpol     clock polarity
  -L --lsb      least significant bit first
  -C --cs-high  chip select active high
  -3 --3wire    SI/SO signals shared
  -N --nocs     Set SPI mode to SPI_NO_CS
  -R --ready    Set SPI mode to SPI_READY
  -h --help     prints help
  
Development environment specifics:
Tested on Raspberry Pi V3 hardware, running Raspbian (4.4.38-v7+)
Building with gcc version 4.9.2 (Raspbian 4.9.2-10)

  
Example-1:

	Transmits 5 byte (40bits) data (0xFF,0xFE,0xFD,0xFC,0xFF) to SPI.0 at 500KHz. 
	The tx data consist of hexadecimal coded bytes stream as below. 
	Also in this tutorial receiver device dont reply back any data 
	as seen in RX section (00 00 00 00 00). Attention this could means 
	MISO terminal is not connected.
	 
	>./spi-tool -s 500000 -t 0xFF,0xFE,0xFD,0xFC,0xFF
	SPI mode       : 0
	Bits per word  : 8
	Max speed      : 500000 Hz (500 KHz)

	TX:
	FF FE FD FC FF

	RX:
	00 00 00 00 00

Example-2:

	In this tutorial we use SS963 Serial Power Driver from izlencebilisim.com
	Because of this we add -S parameter (SS963) and use -crp parameters to 
	drive SS963 card with 96bit. Connection diagra provided in 
	"example-2-connection-diagram.png" Check this:

	> sudo ./spi -c 3 -r 2 -p 1000 --SS963 -t 0xAA,0xAA,0xAA,0xAA,0xAA,0xAA
	SPI mode       : 0
	Bits per word  : 8
	Max speed      : 500000 Hz (500 KHz)

	Using SS963 Serial Driver Card...
	Latch Pin   : 3
	^Reset Pin  : 2
	Latch Delay : 1000

	TX:
	AA AA AA AA AA AA

	RX:
	00 00 00 00 00 00

	In this example we want to take your attention to a few important things as
	relevant using of SS963 Serial Power Driver.
	First you must add sudo command to beginning of command because of the
	wiringPi functions which do latch coudlnt runs in user space rights.
	SS963 uses latch and reset functionalities. To clear (reset) all outputs set it to 
	logical "0" because of it is active low. After all bits transmitted to SS963 registers
	to write all registers to own outputs Lacth is required. 
	Whe you add -S or --SS963 parameters the latch signal automaticly send 
	after the data transmission was completed. Parameter -p (l-delay) sets
	period of latch signal. 
	For using of parameters please check tool's usage list with -h parameter.

 */
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <wiringPi.h>
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

static int GPIOExport(int pin)
{
#define BUFFER_MAX 3
	char buffer[BUFFER_MAX];
	ssize_t bytes_written;
	int fd;
 
	fd = open("/sys/class/gpio/export", O_WRONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open export for writing!\n");
		return(-1);
	}
 
	bytes_written = snprintf(buffer, BUFFER_MAX, "%d", pin);
	write(fd, buffer, bytes_written);
	close(fd);
	return(0);
}

static int GPIOUnexport(int pin)
{
	char buffer[BUFFER_MAX];
	ssize_t bytes_written;
	int fd;
 
	fd = open("/sys/class/gpio/unexport", O_WRONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open unexport for writing!\n");
		return(-1);
	}
 
	bytes_written = snprintf(buffer, BUFFER_MAX, "%d", pin);
	write(fd, buffer, bytes_written);
	close(fd);
	return(0);
}

static int GPIODirection(int pin, int dir)
{
	static const char s_directions_str[]  = "in\0out";
 
	#define DIRECTION_MAX 35
	char path[DIRECTION_MAX];
	int fd;
 
	snprintf(path, DIRECTION_MAX, "/sys/class/gpio/gpio%d/direction", pin);
	fd = open(path, O_WRONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open gpio direction for writing!\n");
		return(-1);
	}
 
	if (-1 == write(fd, &s_directions_str[0 == dir ? 0 : 3], 0 == dir ? 2 : 3)) {
		fprintf(stderr, "Failed to set direction!\n");
		return(-1);
	}
 
	close(fd);
	return(0);
}

static int GPIOWrite(int pin, int value)
{
	static const char s_values_str[] = "01";
 
	char path[100];
	int fd;
 
	snprintf(path, 100, "/sys/class/gpio/gpio%d/value", pin);
	fd = open(path, O_WRONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open gpio value for writing!\n");
		return(-1);
	}
 
	if (1 != write(fd, &s_values_str[0 == value ? 0 : 1], 1)) {
		fprintf(stderr, "Failed to write value!\n");
		return(-1);
	}
 
	close(fd);
	return(0);
}

static void pabort(const char *s)
{
	perror(s);
	abort();
}

static const char *device = "/dev/spidev0.0";
static uint8_t mode;
static uint8_t bits = 8;
static uint32_t speed = 500000;
static uint16_t delayx;
static uint8_t *tx;
static char *tx_arg;
static int tx_i = 0;
static int LATCH_PIN = 3;
static int RESET_PIN = 2;
static int LATCH_DELAY = 50;
static int SS963_USING = 0;

static void transfer(int fd)
{
	int ret;
	uint8_t rx[tx_i];
	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = tx_i,
		.delay_usecs = delayx,
		.speed_hz = speed,
		.bits_per_word = bits,
	};

    printf("\nTX:");
	for (ret = 0; ret < tx_i; ret++) {
		if (!(ret % 11))
			puts("");
		printf("%.2X ", tx[ret]);
	}
	puts("");	
	
	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1)
		pabort("can't send spi message");

    printf("\nRX:");
	for (ret = 0; ret < ARRAY_SIZE(rx); ret++) {
		if (!(ret % 11))
			puts("");
		printf("%.2X ", rx[ret]);
	}
	puts("");
}

static char *omitHexTag (char *charBuffer) {
  char *str;
  if (0 == (int) strlen(charBuffer))
    str = charBuffer;
  else
    str = charBuffer + 2;
  return str;
}

static void print_usage(const char *prog)
{
	printf("Usage: %s [-DstScrpdblHOLC3h]\n", prog);
	puts("  -D --device   device to use (default /dev/spidev0.0)\n"
	     "  -s --speed    max speed (Hz)\n"
		 "  -t --transmit The data to transmit (comma sperated hex coded decimal bytes)\n"
		 "  -S --SS963	using SS963 Serial Driver Card. Set ^reset and apply latch signal.\n"
		 "  -c --latch    latch pin GPIO number (specic for SS963). Default 2\n"
		 "  -r --reset    reset pin GPIO number (specic for SS963). Default 3\n"
		 "  -p --ldelay   latch delay as micro seconds (specic for SS963). Default 50uS\n"
	     "  -d --delay    delay (usec)\n"
	     "  -b --bpw      bits per word \n"
	     "  -l --loop     loopback\n"
	     "  -H --cpha     clock phase\n"
	     "  -O --cpol     clock polarity\n"
	     "  -L --lsb      least significant bit first\n"
	     "  -C --cs-high  chip select active high\n"
	     "  -3 --3wire    SI/SO signals shared\n"
		 "  -N --nocs     Set SPI mode to SPI_NO_CS\n"
		 "  -R --ready    Set SPI mode to SPI_READY\n"
		 "  -h --help     prints help\n"
		 " \n"		 
		 " Examples:\n" 
		 " \n" 
		 "	Example1: Sends data(0xFF,0xFE,0xFD,0xFC,0xFF) at 500KHz\n" 
		 " 	./spi-tool -s 500000 -t 0xFF,0xFE,0xFD,0xFC,0xFF\n" 		 
		 " \n" 
		 " 	Example2: Send data (0xAA,0xAA,0xAA,0xAA,0xAA,0xAA) to SS963 serial driver and latch.\n" 
		 " 	sudo ./spi-tool -c 3 -r 2 -p 1000 --SS963 -t 0xAA,0xAA,0xAA,0xAA,0xAA,0xAA\n" 		 
		 " \n" 
		 "You can contact from support@izlencebilisim.com / iletisim@izlencebilisim.com\n"		 
		 );
	exit(1);
}

static void parse_opts(int argc, char *argv[])
{
	static const struct option lopts[] = {
		{ "device",  1, 0, 'D' },
		{ "speed",   1, 0, 's' },
		{ "transmit",1, 0, 't' },
		{ "latch",	 1, 0, 'c' },
		{ "reset",	 1, 0, 'r' },
		{ "ldelay",	 1, 0, 'p' },
		{ "SS963",	 0, 0, 'S' },				
		{ "delay",   1, 0, 'd' },
		{ "bpw",     1, 0, 'b' },
		{ "loop",    0, 0, 'l' },
		{ "cpha",    0, 0, 'H' },
		{ "cpol",    0, 0, 'O' },
		{ "lsb",     0, 0, 'L' },
		{ "cs-high", 0, 0, 'C' },
		{ "3wire",   0, 0, '3' },
		{ "nocs",    0, 0, 'N' },
		{ "ready",   0, 0, 'R' },
		{ "help",    0, 0, 'h' },
		{ NULL, 0, 0, 0 },
	};	
	int c;	
	while (1) {
		c = getopt_long(argc, argv, "D:s:t:c:r:p:Sd:b:lHOLC3NRh", lopts, NULL);
		if (c == -1)
			break;

		switch (c) {			
		case 't':
			tx = optarg;
			uint8_t dec;
			tx_i = 0;
			char *hexes;;
			hexes = strtok(tx, ",");			
			while (hexes != NULL)
			{
				hexes = omitHexTag(hexes);												
				tx[tx_i] = (uint8_t) strtol(hexes, NULL, 16);				
				hexes = strtok(NULL, " ,-");				
				tx_i++;
			}			
			break;
		case 'c':			
			LATCH_PIN = atoi(optarg);			
			break;
		case 'r':
			RESET_PIN = atoi(optarg);
			break;
		case 'p':
			LATCH_DELAY = atoi(optarg);
			break;
		case 'S':
			SS963_USING = 1;
			break;
		case 'h':
			print_usage(argv[0]);
			break;
		case 'D':
			device = optarg;
			break;
		case 's':
			speed = atoi(optarg);
			break;
		case 'd':
			delayx = atoi(optarg);
			break;
		case 'b':
			bits = atoi(optarg);
			break;
		case 'l':
			mode |= SPI_LOOP;
			break;
		case 'H':
			mode |= SPI_CPHA;
			break;
		case 'O':
			mode |= SPI_CPOL;
			break;
		case 'L':
			mode |= SPI_LSB_FIRST;
			break;
		case 'C':
			mode |= SPI_CS_HIGH;
			break;
		case '3':
			mode |= SPI_3WIRE;
			break;
		case 'N':
			mode |= SPI_NO_CS;
			break;
		case 'R':
			mode |= SPI_READY;
			break;
		default:
			print_usage(argv[0]);
			break;
		}
	}
}

int main(int argc, char *argv[])
{
	int ret = 0;
	int fd;
	
	parse_opts(argc, argv);

	fd = open(device, O_RDWR);
	if (fd < 0)
		pabort("can't open device");

	/*
	 * spi mode
	 */
	ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
	if (ret == -1)
		pabort("can't set spi mode");

	ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
	if (ret == -1)
		pabort("can't get spi mode");

	/*
	 * bits per word
	 */
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't set bits per word");

	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't get bits per word");

	/*
	 * max speed hz
	 */
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't set max speed hz");

	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't get max speed hz");

	printf("SPI mode       : %d\n", mode);
	printf("Bits per word  : %d\n", bits);
	printf("Max speed      : %d Hz (%d KHz)\n", speed, speed/1000);

	if (SS963_USING) {
		
		printf("\nUsing SS963 Serial Driver Card...\n", LATCH_PIN);
		printf("Latch Pin   : %d\n", LATCH_PIN);
		printf("^Reset Pin  : %d\n", RESET_PIN);
		printf("Latch Delay : %d\n", LATCH_DELAY);
		
		// Open the GPIO devices and allows our program to access it
		wiringPiSetup();	
		
		// Set pin modes
		pinMode (RESET_PIN, OUTPUT);
		pinMode (LATCH_PIN, OUTPUT);

		// Disable Low-Active reset pin of SS963	
		digitalWrite(RESET_PIN, HIGH);
		digitalWrite(LATCH_PIN, LOW);
	}
	
	transfer(fd);
	
	if (SS963_USING) {
		
		// Latch data to outputs (Qn) from SS963 registers (RQn)	
		digitalWrite(LATCH_PIN, HIGH);
		usleep(LATCH_DELAY);
		digitalWrite(LATCH_PIN, LOW);
	}

	close(fd);

	return ret;
}
