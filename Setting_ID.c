//setting new ID & make it's angel=7500
#include "ASA_Lib.h"
#include <avr/interrupt.h>
#include <math.h>
#include <string.h>
#define FOSC 11059200// Clock Speed

void UART1_Init( unsigned int ubrr )
{
  /* Set baud rate */
  UBRR1H |= (unsigned char)(ubrr>>8);   //p.362 // fosc = 11.0592MHz，Baud Rate=9600，U2X=0 =>UBRR=71，U2X=1=>UBRR=143
  UBRR1L |= (unsigned char)ubrr;
  /* Enable receiver and transmitter */
  UCSR1B |= (1<<TXEN1);    //enables the USARTn Receiver，enables the USARTn Transmitter
  /* Set frame format: 8data, 2stop bit ,ParityCheck:EVEN */
  UCSR1C |= (1<<UPM11)|(0<<UPM10)|(1<<USBS1)|(1<<UCSZ11)|(1<<UCSZ10)|(0<<UCPOL1);
}

void UART1_Transmit( unsigned char data )
{
while ( !( UCSR1A & (1<<UDRE1)) )  //If UDREn is one, the buffer is empty
;
UDR1 = data;
}


int main(){
  ASA_M128_set();
  DDRB |= (1<<DDB7)|(1<<DDB6)|(1<<DDB5);   //洞洞板通道開啟
  PORTB |= (1<<PB6);//洞洞板通道開啟(洞洞板轉到2
  PORTB &= ~(1<<PB7);
  PORTB &= ~(1<<PB5);
  DDRF=0xff;
  UART1_Init(  FOSC/16/115200-1 );//KONDO

  char New_id=0;
  UART1_Transmit((0XE0) | New_id);
  _delay_ms(1);
  UART1_Transmit(0x01);
  _delay_ms(1);
  UART1_Transmit(0x01);
  _delay_ms(1);
  UART1_Transmit(0x01);
  _delay_ms(1000);
  printf("ID SETTING Done\n" );

  int   a=3500;
  while(1)
  {
    if(a>=7500){
      a=7500;
    }
    printf("a=%d\n",a );
    UART1_Transmit(0b10000000+New_id);
    _delay_ms(1);
    UART1_Transmit(a>>7);
    _delay_ms(1);
    UART1_Transmit(a&127);
    a+=100;
    _delay_ms(500);
  }


  return 0;
}
