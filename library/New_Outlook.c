//New Structure New Outlook 20181115
#include "ASA_Lib.h"
#include <avr/interrupt.h>
#include <math.h>
#include <string.h>

#define FOSC 11059200// Clock Speed
#define BAUD0 9600  //
#define BAUD1 115200//UL 115200
#define MYUBRR0 FOSC/16/BAUD0-1
#define MYUBRR1 FOSC/16/BAUD1-1


void timer2_Init();
void my_decoder();
void USART1_Init();
void USART0_Init();

void KONDO();//伺服機監控器
void command_processor(uint8_t c);//監控命令處理器
void robot_gesture_player();//機器人姿態播放器

typedef enum {Header,Bytes,type,Data,checksum}state;
state now_state;
uint16_t t;//計時中斷
uint8_t mode;//輪循(mode=0:學習,mode=1:控制)
uint8_t get[3];//解封包前用的

uint_8 angle_FIFO[2][10];//伺服機設定角度
uint_8 command;//伺服機設定命令
uint8_t now[17];//姿態FIFO
uint8_t SDC_FIFO[170];//動作姿態串暫存器

int main(){
  ASA_M128_set();
  DDRB |= (1<<DDB7)|(1<<DDB6)|(1<<DDB5);   //洞洞板通道開啟
  PORTB |= (1<<PB6);   //洞洞板通道開啟(洞洞板轉到2)
	PORTB &= ~(1<<PB7);
	PORTB &= ~(1<<PB5);
	DDRF=0xff;        //用PF1~4控制機器人左右半邊以及要送還是收
	USART0_Init ( MYUBRR0 );
	USART1_Init ( MYUBRR1 );
  timer2_Init();
  sei();
  while (1) {
    switch (mode) {
      case 0:{//學習模式
        robot_gesture_player();//機器人姿態播放器
      }
      case 1:{//控制模式
        command_processor(command);//監控命令處理器
      }
    }
  }
  return 0;
}




//待寫函式/////////////////////////////////////////////////////////////////////
void robot_gesture_player(){//機器人姿態播放器
  if(mode==0){
    ;//處理angle_FIFO(伺服機設定角度)裡的東西,更新now[](姿態)
  }else{
    ;//處理SDC_FIFO(動作姿態串暫存器)裡的東西,更新now[](姿態)
  }
}
void command_processor(uint8_t c){//監控命令處理器
  if(c<=7 && c>=1){
    switch (c) {
      case 1: ;//使用者按下紀錄 此姿態和上一個姿態間格:很快
      case 2: ;//使用者按下紀錄 此姿態和上一個姿態間格:  快
      case 3: ;//使用者按下紀錄 此姿態和上一個姿態間格:正常
      case 4: ;//使用者按下紀錄 此姿態和上一個姿態間格:  慢
      case 5: ;//使用者按下紀錄 此姿態和上一個姿態間格:很慢
      case 6: ;//使用者按下DEL 刪除前一個姿態
      case 7: ;//使用者按下清空
    }
  }else if(c<=18 && c>=11){
    case 11: ;//資料寫入SD卡
    case 12: ;//資料寫入SD卡
    case 13: ;//資料寫入SD卡
    case 14: ;//資料寫入SD卡
    case 15: ;//資料寫入SD卡
    case 16: ;//資料寫入SD卡
    case 17: ;//資料寫入SD卡
    case 18: ;//資料寫入SD卡
  }else if(c<=28 && c>=21){
    case 21: ;//播放套裝動作1
    case 22: ;//播放套裝動作2
    case 23: ;//播放套裝動作3
    case 24: ;//播放套裝動作4
    case 25: ;//播放套裝動作5
    case 26: ;//播放套裝動作6
    case 27: ;//播放套裝動作7
    case 28: ;//播放套裝動作8
  }

}
void KONDO(){//伺服機監控器  執行時機:計時中斷
  ;
  //1.設定及監控:讀取姿態 now[]
  //2.控制:把第0軸轉到now[0],第1軸轉到now[1]....
}


//中斷函式/////////////////////////////////////////////////////////////////////
ISR(TIMER2_COMP_vect){
  t++;
  if(t>100){ //頻率待測
    KONDO();//伺服機監控器
    t=0;
  }
}
ISR(USART0_RX_vect) {
	uint8_t g=UDR0;
  my_decoder(g);
}
//應該不會再大改的函式///////////////////////////////////////////////////////////
void USART1_Init( unsigned int ubrr )
{
/* Set baud rate */
// UCSR1B|=(1<<TXCIE1);  //致能TX，RX complete interrupt，沒有用到的話，不可以致能
UBRR1H |= (unsigned char)(ubrr>>8);   //p.362 // fosc = 11.0592MHz，Baud Rate=9600，U2X=0 =>UBRR=71，U2X=1=>UBRR=143
UBRR1L |= (unsigned char)ubrr;
/* Enable receiver and transmitter */
UCSR1B |= (1<<RXEN1)|(1<<TXEN1);    //enables the USARTn Receiver，enables the USARTn Transmitter
/* Set frame format: 8data, 2stop bit */
//UCSR1C = (0<<USBS1)|(3<<UCSZ10); //selects the number of stop bits，USBS1=1=> 2 bits
UCSR1C |= (1<<UPM11)|(0<<UPM10)|(1<<USBS1)|(1<<UCSZ11)|(1<<UCSZ10)|(0<<UCPOL1);//Character Size=8 bits，UCPOL1=上升/下降
}
void USART0_Init( unsigned int ubrr )
{
  UBRR0H |= (unsigned char)(ubrr>>8);
  UBRR0L |= (unsigned char)ubrr;
  UCSR0B|=(1<<RXCIE0);// Rx中斷致能
  UCSR0B |= (1<<RXEN0)|(1<<TXEN0);//Tx Rx致能
  UCSR0B &= (~(1<<UCSZ02));//8-bit: UCSZn2=0,UCSZn1=1,UCSZn0=1
  UCSR0C |=(1<<UCSZ01)|(1<<UCSZ00);
  UCSR0C &= (~(1<<USBS0));//stopbit=1
}
void timer2_Init(){
  TCCR2|=(1<<WGM21)|(0<<WGM20);  //CTC Mode
  TCCR2|=(1<<CS21)|(1<<CS20)|(0<<CS22); //clkI/O/64 (From prescaler)
  TIMSK|=(1<<OCIE2);
  OCR2=171;
}
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
        now_state=Data;
      }else{
        now_state=Header;
      }
      break;
    }
    case Data:{
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
