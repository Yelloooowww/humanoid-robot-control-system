#include "ASA_Lib.h"
//UART1接藍牙
//可以正確解包

#define FOSC 11059200// Clock Speed
#define BAUD1 9600
#define MYUBRR1 (FOSC/16/BAUD1-1)
volatile uint8_t i,j,get[100];

typedef enum {Header,Bytes,type,data,checksum}state;
state now_state;
void my_decoder(uint8_t u){
  static uint8_t b,d,sum_check;
  static uint16_t len_data;
  switch (now_state) {
    case Header:{
      b=0;
      d=0;
      len_data=0;
      sum_check=0;
      if(u==0xfd) now_state=type;
      break;
    }
    case type:{
      if(u) now_state=Bytes; //只要是正數就好了@@
      else  now_state=Header;

      break;
    }
    case Bytes:{
      if(b==0){
        sum_check+=u;
        len_data= (u<<8);
        b++;
      }else if(b==1){
        sum_check+=u;
        len_data|=u;
        b=0;
        now_state=data;
      }else{
        now_state=Header;
      }
      break;
    }
    case data:{
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
      uint8_t tmp=(sum_check&0xff);
      if(tmp == u){
        sum_check=0;
        now_state=Header;
      }else{
        now_state=Header;
      }
      break;
    }
  }
}




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
  printf("START\n" );
  DDRB |= (1<<DDB7)|(1<<DDB6)|(1<<DDB5);   //洞洞板通道開啟
  PORTB |= (1<<PB6);//洞洞板通道開啟(洞洞板轉到2
  USART_Init( MYUBRR1 );
  sei();
  while (1) {
    if(get[0]){
      for(int j=0;j<100;j++){
        if(get[j]){
          printf("get[%d]=%d \n",j,get[j] );
        }
      }
      // for(int i=0;i<100;i++) get[i]=0;
    }else{  ;}
  }
  return 0;
}


ISR(USART1_RX_vect) {
	uint8_t g=UDR1;
  my_decoder(g);
}
ISR(USART1_TX_vect) {
	j++;
}
