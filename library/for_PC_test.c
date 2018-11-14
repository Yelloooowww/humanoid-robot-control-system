//測試完成 11/13 18:34
#include "ASA_Lib.h"
// #include "ASA_Lib_DAC00.h"
#include <math.h>
#include <string.h>

#define FOSC 11059200// Clock Speed
#define BAUD0 9600  //
#define BAUD1 9600//UL 115200
#define MYUBRR0 FOSC/16/BAUD0-1
#define MYUBRR1 FOSC/16/BAUD1-1

// volatile uint8_t i;
// volatile char get[100];
int mode,ID,angle,code;
int data[100];
int SDC_data[100];
char get[100];
char flag_pac;

// void USART1_Init( unsigned int ubrr )
// {
// /* Set baud rate */
// UCSR1B|=(1<<TXCIE1);  //致能TX，RX complete interrupt，沒有用到的話，不可以致能
// UBRR1H |= (unsigned char)(ubrr>>8);   //p.362 // fosc = 11.0592MHz，Baud Rate=9600，U2X=0 =>UBRR=71，U2X=1=>UBRR=143
// UBRR1L |= (unsigned char)ubrr;
// /* Enable receiver and transmitter */
// UCSR1B |= (1<<RXEN1)|(1<<TXEN1);    //enables the USARTn Receiver，enables the USARTn Transmitter
// /* Set frame format: 8data, 2stop bit */
// //UCSR1C = (0<<USBS1)|(3<<UCSZ10); //selects the number of stop bits，USBS1=1=> 2 bits
// UCSR1C |= (1<<UPM11)|(0<<UPM10)|(1<<USBS1)|(1<<UCSZ11)|(1<<UCSZ10)|(0<<UCPOL1);//Character Size=8 bits，UCPOL1=上升/下降
// }

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
				code=get[1];
				printf("code=%d\n",code );
			}
			break;

		case 132:
			{
				code=get[1];
				printf("code=%d\n",code );
			}
			break;

	}//switch(mode)

}//KONDO_data_convert

void KONDO_SDC_read(void)
{
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
  uint8_t swap_buffer[100];// 宣告 與SDC00交換資料的資料陣列緩衝區
 	char data[1000];//陣列緩衝區
 	int z=0,sum=0;

 	char check = 0;	// module communication result state flag

 	unsigned char Mask = 0xFF, Shift = 0, Setting = 0xFF;

 	ASA_SDC00_put(ASA_ID, 64, 8, name);
  ASA_SDC00_put(ASA_ID, 72, 3, "txt");

  // Configure to open file
  Setting = 0x01;		// Setting mode for openFile (readonly)
  ASA_SDC00_set(ASA_ID, 200, Mask, Shift, Setting);	// 送出旗標組合

  printf("Start reading file data\n");
  int rec = 0;
  for(int i=0; i<4000; i++) {

 	 if(rec = ASA_SDC00_get(ASA_ID, 0, 64, swap_buffer)) {//每次收64Bytes
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
	SDC_data[0]=sum;
 	for(int j=0;j<sum;j++)
 	{
 			printf("data[%d]=%d\n",j,data[j]*100);
 		 SDC_data[j+1]=data[j]*100;
 	}

	for(int j=0;j<=sum;j++)
 	{
 			printf("data[%d]=%d\n",j,data[j]*100);
		 printf("SDC_data[%d]=%d\n",j,SDC_data[j] );
 	}


  // Configure to close file mode
  Setting = 0x00;
  check = ASA_SDC00_set(ASA_ID, 200, Mask, Shift, Setting);// 送出旗標組合
  // if( check != 0 ) {				// 檢查回傳值做後續處理
  // 	printf("Debug point 32, error code <%d>\n", check);
  // 	//return;
  // }
 	printf("finish SDC1\n");
 	for(int i=0;i<100;i++)
 	{swap_buffer[i]=0;}
}//SDC



void KONDO_SDC_write(void)
{
	char name[4];
	uint8_t a[4];
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

  for(int j=3;j<(get[2]+3);j++)
  {
    // if(kk[i]<16)
    // {sprintf(a,"%c%x\r\n",'0',kk[i]);}//將0填到字串中
    // else
    // sprintf(a,"%x\r\n",kk[i]);//將數字轉成16進位後填到字串中

    if(get[j]<16)//若數字小於16
    {sprintf(a,"%c%x\r\n",'0',get[j]);}//將0填到字串中,將數字轉成16進位後填到字串中
    else//若數字>=16
    sprintf(a,"%x\r\n",get[j]);//將數字轉成16進位後填到字串中

    int sizeof_string =4;
		for(int i=0; i<sizeof(a); i++) {
			if( a[i] == '\0' ) {
        // printf("null\n" );
				sizeof_string = i;
				break;
			}
		}
    ASA_SDC00_put(ASA_ID, 0, sizeof_string, a);
  }

	ASA_SDC00_set(ASA_ID,200,0x01,0,0x00);  //關檔
	printf("mode132 done\n" );

}

void KONDO_transmit(int mode){
  int n = 1;  //於mode 130、131使用的計數用變數

	/*洞洞板重新設定*/
	DDRB |= (1<<DDB7)|(1<<DDB6)|(1<<DDB5);   //洞洞板通道開啟
	PORTB |= (1<<PB6);   //洞洞板通道開啟(洞洞板轉到2)

	PORTB &= ~(1<<PB7);
	PORTB &= ~(1<<PB5);

  switch (mode) {
    /*Teaching_mode*/
		angle=angle*100;
    case 128:
      printf("---128---\n");
      DDRF = 0xFF;

      if(ID > 8){
        PORTF = 127;
				while ( !( UCSR1A & (1<<UDRE1)) );  //If UDREn is one, the buffer is empty
        UDR1 = (ID-8)+128;
      }else{
        PORTF = 191;
				while ( !( UCSR1A & (1<<UDRE1)) );  //If UDREn is one, the buffer is empty
        UDR1 = ID+128;
      }
        printf("(128)[ID] = %d\n",ID);
      while ( !( UCSR1A & (1<<UDRE1)) );  //If UDREn is one, the buffer is empty
      UDR1 = angle>>7;
        printf("(128)[angle_1] = %d\n",angle>>7);
      while ( !( UCSR1A & (1<<UDRE1)) );  //If UDREn is one, the buffer is empty
      UDR1 = angle&127;
        printf("(128)[angle_2] = %d\n",angle&127);
      while ( !( UCSR1A & (1<<UDRE1)) );  //If UDREn is one, the buffer is empty

      break;

    /*Play_mode*/
    case 130:
      printf("---130---\n");
      /*判斷左右兩側*/
      DDRF = 0xFF;

      /*以data[0]決定資料總筆數，換算為姿態數*/
      for(int i = 0;i < data[0]/17; i++){
        /*一姿態之資料*/
        for(int j = 0;j < 17; j++){
          /*依照順序送至各角度*/
          if(j > 8){
            PORTF = 191;
            UDR1 = (j-8)+128;
          }else{
            PORTF = 127;
            UDR1 = j+128;
          }
            printf("(130)[ID] = %d\n",j);
          while ( !( UCSR1A & (1<<UDRE1)) );  //If UDREn is one, the buffer is empty
          UDR1 = data[n] >> 7;
            printf("(130)[%d] = %d\n",n,data[n] >> 7);
          while ( !( UCSR1A & (1<<UDRE1)) );  //If UDREn is one, the buffer is empty
          UDR1 = data[n] & 127;
            printf("(130)[%d] = %d\n",n,data[n] & 127);
          while ( !( UCSR1A & (1<<UDRE1)) );  //If UDREn is one, the buffer is empty
          n++;
        }
        /*送完一姿態之資料，等待一秒，再繼續下一姿態*/
        _delay_ms(1000);

      }

      break;

    /*Remote_mode*/
    case 131:
      printf("---131---\n");
      /*判斷左右兩側*/
      DDRF = 0xFF;

      for(int i = 0;i < SDC_data[0]/17; i++){
        /*一姿態之資料*/
        for(int j = 0;j < 17; j++){
          /*依照順序送至各角度*/
          if(j > 8){
            PORTF = 191;
            UDR1 = (j-8)+128;
          }else{
            PORTF = 127;
            UDR1 = j+128;
          }

            printf("(131)[ID] = %d\n",j);
          while ( !( UCSR1A & (1<<UDRE1)) );  //If UDREn is one, the buffer is empty
          UDR1 = SDC_data[n] >> 7;
            printf("(131)[%d] = %d\n",n,SDC_data[n] >> 7);
          while ( !( UCSR1A & (1<<UDRE1)) );  //If UDREn is one, the buffer is empty
          UDR1 = SDC_data[n] & 127;
            printf("(131)[%d] = %d\n",n,SDC_data[n] & 127);
          while ( !( UCSR1A & (1<<UDRE1)) );  //If UDREn is one, the buffer is empty
          n++;
        }
        /*送完一姿態之資料，等待一秒，再繼續下一姿態*/
        _delay_ms(1000);

      }

      break;

    default:
      // printf("-----ERROR-----\n");
      break;
  }

}

typedef enum {Header,Bytes,type,Data,checksum}state;
state now_state;
void my_decoder(uint8_t u){
	DDRB |= (1<<DDB7)|(1<<DDB6)|(1<<DDB5);   //洞洞板通道開啟
	PORTB |= (1<<PB6);   //洞洞板通道開啟(洞洞板轉到2)

	PORTB &= ~(1<<PB7);
	PORTB &= ~(1<<PB5);

  USART1_Transmit(0xAB);
  static uint8_t b,d,sum_check;
  static uint16_t len_data;
  switch (now_state) {
    case Header:{
			printf("Header\n" );
      USART1_Transmit(1);
      b=0;
      d=0;
      len_data=0;
      sum_check=0;
      if(u==0xfd) now_state=type;
      break;
    }
    case type:{
			printf("type\n" );
      USART1_Transmit(2);
      if(u) now_state=Bytes; //只要是正數就好了@@
      else  now_state=Header;

      break;
    }
    case Bytes:{
			printf("Bytes\n" );
      USART1_Transmit(3);
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
			printf("Data\n" );
      USART1_Transmit(4);
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
			printf("checksum\n" );
      USART1_Transmit(5);
      uint8_t tmp=(sum_check&0xff);
      if(tmp == u){
        sum_check=0;
        now_state=Header;

        USART1_Transmit(get[0]);
        USART1_Transmit(get[1]);
        USART1_Transmit(get[2]);
				flag_pac=1;
      }else{
        now_state=Header;
      }
      break;
    }
  }
}




void USART1_Init( unsigned int ubrr )
{
  UBRR1H |= (unsigned char)(ubrr>>8);
  UBRR1L |= (unsigned char)ubrr;
  UCSR1B|=(1<<RXCIE1);// Rx中斷致能
  UCSR1B |= (1<<RXEN1)|(1<<TXEN1);//Tx Rx致能
  UCSR1B &= (~(1<<UCSZ12));//8-bit: UCSZn2=0,UCSZn1=1,UCSZn0=1
  UCSR1C |=(1<<UCSZ11)|(1<<UCSZ10);
  UCSR1C &= (~(1<<USBS1));//stopbit=1
}

void USART1_Transmit( unsigned char data )
{
  UDR1= data;
  while ( !( UCSR0A & (1<<UDRE0)) )  //If UDREn is one, the buffer is empty
  ;
}
int main(void)
{
  ASA_M128_set();
  printf("start test Chang\n");
  DDRB |= (1<<DDB7)|(1<<DDB6)|(1<<DDB5);   //洞洞板通道開啟
  PORTB |= (1<<PB6);   //洞洞板通道開啟(洞洞板轉到2)

	PORTB &= ~(1<<PB7);
	PORTB &= ~(1<<PB5);
	DDRF=0xff;        //用PF1~4控制機器人左右半邊以及要送還是收
	// USART1_Init ( MYUBRR1 );
	USART1_Init ( MYUBRR1 );

	sei();	// 開啟所有中斷功能

  // USART1_Init ( MYUBRR1 );
  // USART1_Init ( MYUBRR1 );
	// sei();	// 開啟所有中斷功能
	int num;


	while (1) {

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

		// for (int i=0; i<num; i++) {
		// 	printf("get[%d]=%d\n",i,get[i] );
		// }

		if(flag_pac==1)
		{mode=get[0];}
		printf("mode=%d\n",mode );
		printf("flag_pac=%d\n",flag_pac );
		printf("get[0]=%d\n",get[0] );
		_delay_ms(100);

		switch (mode) {
			case 128:
			{
				KONDO_data_convert();
				KONDO_transmit(mode);
				mode=0;
				code=0;
				num=0;
				for (int i=0; i<100; i++) {
					get[i]=0;
					data[i]=0;
					SDC_data[i]=0;
				}
				flag_pac=0;
			}	break;

	  	case 130:
			{
				KONDO_data_convert();
				KONDO_transmit(mode);
				mode=0;
				code=0;
				num=0;
				for (int i=0; i<100; i++) {
					get[i]=0;
					data[i]=0;
					SDC_data[i]=0;
				}
				flag_pac=0;
			}	break;

	  	case 131:
			{
				KONDO_data_convert();
				KONDO_SDC_read();
				KONDO_transmit(mode);
				mode=0;
				code=0;
				num=0;
				for (int i=0; i<100; i++) {
					get[i]=0;
					data[i]=0;
					SDC_data[i]=0;
				}
				flag_pac=0;
			}	break;

	  	case 132:
			{
				KONDO_data_convert();
				KONDO_SDC_write();
				KONDO_transmit(mode);
				mode=0;
				code=0;
				num=0;
				for (int i=0; i<100; i++) {
					get[i]=0;
					data[i]=0;
					SDC_data[i]=0;
				}
				flag_pac=0;
			}	break;

	  	default:
			{
				// printf("mode wrong");
				mode=0;
			}break;
		}//switch(mode)






	}//while (1)


    return 0;
}

// ISR(USART1_TX_vect) {
// 	printf("hi\n" );
// }
ISR(USART1_RX_vect) {
	uint8_t g=UDR1;
  my_decoder(g);
}
