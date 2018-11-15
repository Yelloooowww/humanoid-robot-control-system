//模擬程式:學習mode完成測試成功
//藍牙改過baudrate(115200)
#include "ASA_Lib.h"
#define FOSC 11059200// Clock Speed
#define BAUD1 115200
#define MYUBRR1 (FOSC/16/BAUD1-1)
volatile uint8_t i,j,get[200];

struct FIFO_for_angle{
  uint8_t index_start;
  uint8_t index_end;
  uint8_t container[10][2];
}angle_FIFO={-1,-1,{{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}}};


uint8_t mode;//輪循(mode=0:學習,mode=1:控制)
uint8_t command;//伺服機設定命令
uint8_t now[17];//姿態FIFO
typedef enum {Header,Bytes,type,Data,checksum}state;
state now_state;

void messenger_dealer(){     //訊息交換機MCU
  if(get[0]!=0 && get[1]!=0 && get[2]==0){
    mode=0;//進入學習模式
    while ((angle_FIFO.index_start+1)%10 == angle_FIFO.index_end) {
      printf("FIFO is FULL\n" );
    }
    //存入FIFO
    printf("ADD to FIFO\n" );
    angle_FIFO.index_start=(angle_FIFO.index_start+1)%10;
    angle_FIFO.container[0][angle_FIFO.index_start]=get[0];
    angle_FIFO.container[1][angle_FIFO.index_start]=get[1];
  }else if(get[0]==0 && get[1]==0 && get[2]!=0){
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
  UCSR1B|=(1<<RXCIE1)|(1<<TXCIE1);//Tx Rx中斷致能
  UCSR1B |= (1<<RXEN1)|(1<<TXEN1);//Tx Rx致能
  UCSR1B &= (~(1<<UCSZ12));//8-bit: UCSZn2=0,UCSZn1=1,UCSZn0=1
  UCSR1C |=(1<<UCSZ11)|(1<<UCSZ10);
  UCSR1C &= (~(1<<USBS1));//stopbit=1
}


void robot_gesture_player(){//機器人姿態播放器
  if(mode==0){
    if(angle_FIFO.index_end == angle_FIFO.index_start){
      printf("FIFO Is Gone\n" );
    }else{
      angle_FIFO.index_end = (angle_FIFO.index_end+1)%10;
      printf("index_end = %d\n", angle_FIFO.index_end);
      uint8_t ID=angle_FIFO.container[0][angle_FIFO.index_end];
      uint8_t a=angle_FIFO.container[1][angle_FIFO.index_end];
      printf("ID=%d to angle=%d\n",ID,a );
      now[ID]=a;
      printf("now[]= " );
      for(int i=0;i<17;i++) printf("%d ;",now[i]);
      printf("\n" );
    }
  }else{
    ;//處理SDC_FIFO(動作姿態串暫存器)裡的東西,更新now[](姿態)
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
        printf("Control Mode\n" );
        // command_processor(command);//監控命令處理器
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
ISR(USART1_TX_vect) {
	j++;
}











/*    暫用測試python程式 人機程式尚未更新
import sys
import time
import serial

a1=[3,75,0]
a2=[3,76,0]
a3=[3,77,0]
a4=[3,78,0]
a5=[3,79,0]

def what_is_type(data):
    return 1
def encoder_and_send_pac(data):
    pac=[]
    pac +=  [253]  #Header=0xfd
    typeeeee=what_is_type(data)
    pac+=bytes([typeeeee]) #type
    l=bytes([len(data)>>8,len(data)&0xff])
    pac+= l  #bytes(2bytes)
    pac += data  #data
    checksum= bytes([(sum(data)+sum(l))&0xFF])
    pac += checksum #checksum
    for x in pac:
        ser.write(bytes([x]))
        print('pac=',x)
        # time.sleep(0.5)



if __name__ == "__main__":
    ser = serial.Serial()
    ser.baudrate = 115200
    ser.timeout=1000
    ser.port = str('COM10')
    ser.open()
    time.sleep(5)
    print('HaveOpen')
    while 1:
        encoder_and_send_pac(a1)
        encoder_and_send_pac(a2)
        encoder_and_send_pac(a3)
        encoder_and_send_pac(a4)
        encoder_and_send_pac(a5)
    print('Done')

*/
