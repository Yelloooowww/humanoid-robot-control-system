#include "ASA_Lib.h" //測試程式
#include "ASA_Lib_DAC00.h"
#include <math.h>
#include <string.h>

#define FOSC 11059200// Clock Speed
#define BAUD0 9600  //
#define BAUD1 115200//UL 115200
#define MYUBRR0 FOSC/16/BAUD0-1
#define MYUBRR1 FOSC/16/BAUD1-1

volatile uint8_t i;
volatile char get[100];
int mode,k,data_1,position[100],side,save;
unsigned char ID;
int y[100],sum;//for SDC



void USART_Flush( void )
{
	unsigned char dummy;
	while ( UCSR1A & (1<<RXC1) ) dummy = UDR1;
}

void USART0_Init( unsigned int ubrr )
{
/* Set baud rate */
UCSR0B|=(1<<RXCIE0);  //致能TX，RX complete interrupt
UBRR0H |= (unsigned char)(ubrr>>8);   //p.362 // fosc = 11.0592MHz，Baud Rate=9600，U2X=0 =>UBRR=71，U2X=1=>UBRR=143
UBRR0L |= (unsigned char)ubrr;
/* Enable receiver and transmitter */
UCSR0B |= (1<<RXEN0);//|(1<<TXEN0);    //enables the USARTn Receiver，enables the USARTn Transmitter
/* Set frame format: 8data, 2stop bit */
// UCSR1C = (0<<USBS1)|(3<<UCSZ10); //selects the number of stop bits，USBS1=1=> 2 bits
UCSR0C |= (1<<UPM01)|(0<<UPM00)|(1<<USBS0)|(1<<UCSZ01)|(1<<UCSZ00)|(0<<UCPOL0);//Character Size=8 bits，UCPOL1=上升/下降

}

void USART1_Init( unsigned int ubrr )
{
/* Set baud rate */
// UCSR1B|=(1<<RXCIE1);  //致能TX，RX complete interrupt，沒有用到的話，不可以致能
UBRR1H |= (unsigned char)(ubrr>>8);   //p.362 // fosc = 11.0592MHz，Baud Rate=9600，U2X=0 =>UBRR=71，U2X=1=>UBRR=143
UBRR1L |= (unsigned char)ubrr;
/* Enable receiver and transmitter */
UCSR1B |= (1<<RXEN1)|(1<<TXEN1);    //enables the USARTn Receiver，enables the USARTn Transmitter
/* Set frame format: 8data, 2stop bit */
//UCSR1C = (0<<USBS1)|(3<<UCSZ10); //selects the number of stop bits，USBS1=1=> 2 bits
UCSR1C |= (1<<UPM11)|(0<<UPM10)|(1<<USBS1)|(1<<UCSZ11)|(1<<UCSZ10)|(0<<UCPOL1);//Character Size=8 bits，UCPOL1=上升/下降
}

void USART1_Transmit( unsigned char data )
{

/* Wait for empty transmit buffer */

/* Put data into buffer, sends the data */
PORTB|=(1<<PB4);    //收訊關閉
PORTB|=(1<<PB2);    //收訊關閉
PORTB|=(1<<PB1);    //送訊關閉
PORTB|=(1<<PB3);    //送訊關閉

if(side==1)
{PORTB&= ~(1<<PB1);}  //左半邊送訊打開

else if(side==2)
{PORTB&= ~(1<<PB2);}  //右半邊送訊打開
//printf("transmit:%d\n",data );

while ( !( UCSR1A & (1<<UDRE1)) )  //If UDREn is one, the buffer is empty
;
UDR1 = data;

}

// void USART0_Transmit( unsigned char data )
// {
// /* Wait for empty transmit buffer */
// /* Put data into buffer, sends the data */
// //printf("transmit:%d\n",data );
// while ( !( UCSR0A & (1<<UDRE0)) )  //If UDREn is one, the buffer is empty
// ;
// UDR0 = data;
//
// }

void convert_1(int a)
{
	// data_1=a*20+6000;   //電腦傳送:0~99，將其轉為: 6000~7980
	// if(data_1>7980)     //若角度錯誤
	// {data_1=7500;}      //設定角度為7500
	data_1=a*100;         //電腦傳送:35~115，將其轉為: 3500~17500
}

void convert_2(int a,int b)
{
	position[k]=(a*128)+b;
	position[k]=(position[k]-6000)/20;
}

void ID_convert(int a)     //將收到的ID(0~16) 轉換為 機器人上的ID且用side來區分左、右半邊
{
	if(a<3)
	{ID=a;}
	else if(a==3)
	{ID=a+1;}
	else if(a>3 && a<9)
	{ID=a+2;}

	else if(a==9 || a==10)
	{ID=a-8;}
	else if(a==11)
	{ID=a-7;}
	else if(a>11 && a<17)
	{ID=a-6;}
	else
	{ID=0;}

	if(a>0 && a<9)//若為左半邊
	{side=1;}
	else         //若為右半邊
	{side=2;}
}

void SDC_read(uint8_t package)
{
 char name[4];
 // printf("package=%d\n",package );
 // package=8;
 switch (package) {
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
  uint8_t swap_buffer[100];// 宣告 與SDC00交換資料的資料陣列緩衝區
	char data[1000];
	int z=0;
	// char sum;
	char check = 0;	// module communication result state flag

	unsigned char Mask = 0xFF, Shift = 0, Setting = 0xFF;

	ASA_SDC00_put(ASA_ID, 64, 8, name);
 ASA_SDC00_put(ASA_ID, 72, 3, "txt");

 // Configure to open file
 Setting = 0x01;		// Setting mode for openFile (readonly)
 check = ASA_SDC00_set(ASA_ID, 200, Mask, Shift, Setting);	// 送出旗標組合
 if( check != 0 ) {	// 檢查回傳值做後續處理, If check equal 0 then operation was successful
	 // printf("Debug point 31, error code <%d>\n", check);
	 return;
 }



 // printf("Start reading file data\n");
 int rec = 0;
 for(int i=0; i<4000; i++) {

	 if(rec = ASA_SDC00_get(ASA_ID, 0, 64, swap_buffer)) {//每次收64Bytes
		 for(int i=0; i<64; i++) {

				if(i%4==1)
				{


					if(swap_buffer[i-1]>=48 && swap_buffer[i-1]<=57)
					{swap_buffer[i-1]=swap_buffer[i-1]-48;}
					if(swap_buffer[i-1]>=97 && swap_buffer[i-1]<=102)
					{swap_buffer[i-1]=swap_buffer[i-1]-87;}
				 else if(swap_buffer[i-1]>=65 && swap_buffer[i-1]<=70)
				 {swap_buffer[i-1]=swap_buffer[i-1]-55;}

					if(swap_buffer[i]>=48 && swap_buffer[i]<=57)
					{swap_buffer[i]=swap_buffer[i]-48;}
					if(swap_buffer[i]>=97 && swap_buffer[i]<=102)
					{swap_buffer[i]=swap_buffer[i]-87;}
				 else if(swap_buffer[i]>=65 && swap_buffer[i]<=70)
				 {swap_buffer[i]=swap_buffer[i]-55;}

					data[z]=swap_buffer[i-1]*16+swap_buffer[i];
					z++;

				}//if
		 }

			sum=((long)i*64 + swap_buffer[63])/4;
		 break;
	 }



	 for(int i=0; i<64; i++) {

			if(i%4==1)
			{

				if(swap_buffer[i-1]>=48 && swap_buffer[i-1]<=57)
				{swap_buffer[i-1]=swap_buffer[i-1]-48;}
				else if(swap_buffer[i-1]>=97 && swap_buffer[i-1]<=102)
				{swap_buffer[i-1]=swap_buffer[i-1]-87;}
				else if(swap_buffer[i-1]>=65 && swap_buffer[i-1]<=70)
				{swap_buffer[i-1]=swap_buffer[i-1]-55;}

				if(swap_buffer[i]>=48 && swap_buffer[i]<=57)
				{swap_buffer[i]=swap_buffer[i]-48;}
				else if(swap_buffer[i]>=97 && swap_buffer[i]<=102)
				{swap_buffer[i]=swap_buffer[i]-87;}
				else if(swap_buffer[i]>=65 && swap_buffer[i]<=70)
				{swap_buffer[i]=swap_buffer[i]-55;}

				data[z]=swap_buffer[i-1]*16+swap_buffer[i];
				z++;
			}//if

	 }


 }
 // printf("Finish reading file data \n");
	for(int j=0;j<sum;j++)
	{
			// printf("data[%d]=%d\n",j,data[j]*100);
		 y[j]=data[j]*100;
	}


 // Configure to close file mode
 Setting = 0x00;
 check = ASA_SDC00_set(ASA_ID, 200, Mask, Shift, Setting);// 送出旗標組合
 // if( check != 0 ) {				// 檢查回傳值做後續處理
 // 	printf("Debug point 32, error code <%d>\n", check);
 // 	//return;
 // }
	// printf("finish SDC1\n");
	for(int i=0;i<100;i++)
	{swap_buffer[i]=0;}
}



int main(void)
{
  // ASA_M128_set();
  // printf("start test\n");
	// printf("start test2s\n");
  DDRB |= (1<<DDB7)|(1<<DDB6)|(1<<DDB5);   //洞洞板通道開啟
  PORTB |= (1<<PB6);//洞洞板通道開啟(洞洞板轉到2)
	DDRB=0xff;   //用PB1~4控制機器人左右半邊以及要送還是收
	//DDRF=0xff;

	i=0;
	for(int j=0;j<100;j++)
	{
		get[j]=0;
		position[j]=0;
		y[j]=0;
	}
	mode=0;
	ID=0;
	data_1=0;
  side=0;
	k=0;
	save=0;
	sum=0;

  USART1_Init ( MYUBRR1 );
  USART0_Init ( MYUBRR0 );
	sei();	// 開啟所有中斷功能

	while(1)
  {

		while(mode==0)      //選擇模式
		{//printf("hi\n" );
		//_delay_ms(100);
			if(i==1)
			{
				mode=get[0];
				if(mode==128)  //若模式128
						{
							// printf("get128\n" );

							// side=2;
							// 	k=8000;         //將頭轉到7500，以表示進入mode128
							// 	USART1_Transmit(0b10000000);
							// 	USART1_Transmit(k>>7);
							// 	USART1_Transmit(k&127);
							// 	_delay_ms(2000);
								// USART0_Transmit(1);

				    }

				else if(mode==129)  //若模式129
				    {
							// printf("get129\n" );
							  // side=2;
								// k=6000;       //將頭轉到6000，以表示進入mode129
								// USART1_Transmit(0b10000000);
								// USART1_Transmit(k>>7);
								// USART1_Transmit(k&127);
								// _delay_ms(2000);
								// USART0_Transmit(2);
				    }
				else if(mode==130)  //若模式130
					  {
								side=2;
								k=6000;       //將頭轉到6000，以表示進入mode130
								USART1_Transmit(0b10000000);
								USART1_Transmit(k>>7);
								USART1_Transmit(k&127);
								_delay_ms(2000);
										// USART0_Transmit(2);
						}
				else if(mode==131)  //若模式131
						{
								side=2;
								k=6000;       //將頭轉到6000，以表示進入mode131
								USART1_Transmit(0b10000000);
								USART1_Transmit(k>>7);
								USART1_Transmit(k&127);
								_delay_ms(2000);
												// USART0_Transmit(2);
						}
				else       //如果不是上述的模式，視為錯誤資料
						{
								for(int j=0;j<100;j++)
								{get[j]=0;}
								i=0;
								mode=0;
						}

			}   //if(i==1)
		}    //while for the mode=0

		if(mode>0)        //選擇完mode以後，將矩陣清空
		{
		for(int j=0;j<100;j++)
		{get[j]=0;}
		i=0;
	  }

		if(mode==128)         //mode128 電腦教學模式
		{
			// printf("mode128\n");

			while (i<2) {//收集兩筆資料
        if(get[0]==129 && i==1)   //若切換至mode129
        { mode=129;
          break;}
				if(get[0]==130 && i==1)   //若切換至mode129
	      { mode=130;
	        break;}
				if(get[0]==131 && i==1)   //若切換至mode129
		    { mode=131;
		      break;}

				if(get[0]>17)   //若ID錯誤
				{ i=0;
				  break;}
      }  //while (i<2)

			if(mode==129)//進入mode128後又切換成129
			{
				mode=0;//進入模式選擇
				i=1;//已選擇模式
				get[0]=129;//選擇的模式為mode129
			}

			if(mode==130)//進入mode128後又切換成130
			{
				mode=0;//進入模式選擇
				i=1;//已選擇模式
				get[0]=130;//選擇的模式為mode130
			}

			if(mode==131)//進入mode128後又切換成131
			{
				mode=0;//進入模式選擇
				i=1;//已選擇模式
				get[0]=131;//選擇的模式為mode131
			}

    	if(i==2)         //i==2表示已收集到兩筆資料
    	{
				// ID=get[0];
				ID_convert(get[0]);      //轉換ID
				convert_1(get[1]);   //轉換角度資料

				USART1_Transmit(128+ID);       //傳送ID
				USART1_Transmit(data_1>>7);  //傳送角度
				USART1_Transmit(data_1&127);  //傳送角度
				// printf("ID=%d\n",ID );
				// printf("position=%d\n",data_1 );

				for(int j=0;j<100;j++)
				{get[j]=0;}
				mode=0;//進入模式選擇
				i=1;//已選擇模式
				get[0]=128;//選擇的模式為mode128
    	}  //if(i==2)

			else if(i==0)//若ID錯誤，清空資料
			{
				for(int j=0;j<100;j++)
				{get[j]=0;}
				mode=0;//進入模式選擇
				i=1;//已選擇模式
				get[0]=128;//選擇的模式為mode128
			}

	  	if(i>2)
		  {
				for(int j=0;j<100;j++)
				{get[j]=0;}
				mode=0;//進入模式選擇
				i=1;//已選擇模式
				get[0]=128;//選擇的模式為mode128
		  }

			//i=0;
			//mode=0;//進入模式選擇
	  }  //if(mode==128)

		else if(mode==129)         //mode2 手動教學模式
		{
			// printf("mode129\n" );
			for(int j=0;j<100;j++)
			{get[j]=0;
			position[j]=0;}
			i=0;
			mode=0;//進入模式選擇
	  }

		else if(mode==130)         //mode130 播放模式
		{
			// printf("mode130\n" );
			// _delay_ms(2000);
			// USART0_Transmit(3);//ACK，表示進入mode130

			while (i<1) {     //接收資料總數
				sum=get[0];
			}

			for(int j=0;j<100;j++)
			{get[j]=0;}

			while (i<sum) {;}    //等待至接收完?筆資料

			for(int j=0;j<sum;j++)    //轉換資料
			{position[j]=get[j]*100;} //轉換資料 35~115 => 3500~11500

			for(int l=0;l<(sum/17);l++)         // 播放(sum/17)個分解動作
			{
				for(int k=0;k<17;k++)
				{
					USART1_Transmit(128+k);  // 128=0b1000 0000
					USART1_Transmit(position[(l*17+k)]>>7);
					USART1_Transmit(position[(l*17+k)]&127);
					// printf("position=%d\n",position[(l*17+k)] );
				}
				_delay_ms(1000);   //每個分解動作之間間隔1秒
			}

			_delay_ms(2000);
			// USART0_Transmit(3);//ACK，表示撥放完動作了

			for(int j=0;j<100;j++)
			{get[j]=0;
			position[j]=0;}

			mode=0;      //播放結束以後進入模式選擇(mode==0)
			i=1;         //已選擇模式
			get[0]=128;  //選擇的模式為mode128
	  }                      //if(mode==130)

		else if(mode==131)    //播放套裝動作
		{
			// printf("mode131\n" );
			for(int j=0;j<100;j++)
			{get[j]=0;}
			uint8_t number;
			// _delay_ms(2000);
			// USART0_Transmit(4);//ACK，表示進入mode131
			while (i<1) {//接收套裝動作編號(1~8)
				;
			}
			number=get[0];

	    // printf("plz input a number (1~8) ");
	    // scanf("%d",&number );
	    SDC_read(number);//選擇套裝動作1~8
			DDRB |= (1<<DDB7)|(1<<DDB6)|(1<<DDB5);   //洞洞板通道開啟
	    PORTB |= (1<<PB6);//洞洞板通道開啟(洞洞板轉到2)


			for(int l=0;l<(sum/17);l++)         // 播放(sum/17)個分解動作
			{
				for(int k=0;k<17;k++)
				{
					USART1_Transmit(128+k);  // 128=0b1000 0000
					USART1_Transmit(y[(l*17+k)]>>7);
					USART1_Transmit(y[(l*17+k)]&127);
					// printf("position=%d\n",y[(l*17+k)] );
				}
				_delay_ms(1000);   //每個分解動作之間間隔1秒
			}

			for(int j=0;j<100;j++)
			{get[j]=0;
		  y[j]=0;}

			mode=0;      //播放結束以後進入模式選擇(mode==0)
			i=1;         //已選擇模式
			get[0]=128;  //選擇的模式為mode128
		}//else if(mode==131)


  }              //while(1)


    return 0;
}

ISR(USART0_RX_vect) {
	get[i]=UDR0;
	i++;
}
