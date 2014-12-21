/*
  temp_transmitter - readings sender from attiny board
  Copyright (c) 2014 Daniel Alkemic Czuba.  All right reserved.

  Project home: https://github.com/Alkemic/433_trans_receiver

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include <OneWire.h> // http://www.pjrc.com/teensy/td_libs_OneWire.html
#include <RCSwitch.h> // https://code.google.com/p/rc-switch/
#include <avr/sleep.h>

// Sensor address can be easily obtained using OneWire's DS18x22_Temperature sketch
#define DS_PIN 2 // 3
#define TX_PIN 3 // 4
#define NODE_ID 1

// utility macros
#define adc_disable() (ADCSRA &= ~(1<<ADEN)) // disable ADC (before power-off)
#define adc_enable()  (ADCSRA |=  (1<<ADEN)) // re-enable ADC
#define GET_BIT(var, pos) (((var) & (1<<(pos))) >> pos)

OneWire ds(DS_PIN);
RCSwitch tx = RCSwitch();

byte i;
byte present = 0;
byte data[12];
byte addr[8] = {0x00};
byte cfg;
unsigned int raw;
unsigned long packet;

void setup(void) {
  adc_disable();
  tx.enableTransmit(TX_PIN);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

  ds.search(addr);
}

void loop(void) {
  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);

  delay(1000);

  present = ds.reset();
  ds.select(addr);
  ds.write(0xBE);

  for ( i = 0; i < 9; i++) {
    data[i] = ds.read();
  }

  raw = (data[1] << 8) | data[0];
  cfg = (data[4] & 0x60);
  if (cfg == 0x00) raw = raw << 3;
  else if (cfg == 0x20) raw = raw << 2;
  else if (cfg == 0x40) raw = raw << 1;

  // some lame "checksum", need to implement CRC8
  packet = raw;
  packet <<= 4;
  packet |= (GET_BIT(raw, 7) << 3) | (GET_BIT(raw, 5) << 2) | (GET_BIT(raw, 3) << 1) | GET_BIT(raw, 1);
  packet <<= 4;
  packet |= NODE_ID;

  tx.send(packet, 32);
  delay(60000);
}
