#include "ASA_Lib.h"
#include <avr/interrupt.h>
#include <math.h>
#include <string.h>
#define FOSC 11059200// Clock Speed
#define BAUD1 9600//(暫時改回來)
#define MYUBRR1 (FOSC/16/BAUD1-1)
volatile uint8_t g[3];

unsigned int SDC_data[10];//for FIFO
// unsigned int get[100];

unsigned int KONDO_SDC_FIFO[10];//for FIFO，需小於最小檔案之資料量
unsigned int SDC_FIFO_max=10;//for FIFO
unsigned int SDC_FIFO_rear=9;//for FIFO
unsigned int SDC_FIFO_front=9;//for FIFO

unsigned int clock=0;//for FIFO
unsigned int clock1=0;//for FIFO

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

void KONDO_SDC_write(uint8_t code);
void KONDO_SDC_write(uint8_t code);
// Internal functions declare
static int stdio_putchar(char c, FILE *stream);
static int stdio_getchar(FILE *stream);

// Internal variables declare
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


void messenger_dealer(){     //訊息交換機MCU

  if((g[0]|g[1])!=0 && g[2]==0){
    mode=0;//進入學習模式
    while ((angle_FIFO.index_start+1)%10 == angle_FIFO.index_end) {
      printf("FIFO is FULL\n" );
      // char p[]="FIFO_FULL";my_printf(&p);
    }
    //存入FIFO
    // printf("ADD to FIFO\n" );
    angle_FIFO.index_start=(angle_FIFO.index_start+1)%10;
    angle_FIFO.container[0][angle_FIFO.index_start]=g[0];
    angle_FIFO.container[1][angle_FIFO.index_start]=g[1];
  }else if((g[0]|g[1])==0 && g[2]!=0){
    mode=1;//進入控制模式
    command=g[2];
    // printf("command=%d~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n",command );
    printf("ENTER Control Mode\n" );
    // char p[]="EnterMode1";my_printf(&p);
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
        g[d]=u;
        d++;
      }else if(d==len_data-1){
        sum_check+=u;
        g[d]=u;
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
  UBRR0H |= (unsigned char)(ubrr>>8);
  UBRR0L |= (unsigned char)ubrr;
  UCSR0B|=(1<<RXCIE0);//Rx中斷致能
  UCSR0B |= (1<<RXEN0)|(1<<TXEN0);//Tx Rx致能
  UCSR0B &= (~(1<<UCSZ02));//8-bit: UCSZn2=0,UCSZn1=1,UCSZn0=1
  UCSR0C |=(1<<UCSZ01)|(1<<UCSZ10);
  UCSR0C &= (~(1<<USBS0));//stopbit=1
  stdout = &STDIO_BUFFER;
	stdin = &STDIO_BUFFER;


  // UBRR1H |= (unsigned char)(ubrr>>8);
  // UBRR1L |= (unsigned char)ubrr;
  // UCSR1B|=(1<<RXCIE1);//Rx中斷致能
  // UCSR1B |= (1<<RXEN1)|(1<<TXEN1);//Tx Rx致能
  // UCSR1B &= (~(1<<UCSZ12));//8-bit: UCSZn2=0,UCSZn1=1,UCSZn0=1
  // UCSR1C |=(1<<UCSZ11)|(1<<UCSZ10);
  // UCSR1C &= (~(1<<USBS1));//stopbit=1
}


void robot_gesture_player(){//機器人姿態播放器
  if(mode==0){
    while(angle_FIFO.index_end != angle_FIFO.index_start){    //一直處理直到FIFO變成空的
      angle_FIFO.index_end = (angle_FIFO.index_end+1)%10;
      // printf("index_end = %d\n", angle_FIFO.index_end);
      uint8_t ID=angle_FIFO.container[0][angle_FIFO.index_end];
      uint8_t a=angle_FIFO.container[1][angle_FIFO.index_end];
      // printf("ID=%d to angle=%d\n",ID,a );
      now[ID]=a*100;
      // printf("now[]= " );
      // for(int i=0;i<17;i++) printf("%d ;",now[i]);
      // printf("\n" );
    }
    // printf("FIFO Is Gone\n");
  }else{
    static unsigned int ppp=0;
	static unsigned int temp[17];

	if(SDC_FIFO_front == SDC_FIFO_rear){;}
	// {printf("Queue is empty\n" );}
	else
	{
		SDC_FIFO_front=(SDC_FIFO_front+1)%SDC_FIFO_max;   //第一次進來，front=0;第二次進來，front=1....
		temp[ppp]=KONDO_SDC_FIFO[SDC_FIFO_front];
		// for (int i=0 ; i<17 ; i++) {
		// 	printf("item: temp[%d]=%d\n",i,temp[i] );
		// }
		_delay_ms(100);
		KONDO_SDC_FIFO[SDC_FIFO_front]=0;
		ppp++;
		if(ppp==17)
		{
			for (int i=0 ; i<17 ; i++) {
				now[i]=temp[i];
				// printf("item: now[%d]=%d\n",i,now[i] );
			}
			ppp=0;
		}
	}
    ;//處理SDC_FIFO(動作姿態串暫存器)裡的東西,更新now[](姿態)
  }
}
void ACK( unsigned char data ) //單純的UART1送訊
{
  UDR0 = data;
  while ( !( UCSR0A & (1<<UDRE0)) )  //If UDREn is one, the buffer is empty
  ;
  // UDR1 = data;
  // while ( !( UCSR1A & (1<<UDRE1)) )  //If UDREn is one, the buffer is empty
  // ;
}
void Update_accmulate(uint8_t c){  //更新總表
  static uint8_t num_of_active;//紀錄目前有幾個定格姿態
  if(c<=6 && c>=1){ //使用者按紀錄
    if(c<=5 && c>=1)  accumulate[num_of_active*18-1]=c;//間格
    for(int i=0;i<17;i++) accumulate[num_of_active*18+i]=now[i];
    accumulate[num_of_active*18+17]=255;//資料結尾
    num_of_active++;
    printf("Have Recorded\n");
    // char p[]="HaveRecorded";my_printf(&p);
  }else if(c==7){//按DEL 清除前一時刻資料
    num_of_active--;
    if(num_of_active>0) accumulate[num_of_active*18-1]=255;//資料結尾
    for(int i=0;i<=17;i++) accumulate[num_of_active*18+i]=0;
    printf("Have DEL\n");
    // char p[]="HaveDEL";my_printf(&p);
  }else if(c==8){//按清空
    for(int i=0;i<=(num_of_active*18-1);i++) accumulate[i]=0;
    num_of_active=0;
  }
  // printf("num_of_active=%d\n",num_of_active );
  // for(int i=0; i<60 ;i++) printf("acc[%d]=%d\n",i,accumulate[i] );
}

void KONDO_SDC_read(uint8_t code)
{
	printf("start read\n" );
  // char p[]="StartRead";my_printf(&p);
  _delay_ms(500);
	char name[4];
  switch (code) {
 	 case 21:sprintf(name,"%.4s","SDC1");break;
 	 case 22:sprintf(name,"%.4s","SDC2");break;
 	 case 23:sprintf(name,"%.4s","SDC3");break;
 	 case 24:sprintf(name,"%.4s","SDC4");break;
 	 case 25:sprintf(name,"%.4s","SDC5");break;
 	 case 26:sprintf(name,"%.4s","SDC6");break;
 	 case 27:sprintf(name,"%.4s","SDC7");break;
 	 case 28:sprintf(name,"%.4s","SDC8");break;
 	 default:sprintf(name,"%.4s","SDC8");break;
  }

  static unsigned char ASA_ID = 4;
  static uint8_t swap_buffer[10];// 宣告 與SDC00交換資料的資料陣列緩衝區
 	static unsigned int temp[10];//陣列緩衝區
 	static int z=0;
	static int ack=0;
	static int i=0;
	SDC_data[0]=0;

 	char check = 0;	// module communication result state flag

 	unsigned char Mask = 0xFF, Shift = 0, Setting = 0xFF;

 	ASA_SDC00_put(ASA_ID, 64, 8, name);
  ASA_SDC00_put(ASA_ID, 72, 3, "txt");

  // Configure to open file
  Setting = 0x01;		// Setting mode for openFile (readonly)
  ASA_SDC00_set(ASA_ID, 200, Mask, Shift, Setting);	// 送出旗標組合
  // printf("Start reading file data\n");

	while(ack!=35)     //若資料不為#，則持續執行
	{
		ack=0;

		SDC_FIFO_rear=(SDC_FIFO_rear+1) % SDC_FIFO_max; //第一次進來，SDC_FIFO_rear=0;第二次進來，SDC_FIFO_rear=1....
	  if(SDC_FIFO_front == SDC_FIFO_rear)
	  {
	    if(SDC_FIFO_rear == 0)
	    {SDC_FIFO_rear=SDC_FIFO_max-1;}
	    else
	    {
	      SDC_FIFO_rear=SDC_FIFO_rear-1;
	        // printf("\nQueue is full!\n");
	    }
	  }
	  else
    {
			while (ack!=32)
			{
				// printf("start get\n" );
				ASA_SDC00_get(ASA_ID, 0, 1, &swap_buffer[i]);//從SD卡中取出1 Byte資料
				// printf("swap_buffer[%d]=%d\n",i,swap_buffer[i] );
				ack=swap_buffer[i];
				if(ack==35)
				{break;}
				// printf("ack=%d\n",ack );
				i++;
				if(ack==32)//
				{
					swap_buffer[i-1]=0;
					i--;
					// printf("get data!\n" );
					for(int j=0 ; j<i ; j++)
					{
						if(swap_buffer[j]>=48 && swap_buffer[j]<=57)
						{
							temp[j]=swap_buffer[j]-48;
							for(int k=0 ; k<(i-j-1) ; k++)
							{temp[j]=temp[j]*10;}
							// printf("data[%d]=%d\n",j,data[j] );
						}
						else if(swap_buffer[j]==13 || swap_buffer[j]==10)
						{
							swap_buffer[j]=0;
							temp[j]=0;
						}
					}
					for(int j=0 ; j<i ; j++)
					{SDC_data[z]=SDC_data[z]+temp[j];}   //產生最終資料SDC_data
					// printf("SDC_data[%d]=%d\n",z,SDC_data[z] );
				}//if(ack==32)
			}//while (ack!=32)

	    // printf("get one item: ");
	    KONDO_SDC_FIFO[SDC_FIFO_rear]=SDC_data[z];
			// printf("%d\n",KONDO_SDC_FIFO[SDC_FIFO_rear] );

			SDC_data[z]=0;
			for(int j=0 ; j<10 ; j++)//清空緩衝器
			{
				swap_buffer[j]=0;
				temp[j]=0;
			}
			i=0;
	  }


	}//while(ack!=35)
	// Configure to close file mode
  Setting = 0x00;
  check = ASA_SDC00_set(ASA_ID, 200, Mask, Shift, Setting);// 送出旗標組合
	printf("close the file\n" );
  // char p1[]="CloseFile";my_printf(&p1);
  DDRB |= (1<<DDB7)|(1<<DDB6)|(1<<DDB5);   //洞洞板通道開啟
  PORTB |= (1<<PB6);   //洞洞板通道開啟(洞洞板轉到2)

  PORTB &= ~(1<<PB7);
  PORTB &= ~(1<<PB5);

	// for(int j=0 ; j<z ; j++)
	// printf("SDC_data[%d]=%d\n",j,SDC_data[j] );
}//KONDO_SDC_read




void KONDO_SDC_write(uint8_t code)
{
  printf("START　write\n" );
  // char p[]="StartWrite";my_printf(&p);
  _delay_ms(500);
	char name[4];
	uint8_t a[10];
	unsigned int temp[36];
	unsigned int gap[17];
	unsigned int swap_buffer[100];
	int num=0;//插入數目
	int count=0;
  switch (code) {
 	 case 11:sprintf(name,"%.4s","SDC1");break;
 	 case 12:sprintf(name,"%.4s","SDC2");break;
 	 case 13:sprintf(name,"%.4s","SDC3");break;
 	 case 14:sprintf(name,"%.4s","SDC4");break;
 	 case 15:sprintf(name,"%.4s","SDC5");break;
 	 case 16:sprintf(name,"%.4s","SDC6");break;
 	 case 17:sprintf(name,"%.4s","SDC7");break;
 	 case 18:sprintf(name,"%.4s","SDC8");break;
 	 default:sprintf(name,"%.4s","SDC8");break;
  }
	unsigned char ASA_ID = 4;
	ASA_SDC00_put(ASA_ID, 64, 8, name);
	ASA_SDC00_put(ASA_ID, 72, 3, "txt");
	ASA_SDC00_set(ASA_ID, 200, 0x07, 0, 0x05);//開檔續寫

	while (1)
	{
		int j=0;
		for (int i=(count*18) ; i<(count*18+36) ; i++) //將36筆資料丟入緩衝器1
		{
			temp[j]=accumulate[i];
			// printf("temp[%d]=%d\n",j,temp[j]);
			j++;
		}

		for (int i=0 ; i<17 ; i++) //將前17筆資料寫入SD卡
		{
			if(i==16)
			{sprintf(a,"%d \r\n",temp[i]);}
			else
			{sprintf(a,"%d ",temp[i]);}
			int sizeof_string =4;
			for(int m=0; m< sizeof(a); m++)
			{
				if( a[m] == '\0' )
				{
					printf("null\n" );
          // char p[]="NULL";my_printf(&p);
					sizeof_string = m;
					break;
				}
			}
			ASA_SDC00_put(ASA_ID, 0, sizeof_string, a);
		}

		num=temp[17];//決定插入數目
		// printf("num=%d\n",num );

		for (int i=0; i<17; i++)
		{gap[i]=abs(temp[i+18]-temp[i])/(num+1);}//間隔=(get[n+18]-get[n])/(插入數目+1)

		for (int i=0; i<num ; i++)//將內插值寫入SD卡
		{
			for (int k=0 ; k<17 ; k++)
			{
				if(temp[k+18]>=temp[k])
				{swap_buffer[k]=temp[k]+(i+1)*abs(temp[k+18]-temp[k])/(num+1);}
				else
				{swap_buffer[k]=temp[k]-(i+1)*abs(temp[k+18]-temp[k])/(num+1);}


				// printf("swap_buffer[%d]=%d\n",k,swap_buffer[k] );

				if(k==16)
				{sprintf(a,"%d \r\n",swap_buffer[k]);}
				else
				{sprintf(a,"%d ",swap_buffer[k]);}

				int sizeof_string =4;
				for(int m=0; m< sizeof(a); m++)
				{
					if( a[m] == '\0' )
					{
						printf("null\n" );
            // char p[]="NULL";my_printf(&p);
						sizeof_string = m;
						break;
					}
				}
				ASA_SDC00_put(ASA_ID, 0, sizeof_string, a);
			}
			for (int i = 0; i < 17; i++) {
				swap_buffer[i]=0;
			}

		}
		if(temp[35]==255)
		break;

	}//while

	for (int i=0 ; i<17 ; i++) //將最後17筆資料寫入SD卡
	{
		if(i==16)
		{sprintf(a,"%d %c",temp[i+18],35);}
		else
		{sprintf(a,"%d ",temp[i+18]);}
		int sizeof_string =4;
		for(int m=0; m< sizeof(a); m++)
		{
			if( a[m] == '\0' )
			{
				printf("null\n" );
        // char p[]="NULL";my_printf(&p);
				sizeof_string = m;
				break;
			}
		}
		ASA_SDC00_put(ASA_ID, 0, sizeof_string, a);
	}

	ASA_SDC00_set(ASA_ID,200,0x01,0,0x00);  //關檔
	// printf("mode132 done\n" );
  printf("Have writen~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  // char p1[]="HaveWriten";my_printf(&p1);
  DDRB |= (1<<DDB7)|(1<<DDB6)|(1<<DDB5);   //洞洞板通道開啟
  PORTB |= (1<<PB6);   //洞洞板通道開啟(洞洞板轉到2)

	PORTB &= ~(1<<PB7);
  PORTB &= ~(1<<PB5);

}//KONDO_SDC_write





void command_processor(uint8_t c){//監控命令處理器
  // printf("c=%d\n",c );
  if(c!=0){//伺服機設定命令不是空的
      // printf("c=%d\n",c );
      if(c>=1 && c<=8){
        Update_accmulate(c); //更新總表
        command=0;//處理完命令把command歸零
        // printf("ACC\n" );
      }
      // }else if(c==9){//按試播
      //   printf("This function is in preparation\n" );
      //   ACK(130);//模擬一下可以ack的情形
      //   command=0;//處理完命令把command歸零
      // }

    if(c<=18 && c>=11){//SDC寫入相關命令
      // printf("(c<=18 && c>=11)\n" );
      KONDO_SDC_write(c);
      command=0;
      ACK(132);
      // char p[]="HaveACK132";my_printf(&p);
      printf("Have ACK132~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n" );
    }else if(c<=28 && c>=21){//SDC資料播放相關命令
      // printf("(c<=28 && c>=21)\n" );
      KONDO_SDC_read(c);
      command=0;

      ACK(131);
      printf("Have ACK131\n" );
      // char p[]="HaveACK131";my_printf(&p);
    }
  }
}




void USART1_Init( unsigned int ubrr )
{
/* Set baud rate */
// UCSR1B|=(1<<TXCIE1);  //致能TX，RX complete interrupt，沒有用到的話，不可以致能
UBRR1H |= (unsigned char)(ubrr>>8);   //p.362 // fosc = 11.0592MHz，Baud Rate=9600，U2X=0 =>UBRR=71，U2X=1=>UBRR=143
UBRR1L |= (unsigned char)ubrr;
/* Enable receiver and transmitter */
UCSR1B |= (1<<TXEN1);    //enables the USARTn Receiver，enables the USARTn Transmitter
/* Set frame format: 8data, 2stop bit */
//UCSR1C = (0<<USBS1)|(3<<UCSZ10); //selects the number of stop bits，USBS1=1=> 2 bits
UCSR1C |= (1<<UPM11)|(0<<UPM10)|(1<<USBS1)|(1<<UCSZ11)|(1<<UCSZ10)|(0<<UCPOL1);//Character Size=8 bits，UCPOL1=上升/下降
}
void KONDO_transmit(){
  int Servo_ID[9] = {0,1,2,4,6,7,8,9,10};
  // for(int i=0;i<17;i++) printf("*%d, ",now[i] );
  // printf("\n" );
  // ACK(171);ACK(171);ACK(171);

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
      UDR1 = now[i]>>7;
      while ( !( UCSR1A & (1<<UDRE1)) );  //If UDREn is one, the buffer is empty
      UDR1 = now[i]&127;
    }else{
      PORTF = 191;  //9~16
      while ( !( UCSR1A & (1<<UDRE1)) );  //If UDREn is one, the buffer is empty
      UDR1 = Servo_ID[i] + 128;
      while ( !( UCSR1A & (1<<UDRE1)) );  //If UDREn is one, the buffer is empty
      UDR1 = now[i]>>7;
      while ( !( UCSR1A & (1<<UDRE1)) );  //If UDREn is one, the buffer is empty
      UDR1 = now[i]&127;
    }

  }

}
void my_printf(char* Data_p){
  for(int i=0;i<19;i++) {
    ACK(((char*)Data_p)[i]);
  }
  ACK(0xee);
}
int main(){
  // ASA_M128_set();
  // printf("START\n" );
  TCCR2|=(1<<WGM21);  //CTC Mode
	TCCR2|=(1<<CS21)|(1<<CS20); //clkI/O/64 (From prescaler)
	TIMSK|=(1<<OCIE2);
	OCR2=171;
  DDRB |= (1<<DDB7)|(1<<DDB6)|(1<<DDB5);   //洞洞板通道開啟
  PORTB |= (1<<PB6);//洞洞板通道開啟(洞洞板轉到2
    DDRF=0xff;
  USART_Init( FOSC/16/9600-1);//BlueTooth
  USART1_Init(  FOSC/16/115200-1 );//KONDO
  sei();
  char p[]="START";my_printf(&p);
  printf("SSSTTTAAARRRTTT\n");
  while (1) {
        command_processor(command);//監控命令處理器
      }
  return 0;
}


ISR(USART0_RX_vect) {
	uint8_t g=UDR0;
  my_decoder(g);
}
ISR(TIMER2_COMP_vect){
  clock++;
  if(clock%1000==0){
    KONDO_transmit();
  }
	if(clock==2000)
	{
		clock1++;
		// printf("clock1=%d \n",clock1);
		robot_gesture_player();
    clock=0;
	}

}
