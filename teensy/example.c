/* Teensy RawHID example
 * http://www.pjrc.com/teensy/rawhid.html
 * Copyright (c) 2009 PJRC.COM, LLC
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above description, website URL and copyright notice and this permission
 * notice shall be included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "usb_rawhid.h"

#define CPU_PRESCALE(n)	(CLKPR = 0x80, CLKPR = (n))

volatile uint16_t output_count=0;
volatile uint8_t active_buffer=0;
volatile uint8_t send_buffer=0;
uint8_t read_buffer[64];
uint8_t write_buffer[2][64];

int main(void)
{
  // set for 16 MHz clock
  CPU_PRESCALE(0);
  DDRF|=3;
  PORTF&=~1;
  // Configure timer 0 to generate a timer overflow interrupt every
  // 200*8 clock cycles, 100us
  TCCR0A = 1<<WGM01;
  TCCR0B = 1<<CS01;
  OCR0A = 199;
  TCNT0 = 0;
  TIMSK0 = (1<<OCIE0A);

  // Initialize the USB, and then wait for the host to set configuration.
  // If the Teensy is powered without a PC connected to the USB port,
  // this will wait forever.
  usb_init();
  while (!usb_configured()) /* wait */ ;
  // Wait an extra second for the PC's operating system to load drivers
  // and do whatever it does to actually be ready for input
  _delay_ms(1000);
  while (1) {
    // if received data, do something with it
    //r = usb_rawhid_recv(read_buffer, 0);
    //if (r>0)
    if (send_buffer)
    {
      send_buffer=0;
      usb_rawhid_send(write_buffer[active_buffer?0:1], 45);
    }
  }
}

// This interrupt routine is run approx 61 times per second.
ISR(TIMER0_COMPA_vect)
{
  PORTF^=2;
  write_buffer[active_buffer][output_count/8]<<=1;
  write_buffer[active_buffer][output_count++/8]|=PINB&1;
  if (output_count>=64*8)
  {
    output_count=0;
    active_buffer=active_buffer?0:1;
    send_buffer=1;
  }
}


