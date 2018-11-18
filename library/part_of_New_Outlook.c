//模擬程式:學習mode 總表紀錄,清空,刪除 完成 人機已更新測試正常
//now[] 和accumulate[] 資料range:3500-11500
#include "ASA_Lib.h"
#include <avr/interrupt.h>
#include <math.h>
#include <string.h>
#define FOSC 11059200// Clock Speed
#define BAUD1 9600//(暫時改回來)
#define MYUBRR1 (FOSC/16/BAUD1-1)
volatile uint8_t get[3];

struct FIFO_for_angle{
  uint8_t index_start;//存到哪
  uint8_t index_end;//處理到哪
  uint8_t container[10][2];
}angle_FIFO={-1,-1,{{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}}};
typedef enum {Header,Bytes,type,Data,checksum}state;
state now_state;

uint8_t mode;//輪循(mode=0:學習,mode=1:控制)
uint8_t command;//伺服機設定命令
uint16_t now[17];//當下姿態 (range:3500-11500)
uint16_t accumulate[200];//準備寫入SDC的資料(和人機上的總表大致相同)(寫入SDC的過程要內插)
//範例:accumulate[]={7500,7500,7500,7500,7500,7500,7500,7500,7500,7500,7500,7500,7500,7500,7500,7500,7500,3,7600,7500,7500,7500,7500,7500,7500,7500,7500,7500,7500,7500,7500,7500,7500,7500,7500,255}
//說明:7500..:該時刻17軸角度資料(range:3500-11500); 3:和下一時刻資料要內插成3個間格 ;255:資料結束(沒有下一時刻了)


void messenger_dealer(){     //訊息交換機MCU
  if((get[0]|get[1])!=0 && get[2]==0){
    mode=0;//進入學習模式
    while ((angle_FIFO.index_start+1)%10 == angle_FIFO.index_end) {
      printf("FIFO is FULL\n" );
    }
    //存入FIFO
    // printf("ADD to FIFO\n" );
    angle_FIFO.index_start=(angle_FIFO.index_start+1)%10;
    angle_FIFO.container[0][angle_FIFO.index_start]=get[0];
    angle_FIFO.container[1][angle_FIFO.index_start]=get[1];
  }else if((get[0]|get[1])==0 && get[2]!=0){
    mode=1;//進入控制模式
    command=get[2];
    printf("ENTER Control Mode\n" );
  }
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
        messenger_dealer();
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
  UCSR1B|=(1<<RXCIE1);//Rx中斷致能
  UCSR1B |= (1<<RXEN1)|(1<<TXEN1);//Tx Rx致能
  UCSR1B &= (~(1<<UCSZ12));//8-bit: UCSZn2=0,UCSZn1=1,UCSZn0=1
  UCSR1C |=(1<<UCSZ11)|(1<<UCSZ10);
  UCSR1C &= (~(1<<USBS1));//stopbit=1
}


void robot_gesture_player(){//機器人姿態播放器
  if(mode==0){
    while(angle_FIFO.index_end != angle_FIFO.index_start){    //一直處理直到FIFO變成空的
      angle_FIFO.index_end = (angle_FIFO.index_end+1)%10;
      // printf("index_end = %d\n", angle_FIFO.index_end);
      uint8_t ID=angle_FIFO.container[0][angle_FIFO.index_end];
      uint8_t a=angle_FIFO.container[1][angle_FIFO.index_end];
      printf("ID=%d to angle=%d\n",ID,a );
      now[ID]=a*100;
      printf("now[]= " );
      for(int i=0;i<17;i++) printf("%d ;",now[i]);
      printf("\n" );
    }
    // printf("FIFO Is Gone\n");
  }else{
    ;//處理SDC_FIFO(動作姿態串暫存器)裡的東西,更新now[](姿態)
  }
}
void ACK( unsigned char data ) //單純的UART1送訊
{
  UDR1 = data;
  while ( !( UCSR1A & (1<<UDRE1)) )  //If UDREn is one, the buffer is empty
  ;
}
void Update_accmulate(uint8_t c){  //更新總表
  static uint8_t num_of_active;//紀錄目前有幾個定格姿態
  if(c<=6 && c>=1){ //使用者按紀錄
    if(c<=5 && c>=1)  accumulate[num_of_active*18-1]=c;//間格
    for(int i=0;i<17;i++) accumulate[num_of_active*18+i]=now[i];
    accumulate[num_of_active*18+17]=255;//資料結尾
    num_of_active++;
    printf("Have Recorded\n");
  }else if(c==7){//按DEL 清除前一時刻資料
    num_of_active--;
    if(num_of_active>0) accumulate[num_of_active*18-1]=255;//資料結尾
    for(int i=0;i<=17;i++) accumulate[num_of_active*18+i]=0;
    printf("Have DEL\n");
  }else if(c==8){//按清空
    for(int i=0;i<=(num_of_active*18-1);i++) accumulate[i]=0;
    num_of_active=0;
  }
  printf("num_of_active=%d\n",num_of_active );
  for(int i=0; i<60 ;i++) printf("acc[%d]=%d\n",i,accumulate[i] );
}

void command_processor(uint8_t c){//監控命令處理器
  if(c!=0){//伺服機設定命令不是空的
    if(c<=9 && c>=1){//人機相關命令
      if(c>=1 && c<=8){
        Update_accmulate(c); //更新總表
        command=0;//處理完命令把command歸零
      }else if(c==9){//按試播
        printf("This function is in preparation\n" );
        ACK(130);//模擬一下可以ack的情形
        command=0;//處理完命令把command歸零
      }




    }else if(c<=18 && c>=11){//SDC寫入相關命令
      switch (c) {
        case 11:
            printf("Save To SDC(1)\n");
            ACK(132);//模擬一下可以ack的情形
            command=0;//處理完命令把command歸零
            break;
        case 12:
            printf("Save To SDC(2)\n");
            ACK(132);//模擬一下可以ack的情形
            command=0;//處理完命令把command歸零
            break;
        case 13:
            printf("Save To SDC(3)\n");
            ACK(132);//模擬一下可以ack的情形
            command=0;//處理完命令把command歸零
            break;
        case 14:
            printf("Save To SDC(4)\n");
            ACK(132);//模擬一下可以ack的情形
            command=0;//處理完命令把command歸零
            break;
        case 15:
            printf("Save To SDC(5)\n");
            ACK(132);//模擬一下可以ack的情形
            command=0;//處理完命令把command歸零
            break;
        case 16:
            printf("Save To SDC(6)\n");
            ACK(132);//模擬一下可以ack的情形
            command=0;//處理完命令把command歸零
            break;
        case 17:
            printf("Save To SDC(7)\n");
            ACK(132);//模擬一下可以ack的情形
            command=0;//處理完命令把command歸零
            break;
        case 18:
            printf("Save To SDC(8)\n");
            ACK(132);//模擬一下可以ack的情形
            command=0;//處理完命令把command歸零
            break;
        }



    }else if(c<=28 && c>=21){//SDC資料播放相關命令
      switch (c) {
        case 21:
            printf("Action 1\n");
            ACK(131);//模擬一下可以ack的情形
            command=0;//處理完命令把command歸零
            break;
        case 22:
            printf("Action 2\n");
            ACK(131);//模擬一下可以ack的情形
            command=0;//處理完命令把command歸零
            break;
        case 23:
            printf("Action 3\n");
            ACK(131);//模擬一下可以ack的情形
            command=0;//處理完命令把command歸零
            break;
        case 24:
            printf("Action 4\n");
            ACK(131);//模擬一下可以ack的情形
            command=0;//處理完命令把command歸零
            break;
        case 55:
            printf("Action 5\n");
            ACK(131);//模擬一下可以ack的情形
            command=0;//處理完命令把command歸零
            break;
        case 26:
            printf("Action 6\n");
            ACK(131);//模擬一下可以ack的情形
            command=0;//處理完命令把command歸零
            break;
        case 27:
            printf("Action 7\n");
            ACK(131);//模擬一下可以ack的情形
            command=0;//處理完命令把command歸零
            break;
        case 28:
            printf("Action 8\n");
            ACK(131);//模擬一下可以ack的情形
            command=0;//處理完命令把command歸零
            break;
      }
    }
  }
}

int main(){
  ASA_M128_set();
  printf("START\n" );
  DDRB |= (1<<DDB7)|(1<<DDB6)|(1<<DDB5);   //洞洞板通道開啟
  PORTB |= (1<<PB6);//洞洞板通道開啟(洞洞板轉到2
  USART_Init( MYUBRR1 );
  sei();
  while (1) {
    switch (mode) {
      case 0:{//學習模式
        robot_gesture_player();//機器人姿態播放器
        break;
      }
      case 1:{//控制模式
        command_processor(command);//監控命令處理器
        break;
      }
    }
  }
  return 0;
}


ISR(USART1_RX_vect) {
	uint8_t g=UDR1;
  my_decoder(g);
}
