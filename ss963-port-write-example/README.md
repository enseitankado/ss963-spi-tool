## Raspberry Pi SPI interface demo with SS963 Serial Power Driver



**Resources:**
This example makes use of the Wiring Pi library, which streamlines the interface to the the I/O pins on the Raspberry Pi, providing an API that is similar to the Arduino.  You can learn about installing Wiring Pi [here](http://wiringpi.com/download-and-install/). The wiringPi SPI API is documented [here](https://projects.drogon.net/raspberry-pi/wiringpi/spi-library/)

**Hardware connections:**
This source code interfaces with the [SS963 Serial Driver card](http://www.izlencebilisim.com/urun/ss963-seri-surucu-karti/2/) and Raspberry Pi connected as follows:

|Raspberry Pi    |SS963|
|----------------|---------------|
|GND  	            |`GND`            |
|GPIO.2            |`RESET`            |
|GPIO.3            |`LATCH`            |
|SCK            |`CLOCK`            |
|MOSI            |`DATA`            |

To build source code use the command:
>  g++ write-ss963.cpp -lwiringPi -o write-ss963.out -Ofast

Then to run it, first the spi kernel module needs to be loaded.  This can be  done using the GPIO utility in old Raspbian versions unfortunately you need to run the raspi-config program (as root) and select the module SPI
that you wish to load/unload there and reboot.

> sudo ./write-ss963.out

![compile-screenshot.png](https://github.com/enseitankado/ss963-serial-power-driver/blob/master/ss963-port-write-example/compile-screenshot.png?raw=true)

This test uses the single SS963 Serial Power Driver Card.  It shifts a  bit through the card and logic analyzer output as follows:

![port-write-analyzer-screenshot](https://github.com/enseitankado/ss963-serial-power-driver/blob/master/ss963-port-write-example/port-write-analyzer-screenshot.png?raw=true)


Development environment specifics:

 - Tested on Raspberry Pi V3 hardware, running Raspbian (4.4.38-v7+)
 - Building with gcc version 4.9.2 (Raspbian 4.9.2-10)


# Videos 

See some similar tutorials in action: 

(https://www.youtube.com/watch?v=Z-NB86WDfvY&list=PL074LABmfmYpe0lODZZtrJP0slMUnYEdx) 

*Distributed as-is; no warranty is given.*
