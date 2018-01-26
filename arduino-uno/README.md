## Simple test program to drive SS963 with Arduino Uno's SSPI port
The SS963 serial driver board includes code samples that you can test and use on Raspberry Pi (GCC) and Arduino.
You can also use the SPI tool, a generic tool, with a flexible command line from the Raspberry Pi's SPI port.

## ss963 seri sürücü (Turkish)
Bu örnekte seri sürücü 963'ün çıkışlarını sırayla yakan bir Arduino programının kodu verilmiştir. Bağlantı şekli aşağıdaki gibidir:

 ## connection diagram
 
  |ss963  | Uno|         
  |-------|-----------|         
  |Latch|   9           |
  |CLK|     13  (SCL)   |
  |Data|    11  (MOSI)  |
  |Reset|   8           |
  |Gnd|     Gnd         |


## See in Action
![ss963]()
[![See arduino code in action](http://www.tankado.com/wp-content/uploads/2018/01/2018-01-26_5-41-11.png)](https://www.youtube.com/watch?v=2ZBsiXzz8tk&feature=youtu.be)
