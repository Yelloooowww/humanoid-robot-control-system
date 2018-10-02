#include "ASA_Lib.h"
#include "ASA_Lib_DAC00.h"
#include <math.h>
#include <string.h>

int main(void)
{
  ASA_M128_set();
  printf("start SDC_3\n");


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

/////////////////////////////////





  // uint8_t a[4];
  // uint8_t num=1;
  // unsigned char ASA_ID = 4;
	// ASA_SDC00_put(ASA_ID, 64, 8, "SDC5");
	// ASA_SDC00_put(ASA_ID, 72, 3, "txt");
	// ASA_SDC00_set(ASA_ID, 200, 0x07, 0, 0x05);//開檔續寫
  //
  // for(int num=1;num<101;num++)
  // {
  //   if(num<16)
  //   {sprintf(a,"%c%x\r\n",'0',num);}//將0填到字串中
  //   else
  //   sprintf(a,"%x\r\n",num);//將數字填到字串中
  //   int sizeof_string =4;
	// 	for(int i=0; i<sizeof(a); i++) {
	// 		if( a[i] == '\0' ) {
  //       printf("null\n" );
	// 			sizeof_string = i;
	// 			break;
	// 		}
	// 	}
  //   ASA_SDC00_put(ASA_ID, 0, sizeof_string, a);
  // }
  //
	// ASA_SDC00_set(ASA_ID,200,0x01,0,0x00);  //關檔




  ///////////////////////////////////
  uint8_t swap_buffer[100];// 宣告 與SDC00交換資料的資料陣列緩衝區
  int position[100];
  int data_H[100],data_L[100],data[100];
  char check = 0;	// module communication result state flag

  unsigned char ASA_ID = 4, Mask = 0xFF, Shift = 0, Setting = 0xFF;
  ASA_SDC00_put(ASA_ID, 64, 8, "SDC6");
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

          if(position[i-1]>=48 && position[i-1]<=57)
          {position[i-1]=position[i-1]-48;}
          if(position[i-1]>=97 && position[i-1]<=102)
          {position[i-1]=position[i-1]-87;}

          if(position[i]>=48 && position[i]<=57)
          {position[i]=position[i]-48;}
          if(position[i]>=97 && position[i]<=102)
          {position[i]=position[i]-87;}



          printf("%x\n",position[i-1]*16+position[i] );



        }//if
			}
			printf("\nReading finish! Get %ld  bytes %d\n", (long)i*64 + swap_buffer[63], rec);
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



      }//if

		}


	}
	printf("Finish reading file data \n");


	// Configure to close file mode
	Setting = 0x00;
	check = ASA_SDC00_set(ASA_ID, 200, Mask, Shift, Setting);// 送出旗標組合
	if( check != 0 ) {				// 檢查回傳值做後續處理
		printf("Debug point 32, error code <%d>\n", check);
		//return;
	}






  ////////////////////////////////
  ////////讀取
  /*uint8_t swap_buffer[100];// 宣告 與SDC00交換資料的資料陣列緩衝區
  char check = 0;	// module communication result state flag
  int k=0;

  unsigned char ASA_ID = 4, Mask = 0xFF, Shift = 0, Setting = 0xFF;
  ASA_SDC00_put(ASA_ID, 64, 8, "SDC2");
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
				printf("i=%d  %c\n", i,swap_buffer[i]);
			}
			printf("\nReading finish! Get %ld  bytes %d\n", (long)i*64 + swap_buffer[63], rec);
			break;
		}

		for(int i=0; i<64; i++) {
      swap_buffer[-1]=0;
      swap_buffer[-2]=0;
      swap_buffer[-3]=0;
      swap_buffer[-4]=0;
      if(swap_buffer[i]==10 && swap_buffer[i-1]==13){
        if((swap_buffer[i-3]-48)>0 && (swap_buffer[i-4]-48)>0)
        printf("3no.%d %d\n", i, (swap_buffer[i-2]-48)*1+(swap_buffer[i-3]-48)*10+(swap_buffer[i-4]-48)*100);

        else if((swap_buffer[i-3])>48)
        printf("2no.%d %d\n", i, (swap_buffer[i-2]-48)*1+(swap_buffer[i-3]-48)*10);

        else
        printf("1no.%d %d\n", i, (swap_buffer[i-2]-48));
      }
      // if(swap_buffer[i]==10 && swap_buffer[i-1]==13)
      // i=64;

		}
    k++;
	}
	printf("Finish reading file data \n");
  printf("k=%d\n",k);

	// Configure to close file mode
	Setting = 0x00;
	check = ASA_SDC00_set(ASA_ID, 200, Mask, Shift, Setting);// 送出旗標組合
	if( check != 0 ) {				// 檢查回傳值做後續處理
		printf("Debug point 32, error code <%d>\n", check);
		//return;
	}*/     //讀取

  /*uint8_t a[4];
  uint8_t num=1;
  unsigned char ASA_ID = 4;
	ASA_SDC00_put(ASA_ID, 64, 8, "SDC4");
	ASA_SDC00_put(ASA_ID, 72, 3, "txt");
	ASA_SDC00_set(ASA_ID, 200, 0x07, 0, 0x05);//開檔續寫

  for(num=1;num<101;num++)
  {
    sprintf(a,"%x\r\n",num);//將數字填到字串中
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

	ASA_SDC00_set(ASA_ID,200,0x01,0,0x00);  //關檔*/

  printf("finish SDC_3\n");



  return 0;


}
