#include "ASA_Lib.h"       //get robot 位置
#include <math.h>
#include <string.h>

#define FOSC 11059200// Clock Speed
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1

volatile char get[100];
int i;

void USART_Flush( void )
{
	unsigned char dummy;
	while ( UCSR1A & (1<<RXC1) ) dummy = UDR1;
}

void USART_Init( unsigned int ubrr )
{
/* Set baud rate */
UCSR1B|=(1<<RXCIE1)|(1<<TXCIE1);  //致能TX，RX complete interrupt
UBRR1H |= (unsigned char)(ubrr>>8);   //p.362 // fosc = 11.0592MHz，Baud Rate=9600，U2X=0 =>UBRR=71，U2X=1=>UBRR=143
UBRR1L |= (unsigned char)ubrr;
/* Enable receiver and transmitter */
UCSR1B |= (1<<RXEN1)|(1<<TXEN1);    //enables the USARTn Receiver，enables the USARTn Transmitter
/* Set frame format: 8data, 2stop bit */
//UCSR1C = (0<<USBS1)|(3<<UCSZ10); //selects the number of stop bits，USBS1=1=> 2 bits
UCSR1C |= (1<<UPM11)|(0<<UPM10)|(1<<USBS1)|(1<<UCSZ11)|(1<<UCSZ10)|(0<<UCPOL1);//Character Size=8 bits，UCPOL1=上升/下降
}

void USART_Transmit( unsigned char data )
{
	// PORTB=0b00001000;
	PORTB|=(1<<PB4);
	PORTB&= ~(1<<PB3);
/* Wait for empty transmit buffer */

/* Put data into buffer, sends the data */
UDR1 = data;
while ( !( UCSR1A & (1<<UDRE1)) )  //If UDREn is one, the buffer is empty
;


// printf("UDR1=%d\n",UDR1 );

}


uint8_t uart1_get() {

    // while(!(UCSR1A&(1<<RXC1))){
		// 	// printf("!!!!!\n" );
		// };

    return UDR1;

}



int main(void)
{
  ASA_M128_set();
  printf("start\n");
	DDRB |= (1<<DDB7)|(1<<DDB6)|(1<<DDB5);   //洞洞板通道開啟
  PORTB |= (1<<PB5);//洞洞板通道開啟(洞洞板轉到1)

  USART_Init ( MYUBRR );
	sei();	// 開啟所有中斷功能

	for(int i=0;i<100;i++){
		get[i]=0;
	}



	while(1)
  {
	  for(int j=0;j<100;j++){
        if(j%4 ==0 && j>=4){
          printf("%d get: %d %d %d %d ",j-4,get[j-4],get[j-3],get[j-2],get[j-1] );   //印出收到的值
          printf("\n");
        }
        if(j==99) printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n" );
    }

    if(i>99){
      i=0;
      printf("^___________________________________________^" );
    }
  }




    return 0;
}

ISR(USART1_RX_vect) {
	get[i]=UDR1;
	i++;
}
ISR(USART1_TX_vect) {
  ;
}
