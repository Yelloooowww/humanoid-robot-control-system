#include "ASA_Lib.h"       //get robot 位置
#include "ASA_Lib_DAC00.h"
#include <math.h>
#include <string.h>

#define FOSC 11059200// Clock Speed
#define BAUD 115200//UL
#define MYUBRR FOSC/16/BAUD-1

volatile uint8_t test,get[3];
int i;
int a=6000 ,b = 6000;

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
  uint8_t python_get[1];
  M128_HMI_get(1, python_get);

  USART_Init ( MYUBRR );
	DDRB=0xff;

	sei();	// 開啟所有中斷功能


	while(1)
  {
    M128_HMI_get(1, python_get);

    a=(python_get[0]*80)+3500;
    b=(python_get[0]*80)+3500;
    printf("python_get[0]=%d a=b=%d\n",python_get[0],(python_get[0]/0.0125)+3500 );


		DDRB |= (1<<DDB7)|(1<<DDB6)|(1<<DDB5);   //洞洞板通道開啟
		PORTB &= ~(1<<PB6);
		PORTB |= (1<<PB5);//洞洞板通道開啟(洞洞板轉到1)


    USART_Transmit(0b10000010);//ID
    USART_Transmit(a>>7);
    USART_Transmit(a&127);
		if(i>2) i=0;

		DDRB |= (1<<DDB7)|(1<<DDB6)|(1<<DDB5);   //洞洞板通道開啟
		PORTB &= ~(1<<PB5);
	  PORTB |= (1<<PB6);//洞洞板通道開啟(洞洞板轉到1)



    USART_Transmit(0b10000010);//ID
    USART_Transmit(b>>7);
    USART_Transmit(b&127);
		if(i>2) i=0;




  }


    return 0;
}
ISR(USART1_RX_vect) {
	get[i]=UDR1;
	i++;
}
ISR(USART1_TX_vect) {
	PORTB&= ~(1<<PB4);
	PORTB|=(1<<PB3);
}
