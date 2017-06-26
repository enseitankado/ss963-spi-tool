#spi-tool v1.0
##SS963 Serial Power Driver Card Testing Utility and Generic SPI Testing Tool

Written by: Ensei Tankado, ensei@tankado.com
**Copyright (c) 2017  Izlence Electronic Software Inc.**  [www.izlencebilisim.com](http://www.izlencebilisim.com/urun/ss963-seri-surucu-karti/2/)

Referances:
Copyright (c) 2007  MontaVista Software, Inc.
Copyright (c) 2007  Anton Vorontsov <avorontsov@ru.mvista.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License.

> This program uses wiringPi library from wiringpi.com.  
> First you must install it: http://wiringpi.com/download-and-install/
 
 And enable SPI port with raspi-config tool then you can compile the spi-tool as below:
 > gcc spi-tool.c -lwiringPi -o spi-tool -Ofast -s
 
 To see supported parameters use -h:
 > ./spi-tool -h

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

  
**Development environment specifics:**

 - Tested on Raspberry Pi V3 hardware, running Raspbian (4.4.38-v7+)
 - Building with gcc version 4.9.2 (Raspbian 4.9.2-10)
  
### Example-1:

Transmits **5 byte (40bits)** data (*0xFF,0xFE,0xFD,0xFC,0xFF*) to **SPI.0** at **500KHz**.  The **tx data** consist of **hexadecimal coded bytes** stream as below.  Also in this tutorial receiver device SS963 dont reply back any data  as seen in **RX section** (**00 00 00 00 00**). Attention this could means  **MISO** terminal is not connected.

>  **./spi-tool -s 500000 -t 0xFF,0xFE,0xFD,0xFC,0xFF**

	
    SPI mode       : 0
    Bits per word  : 8
    Max speed      : 500000 Hz (500 KHz)
    
    TX:
    FF FE FD FC FF
    
    RX:
    00 00 00 00 00

### Example-2:

In this tutorial we use **SS963 Serial Power Driver** from [izlencebilisim.com](http://www.izlencebilisim.com/urun/ss963-seri-surucu-karti/2/). Because of this we add -**S parameter** (SS963) and use **-crp** parameters to  drive SS963 card with 96bit. Connection diagram provided in  "example-2-connection-diagram.png" below. Check this:

![enter image description here](https://github.com/enseitankado/ss963-serial-power-driver/blob/master/spi-tool/example-2-connection-diagram.png?raw=true)

> **sudo ./spi-tool -c 3 -r 2 -p 1000 --SS963 -t  0xAA,0xAA,0xAA,0xAA,0xAA,0xAA**

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

In this example we want to take your attention to a few important things as relevant using of SS963 Serial Power Driver. First you must add **sudo** command to beginning of command because of the wiringPi functions which do latch **coudlnt runs in user space rights**. SS963 uses **latch** and **reset** functionalities. To clear (reset) all outputs set it to  **logical "0"** because of it is **active low**. After all bits transmitted to SS963 registers to write all registers to own outputs **Lacth is required**.  Whe you add **-S or --SS963** parameters the **latch signal will automaticly send**  after the data transmission was completed. Parameter -p (l-delay) sets period of latch signal.  For using of parameters please check tool's usage list with -h parameter. Examples 2 logic analyzer output is as seen as below:

![enter image description here](https://github.com/enseitankado/ss963-serial-power-driver/blob/master/spi-tool/example-2-analyzer-screenshot.png?raw=true)


### See the tutorial video
[![See tutorial video](https://img.youtube.com/vi/D1Yi7_5iR5U/0.jpg)](https://www.youtube.com/watch?v=D1Yi7_5iR5U)

