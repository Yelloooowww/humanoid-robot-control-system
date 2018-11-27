#include "ASA_Lib.h"

unsigned int  SDC_FIFO_max=10;//for FIFO
unsigned int  SDC_FIFO_rear=9;//for FIFO
unsigned int  SDC_FIFO_front=9;//for FIFO
uint8_t KONDO_SDC_FIFO[10][17];//for FIFO，需小於最小檔案之資料量

void PutIn(uint8_t *p){
  if((SDC_FIFO_rear+1)%SDC_FIFO_max==SDC_FIFO_front){
    printf("FIFO Is FULL\n" );
  }else{
    printf("put in\n" );
    SDC_FIFO_rear=(SDC_FIFO_rear+1)%SDC_FIFO_max;
    for(int i=0;i<17;i++){
      KONDO_SDC_FIFO[i][SDC_FIFO_rear]=*(p+i);
    }
  }
  printf("AfterPut:SDC_FIFO_rear=%d SDC_FIFO_front=%d\n", SDC_FIFO_rear,SDC_FIFO_front);
}//void PutIn(uint8_t *p)

void TakeOut(uint8_t *t){
  if(SDC_FIFO_front == SDC_FIFO_rear){
    printf("FIFO Is Empty\n");
  }else{
    printf("take out\n" );
    SDC_FIFO_front=(SDC_FIFO_front+1)%SDC_FIFO_max;
    for(int i=0;i<17;i++){
      *(t+i)=KONDO_SDC_FIFO[i][SDC_FIFO_front];
    }
  }
  printf("AfterTake:SDC_FIFO_rear=%d SDC_FIFO_front=%d\n", SDC_FIFO_rear,SDC_FIFO_front);
}

int main(){
  ASA_M128_set();
  int i=1;
  printf("START~~~~~~~~~~~~~~~~~~~~~~~~\n" );
  while (i++) {
    uint8_t put_into_FIFO[17]={i,i+1,i+2,i+3,i+4,i+5,i+6,i+7,i+8,i+9,i+10,i+11,i+12,i+13,i+14,i+15,i+16};
    PutIn(put_into_FIFO);

    uint8_t take_out_from_FIFO[17];
    TakeOut(take_out_from_FIFO);
    printf("take_out_from_FIFO=");
    for(int i=0;i<17;i++){
      printf("%d ",  take_out_from_FIFO[i] );
    }
    printf("\n" );
  }
  // while (i++) {
  //   // put in
  //     uint8_t put_into_FIFO[17]={i,i+1,i+2,i+3,i+4,i+5,i+6,i+7,i+8,i+9,i+10,i+11,i+12,i+13,i+14,i+15,i+16};
  //     if((SDC_FIFO_rear+1)%SDC_FIFO_max==SDC_FIFO_front){
  //       printf("FIFO Is FULL\n" );
  //     }else{
  //       printf("put in\n" );
  //       SDC_FIFO_rear=(SDC_FIFO_rear+1)%SDC_FIFO_max;
  //       for(int i=0;i<17;i++){
  //         KONDO_SDC_FIFO[i][SDC_FIFO_rear]=put_into_FIFO[i];
  //       }
  //     }
  //     printf("AfterPut:SDC_FIFO_rear=%d SDC_FIFO_front=%d\n", SDC_FIFO_rear,SDC_FIFO_front);

    // //take out
    //   uint8_t take_out_from_FIFO[17];
    //   if(SDC_FIFO_front == SDC_FIFO_rear){
    //     printf("FIFO Is Empty\n");
    //   }else{
    //     printf("take out\n" );
    //     SDC_FIFO_front=(SDC_FIFO_front+1)%SDC_FIFO_max;
    //     for(int i=0;i<17;i++){
    //       take_out_from_FIFO[i]=KONDO_SDC_FIFO[i][SDC_FIFO_front];
    //     }
    //   }
    //   printf("AfterTake:SDC_FIFO_rear=%d SDC_FIFO_front=%d\n", SDC_FIFO_rear,SDC_FIFO_front);
    //
    //   printf("take_out_from_FIFO=");
    //   for(int i=0;i<17;i++){
    //     printf("%d ",  take_out_from_FIFO[i] );
    //   }
    //   printf("\n" );
  }




  return 0;
}
