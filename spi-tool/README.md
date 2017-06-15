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
> **TR:** Öncelikle wiring-pi kütüphanesini yükleyelim.
 
 After you can compile the spi-tool as below:
 TR: Daha sonra spi-tool aracını aşağıdaki gibi derleyebilirsiniz.
 > gcc spi-tool.c -lwiringPi -o spi -Ofast -s
 
 To see supported parameters use -h:
 **TR:** Aracın desteklediği argümanları görmek için -h parametresini kullanabilirsiniz:
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
 **TR:** Bu araç Raspberry Pi v3 donanımı üzerinde çalışan Raspbian (4.4.38-v7+) üzerinde denenmiştir.
  
### Example-1:

Transmits **5 byte (40bits)** data (*0xFF,0xFE,0xFD,0xFC,0xFF*) to **SPI.0** at **500KHz**.  The **tx data** consist of **hexadecimal coded bytes** stream as below.  Also in this tutorial receiver device SS963 dont reply back any data  as seen in **RX section** (**00 00 00 00 00**). Attention this could means  **MISO** terminal is not connected.

**TR:** Bu örnek 5 Byte(40bit) 'lık (*0xFF,0xFE,0xFD,0xFC,0xFF*) verisini **SPI.0** portuna **500KHz** hızında aktarır. **RX bölümündeki** (**00 00 00 00 00**) çıktıdan da görülebileceği üzere cihaz geriye dönüt olarak herhangi bir veri göndermemiştir. Hatırlatmak isteriz ki: bu örnek SPI-tool aracının SPI portu ile kullanımına basit bir örnektir. SS963 ile kullanım gerekliliklerini tam olarak karşılamaz. Aracı SPI port iletişimi destekleyen herhangi bir SPI cihazı ile kullanımına örnek için verilmiştir.
	

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

**TR:** Bu örnek ise Raspberry Pi bilgisayarının SPI portunun şemada yer alan bağlantıya göre SS963 ile kullanımını anlatır. Örnekte **-S** parametresi ile alıcı olarak SS963 seri sürücü kartının kullanılacağı belirtilir. **-crp** parametreleri ise sırasıyla latch pininin GPIO numarasını, reset(clear) pininin GPIO numarasını ve latch sinyalinin ne kadar süre uygulanacağını ms olarak belirtir.
![enter image description here](https://github.com/enseitankado/ss963-serial-power-driver/blob/master/spi-tool/example-2-connection-diagram.png?raw=true)

> **sudo ./spi -c 3 -r 2 -p 1000 --SS963 -t  0xAA,0xAA,0xAA,0xAA,0xAA,0xAA**

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

**TR:** Bu  örnekte SS963 seri güç sürücü hakkında bazı önemli noktalara dikkatinizi çekmek istiyoruz. Öncelikle fiziksel port SPI'ye erişmek için **sudo** ile araca yetki vermek gerekir. **SS963** latch ve reset fonksiyonlarına sahiptir. Tüm çıkışları sıfırlamak/temizlemek için reset pini lojik 0 yapılmalıdır. Çünkü bu fonksiyon lojik 0'da tetiklenmekte lojik 1'de ise pasif kalmaktadır. Tüm veri karta seri olarak gönderildikten sonra (SPI portu aracılığı ile) kartın geçici kaydedicilerine depolanır. Kaydedicilerin mevcut değerlerini çıkışlara aktarması için ise latch pini kullanılır. Gönderilen verinin çıkışlara yansıması için latch pininin lojik 0'dan lojik 1'e taşınması gerekir. Veri aktarımını ve latch işlevini yukarıdaki analizör çıktısından görebilirsiniz. Kısaca veri SS963'e seri olarak gönderilir ve latch sinyali ile tüm veri çıkış portlarına kalıcı olarak yazılır.

