//測試完成 11/19 11:20
//功能1:讀取資料，插值，存入SD卡
//功能1:讀取SD卡中的資料，放進KONDO_SDC_FIFO[]，利用中斷去將KONDO_SDC_FIFO[]中的資料讀出，並以每次17筆的方式更新Now[]
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
// volatile char get[100];
int mode,ID,angle,code;
int data[100];
unsigned int SDC_data[10];
unsigned int get[100];

unsigned int KONDO_SDC_FIFO[10];//for FIFO，需小於最小檔案之資料量
unsigned int SDC_FIFO_max=10;//for FIFO
unsigned int SDC_FIFO_rear=9;//for FIFO
unsigned int SDC_FIFO_front=9;//for FIFO

unsigned int clock=0;//for FIFO
unsigned int clock1=0;//for FIFO

unsigned int Now[17];


void KONDO_data_convert(void)
{
	mode=get[0];
	switch (mode) {
		case 128://raw data==35~115，angle==3500~11500
			{
				ID=get[1];
				angle=get[2]*100;
				printf("ID=%d\n",ID );
				printf("angle=%d\n",angle );
			}
			break;

		case 130://raw data==35~115，data[]==3500~11500
			{
				int number=get[1];
				data[0]=get[1];
				for(int i=1;i<=number;i++)
				{data[i]=get[i+1]*100;}

				for(int j=0;j<=number;j++)
				{printf("data[%d]=%d\n",j,data[j]);}
			}
			break;

		case 131:
			{
				printf("get[1]=%d\n",get[1] );
				code=get[1];
				printf("code=%d\n",code );
			}
			break;

		case 132:
			{
				code=get[1];
				printf("get[1]=%d\n",get[1] );
				printf("code=%d\n",code );
			}
			break;

	}//switch(mode)

}//KONDO_data_convert

void KONDO_SDC_read(void)
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
		// printf("ackackackackackackackackackackack\n" );
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
					{SDC_data[0]=SDC_data[0]+temp[j];}   //產生最終資料SDC_data
					// printf("SDC_data[%d]=%d\n",z,SDC_data[z] );
				}//if(ack==32)
			}//while (ack!=32)

	    // printf("get one item: ");
			if(ack!=35)
	    {KONDO_SDC_FIFO[SDC_FIFO_rear]=SDC_data[0];}

			if(ack==35)
			{
				if(SDC_FIFO_rear==0)
				{SDC_FIFO_rear=9;}
				else
				{SDC_FIFO_rear=SDC_FIFO_rear-1;}
			}
			// printf("%d\n",KONDO_SDC_FIFO[SDC_FIFO_rear] );

			SDC_data[0]=0;
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
	// for(int j=0 ; j<z ; j++)
	// printf("SDC_data[%d]=%d\n",j,SDC_data[j] );
}//KONDO_SDC_read




void KONDO_SDC_write(void)
{
	char name[4];
	uint8_t a[10];
	unsigned int temp[36];
	unsigned int gap[17];
	unsigned int swap_buffer[100];
	int num=0;//插入數目
	int count=0;
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
	ASA_SDC00_put(ASA_ID, 64, 8, name);
	ASA_SDC00_put(ASA_ID, 72, 3, "txt");
	ASA_SDC00_set(ASA_ID, 200, 0x07, 0, 0x05);//開檔續寫

	while (1)
	{
		int j=0;
		for (int i=(count*18+2) ; i<(count*18+38) ; i++) //將36筆資料丟入緩衝器1，for (int i=(count*18) ; i<(count*18+36) ; i++) //將36筆資料丟入緩衝器1
		{
			temp[j]=get[i];
			printf("temp[%d]=%d\n",j,temp[j]);
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
					sizeof_string = m;
					break;
				}
			}
			ASA_SDC00_put(ASA_ID, 0, sizeof_string, a);
		}

		num=temp[17];//決定插入數目
		printf("num=%d\n",num );

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


				printf("swap_buffer[%d]=%d\n",k,swap_buffer[k] );

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
				sizeof_string = m;
				break;
			}
		}
		ASA_SDC00_put(ASA_ID, 0, sizeof_string, a);
	}

	ASA_SDC00_set(ASA_ID,200,0x01,0,0x00);  //關檔
	printf("mode132 done\n" );

}//KONDO_SDC_write



void robot_gesture_player(void){//機器人姿態播放器
	static unsigned int ppp=0;
	static unsigned int temp[17];

	if(SDC_FIFO_front == SDC_FIFO_rear)
	{printf("Queue is empty\n" );}
	else
	{
		// printf("SDC_FIFO_front=%d\n",SDC_FIFO_front );
		SDC_FIFO_front=(SDC_FIFO_front+1)%SDC_FIFO_max;   //第一次進來，front=0;第二次進來，front=1....
		temp[ppp]=KONDO_SDC_FIFO[SDC_FIFO_front];
		printf("temp[%d]=%d\n",ppp,temp[ppp] );
		// for (int i=0 ; i<17 ; i++) {
		// 	printf("item: temp[%d]=%d\n",i,temp[i] );
		// }
		_delay_ms(100);
		KONDO_SDC_FIFO[SDC_FIFO_front]=0;
		ppp++;
		if(ppp==17)
		{
			for (int i=0 ; i<17 ; i++) {
				Now[i]=temp[i];
				printf("item: Now[%d]=%d\n",i,Now[i] );
			}
			ppp=0;
			printf("ppp=%d\n",ppp );
			_delay_ms(100);
		}
	}

}//robot_gesture_player


int main(void)
{
  ASA_M128_set();
  printf("start test Chang1117\n");

  DDRB |= (1<<DDB7)|(1<<DDB6)|(1<<DDB5);   //洞洞板通道開啟
  PORTB |= (1<<PB6);   //洞洞板通道開啟(洞洞板轉到2)
	// PORTB &= ~(1<<PB7);
	// PORTB &= ~(1<<PB5);
	// DDRF=0xff;        //用PF1~4控制機器人左右半邊以及要送還是收

  // USART1_Init ( MYUBRR1 );
  // USART0_Init ( MYUBRR0 );
	// sei();	// 開啟所有中斷功能
	//int num;


	while (1) {
		printf("hi~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n" );
		TCCR2|=(1<<WGM21);  //CTC Mode
	  TCCR2|=(1<<CS21)|(1<<CS20); //clkI/O/64 (From prescaler)
	  TIMSK|=(1<<OCIE2);
	  OCR2=171;


	  sei();



		// printf("num? \n" );
		// scanf("%d",&num );      //ex1:3         ,ex2:(17*?)+2           ,ex3:2   ,ex4:(17*?)+3  ,?=1 or 2 or 3....
		// printf("num=%d\n",num );
		// _delay_ms(100);
		//
		// for (int i=0; i<num; i++) {
		// 	printf("get[%d]=\n",i );
		// 	scanf("%d",&get[i] );//ex1:128,ID,angle     ,ex2:130,(17*?)+2 ,1,2,3....(17*?)  ,ex3:131,n   (n=1 or 2 or 3 or...8)
		// 	//ex4:132,n,(17*?)+2 ,1,2,3....(17*?)
		// }
		//
		// for (int i=0; i<num; i++) {
		// 	printf("get[%d]=%d\n",i,get[i] );
		// }
		static int h;
		printf("hi=");
		scanf("%d",&h );
		printf("h=%d!!!!!!!!!!!!!!!!!!\n",h );
		switch (h) {
			case 1:
			{
				get[0]=131;
				get[1]=2;
				code=2;
			}
			break;

			case 2:
			{
				get[0]=132;
				get[1]=2;
				code=2;
				for (int i = 2; i < 19; i++) {
					get[i]=i+2+7500;
					get[18+i]=i-1+7500;
				}
				get[19]=2;
				get[37]=255;
			}
			break;
		}



		mode=get[0];
		printf("mode=%d\n",mode );

		switch (mode) {
			case 128:
			{
				KONDO_data_convert();
				h=0;
				mode=0;
				for (int i=0; i<100; i++) {
					get[i]=0;
				}
			}	break;

	  	case 130:
			{
				KONDO_data_convert();
				h=0;
				mode=0;
				for (int i=0; i<100; i++) {
					get[i]=0;
				}
			}	break;

	  	case 131:
			{
				KONDO_data_convert();
				_delay_ms(100);
				KONDO_SDC_read();
				_delay_ms(500);
				// h=0;
				// mode=0;
				// for (int i=0; i<100; i++) {
				// 	get[i]=0;
				// }
			}	break;

	  	case 132:
			{
				KONDO_data_convert();
				_delay_ms(100);
				KONDO_SDC_write();
				_delay_ms(500);
				// h=0;
				// mode=0;
				// for (int i=0; i<100; i++) {
				// 	get[i]=0;
				// }
			}	break;

	  	default:
			{
				printf("mode wrong");
				mode=0;
			}break;
		}//switch(mode)

		for (int i = 0; i < 100; i++) {
			get[i]=0;
			SDC_data[i]=0;
		}

		// num=0;
		// h=0;
		// mode=0;
		// for (int i=0; i<100; i++) {
		// 	get[i]=0;
		// }



	}//while (1)


    return 0;
}
ISR(TIMER2_COMP_vect){
  clock++;
	if(clock==100)
	{
		clock1++;
		clock=0;
		// printf("clock1=%d \n",clock1);
		robot_gesture_player();
	}

}
