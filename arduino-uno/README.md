## Simple test program to drive SS963 with Arduino Uno's SSPI port
In this example, the code of the Arduino program, which controls the outputs of the serial driver 963 with an output of 96 units of 3Amper, is given according to an animation. The circuit connection is as follows

## ss963 seri sürücü (Turkish)
Bu örnekte 96 adet 3Amper'lik çıkaşa sahip seri sürücü 963'ün çıkışlarını bir animasyona göre kontrol eden Arduino programının kodu verilmiştir. Devre bağlantısı aşağıdaki gibidir:

 ## connection diagram
 
  |ss963  | Uno|         
  |-------|-----------|         
  |Latch|   9           |
  |CLK|     13  (SCL)   |
  |Data|    11  (MOSI)  |
  |Reset|   8           |
  |Gnd|     Gnd         |


## See in Action
[![See arduino code in action](http://www.tankado.com/wp-content/uploads/2018/01/2018-01-26_5-41-11.png)](https://www.youtube.com/watch?v=2ZBsiXzz8tk&feature=youtu.be)
