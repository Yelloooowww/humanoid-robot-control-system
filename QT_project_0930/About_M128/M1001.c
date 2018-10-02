#include "ASA_Lib.h"
#include <math.h>
#include <string.h>

#define FOSC 11059200// Clock Speed
#define BAUD1 9600  //
#define MYUBRR1 (FOSC/16/BAUD1-1)

volatile uint8_t i,j,get[100];
int mode;
void USART_Flush( void )
{
	unsigned char dummy;
	while ( UCSR1A & (1<<RXC1) ) dummy = UDR1;
}

void USART_Init( unsigned int ubrr )
{
UCSR1B|=(1<<RXCIE1)|(1<<TXCIE1);
UBRR1H |= (unsigned char)(ubrr>>8);
UBRR1L |= (unsigned char)ubrr;
UCSR1B |= (1<<RXEN1)|(1<<TXEN1)|(1<<UCSZ12);
UCSR1C &=(0<<UCSZ11)&(0<<UCSZ10);//8bit
UCSR1C &=(0<<UPM11)&(0<<UPM10);//奇偶檢查禁能
UCSR1C |= (1<<USBS1);//stopbit=1
}

void USART_Transmit( unsigned char data )
{
  UDR1 = data;
  while ( !( UCSR1A & (1<<UDRE1)) )  //If UDREn is one, the buffer is empty
  ;
}


void Alway_Check_Mode(){
  for(char f=0;f<20;f++){
    USART_Transmit(f);//ACK很重要所以回應三次
    _delay_ms(50);
  }
  if(get[i]==130 ) {
    mode=130;
  }else if(get[i]==129 ) {
    mode=129;
  }else if(get[i]==128 ) {
    mode=128;
  }
  for(int j=0;j<100;j++) get[j]=0;
  i=0;
}

int main(){
  ASA_M128_set();
	DDRB |= (1<<DDB7)|(1<<DDB6)|(1<<DDB5);   //洞洞板通道開啟
  PORTB |= (1<<PB5);//洞洞板通道開啟(洞洞板轉到1
  USART_Init( MYUBRR1 );
  sei();
  i=0;
  j=0;
  mode=0;
  for(int k=0;k<100;k++) get[k]=0;
  while(1){
		// printf("mode=%d i=%d \n",mode,i);
    if(mode==128){
      // printf("HereIsMode128\n" );
    }else if(mode==129){
      // printf("HereIsMode129\n" );
    }else if(mode==130){
      // printf("HereIsMode130\n" );
    }
	}


  return 0;
}



ISR(USART1_RX_vect) {
	get[i]=UDR1;
	if((get[i]!=128) &&(get[i]!=129) &&(get[i]!=130)) i++;
  else Alway_Check_Mode();
}
ISR(USART1_TX_vect) {
	j++;
}
