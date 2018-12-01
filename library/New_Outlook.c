// 20181201
//FIFO函式增加return 可以check看一下資料有沒有正常放進去
//學習mode完成
//KONDO_transmit()已直接加上FIFO的使用
#include "ASA_Lib.h"
#include <avr/interrupt.h>
#include <math.h>
#include <string.h>
#define FOSC 11059200// Clock Speed
void KONDO_transmit();//伺服機監控器
void command_processor(uint8_t *c );//監控命令處理器
void robot_gesture_player();//機器人姿態播放器
void messenger_dealer(); //訊息交換機MCU
uint16_t clock;//計時中斷
uint8_t mode;//mode=0:學習(除了播放以外),mode=1:控制(播放連續動作)
uint8_t action_num;//動作編號
uint8_t command;//伺服機設定命令
uint16_t now[17]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};//伺服機量測角度
// Internal functions declare~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static int stdio_putchar(char c, FILE *stream);
static int stdio_getchar(FILE *stream);
static FILE STDIO_BUFFER = FDEV_SETUP_STREAM(stdio_putchar, stdio_getchar, _FDEV_SETUP_RW);
static int stdio_putchar(char c, FILE *stream) {
    if (c == '\n')
        stdio_putchar('\r',stream);
    while((UCSR0A&(1<<UDRE0))==0)
        ;
    UDR0 = c;
    return 0;
}
static int stdio_getchar(FILE *stream) {
	int UDR_Buff;
    while((UCSR0A&(1<<RXC0))==0)
        ;
	UDR_Buff = UDR0;
	stdio_putchar(UDR_Buff,stream);
	return UDR_Buff;
}

// FIFO~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
unsigned int  SDC_FIFO_max=10;//for FIFO
unsigned int  SDC_FIFO_rear=9;//for FIFO
unsigned int  SDC_FIFO_front=9;//for FIFO
uint16_t KONDO_SDC_FIFO[10][17];//姿態FIFO
char PutIn(uint16_t *p){
  if((SDC_FIFO_rear+1)%SDC_FIFO_max==SDC_FIFO_front){
    // printf("FIFO Is FULL\n" );
    return 1;
  }else{
    // printf("put in\n" );
    SDC_FIFO_rear=(SDC_FIFO_rear+1)%SDC_FIFO_max;
    printf("*(p+i)= " );
    for(int i=0;i<17;i++){
      printf("%d ",*(p+i) );
      KONDO_SDC_FIFO[i][SDC_FIFO_rear]=*(p+i);
    }
    printf("___________________\n" );
  }
  // printf("AfterPut:SDC_FIFO_rear=%d SDC_FIFO_front=%d\n", SDC_FIFO_rear,SDC_FIFO_front);
  return 0;
}

char TakeOut(uint16_t *t){
  if(SDC_FIFO_front == SDC_FIFO_rear){
    // printf("FIFO Is Empty\n");
    return 1;
  }else{
    // printf("take out\n" );
    SDC_FIFO_front=(SDC_FIFO_front+1)%SDC_FIFO_max;
    printf("*(t+i)= " );
    for(int i=0;i<17;i++){
      *(t+i)=KONDO_SDC_FIFO[i][SDC_FIFO_front];
      printf("%d ",*(t+i));
    }
    printf("___________________\n" );
  }
  // printf("AfterTake:SDC_FIFO_rear=%d SDC_FIFO_front=%d\n", SDC_FIFO_rear,SDC_FIFO_front);
  return 0;
}
// decoder~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
typedef enum {Header,Bytes,type,Data,checksum}state;
state now_state;//解包狀態
uint8_t get[3];//for my_decoder
void my_decoder(uint8_t u){
  static uint8_t b,d,sum_check;
  static uint16_t len_data;
  switch (now_state) {
    case Header:{
      // printf("decoder_Header_____\n" );
      b=0;
      d=0;
      len_data=0;
      sum_check=0;
      if(u==0xfd) now_state=type;
      break;
    }
    case type:{
      // printf("decoder_type_______\n");
      if(u) now_state=Bytes; //只要是正數就好了@@
      else  now_state=Header;

      break;
    }
    case Bytes:{
      // printf("decoder_Bytes______\n" );
      if(b==0){
        sum_check+=u;
        len_data= (u<<8);
        b++;
      }else if(b==1){
        sum_check+=u;
        len_data|=u;
        now_state=Data;
      }else{
        now_state=Header;
      }
      break;
    }
    case Data:{
      // printf("decoder_Data_______\n" );
      if(d<len_data-1){
        sum_check+=u;
        get[d]=u;
        d++;
      }else if(d==len_data-1){
        sum_check+=u;
        get[d]=u;
        now_state=checksum;
      }else{
        now_state=Header;
      }
      break;
    }
    case checksum:{
      // printf("decoder_checksum___\n" );
      uint8_t tmp=(sum_check&0xff);
      if(tmp == u){
        messenger_dealer();
        now_state=Header;
      }else{
        now_state=Header;
      }
      break;
    }
  }
}
//messenger_dealer~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
struct FIFO_for_angle{
  uint8_t index_start;//存到哪
  uint8_t index_end;//處理到哪
  uint8_t container[10][2];//伺服機設定角度
}angle_FIFO={-1,-1,{{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}}};
void messenger_dealer(){//訊息交換機MCU
  if(get[2]>=21 && get[2]<=28){// 切入控制mode 播放連續動作
    action_num=get[2]-20;//動作編號
    mode=1;// 切入控制mode
    printf("mode=1!!!\n" );
  }else{
    if((get[0]|get[1])!=0 ){//前兩byte資料為 伺服機設定角度
      while ((angle_FIFO.index_start+1)%10 == angle_FIFO.index_end) {
        printf("FIFO_for_angle is FULL\n" );
      }
      angle_FIFO.index_start=(angle_FIFO.index_start+1)%10;
      angle_FIFO.container[0][angle_FIFO.index_start]=get[0];
      angle_FIFO.container[1][angle_FIFO.index_start]=get[1];
    }
    command=get[2];
    mode=0;
    printf("mode=0!!!\n" );
  }
}
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void command_processor(uint8_t *c){//監控命令處理器
  if(*c !=0){//伺服機設定命令不是空的
      printf("command=%d\n", *c );
      if( *c <=18 && *c>=11){
        printf("OpenFile(%d)_________________\n", *c -10);
      }
      if( *c<=5 && *c>=1){
        printf("Record with Grid=%d_________\n", *c );
      }
      if( *c ==6) printf("First Time to Record\n" );
      if( *c==7 ) printf("DEL_________________\n");
      *c =0;
  }
}
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void robot_gesture_player(){//機器人姿態播放器
  if(mode==0){
    uint16_t tmp[17]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    while(angle_FIFO.index_end != angle_FIFO.index_start){//一直處理直到FIFO變成空的
      angle_FIFO.index_end = (angle_FIFO.index_end+1)%10;
      // printf("index_end = %d\n", angle_FIFO.index_end);
      for(int i=0;i<17;i++) tmp[i]=now[i];
      uint8_t ID=angle_FIFO.container[0][angle_FIFO.index_end];
      uint8_t a=angle_FIFO.container[1][angle_FIFO.index_end];
      printf("ID=%d to angle=%d\n",ID,a );
      tmp[ID]=a*100;
      PutIn(tmp);
    }

  }
  if(mode==1){
    if(action_num!=0){
      printf("Playing Action( %d )___________\n",action_num);
      action_num=0;
    }else{
      ;
      // printf("All Action Have Finished\n");
    }
  }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ACK( unsigned char data ) //單純的UART送訊
{
  UDR0 = data;
  while ( !( UCSR0A & (1<<UDRE0)) )  //If UDREn is one, the buffer is empty
  ;
}
//KONDO~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void KONDO_transmit(){
  uint16_t Take[17];
  char check=TakeOut(Take);//return=1:空的 return=0:順利拿出資料
  if(check==0){ //姿態FIFO不是空的
    // printf("FIFO is Not Empty\n");
    int Servo_ID[9] = {0,1,2,4,6,7,8,9,10};
    /*洞洞板重新設定*/
    DDRB |= (1<<DDB7)|(1<<DDB6)|(1<<DDB5);   //洞洞板通道開啟
    PORTB |= (1<<PB6);   //洞洞板通道開啟(洞洞板轉到2)
    PORTB &= ~(1<<PB7);
    PORTB &= ~(1<<PB5);

    for(int i = 0;i < 17; i++){
      //判斷ID，決定致能左或右
      if(i > 8){  //0~8
        PORTF = 191;
        // PORTF = 127;
        while ( !( UCSR1A & (1<<UDRE1)) );  //If UDREn is one, the buffer is empty
        UDR1 = Servo_ID[i-8] + 128;
        while ( !( UCSR1A & (1<<UDRE1)) );  //If UDREn is one, the buffer is empty
        UDR1 = Take[i]>>7;
        while ( !( UCSR1A & (1<<UDRE1)) );  //If UDREn is one, the buffer is empty
        UDR1 = Take[i]&127;
      }else{
        PORTF = 127;
        // PORTF = 191;  //9~16
        while ( !( UCSR1A & (1<<UDRE1)) );  //If UDREn is one, the buffer is empty
        UDR1 = Servo_ID[i] + 128;
        while ( !( UCSR1A & (1<<UDRE1)) );  //If UDREn is one, the buffer is empty
        UDR1 = Take[i]>>7;
        while ( !( UCSR1A & (1<<UDRE1)) );  //If UDREn is one, the buffer is empty
        UDR1 = Take[i]&127;
     }
    }
    for(int i=0;i<17;i++) {
      now[i]=Take[i];// feedback伺服機量測角度
    }
  }

}
//INIT~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UART0_Init( unsigned int ubrr )
{
  UBRR0H |= (unsigned char)(ubrr>>8);
  UBRR0L |= (unsigned char)ubrr;
  UCSR0B|=(1<<RXCIE0);//Rx中斷致能
  UCSR0B |= (1<<RXEN0)|(1<<TXEN0);//Tx Rx致能
  UCSR0B &= (~(1<<UCSZ02));//8-bit: UCSZn2=0,UCSZn1=1,UCSZn0=1
  UCSR0C |=(1<<UCSZ01)|(1<<UCSZ10);
  UCSR0C &= (~(1<<USBS0));//stopbit=1
  stdout = &STDIO_BUFFER;
  stdin = &STDIO_BUFFER;
}
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

void TIMER2_INIT(){  //計時中斷設定
  TCCR2|=(1<<WGM21);  //CTC Mode
	TCCR2|=(1<<CS21)|(1<<CS20); //clkI/O/64 (From prescaler)
	TIMSK|=(1<<OCIE2);
	OCR2=171;
}
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int main(){
  DDRB |= (1<<DDB7)|(1<<DDB6)|(1<<DDB5);   //洞洞板通道開啟
  PORTB |= (1<<PB6);//洞洞板通道開啟(洞洞板轉到2
  PORTB &= ~(1<<PB7);
  PORTB &= ~(1<<PB5);
  DDRF=0xff;
  TIMER2_INIT();//計時中斷設定
  UART0_Init( FOSC/16/115200-1);//BlueTooth
  UART1_Init(  FOSC/16/115200-1 );//KONDO
  sei();
  for(int i=0;i<20;i++) ACK(65+i); //只是monitor顯示測試
  printf("START_____________\n" );
  while (1) {
    command_processor(&command);
    robot_gesture_player();
  }

  return 0;
}

//interrupt~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ISR(USART0_RX_vect) {
	uint8_t g=UDR0;
  my_decoder(g);
}
ISR(TIMER2_COMP_vect){
  clock++;
  if(clock>=1000){
    KONDO_transmit();
    clock=0;
  }
}
