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
#define RF_SEND_BUFFER_LEN_MAX 128
volatile uint16_t output_count=0;
volatile uint8_t active_buffer=0;
volatile uint16_t send_buffer=0;
volatile uint8_t capture=0;
volatile uint16_t rf_send=0; //count of bits in rf_send_buffer that should be sent
volatile uint16_t rf_send_reload=0; //count of bits in rf_send_buffer that should be sent
volatile uint8_t rf_send_reload_count=0; // number of repetitions (times rf_send gets reloaded;
uint8_t read_buffer[64]; // buffer for reading usb signals
uint8_t write_buffer[2][64]; // buffer for writing usb signals
uint8_t rf_send_buffer[RF_SEND_BUFFER_LEN_MAX]; // buffer for sending rf433 signals
volatile uint16_t rf_send_buffer_len=0;


void reset()
{
  cli();
  // disable watchdog, if enabled
  // disable all peripherals
  UDCON = 1;
  USBCON = (1<<FRZCLK);  // disable USB
  UCSR1B = 0;
  _delay_ms(5);
  #if defined(__AVR_AT90USB162__)                // Teensy 1.0
      EIMSK = 0; PCICR = 0; SPCR = 0; ACSR = 0; EECR = 0;
      TIMSK0 = 0; TIMSK1 = 0; UCSR1B = 0;
      DDRB = 0; DDRC = 0; DDRD = 0;
      PORTB = 0; PORTC = 0; PORTD = 0;
      asm volatile("jmp 0x3E00");
  #elif defined(__AVR_ATmega32U4__)              // Teensy 2.0
      EIMSK = 0; PCICR = 0; SPCR = 0; ACSR = 0; EECR = 0; ADCSRA = 0;
      TIMSK0 = 0; TIMSK1 = 0; TIMSK3 = 0; TIMSK4 = 0; UCSR1B = 0; TWCR = 0;
      DDRB = 0; DDRC = 0; DDRD = 0; DDRE = 0; DDRF = 0; TWCR = 0;
      PORTB = 0; PORTC = 0; PORTD = 0; PORTE = 0; PORTF = 0;
      asm volatile("jmp 0x7E00");
  #elif defined(__AVR_AT90USB646__)              // Teensy++ 1.0
      EIMSK = 0; PCICR = 0; SPCR = 0; ACSR = 0; EECR = 0; ADCSRA = 0;
      TIMSK0 = 0; TIMSK1 = 0; TIMSK2 = 0; TIMSK3 = 0; UCSR1B = 0; TWCR = 0;
      DDRA = 0; DDRB = 0; DDRC = 0; DDRD = 0; DDRE = 0; DDRF = 0;
      PORTA = 0; PORTB = 0; PORTC = 0; PORTD = 0; PORTE = 0; PORTF = 0;
      asm volatile("jmp 0xFC00");
  #elif defined(__AVR_AT90USB1286__)             // Teensy++ 2.0
      EIMSK = 0; PCICR = 0; SPCR = 0; ACSR = 0; EECR = 0; ADCSRA = 0;
      TIMSK0 = 0; TIMSK1 = 0; TIMSK2 = 0; TIMSK3 = 0; UCSR1B = 0; TWCR = 0;
      DDRA = 0; DDRB = 0; DDRC = 0; DDRD = 0; DDRE = 0; DDRF = 0;
      PORTA = 0; PORTB = 0; PORTC = 0; PORTD = 0; PORTE = 0; PORTF = 0;
      asm volatile("jmp 0x1FC00");
  #endif 
}

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
  OCR0A = 249;
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
    int8_t r = usb_rawhid_recv(read_buffer, 0);
    if (r>0)
    {
      if (read_buffer[0]=='r') //reset
        reset();
      else if (read_buffer[0]=='b') //begin capture
        capture=1;
      else if (read_buffer[0]=='e') //end capture
        capture=0;
      else if (read_buffer[0]=='f') //fill send buffer
      {
        int8_t buffer_pos = 1;
        while(buffer_pos < r && rf_send_buffer_len<RF_SEND_BUFFER_LEN_MAX)
        {
          rf_send_buffer[rf_send_buffer_len]=read_buffer[buffer_pos];
          rf_send_buffer_len++;
          buffer_pos++;
        }
      }
      else if (read_buffer[0]=='c') // clear send buffer
      {
        rf_send_buffer_len=0;
      }
      else if (read_buffer[0]=='s') //send
      {
        capture=0;
        //if (r>2)
        //  rf_send_reload=rf_send_buffer_len*8-read_buffer[2]; // substract bit offset
        //else
        usb_rawhid_send(rf_send_buffer, 145);
        rf_send=0;
        rf_send_reload=rf_send_buffer_len*8;
        rf_send_reload_count=read_buffer[1];  
        //read_buffer[0]=rf_send_reload;
        //read_buffer[1]=rf_send_reload>>8;
        //read_buffer[2]=0;
      }
    }
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
  if (rf_send)
  {
    if ( ( rf_send_buffer[rf_send/8] >> ( (rf_send%8)?8-(rf_send%8):0 ) ) & 1)
    {
     PORTF&=~1;
    } else {
     PORTF|=1;
    }
    //rf_send_buffer[rf_send/8]>>=1;
    rf_send--;
  } else if (rf_send_reload_count) {
    rf_send=rf_send_reload;
    rf_send_reload_count--;
  } else {
    PORTF&=~1;
    if (capture) {
      write_buffer[active_buffer][output_count/8]<<=1;
      write_buffer[active_buffer][output_count++/8]|=PINB&1;
      if (output_count>=64*8)
      {
        output_count=0;
        active_buffer=active_buffer?0:1;
        send_buffer=1;
      }
    }
  }
}
