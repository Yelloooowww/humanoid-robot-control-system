//void KONDO_data_convert(void)
//void KONDO_SDC_read(void)
//void KONDO_SDC_write(void)
//測試完成 11/8 00:42
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
int SDC_data[100];
char get[100];

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

  // printf("Start reading file data\n");
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
  // printf("Finish reading file data \n");
	SDC_data[0]=sum;
 	for(int j=0;j<sum;j++)
 	{
 			// printf("data[%d]=%d\n",j,data[j]*100);
 		 SDC_data[j+1]=data[j]*100;
 	}

	for(int j=0;j<=sum;j++)
 	{
 			// printf("data[%d]=%d\n",j,data[j]*100);
		 printf("SDC_data[%d]=%d\n",j,SDC_data[j] );
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
        printf("null\n" );
				sizeof_string = i;
				break;
			}
		}
    ASA_SDC00_put(ASA_ID, 0, sizeof_string, a);
  }

	ASA_SDC00_set(ASA_ID,200,0x01,0,0x00);  //關檔
	printf("mode132 done\n" );

}


int main(void)
{
  ASA_M128_set();
  printf("start test Chang\n");
  DDRB |= (1<<DDB7)|(1<<DDB6)|(1<<DDB5);   //洞洞板通道開啟
  PORTB |= (1<<PB6);   //洞洞板通道開啟(洞洞板轉到2)
	// PORTB &= ~(1<<PB7);
	// PORTB &= ~(1<<PB5);
	// DDRF=0xff;        //用PF1~4控制機器人左右半邊以及要送還是收

  // USART1_Init ( MYUBRR1 );
  // USART0_Init ( MYUBRR0 );
	// sei();	// 開啟所有中斷功能
	int num;


	while (1) {

		printf("num? \n" );
		scanf("%d",&num );      //ex1:3         ,ex2:(17*?)+2           ,ex3:2   ,ex4:(17*?)+3  ,?=1 or 2 or 3....
		printf("num=%d\n",num );
		_delay_ms(100);

		for (int i=0; i<num; i++) {
			printf("get[%d]=\n",i );
			scanf("%d",&get[i] );//ex1:128,ID,angle     ,ex2:130,(17*?)+2 ,1,2,3....(17*?)  ,ex3:131,n   (n=1 or 2 or 3 or...8)
			//ex4:132,n,(17*?)+2 ,1,2,3....(17*?)
		}

		for (int i=0; i<num; i++) {
			printf("get[%d]=%d\n",i,get[i] );
		}
		mode=get[0];
		printf("mode=%d\n",mode );

		switch (mode) {
			case 128:
			{
				KONDO_data_convert();
				mode=0;
			}	break;

	  	case 130:
			{
				KONDO_data_convert();
				mode=0;
			}	break;

	  	case 131:
			{
				KONDO_data_convert();
				KONDO_SDC_read();
				mode=0;
			}	break;

	  	case 132:
			{
				KONDO_data_convert();
				KONDO_SDC_write();
				mode=0;
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
		code=0;
		num=0;
		mode=0;
		for (int i=0; i<100; i++) {
			get[i]=0;
		}



	}//while (1)


    return 0;
}
