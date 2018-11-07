#include "ASA_Lib.h"
//UART1接藍牙 

#define FOSC 11059200// Clock Speed
#define BAUD1 9600
#define MYUBRR1 (FOSC/16/BAUD1-1)

volatile uint8_t i,j,get[100];


void USART_Init( unsigned int ubrr )
{
  UBRR1H |= (unsigned char)(ubrr>>8);
  UBRR1L |= (unsigned char)ubrr;
  UCSR1B|=(1<<RXCIE1)|(1<<TXCIE1);//Tx Rx中斷致能
  UCSR1B |= (1<<RXEN1)|(1<<TXEN1);//Tx Rx致能
  UCSR1B &= (~(1<<UCSZ12));//8-bit: UCSZn2=0,UCSZn1=1,UCSZn0=1
  UCSR1C |=(1<<UCSZ11)|(1<<UCSZ10);
  UCSR1C &= (~(1<<USBS1));//stopbit=1
}

void USART_Transmit( unsigned char data )
{
  UDR1 = data;
  while ( !( UCSR1A & (1<<UDRE1)) )  //If UDREn is one, the buffer is empty
  ;
}



int main(){
  ASA_M128_set();
	DDRB |= (1<<DDB7)|(1<<DDB6)|(1<<DDB5);   //洞洞板通道開啟
  PORTB |= (1<<PB6);//洞洞板通道開啟(洞洞板轉到2
  USART_Init( MYUBRR1 );
  sei();
  while(1){
    printf("i=%d j=%d \n",i,j);

  }

  return 0;
}


ISR(USART1_RX_vect) {
	get[i]=UDR1;
  i++;
  USART_Transmit(0xAA);//ThisIsAck
}
ISR(USART1_TX_vect) {
	j++;
}
