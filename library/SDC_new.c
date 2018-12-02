//測試完成 12/2 17:46
//功能1:讀取資料Now[]，插值，存入SD卡
//功能2:讀取SD卡中的資料，放進KONDO_SDC_FIFO[]
#include "ASA_Lib.h"
// #include "ASA_Lib_DAC00.h"
#include <math.h>
#include <string.h>

// #define FOSC 11059200// Clock Speed
// #define BAUD0 9600  //
// #define BAUD1 115200//UL 115200
// #define MYUBRR0 FOSC/16/BAUD0-1
// #define MYUBRR1 FOSC/16/BAUD1-1

// volatile uint8_t i;
// volatile char Now[100];
int mode,ID,angle,code;
int data[100];
unsigned int SDC_data[10];


unsigned int KONDO_SDC_FIFO[10][17];//for FIFO，需小於最小檔案之資料量
unsigned int SDC_FIFO_max=10;//for FIFO
unsigned int SDC_FIFO_rear=9;//for FIFO
unsigned int SDC_FIFO_front=9;//for FIFO

unsigned int clock=0;//for FIFO
unsigned int clock1=0;//for FIFO
unsigned char FIFO_flag;//for FIFO

unsigned int Now[17];


char PutIn(uint16_t *p){
	SDC_FIFO_rear=(SDC_FIFO_rear+1) % SDC_FIFO_max; //第一次進來，SDC_FIFO_rear=0;第二次進來，SDC_FIFO_rear=1....
  if(SDC_FIFO_front == SDC_FIFO_rear)
	{
		if(SDC_FIFO_rear == 0)
		{SDC_FIFO_rear=SDC_FIFO_max-1;}
		else
		{
			SDC_FIFO_rear=SDC_FIFO_rear-1;
			printf("FIFO Is FULL\n" );
	    return 1;
		}
  }
	else
	{
    printf("put in\n" );
    // SDC_FIFO_rear=(SDC_FIFO_rear+1)%SDC_FIFO_max;
    for(int i=0;i<17;i++)
		{KONDO_SDC_FIFO[SDC_FIFO_rear][i]=*(p+i);}
		FIFO_flag=1;

		printf("AfterPut:SDC_FIFO_rear=%d SDC_FIFO_front=%d\n", SDC_FIFO_rear,SDC_FIFO_front);
		return 0;

  }


}//void PutIn(uint8_t *p)


char TakeOut(uint16_t *t){
  if(SDC_FIFO_front == SDC_FIFO_rear)
	{
    printf("FIFO Is Empty\n");
    return 1;
  }
	else
	{
		if(FIFO_flag==1)
		{
    printf("take out\n" );
    SDC_FIFO_front=(SDC_FIFO_front+1)%SDC_FIFO_max;
    for(int i=0;i<17;i++)
		{*(t+i)=KONDO_SDC_FIFO[SDC_FIFO_front][i];}
		printf("AfterTake:SDC_FIFO_rear=%d SDC_FIFO_front=%d\n", SDC_FIFO_rear,SDC_FIFO_front);
		return 0;
		}

  }


}



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
			temp[i]=Now[i];
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
			temp[j]=Now[j-17];
			printf("temp[%d]=%d\n",j,temp[j]);
		}

		num=code;//決定插入數目
		printf("num=%d\n",num );

		for (int i=0; i<num ; i++)//將內插值寫入SD卡
		{
			for (int k=0 ; k<17 ; k++)
			{
				if(temp[k+17]>=temp[k])
				{swap_buffer[k]=temp[k]+(i+1)*abs(temp[k+17]-temp[k])/(num+1);}
				else
				{swap_buffer[k]=temp[k]-(i+1)*abs(temp[k+17]-temp[k])/(num+1);}


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
			temp[i]=Now[i];
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


void robot_gesture_player(void){//機器人姿態播放器

	uint16_t take_out_from_FIFO[17];
	char check2=TakeOut(take_out_from_FIFO);
	if (check2==0) {
		printf("take_out_from_FIFO=");
	  for(int i=0;i<17;i++)
		{printf("%d ",  take_out_from_FIFO[i] );}
	}




	/*static unsigned int ppp=0;
	static unsigned int temp[17];

	if(SDC_FIFO_front == SDC_FIFO_rear)
	{printf("Queue is empty\n" );}
	else
	{
		// printf("SDC_FIFO_front=%d\n",SDC_FIFO_front );
		// printf("SDC_FIFO_rear=%d\n",SDC_FIFO_rear );
		// printf("SDC_FIFO_front=%d\n",SDC_FIFO_front );

		if(FIFO_flag==1)
		{
			SDC_FIFO_front=(SDC_FIFO_front+1)%SDC_FIFO_max;   //第一次進來，front=0;第二次進來，front=1....
			temp[ppp]=KONDO_SDC_FIFO[SDC_FIFO_front];
			// FIFO_flag=0;
			KONDO_SDC_FIFO[SDC_FIFO_front]=0;
			ppp++;
		}

		// printf("temp[%d]=%d\n",ppp-1,temp[ppp-1] );
		// for (int i=0 ; i<17 ; i++) {
		// 	printf("item: temp[%d]=%d\n",i,temp[i] );
		// }
		// _delay_ms(1);

		if(ppp==17)
		{
			for (int i=0 ; i<17 ; i++) {
				Now[i]=temp[i];
				printf("item: Now[%d]=%d\n",i,Now[i] );
			}
			ppp=0;
			// printf("ppp=%d\n",ppp );
			// _delay_ms(100);
		}
	}*/

}//robot_gesture_player





int main(void)
{
  ASA_M128_set();
  printf("start SDC_new\n");

  DDRB |= (1<<DDB7)|(1<<DDB6)|(1<<DDB5);   //洞洞板通道開啟
  PORTB |= (1<<PB6);   //洞洞板通道開啟(洞洞板轉到2)
	// PORTB &= ~(1<<PB7);
	// PORTB &= ~(1<<PB5);
	// DDRF=0xff;        //用PF1~4控制機器人左右半邊以及要送還是收

  // USART1_Init ( MYUBRR1 );
  // USART0_Init ( MYUBRR0 );
	sei();	// 開啟所有中斷功能
	//int num;


	while (1) {
		printf("hi~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n" );
		TCCR2|=(1<<WGM21);  //CTC Mode
	  TCCR2|=(1<<CS21)|(1<<CS20); //clkI/O/64 (From prescaler)
	  TIMSK|=(1<<OCIE2);
	  OCR2=171;

	  // sei();

		int h;
		printf("write=1 read=2");
		scanf("%d",&h );
		printf("h=%d\n",h );
		switch (h) {
			case 1:
			{
				char code,d;
				printf("code=");
				scanf("%d",&code);
				printf("\ndata=");
				scanf("%d",&d);


				if(d==1)
				{
					for (int i=0 ; i<17 ; i++)
					{	Now[i]=(i+7500);}
				}

				else if(d==2)
				{
					printf("\ndata=%d",data);
					for (int i = 0; i < 17; i++)
					{Now[i]=7500+code+1+i;}
				}

				else if(d==3)
				{
					printf("\ndata=%d",data);
					for (int i = 0; i < 17; i++)
					{Now[i]=7500;}
				}


				KONDO_SDC_write(code);


			}
			break;

			case 2:
			{
				char code1;
				printf("code1=");
				scanf("%d",&code1 );
				printf("code1=%d",code1);
				KONDO_SDC_read(code1);
			}
			break;
		}


		// for (int i = 0; i < 100; i++) {
		// 	Now[i]=0;
		// 	SDC_data[i]=0;
		// }


	}//while (1)


    return 0;
}
ISR(TIMER2_COMP_vect){
  clock++;//讀取第一筆資料需要45單位clock，其他則是4~5單位clock
	if(clock==50)
	{
		clock1++;
		clock=0;

		// printf("clock1=%d \n",clock1);
		robot_gesture_player();
	}

}
