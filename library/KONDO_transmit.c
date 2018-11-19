#include "ASA_Lib.h"

unsigned int Now[17];

void KONDO_transmit(){
  int Servo_ID[9] = {0,1,2,4,6,7,8,9,10};

  /*洞洞板重新設定*/
  DDRB |= (1<<DDB7)|(1<<DDB6)|(1<<DDB5);   //洞洞板通道開啟
  PORTB |= (1<<PB6);   //洞洞板通道開啟(洞洞板轉到2)
  PORTB &= ~(1<<PB7);
  PORTB &= ~(1<<PB5);

  for(int i = 0;i < 17; i++){
    //判斷ID，決定致能左或右
    if(i > 8){  //0~8
      PORTF = 127;
      while ( !( UCSR1A & (1<<UDRE1)) );  //If UDREn is one, the buffer is empty
      UDR1 = Servo_ID[i-8] + 128;
      while ( !( UCSR1A & (1<<UDRE1)) );  //If UDREn is one, the buffer is empty
      UDR1 = Now[i]>>7;
      while ( !( UCSR1A & (1<<UDRE1)) );  //If UDREn is one, the buffer is empty
      UDR1 = Now[i]&127;
    }else{
      PORTF = 191;  //9~16
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
  return 0;
}

int clock = 0;
ISR(TIMER2_COMP_vect){

  clock++;
  if(clock==90)
	{
		KONDO_transmit();
	}

}
