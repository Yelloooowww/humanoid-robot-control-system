#include "ASA_Lib.h"
#include "ASA_Lib_DAC00.h"
#include <math.h>
#include <string.h>

#define FOSC 11059200// Clock Speed
#define BAUD0 9600  //
#define BAUD1 115200//UL 115200
#define MYUBRR0 FOSC/16/BAUD0-1
#define MYUBRR1 FOSC/16/BAUD1-1

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

int main(void)
{
  ASA_M128_set();
  printf("start SDC1\n");
  DDRB |= (1<<DDB7)|(1<<DDB6)|(1<<DDB5);   //洞洞板通道開啟
  PORTB |= (1<<PB6);//洞洞板通道開啟(洞洞板轉到2)
  USART1_Init ( MYUBRR1 );

  int b;

  b=7500;         //
  USART1_Transmit(0b10000000);
  USART1_Transmit(b>>7);
  USART1_Transmit(b&127);


  uint8_t a[4];
  uint8_t num=1;
  int k[3];
  k[0]=3500;
  k[1]=7500;
  k[2]=11500;

  uint8_t kk[6];
  for(int i=0;i<6;i++)
  {
    kk[i*2]=k[i]>>7;
    kk[i*2+1]=k[i]&127;
  }
  unsigned char ASA_ID = 4;
	ASA_SDC00_put(ASA_ID, 64, 8, "SDC6");
	ASA_SDC00_put(ASA_ID, 72, 3, "txt");
	ASA_SDC00_set(ASA_ID, 200, 0x07, 0, 0x05);//開檔續寫

  for(int i=0;i<6;i++)
  {
    if(kk[i]<16)
    {sprintf(a,"%c%x\r\n",'0',kk[i]);}//將0填到字串中
    else
    sprintf(a,"%x\r\n",kk[i]);//將數字填到字串中
    int sizeof_string =4;
		for(int i=0; i<sizeof(a); i++) {
			if( a[i] == '\0' ) {
        printf("null\n" );
				sizeof_string = i;
				break;
			}
		}
    ASA_SDC00_put(ASA_ID, 0, sizeof_string, a);
  }

	ASA_SDC00_set(ASA_ID,200,0x01,0,0x00);  //關檔




  uint8_t swap_buffer[100];// 宣告 與SDC00交換資料的資料陣列緩衝區
  int position[100];
  char data[1000];
  int z=0;
  char sum;
  char check = 0;	// module communication result state flag

  unsigned char Mask = 0xFF, Shift = 0, Setting = 0xFF;
  ASA_SDC00_put(ASA_ID, 64, 8, "SDC7");
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
          //printf("data=%c %c\n", swap_buffer[i-1], swap_buffer[i]);
          position[i-1]=swap_buffer[i-1];
          position[i]=swap_buffer[i];
          //printf("%x %x\n",position[i-1],position[i] );

          if(swap_buffer[i-1]>=48 && swap_buffer[i-1]<=57)
          {swap_buffer[i-1]=swap_buffer[i-1]-48;}
          if(swap_buffer[i-1]>=97 && swap_buffer[i-1]<=102)
          {swap_buffer[i-1]=swap_buffer[i-1]-87;}

          if(swap_buffer[i]>=48 && swap_buffer[i]<=57)
          {swap_buffer[i]=swap_buffer[i]-48;}
          if(swap_buffer[i]>=97 && swap_buffer[i]<=102)
          {swap_buffer[i]=swap_buffer[i]-87;}



          printf("111   %x\n",swap_buffer[i-1]*16+swap_buffer[i] );
          data[z]=swap_buffer[i-1]*16+swap_buffer[i];
          z++;




        }//if
			}
			printf("\nReading finish! Get %ld  bytes %d\n", (long)i*64 + swap_buffer[63], rec);
      sum=((long)i*64 + swap_buffer[63])/4;
			break;
		}



		for(int i=0; i<64; i++) {

      if(i%4==1)
      {

        position[i-1]=swap_buffer[i-1];
        position[i]=swap_buffer[i];


        if(position[i-1]>=48 && position[i-1]<=57)
        {position[i-1]=position[i-1]-48;}
        if(position[i-1]>=97 && position[i-1]<=102)
        {position[i-1]=position[i-1]-87;}

        if(position[i]>=48 && position[i]<=57)
        {position[i]=position[i]-48;}
        if(position[i]>=97 && position[i]<=102)
        {position[i]=position[i]-87;}

        //printf("tranformed\n" );

        printf("%x\n",position[i-1]*16+position[i] );
        data[z]=position[i-1]*16+position[i];
        z++;



      }//if

		}


	}
	printf("Finish reading file data \n");
  for(int j=0;j<sum;j++)
  {

      // printf("data[%d]=%x\n",j-1,data[j-1] );
      printf("data[%d]=%x\n",j,data[j] );
      int y=data[j]*100;
      USART1_Transmit(128);
      USART1_Transmit(y>>7);
      USART1_Transmit(y&127);

  }


	// Configure to close file mode
	Setting = 0x00;
	check = ASA_SDC00_set(ASA_ID, 200, Mask, Shift, Setting);// 送出旗標組合
	// if( check != 0 ) {				// 檢查回傳值做後續處理
	// 	printf("Debug point 32, error code <%d>\n", check);
	// 	//return;
	// }


  printf("finish SDC_3\n");



  return 0;


}
