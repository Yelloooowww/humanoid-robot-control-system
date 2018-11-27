//[2018.09.27 用以測試轉動 OK]
//[RX接訊號 可直接控]
//[2018.11.3 用以測試轉動 OK]
//[RX接訊號 可直接控]
//[2018.11.5 更改程式，可改為控siwtch方向 (Line:79~86)]

//2018.09.27:
// [0x00可以成功鬆開轉矩，不用while]
// 1.直接輸入0x00 -> 可用手扳動，讀取矩陣正常顯示ID， "但角度都是0"
// 2.先輸入一次角度之後，接著輸入0x00 -> 可用手扳動，且能讀出輸入角度之位置，
// 					但隨後接著扳動時，"讀取的get矩陣不會更新!!!"
// 					嘗試過2個軸，皆為同樣結果
// 註:已嘗試方法: 把get[]型態從volatile改為int -> 失敗

#include "ASA_Lib.h"
// #include <string.h>

#define FOSC 11059200// Clock Speed
#define BAUD 115200//UL
#define MYUBRR FOSC/16/BAUD-1

// volatile uint8_t test,get[3];
int i,get[3];

int Servo_ID[9] = {0,1,2,4,6,7,8,9,10};
unsigned int Now[17] ;



void timer2_Init(){
  TCCR2|=(1<<WGM21)|(0<<WGM20);  //CTC Mode
  TCCR2|=(1<<CS21)|(1<<CS20)|(0<<CS22); //clkI/O/64 (From prescaler)
  TIMSK|=(1<<OCIE2);
  OCR2=171;
}

void USART_Init( unsigned int ubrr )
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


void KONDO_transmit(){

  int Servo_ID[9] = {0,1,2,4,6,7,8,9,10};

  /*洞洞板重新設定*/
  DDRB |= (1<<DDB7)|(1<<DDB6)|(1<<DDB5);   //洞洞板通道開啟
  PORTB |= (1<<PB6);   //洞洞板通道開啟(洞洞板轉到2)
  PORTB &= ~(1<<PB7);
  PORTB &= ~(1<<PB5);
  static int i = 0;

  for( i = 0;i < 17; i++){

    //判斷ID，決定致能左或右
    _delay_ms(1); //軸與軸之間切換(switch切換時間)
    if(i > 8){  //9~16
      PORTF = 127;
      while ( !( UCSR1A & (1<<UDRE1)) );  //If UDREn is one, the buffer is empty
      UDR1 = Servo_ID[i-8] + 128;
      while ( !( UCSR1A & (1<<UDRE1)) );  //If UDREn is one, the buffer is empty
      UDR1 = Now[i]>>7;
      while ( !( UCSR1A & (1<<UDRE1)) );  //If UDREn is one, the buffer is empty
      UDR1 = Now[i]&127;

    }else{
      PORTF = 191;  //0~8
      while ( !( UCSR1A & (1<<UDRE1)) );  //If UDREn is one, the buffer is empty
      UDR1 = Servo_ID[i] + 128;
      while ( !( UCSR1A & (1<<UDRE1)) );  //If UDREn is one, the buffer is empty
      UDR1 = Now[i]>>7;
      while ( !( UCSR1A & (1<<UDRE1)) );  //If UDREn is one, the buffer is empty
      UDR1 = Now[i]&127;
    }

  }

}

void USART_Transmit( unsigned char data )
{
	// PORTB=0b00001000;
	// PORTF|=(1<<PF6);
	// PORTF&= ~(1<<PF7);
  /* Wait for empty transmit buffer */

  /* Put data into buffer, sends the data */
  UDR1 = data;
  while ( !( UCSR1A & (1<<UDRE1)) )  //If UDREn is one, the buffer is empty
  ;
}

void KONDO_series(int a[17]){

  static int i = 0;
  PORTF = 191;
  for(int i = 0 ;i < 9 ;i++){
    USART_Transmit(Servo_ID[i]+128);//ID
    USART_Transmit(a[i]>>7);
    USART_Transmit(a[i]&127);
     _delay_ms(1);
  }
  _delay_ms(1);

  PORTF = 127;
  for(int i = 9 ;i < 17 ;i++){
    USART_Transmit(Servo_ID[i-8]+128);//ID
    USART_Transmit(a[i]>>7);
    USART_Transmit(a[i]&127);
    _delay_ms(1);
  }

}


int main(void)
{
  ASA_M128_set();
	// DDRA = 0xFF;
  DDRB = 0xFF;
  DDRF = 0xFF;
	// PORTA = 0x00;
  PORTF = 0x00; //輸出電壓全為0
  DDRB |= (1<<DDB7)|(1<<DDB6)|(1<<DDB5);   //洞洞板通道開啟
  PORTB |= (1<<PB6);//洞洞板通道開啟(洞洞板轉到2)
  USART_Init ( MYUBRR );
  timer2_Init();
  int a = 0 ,b = 0,F = 999 , j = 0;

  printf("-----start-----\n" );
	sei();

  printf("-----First Initial-----\n" );
  printf("Right->0 ;Left->1 : \n" );
  scanf("%d", &F);
  if(F == 0){
    PORTF = 127;
  }
   if(F == 1){
    PORTF = 191;
  }
  printf("PORTF = %d\n",PORTF );


  while(1)	//輸入角度及ID 控制
  {
      int Stand[17] = {7500,6500,5700,7500,7700,7500,5400,7000,7700
                           ,8500,9300,4800,7300,7500,9600,8000,7300};
      int Squat[17] = {7500,7000,5800,0000,8000,9900,9600,5000,8000
                           ,8000,9200,4800,7000,5100,5400,10000,7000};
      printf("Please input a num as Mode: (999:switch, 1001:squat, 1002:standing, 1003:Walking, 1004:Testing Mode, Other: Control)\n" );
      scanf("%d", &b);
      if (b == 1002) {
          printf("-----Standing-----\n" );
          PORTF = 191;
          for(int i = 0 ;i < 9 ;i++){
              USART_Transmit(Servo_ID[i]+128);//ID
              USART_Transmit(Stand[i]>>7);
              USART_Transmit(Stand[i]&127);
              _delay_ms(1);
          }
          _delay_ms(1);

          PORTF = 127;
          for(int i = 9 ;i < 17 ;i++){
              USART_Transmit(Servo_ID[i-8]+128);//ID
              USART_Transmit(Stand[i]>>7);
              USART_Transmit(Stand[i]&127);
              _delay_ms(1);
          }
        
      }

      if (b == 1001) {
          printf("-----Squat-----\n" );
          PORTF = 191;
          for(int i = 0 ;i < 9 ;i++){
              USART_Transmit(Servo_ID[i]+128);//ID
              USART_Transmit(Squat[i]>>7);
              USART_Transmit(Squat[i]&127);
              _delay_ms(1);
          }
          _delay_ms(1);

          PORTF = 127;
          for(int i = 9 ;i < 17 ;i++){
              USART_Transmit(Servo_ID[i-8]+128);//ID
              USART_Transmit(Squat[i]>>7);
              USART_Transmit(Squat[i]&127);
              _delay_ms(1);
          }
        
      }

      if (b == 1003) {
          printf("-----Walking-----\n" );
          int tmp1[17] = {7500,6500,5700,7500,8000,7500,5400,7000,7300
                              ,8500,9300,4800,7900,7500,9600,8000,7300};
          int tmp2[17] = {7500,6500,5700,7500,8000,7500,5400,7000,7300
                              ,8500,9300,4800,7900,6700,9600,7500,7300};
          int tmp3[17] = {7500,6500,5700,7500,7500,7500,5400,7000,7300
                              ,8500,9300,4800,7500,6700,9600,7500,7300};
          int tmp4[17] = {7500,6500,5700,7500,7500,7500,5400,6900,7500
                              ,8500,9300,4800,7500,6700,9600,7500,7300};
          int tmp5[17] = {7500,6500,5700,7500,7500,7500,5400,6700,7500
                              ,8500,9300,4800,7200,6700,9600,7600,7300};
          int tmp6[17] = {7500,6500,5700,7500,7500,7500,5400,6700,7500
                              ,8500,9300,4800,7200,7000,9600,7600,7300};
          while(1){
              _delay_ms(800);
              KONDO_series(Stand);
              _delay_ms(800);
              KONDO_series(tmp1);
              _delay_ms(800);
              KONDO_series(tmp2);
              _delay_ms(800);
              KONDO_series(tmp3);
              _delay_ms(800);
              KONDO_series(tmp4);
              _delay_ms(800);
              KONDO_series(tmp5);
              _delay_ms(800);
              KONDO_series(tmp6);
              
          }
        
      }
      if(b == 1004){
         while (1) {

            for(int i = 0;i < 17 ;i ++){Now[i] = 7500+j;}
            j=j+10;

            if(j > 100){j=0;}

            // long int t1 = clock;
            KONDO_transmit();
            // long int t2 = clock;
            printf("-----Testing Mode-----\n");
            _delay_ms(100);

        }
      }
      if (b == 999) {
        printf("-----Restart-----\n" );
        printf("Right->0 ;Left->1 : \n" );
        scanf("%d", &F);
        if(F == 0){
          PORTF = 127;
        }
        if(F == 1){
          PORTF = 191;
        }
        printf("PORTF = %d\n",PORTF );
      }else{

        while(b != 999){
          printf("[Control Mode]Please input a num as ID:\n" );
          scanf("%d", &b);
          if(b != 999){
            printf("----------\n");
            printf("NOW ID:%d\n", b);
            printf("Please input a num as POSITION:\n" );
            scanf("%d", &a);	//輸入0 成功鬆開轉矩 (2018.09.27)
            _delay_ms(100);

            // if(a == 0){
            // 	int n = 1;
            // 	printf("a=%d ",a );
            // 	printf("a1=%x ", a>>7);
            // 	printf("a2=%x\n", a&127);
            //
            // 	while (1) {
            // 		printf("-----%d-----\n", n);
            // 		USART_Transmit(b+128);//ID
            // 		USART_Transmit(a>>7);
            // 		USART_Transmit(a&127);
            //
            // 		if (i<=2) {
            // 			printf("**i = %d get = %3d, %3x, %3x\n",i,get[0]-128,get[1],get[2] );   //等待get結束
            // 		}
            // 		// printf("get = %3x, %3x, %3x\n",get[0],get[1],get[2] );   //印出收到的值
            // 		if(i>2) i=0;
            // 		_delay_ms(100);
            // 		// USART_Flush();
            // 		n++;
            // 	}
            // }

            printf("a=%d ",a );
            printf("a1=%x ", a>>7);
            printf("a2=%x\n", a&127);

            USART_Transmit(b+128);//ID
            USART_Transmit(a>>7);
            USART_Transmit(a&127);
          }
        }
      }

  		a=0;
  }

    return 0;
}

ISR(USART1_RX_vect) {
	// PORTA&= ~(1<<PA3);
	// PORTA|=(1<<PA4);
	get[i]=UDR1;
	i++;
}

ISR(TIMER2_COMP_vect){
  // clock++;
}

ISR(USART0_RX_vect) {
	;
}

ISR(USART1_TX_vect) {
	// PORTF&= ~(1<<PF6);
	// PORTF|=(1<<PF7);
}
