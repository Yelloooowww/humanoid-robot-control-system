#include "ASA_Lib.h"

//測試用數據
int mode = 0;
int ID = 5;
int angle = 8000;
int data[] = {51,666,777};
int SDC_data[] = {68,888,999};

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
      printf("-----ERROR-----\n");
      break;
  }

}


int main(void)
{
  ASA_M128_set();
  while (1) {
    printf("mode = ? ");
    scanf("%d", &mode);

    KONDO_transmit(mode);
  }
  return 0;
}
