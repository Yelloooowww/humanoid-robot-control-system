#include "ASA_Lib.h"
#include "ASA_Lib_DAC00.h"
#include <math.h>
#include <string.h>

#define FOSC 11059200// Clock Speed
#define BAUD0 9600  //
#define BAUD1 115200//UL 115200
#define MYUBRR0 FOSC/16/BAUD0-1
#define MYUBRR1 FOSC/16/BAUD1-1
int y[100];

void USART1_Init( unsigned int ubrr )
{
/* Set baud rate */
//UCSR1B|=(1<<RXCIE1);  //致能TX，RX complete interrupt，沒有用到的話，不可以致能
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

while ( !( UCSR1A & (1<<UDRE1)) )  //If UDREn is one, the buffer is empty
;
UDR1 = data;
}

void SDC_read(uint8_t package)
{
	char name[4];
	printf("package=%d\n",package );
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
  char sum;
  char check = 0;	// module communication result state flag

  unsigned char Mask = 0xFF, Shift = 0, Setting = 0xFF;

  ASA_SDC00_put(ASA_ID, 64, 8, name);
	ASA_SDC00_put(ASA_ID, 72, 3, "txt");

	// Configure to open file
	Setting = 0x01;		// Setting mode for openFile (readonly)
	check = ASA_SDC00_set(ASA_ID, 200, Mask, Shift, Setting);	// 送出旗標組合
	if( check != 0 ) {	// 檢查回傳值做後續處理, If check equal 0 then operation was successful
		printf("Debug point 31, error code <%d>\n", check);
		return;
	}



	printf("Start reading file data\n");
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
	printf("Finish reading file data \n");
  for(int j=0;j<sum;j++)
  {
      printf("data[%d]=%d\n",j,data[j]*100);
			y[j]=data[j]*100;
  }


	// Configure to close file mode
	Setting = 0x00;
	check = ASA_SDC00_set(ASA_ID, 200, Mask, Shift, Setting);// 送出旗標組合
	// if( check != 0 ) {				// 檢查回傳值做後續處理
	// 	printf("Debug point 32, error code <%d>\n", check);
	// 	//return;
	// }
  printf("finish SDC1\n");
}


int main(void)
{
  ASA_M128_set();
  printf("start SDC1\n");
  DDRB |= (1<<DDB7)|(1<<DDB6)|(1<<DDB5);   //洞洞板通道開啟
  PORTB |= (1<<PB6);//洞洞板通道開啟(洞洞板轉到2)
  USART1_Init ( MYUBRR1 );
	uint8_t number;
	printf("plz input a number (1~8) ");
	scanf("%d",&number );
	SDC_read(number);//選擇套裝動作1~8

	for(int i=0;i<100;i++)
	{
		if(y[i]==0)
		break;
		printf("y[%d]=%d\n",i,y[i] );
		USART1_Transmit(128);
		USART1_Transmit(y[i]>>7);
		USART1_Transmit(y[i]&127);
		_delay_ms(100);
	}

	printf("finish SDC1\n");

  return 0;

}
