#include "ASA_Lib.h"

//測試用數據
int mode = 0;
int id = 5;
int angle = 8000;
int data[] = {51,666,777};
int SDC_data[] = {68,888,999};

void KONDO_transmit(int mode){
  int n = 1;  /*於mode 130使用的計數用變數*/
  switch (mode) {
    /*Teaching_mode*/
    case 128:
        // printf("---128---\n");
      UDR1 = id+128;
        // printf("(128)[id] = %d\n",id);
      while ( !( UCSR1A & (1<<UDRE1)) );  //If UDREn is one, the buffer is empty
      UDR1 = angle>>7;
        // printf("(128)[angle_1] = %d\n",angle>>7);
      while ( !( UCSR1A & (1<<UDRE1)) );  //If UDREn is one, the buffer is empty
      UDR1 = angle&127;
        // printf("(128)[angle_2] = %d\n",angle&127);
      while ( !( UCSR1A & (1<<UDRE1)) );  //If UDREn is one, the buffer is empty

      break;

    /*Play_mode*/
    case 130:
      // printf("---130---\n");
      /*以data[0]決定資料總筆數，換算為姿態數*/
      for(int i = 0;i < data[0]/17; i++){
        /*一姿態之資料*/
        for(int j = 0;j < 17; j++){
          UDR1 = data[n];
          // printf("(130)[%d] = %d\n",n,data[n]);
          while ( !( UCSR1A & (1<<UDRE1)) );  //If UDREn is one, the buffer is empty
          n++;
        }
        /*送完一姿態之資料，等待一秒，再繼續下一姿態*/
        _delay_ms(1000);
      }

      break;

    /*Remote_mode*/
    case 131:
      // printf("---131---\n");
      for(int i = 0;i < SDC_data[0]/17; i++){
        /*一姿態之資料*/
        for(int j = 0;j < 17; j++){
          UDR1 = SDC_data[n];
          // printf("(131)[%d] = %d\n",n,SDC_data[n]);
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
