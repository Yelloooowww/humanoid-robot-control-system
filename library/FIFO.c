//上次忘記我們的資料長度應該用uint16_t了
#include "ASA_Lib.h"

unsigned int  SDC_FIFO_max=10;//for FIFO
unsigned int  SDC_FIFO_rear=9;//for FIFO
unsigned int  SDC_FIFO_front=9;//for FIFO
uint16_t KONDO_SDC_FIFO[10][17];//for FIFO，需小於最小檔案之資料量

char PutIn(uint16_t *p){
  if((SDC_FIFO_rear+1)%SDC_FIFO_max==SDC_FIFO_front){
    printf("FIFO Is FULL\n" );
    return 1;
  }else{
    printf("put in\n" );
    SDC_FIFO_rear=(SDC_FIFO_rear+1)%SDC_FIFO_max;
    for(int i=0;i<17;i++){
      KONDO_SDC_FIFO[i][SDC_FIFO_rear]=*(p+i);
    }
  }
  printf("AfterPut:SDC_FIFO_rear=%d SDC_FIFO_front=%d\n", SDC_FIFO_rear,SDC_FIFO_front);
  return 0;
}//void PutIn(uint8_t *p)

char TakeOut(uint16_t *t){
  if(SDC_FIFO_front == SDC_FIFO_rear){
    printf("FIFO Is Empty\n");
    return 1;
  }else{
    printf("take out\n" );
    SDC_FIFO_front=(SDC_FIFO_front+1)%SDC_FIFO_max;
    for(int i=0;i<17;i++){
      *(t+i)=KONDO_SDC_FIFO[i][SDC_FIFO_front];
    }
  }
  printf("AfterTake:SDC_FIFO_rear=%d SDC_FIFO_front=%d\n", SDC_FIFO_rear,SDC_FIFO_front);
  return 0;
}

int main(){
  ASA_M128_set();
  int i=1;
  printf("START~~~~~~~~~~~~~~~~~~~~~~~~\n" );
  while (i++) {
    uint16_t put_into_FIFO[17]={i+1000,i+1001,i+1002,i+1003,i+1004,i+1005,i+1006,i+1007,i+1008,i+1009,i+1010,i+1011,i+1012,i+1013,i+1014,i+1015,i+1016};//一定要uint16_t!!!
    char check1=PutIn(put_into_FIFO);
    printf("check1=%d\n",check1 );

    uint16_t take_out_from_FIFO[17];  //一定要uint16_t!!!
    char check2=TakeOut(take_out_from_FIFO);
    printf("check2=%d\n",check2 );
    if(check2==0){
      printf("take_out_from_FIFO=");
      for(int i=0;i<17;i++){
        printf("%d ",  take_out_from_FIFO[i] );
      }
      printf("\n" );
    }
  }


  return 0;
}
