#include "ASA_Lib.h" //測試程式 成功 (Bluetooth to UART1)(UART0 to PC)(SPC傳資料，用ATOM終端機監視)
#include "ASA_Lib_DAC00.h"
#include <math.h>
#include <string.h>

#define FOSC 11059200// Clock Speed
#define BAUD0 9600  //
#define BAUD1 9600//UL 115200
#define MYUBRR0 FOSC/16/BAUD0-1
#define MYUBRR1 FOSC/16/BAUD1-1

volatile uint8_t i,get[100];
int mode,ID,k,data_1,position[100];



void USART_Flush( void )
{
	unsigned char dummy;
	while ( UCSR1A & (1<<RXC1) ) dummy = UDR1;
}

void USART0_Init( unsigned int ubrr )
{
/* Set baud rate */
UCSR0B|=(1<<RXCIE0)|(1<<TXCIE0);  //致能TX，RX complete interrupt
UBRR0H |= (unsigned char)(ubrr>>8);   //p.362 // fosc = 11.0592MHz，Baud Rate=9600，U2X=0 =>UBRR=71，U2X=1=>UBRR=143
UBRR0L |= (unsigned char)ubrr;
/* Enable receiver and transmitter */
UCSR0B |= (1<<RXEN0)|(1<<TXEN0);    //enables the USARTn Receiver，enables the USARTn Transmitter
/* Set frame format: 8data, 2stop bit */
//UCSR1C = (0<<USBS1)|(3<<UCSZ10); //selects the number of stop bits，USBS1=1=> 2 bits
UCSR0C |= (1<<UPM01)|(0<<UPM00)|(1<<USBS0)|(1<<UCSZ01)|(1<<UCSZ00)|(0<<UCPOL0);//Character Size=8 bits，UCPOL1=上升/下降
}

void USART1_Init( unsigned int ubrr )
{
/* Set baud rate */
UCSR1B|=(1<<RXCIE1)|(1<<TXCIE1);  //致能TX，RX complete interrupt
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
PORTB|=(1<<PB4);
PORTB&= ~(1<<PB3);
//printf("transmit:%d\n",data );
UDR1 = data;
while ( !( UCSR1A & (1<<UDRE1)) )  //If UDREn is one, the buffer is empty
;



}
// uint8_t uart1_get() {
//     while(!(UCSR1A&(1<<RXC1)));
// 		// {
// 		// 	//printf("stuck\n" );
// 		// }
//
// 		a++;
//     return UDR1;
// }

void convert_1(int a)
{
	data_1=a*20+6000;   //range: 6000~8000
}

void convert_2(int a,int b)
{
	position[k]=(a*128)+b;
	position[k]=(position[k]-6000)/20;
}


int main(void)
{
  ASA_M128_set();
  printf("start1111\n");
  DDRB |= (1<<DDB7)|(1<<DDB6)|(1<<DDB5);   //洞洞板通道開啟
  PORTB |= (1<<PB6);//洞洞板通道開啟(洞洞板轉到2)
	DDRB=0xff;

	i=0;
	for(int j=0;j<100;j++)
	{
		get[j]=0;
		position[j]=0;
	}
	mode=0;
	ID=0;
	data_1=0;






  USART1_Init ( MYUBRR1 );
  //USART0_Init ( MYUBRR0 );


	sei();	// 開啟所有中斷功能


	while(1)
  {

		while(mode==0)
		{
			//M128_HMI_get(1,&mode);//
			//if(mode==1) i=1;//
			if(i==1)
			{
				mode=get[0];//
				switch (mode) {
					case 128:
					{printf("mode1\n" );}
					break;

					case 129:
					{printf("mode2\n" );}
					break;

					case 130:
					{printf("mode3\n" );}
					break;

					case 4:
					{printf("mode4\n" );}
					break;

					default:
					mode=0;
					break;
				}     //switch case
			}   //if(i==1)
		}    //while for the mode



		if(mode>0)        //選擇完mode以後，將矩陣清空
		{
		printf("start mode%d\n",mode);
		for(int j=0;j<100;j++)
		{get[j]=0;}
		i=0;
	  }

		if(mode==128)         //mode1 電腦教學模式  128==0xA0
		{
			while (i<2) {;}  //收集兩筆資料
			ID=get[0];
			convert_1(get[1]);
			printf("ID:%d position:%d \n",ID,data_1);
			USART1_Transmit(ID+128);       //傳送ID
			USART1_Transmit(data_1>>7);  //傳送角度
			USART1_Transmit(data_1&127);
			for(int j=0;j<100;j++)
			{get[j]=0;}
			i=0;
			mode=0;
	  }

		else if(mode==129)         //mode2 手動教學模式  128==0xA1
		{
			for(k=0;k<17;k++)
			{
				USART1_Transmit(128+k);  // 128=0b1000 0000
				USART1_Transmit(0);
				USART1_Transmit(0);
				convert_2(get[(2*k)],get[(2*k+1)]);  //將送回來的兩筆資料轉成角度(0~100)
			}
			for(k=0;k<17;k++)
			{
				USART1_Transmit(128+k);    //傳送ID
				USART1_Transmit(position[k]); //傳送角度
			}
			for(int j=0;j<100;j++)
			{get[j]=0;
			position[j]=0;}
			i=0;
			mode=0;
	  }

		else if(mode==130)         //mode3 播放模式   130==0xA2
		{
			while (i<85) {;}    //等待至接收完85筆資料
			for(int k=0;k<85;k++)    //轉換資料
			{position[k]=get[k]*20+6000;}//轉換資料

			for(int l=0;l<5;l++)         //5個分解動作
			{
				for(int k=0;k<17;k++)
				{
					USART1_Transmit(128+k);  // 128=0b1000 0000
					USART1_Transmit(position[(l*17+k)]>>7);
					USART1_Transmit(position[(l*17+k)]&127);
				}
			}

			for(int j=0;j<100;j++)
			{get[j]=0;
			position[j]=0;}
			i=0;
			mode=0;
	  }                      //if(mode==3)


  }

    return 0;
}

ISR(USART1_RX_vect) {
	get[i]=UDR1;
	i++;
}
