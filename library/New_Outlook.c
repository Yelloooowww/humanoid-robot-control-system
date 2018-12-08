// 1208測試 
// 人機角度精度3500~11500
// 內插間格1~10
// 最佳位置勉強可以用
// 可以播放10筆以上資料
// 資料寫入SD卡有一點點機率會失敗QQ
// 有些軸還是會亂動 有可能因為電池快沒電了

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
unsigned char FIFO_flag;//for FIFO
char PutIn(uint16_t *p){
	SDC_FIFO_rear=(SDC_FIFO_rear+1) % SDC_FIFO_max; //第一次進來，SDC_FIFO_rear=0;第二次進來，SDC_FIFO_rear=1....
  if(SDC_FIFO_front == SDC_FIFO_rear)
	{
    // if(SDC_FIFO_rear == 0)
		// {SDC_FIFO_rear=SDC_FIFO_max-1;}
		// else
		// {
		// 	SDC_FIFO_rear=SDC_FIFO_rear-1;
		// 	printf("FIFO Is FULL\n" );
	  //   return 1;
		// }
		SDC_FIFO_rear=((SDC_FIFO_rear-1)+SDC_FIFO_max)%10;
    return 1;
  }
	else
	{
    printf("put in\n" );
    // SDC_FIFO_rear=(SDC_FIFO_rear+1)%SDC_FIFO_max;
    for(int i=0;i<17;i++)
		{KONDO_SDC_FIFO[SDC_FIFO_rear][i]=*(p+i);
      printf("*%d ",*(p+i) );
    }
		FIFO_flag=1;
		printf("AfterPut:SDC_FIFO_rear=%d SDC_FIFO_front=%d\n", SDC_FIFO_rear,SDC_FIFO_front);
  }
  return 0;
}//void PutIn(uint8_t *p)


char TakeOut(uint16_t *t){
  if(SDC_FIFO_front == SDC_FIFO_rear)
	{
    // printf("FIFO Is Empty\n");
    return 1;
  }
	else
	{
		// if(FIFO_flag==1)
		// {
    printf("take out\n" );
    SDC_FIFO_front=(SDC_FIFO_front+1)%SDC_FIFO_max;
    for(int i=0;i<17;i++)
		{*(t+i)=KONDO_SDC_FIFO[SDC_FIFO_front][i];
      printf("^%d ", *(t+i));
    }
		printf("AfterTake:SDC_FIFO_rear=%d SDC_FIFO_front=%d\n", SDC_FIFO_rear,SDC_FIFO_front);
		return 0;
		// }

  }


}
// decoder~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
typedef enum {Header,Bytes,type,Data,checksum}state;
state now_state;//解包狀態
uint8_t get[4];//for my_decoder
void my_decoder(uint8_t u){
  static uint16_t b,d,sum_check;
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
        printf("get=%d %d %d %d\n",get[0],get[1],get[2],get[3] );
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
  uint16_t container[10][2];//伺服機設定角度
}angle_FIFO={-1,-1,{{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}}};
void messenger_dealer(){//訊息交換機MCU
  if(get[3]>=21 && get[3]<=28){// 切入控制mode 播放連續動作
    action_num=get[3]-20;//動作編號
    mode=1;// 切入控制mode
    printf("mode=1!!!\n" );
  }else{
    if((get[0]|get[1]|get[2])!=0 ){//前兩byte資料為 伺服機設定角度
      while ((angle_FIFO.index_start+1)%10 == angle_FIFO.index_end) {
        printf("FIFO_for_angle is FULL\n" );
      }
      uint16_t angle=get[1]<<8;
      angle += get[2];
      angle_FIFO.index_start=(angle_FIFO.index_start+1)%10;
      angle_FIFO.container[0][angle_FIFO.index_start]=get[0];
      angle_FIFO.container[1][angle_FIFO.index_start]=angle;
    }
    command=get[3];
    mode=0;
    printf("mode=0!!!\n" );
  }
}
// SDC~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
unsigned int SDC_data[10];
void KONDO_SDC_read(uint8_t code)
{
	printf("start read\n" );
	printf("KONDO_SDC_read  code=%d\n",code );
	char name[4];
  switch (code) {
 	 case 1:sprintf(name,"%.4s","SDC1");break;
 	 case 2:sprintf(name,"%.4s","SDC2");break;
 	 case 3:sprintf(name,"%.4s","SDC3");break;
 	 case 4:sprintf(name,"%.4s","SDC4");break;
 	 case 5:sprintf(name,"%.4s","SDC5");break;
 	 case 6:sprintf(name,"%.4s","SDC6");break;
 	 case 7:sprintf(name,"%.4s","SDC7");break;
 	 case 8:sprintf(name,"%.4s","SDC8");break;
 	 default:sprintf(name,"%.4s","SDC8");break;
  }

  unsigned char ASA_ID = 4;
  uint8_t swap_buffer[10];// 宣告 與SDC00交換資料的資料陣列緩衝區
 	unsigned int temp[10];//陣列緩衝區
 	// int z=0;
	int ack=0;
	int i=0;
	SDC_data[0]=0;
	char number=0;
	unsigned int final_data[17];

 	char check = 0;	// module communication result state flag

 	unsigned char Mask = 0xFF, Shift = 0, Setting = 0xFF;

 	ASA_SDC00_put(ASA_ID, 64, 8, name);
  ASA_SDC00_put(ASA_ID, 72, 3, "txt");

  // Configure to open file
  Setting = 0x01;		// Setting mode for openFile (readonly)
  ASA_SDC00_set(ASA_ID, 200, Mask, Shift, Setting);	// 送出旗標組合
  // printf("Start reading file data\n");

	while(ack!=35)     //若資料不為#，則持續執行；若資料為#，關檔
	{
		// printf("start clock=%d\n",clock );
		// printf("ackackackackackackackackackackack\n" );
		ack=0;

		// SDC_FIFO_rear=(SDC_FIFO_rear+1) % SDC_FIFO_max; //第一次進來，SDC_FIFO_rear=0;第二次進來，SDC_FIFO_rear=1....
	  // if(SDC_FIFO_front == SDC_FIFO_rear)
	  // {
	  //   if(SDC_FIFO_rear == 0)
	  //   {SDC_FIFO_rear=SDC_FIFO_max-1;}
	  //   else
	  //   {
	  //     SDC_FIFO_rear=SDC_FIFO_rear-1;
	  //       // printf("\nQueue is full!\n");
	  //   }
	  // }
	  // else
    // {
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
					{SDC_data[0]=SDC_data[0]+temp[j];}   //產生一筆最終資料SDC_data
					final_data[number]=SDC_data[0];
					number++;//紀錄產生了多少筆資料
					// printf("SDC_data[%d]=%d\n",z,SDC_data[z] );
				}//if(ack==32)
			}//while (ack!=32)

	    // printf("get one item: ");
			if(ack!=35 && number==17)
	    {
				char check=1;
				while (check!=0)
				{
					check=PutIn(final_data);
				}
				number=0;

				// KONDO_SDC_FIFO[SDC_FIFO_rear]=SDC_data[0];
				// FIFO_flag=1;
				// printf("KONDO_SDC_FIFO[%d]=%d\n",SDC_FIFO_rear,KONDO_SDC_FIFO[SDC_FIFO_rear] );

			}
			// printf("end clock=%d\n",clock );

			// if(ack==35)
			// {
			// 	if(SDC_FIFO_rear==0)
			// 	{SDC_FIFO_rear=9;}
			// 	else
			// 	{SDC_FIFO_rear=SDC_FIFO_rear-1;}
			// }
			// printf("%d\n",KONDO_SDC_FIFO[SDC_FIFO_rear] );

			SDC_data[0]=0;
			for(int j=0 ; j<10 ; j++)//清空緩衝器
			{
				swap_buffer[j]=0;
				temp[j]=0;
			}
			i=0;
	  // }


	}//while(ack!=35)
	// Configure to close file mode
  Setting = 0x00;
  check = ASA_SDC00_set(ASA_ID, 200, Mask, Shift, Setting);// 送出旗標組合
	printf("close the file\n" );
	// for(int j=0 ; j<z ; j++)
	// printf("SDC_data[%d]=%d\n",j,SDC_data[j] );
}//KONDO_SDC_read


void KONDO_SDC_write(uint8_t code)
{
	char name[4];
	uint8_t a[10];
	unsigned int temp[36];
	unsigned int gap[17];
	unsigned int swap_buffer[100];
	int num=0;//插入數目
	// int count=0;
	static char start=0;
	static char stop=0;
	unsigned char ASA_ID = 4;

	switch (code) {
	 case 1:sprintf(name,"%.4s","SDC1");break;
	 case 2:sprintf(name,"%.4s","SDC2");break;
	 case 3:sprintf(name,"%.4s","SDC3");break;
	 case 4:sprintf(name,"%.4s","SDC4");break;
	 case 5:sprintf(name,"%.4s","SDC5");break;
	 case 6:sprintf(name,"%.4s","SDC6");break;
	 case 7:sprintf(name,"%.4s","SDC7");break;
	 case 8:sprintf(name,"%.4s","SDC8");break;
	 case 9:start=1;stop=1;break;
	 default:sprintf(name,"%.4s","SDC8");break;
	}

	if (start==0)
	{
		ASA_SDC00_put(ASA_ID, 64, 8, name);
		ASA_SDC00_put(ASA_ID, 72, 3, "txt");
		ASA_SDC00_set(ASA_ID, 200, 0x07, 0, 0x05);//開檔續寫

		for (int i=0 ; i<17 ; i++) //將17筆資料丟入緩衝器1
		{
			temp[i]=now[i];
			printf("temp[%d]=%d\n",i,temp[i]);
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
				{// printf("null\n" );
					sizeof_string = m;
					break;
				}
			}
			ASA_SDC00_put(ASA_ID, 0, sizeof_string, a);
		}
		start=1;
	}//if (start==0)

	else if (start==1 && stop==0)
	{
		for (int j=17 ; j<34 ; j++) //將17筆資料丟入緩衝器1
		{
			temp[j]=now[j-17];
			printf("temp[%d]=%d\n",j,temp[j]);
		}

		num=code;//決定插入數目
		printf("num=%d\n",num );

		for (int i=0; i<num ; i++)//將內插值寫入SD卡
		{
			for (int k=0 ; k<17 ; k++)
			{
				if(temp[k+17]>=temp[k])
				{swap_buffer[k]=temp[k]+(i+1)*(temp[k+17]-temp[k])/(num+1);}
				else
				{swap_buffer[k]=temp[k]-(i+1)*(temp[k]-temp[k+17])/(num+1);}


				printf("swap_buffer[%d]=%d\n",k,swap_buffer[k] );

				if(k==16)
				{sprintf(a,"%d \r\n",swap_buffer[k]);}
				else
				{sprintf(a,"%d ",swap_buffer[k]);}

				int sizeof_string =4;
				for(int m=0; m< sizeof(a); m++)
				{
					if( a[m] == '\0' )
					{// printf("null\n" );
						sizeof_string = m;
						break;
					}
				}
				ASA_SDC00_put(ASA_ID, 0, sizeof_string, a);
			}
			for (int i = 0; i < 17; i++)
			{swap_buffer[i]=0;}
		}//for (int i=0; i<num ; i++)//將內插值寫入SD卡

		for (int i=0 ; i<17 ; i++) //將17筆資料丟入緩衝器1
		{
			temp[i]=now[i];
			printf("temp[%d]=%d\n",i,temp[i]);
		}

		for (int i=0 ; i<17 ; i++) //將17筆資料寫入SD卡
		{
			if(i==16)
			{sprintf(a,"%d \r\n",temp[i]);}
			else
			{sprintf(a,"%d ",temp[i]);}
			int sizeof_string =4;
			for(int m=0; m< sizeof(a); m++)
			{
				if( a[m] == '\0' )
				{// printf("null\n" );
					sizeof_string = m;
					break;
				}
			}
			ASA_SDC00_put(ASA_ID, 0, sizeof_string, a);
		}
	}//else if (start==1)

	else if (stop==1) {
		sprintf(a,"%c",35);//寫入#

		int sizeof_string =4;
		for(int m=0; m< sizeof(a); m++)
		{
			if( a[m] == '\0' )
			{// printf("null\n" );
				sizeof_string = m;
				break;
			}
		}
		ASA_SDC00_put(ASA_ID, 0, sizeof_string, a);

		ASA_SDC00_set(ASA_ID,200,0x01,0,0x00);  //關檔
		printf("mode132 done\n" );

	}



}//KONDO_SDC_write
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void command_processor(uint8_t *c){//監控命令處理器
  if(*c !=0){//伺服機設定命令不是空的
      printf("command=%d\n", *c );
      if( *c <=18 && *c>=11){
        printf("OpenFile(%d)_________________\n", *c -10);
        KONDO_SDC_write( *c -10);
        printf("OpenFileDone_________KONDO_SDC_write_______\n");
      }
      if( *c<=10 && *c>=1){
        printf("Record with Grid=%d_________\n", *c );
        KONDO_SDC_write( *c );
        printf("Done__KONDO_SDC_write_______\n");
      }
      if( *c ==19) {
        printf("CloseFile_________________\n");
        KONDO_SDC_write( *c -10);
        printf("CloseFileDone_________KONDO_SDC_write_______\n");

      }
      if( *c ==31){
        printf("The Best Position\n" );

        uint16_t Good[17]={7500,8600,9500,4700,7300,7500,9000,8300,7500,6400,5500,0,7700,7500,6000,6700,7500};
        for(int i=0;i<17;i++) now[i]=Good[i];
        PutIn(Good);
        printf("DONE___The Best Position___\n" );
      }
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
      uint16_t a=angle_FIFO.container[1][angle_FIFO.index_end];
      printf("ID=%d to angle=%d\n",ID,a );
      tmp[ID]=a;
      PutIn(tmp);
    }

  }
  if(mode==1){
    if(action_num!=0){
      printf("Playing Action( %d )___________\n",action_num);
      KONDO_SDC_read(action_num);
      printf("Playing Action( %d )_____Done_____\n",action_num);
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
    _delay_ms(1);
    for(int i = 0;i < 17; i++){
      //判斷ID，決定致能左或右
      if(i > 8){  //0~8
        PORTF = 191;
        _delay_ms(1);
        // PORTF = 127;
        while ( !( UCSR1A & (1<<UDRE1)) );  //If UDREn is one, the buffer is empty
        UDR1 = Servo_ID[i-8] + 128;
        while ( !( UCSR1A & (1<<UDRE1)) );  //If UDREn is one, the buffer is empty
        UDR1 = Take[i]>>7;
        while ( !( UCSR1A & (1<<UDRE1)) );  //If UDREn is one, the buffer is empty
        UDR1 = Take[i]&127;
      }else{
        PORTF = 127;
        _delay_ms(1);
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
