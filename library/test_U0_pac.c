#include "ASA_Lib.h"
//UART1接藍牙
//可以正確解包

#define FOSC 11059200// Clock Speed
#define BAUD0 9600
#define MYUBRR0 (FOSC/16/BAUD0-1)
volatile uint8_t i,j,get[200];

typedef enum {Header,Bytes,type,Data,checksum}state;
state now_state;
void my_decoder(uint8_t u){
  USART_Transmit(0xAB);
  static uint8_t b,d,sum_check;
  static uint16_t len_data;
  switch (now_state) {
    case Header:{
      USART_Transmit(1);
      b=0;
      d=0;
      len_data=0;
      sum_check=0;
      if(u==0xfd) now_state=type;
      break;
    }
    case type:{
      USART_Transmit(2);
      if(u) now_state=Bytes; //只要是正數就好了@@
      else  now_state=Header;

      break;
    }
    case Bytes:{
      USART_Transmit(3);
      if(b==0){
        sum_check+=u;
        len_data= (u<<8);
        b++;
      }else if(b==1){
        sum_check+=u;
        len_data|=u;
        b=0;
        now_state=Data;
      }else{
        now_state=Header;
      }
      break;
    }
    case Data:{
      USART_Transmit(4);
      if(d<len_data-1){
        sum_check+=u;
        get[d]=u;
        d++;
      }else if(d==len_data-1){
        sum_check+=u;
        get[d]=u;
        d=0;
        len_data=0;
        now_state=checksum;
      }else{
        now_state=Header;
      }
      break;
    }
    case checksum:{
      USART_Transmit(5);
      uint8_t tmp=(sum_check&0xff);
      if(tmp == u){
        sum_check=0;
        now_state=Header;

        USART_Transmit(get[0]);
        USART_Transmit(get[1]);
        USART_Transmit(get[2]);
      }else{
        now_state=Header;
      }
      break;
    }
  }
}




void USART_Init( unsigned int ubrr )
{
  UBRR0H |= (unsigned char)(ubrr>>8);
  UBRR0L |= (unsigned char)ubrr;
  UCSR0B|=(1<<RXCIE1);// Rx中斷致能
  UCSR0B |= (1<<RXEN0)|(1<<TXEN0);//Tx Rx致能
  UCSR0B &= (~(1<<UCSZ02));//8-bit: UCSZn2=0,UCSZn1=1,UCSZn0=1
  UCSR0C |=(1<<UCSZ01)|(1<<UCSZ00);
  UCSR0C &= (~(1<<USBS0));//stopbit=1
}

void USART_Transmit( unsigned char data )
{
  UDR0= data;
  while ( !( UCSR0A & (1<<UDRE0)) )  //If UDREn is one, the buffer is empty
  ;
}





int main(){
  // ASA_M128_set();
  // printf("START\n" );
  DDRB |= (1<<DDB7)|(1<<DDB6)|(1<<DDB5);   //洞洞板通道開啟
  PORTB |= (1<<PB6);//洞洞板通道開啟(洞洞板轉到2
  USART_Init( MYUBRR0);
  sei();
  USART_Transmit(0xAA);
  USART_Transmit(0xAA);
  USART_Transmit(0xAA);
  while (1) {
    _delay_ms(5000);
    USART_Transmit(0xff);

  }
  return 0;
}


ISR(USART0_RX_vect) {
	uint8_t g=UDR0;
  my_decoder(g);
}
// ISR(USART1_TX_vect) {
// 	j++;
// }
