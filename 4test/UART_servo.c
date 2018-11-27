#include "ASA_Lib.h"

#define FOSC 11059200// Clock Speed
#define BAUD 115200//UL
#define MYUBRR FOSC/16/BAUD-1

volatile long long clock = 0;

unsigned int Now[17] ;

void timer2_Init(){
  TCCR2|=(1<<WGM21)|(0<<WGM20);  //CTC Mode
  TCCR2|=(1<<CS21)|(1<<CS20)|(0<<CS22); //clkI/O/64 (From prescaler)
  TIMSK|=(1<<OCIE2);
  OCR2=171;
}

void USART_Init( unsigned int ubrr )
{
  /* Set baud rate */
  UCSR0B|=(1<<RXCIE0);//|(1<<TXCIE0);  //致能TX，RX complete interrupt
  UBRR1H |= (unsigned char)(ubrr>>8);   //p.362 // fosc = 11.0592MHz，Baud Rate=9600，U2X=0 =>UBRR=71，U2X=1=>UBRR=143
  UBRR1L |= (unsigned char)ubrr;
  /* Enable receiver and transmitter */
  UCSR1B |= (1<<RXEN1)|(1<<TXEN1);    //enables the USARTn Receiver，enables the USARTn Transmitter
  /* Set frame format: 8data, 2stop bit */
  //UCSR1C = (0<<USBS1)|(3<<UCSZ10); //selects the number of stop bits，USBS1=1=> 2 bits
  UCSR1C |= (1<<UPM11)|(0<<UPM10)|(1<<USBS1)|(1<<UCSZ11)|(1<<UCSZ10)|(0<<UCPOL1);//Character Size=8 bits，UCPOL1=上升/下降
}


void KONDO_transmit(){

  int Servo_ID[9] = {0,1,2,4,6,7,8,9,10};

  /*洞洞板重新設定*/
  DDRB |= (1<<DDB7)|(1<<DDB6)|(1<<DDB5);   //洞洞板通道開啟
  PORTB |= (1<<PB6);   //洞洞板通道開啟(洞洞板轉到2)
  PORTB &= ~(1<<PB7);
  PORTB &= ~(1<<PB5);
  static int i = 0;

  for( i = 0;i < 17; i++){

    //判斷ID，決定致能左或右
    _delay_ms(1); //軸與軸之間切換(switch切換時間)
    if(i > 8){  //9~16
      PORTF = 127;
      while ( !( UCSR1A & (1<<UDRE1)) );  //If UDREn is one, the buffer is empty
      UDR1 = Servo_ID[i-8] + 128;
      while ( !( UCSR1A & (1<<UDRE1)) );  //If UDREn is one, the buffer is empty
      UDR1 = Now[i]>>7;
      while ( !( UCSR1A & (1<<UDRE1)) );  //If UDREn is one, the buffer is empty
      UDR1 = Now[i]&127;

    }else{
      PORTF = 191;  //0~8
      while ( !( UCSR1A & (1<<UDRE1)) );  //If UDREn is one, the buffer is empty
      UDR1 = Servo_ID[i] + 128;
      while ( !( UCSR1A & (1<<UDRE1)) );  //If UDREn is one, the buffer is empty
      UDR1 = Now[i]>>7;
      while ( !( UCSR1A & (1<<UDRE1)) );  //If UDREn is one, the buffer is empty
      UDR1 = Now[i]&127;
    }

  }

}


int main(void)
{
  ASA_M128_set();
  DDRB = 0xFF;
  DDRF = 0xFF;
  USART_Init ( MYUBRR );
  timer2_Init();
  int j = 0;

  sei();

  while (1) {

      for(int i = 0;i < 17 ;i ++){Now[i] = 7500+j;}
      j=j+10;

      if(j > 100){j=0;}

      long int t1 = clock;
      // while(1){
      //   	// KONDO_transmit();
      // }
  	
      long int t2 = clock;
      printf("time = %ld\n", t2-t1);
      _delay_ms(100);

  }
  return 0;
}


ISR(TIMER2_COMP_vect){
  clock++;
  if(clock >= 400){
     KONDO_transmit();
     clock = 0;
  }
 
}

ISR(USART0_RX_vect) {
	;
}
