/*
  ss963   Uno         Logic Analyzer
  -----   ---         --------------
  Latch   9           3
  CLK     13  (SCL)   2
  Data    11  (MOSI)  1
  Reset   8           4
  Gnd     Gnd         Gnd
*/

#include <SPI.h>

byte ports[96];
byte byte_array[12];

int DATA_PIN = 11; /* MOSI */
int LATCH_PIN = 9;
int RESET_PIN = 8;
int LATCH_DELAY_MS = 1;
int i, j;

void setup() {
  pinMode(RESET_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);
  digitalWrite(RESET_PIN, HIGH);

  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV2); /* 16MHz/2 = 8MHz hiza ayarla */

  Serial.begin(115200);
}

/*
 * Converts 'ports' bit array to bytes array.
 * After, bytes array will transmit to SPI port.
 */
void convert_bit_array_to_byte_array(byte *ports, byte *byte_array) {
  int i, j;
  for (i = 0; i < 12; i++)
  {
    byte_array[i] = 0;
    for (j = 0; j < 8; j++)
      byte_array[i] += ports[i * 8 + j] << j;
  }
}

/*
 * Set all ports of card to LOW or HIGH.
 * Not critial.
 */
void fill_ports(byte *ports, byte on_off)
{
  int i;
  for (i = 0; i < 96; i++)
    ports[i] = on_off;
}

/*
 * Store ports array data to card's buffer registers.
 */
void write_ports(byte *ports)
{
  convert_bit_array_to_byte_array(ports, byte_array);

  for (j = 11; j >= 0; j--) {
    SPI.transfer(byte_array[j]);
  }

  latch();
}


/*
 * Do latch.
 * The latch function writes buffer registers to outs.
 */
void latch() {
  digitalWrite(LATCH_PIN, HIGH);
  delay(LATCH_DELAY_MS);
  digitalWrite(LATCH_PIN, LOW);
}

void loop() {

  fill_ports(ports, 0);
  for (i = 0; i < 96; i++)
  {
    ports[i] = HIGH;
    write_ports(ports);
    delay(20);
  }

  for (i = 0; i < 3; i++)
  {
    fill_ports(ports, 0);
    write_ports(ports);
    delay(500);
    fill_ports(ports, 1);
    write_ports(ports);
    delay(500);
  }

  for (i = 95; i >= 0; i--)
  {
    ports[i] = LOW;
    write_ports(ports);
    delay(20);
  }

}



