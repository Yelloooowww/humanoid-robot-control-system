//Author    : Li Naicheng
//Last Edit : Hsin 2017/08/10
//ASA Lib Gen2

#include "ASA_Lib.h"

//Internal Definitions

//Internal Function Prototypes

//Global Variables

//Functions

char M128_DIO_set(char LSByte, char Mask, char Shift, char Data)
{
	char ACK;
	if(Shift<=7)
	{
		switch(LSByte)
		{
			case 200:
				if((DDRA&Mask)!=((Data<<Shift)&Mask))ACK=2;
				else ACK=0;
				DDRA=(DDRA&(~Mask))|((Data<<Shift)&Mask);
				break;
			case 201:
				if((DDRB&Mask)!=((Data<<Shift)&Mask))ACK=2;
				else ACK=0;
				DDRB=(DDRB&(~Mask))|((Data<<Shift)&Mask);
				break;
			case 202:
				if((DDRC&Mask)!=((Data<<Shift)&Mask))ACK=2;
				else ACK=0;
				DDRC=(DDRC&(~Mask))|((Data<<Shift)&Mask);
				break;
			case 203:
				if((DDRD&Mask)!=((Data<<Shift)&Mask))ACK=2;
				else ACK=0;
				DDRD=(DDRD&(~Mask))|((Data<<Shift)&Mask);
				break;
			case 204:
				if((DDRE&Mask)!=((Data<<Shift)&Mask))ACK=2;
				else ACK=0;
				DDRE=(DDRE&(~Mask))|((Data<<Shift)&Mask);
				break;
			case 205:
				if((DDRF&Mask)!=((Data<<Shift)&Mask))ACK=2;
				else ACK=0;
				DDRF=(DDRF&(~Mask))|((Data<<Shift)&Mask);
				break;
			default:
				return 1;
		}
	}
	else return 1;
	return ACK;
}

char M128_DIO_put(char LSByte, char Bytes, void *Data_p)
{
	char ACK;
	if(Bytes==1)
	{
		switch(LSByte)
		{
			case 0:
				if(DDRA!=0xFF)return 2;
				PORTA=*(char*)Data_p;
				ACK=0;
				break;
			case 1:
				if(DDRB!=0xFF)return 2;
				PORTB=*(char*)Data_p;
				ACK=0;
				break;
			case 2:
				if(DDRC!=0xFF)return 2;
				PORTC=*(char*)Data_p;
				ACK=0;
				break;
			case 3:
				if(DDRD!=0xFF)return 2;
				PORTD=*(char*)Data_p;
				ACK=0;
				break;
			case 4:
				if(DDRE!=0xFF)return 2;
				PORTE=*(char*)Data_p;
				ACK=0;
				break;
			case 5:
				if(DDRF!=0xFF)return 2;
				PORTF=*(char*)Data_p;
				ACK=0;
				break;
			default:
				return 1;
		}
	}
	else return 1;
	return ACK;
}

char M128_DIO_get(char LSByte, char Bytes, void *Data_p)
{
	char ACK;
	if(Bytes==1)
	{
		switch(LSByte)
		{
			case 100:
				if(DDRA!=0x00)ACK=2;
				else ACK=0;
				*(char*)Data_p=PINA;
				break;
			case 101:
				if(DDRB!=0x00)ACK=2;
				else ACK=0;
				*(char*)Data_p=PINB;
				break;
			case 102:
				if(DDRC!=0x00)ACK=2;
				else ACK=0;
				*(char*)Data_p=PINC;
				break;
			case 103:
				if(DDRD!=0x00)ACK=2;
				else ACK=0;
				*(char*)Data_p=PIND;
				break;
			case 104:
				if(DDRE!=0x00)ACK=2;
				else ACK=0;
				*(char*)Data_p=PINE;
				break;
			case 105:
				if(DDRF!=0x00)ACK=2;
				else ACK=0;
				*(char*)Data_p=PINF;
				break;
			default:
				return 1;
		}
	}
	else return 1;
	return ACK;
}

char M128_DIO_fpt(char LSByte, char Mask, char Shift, char Data)
{
	char ACK;
	if(Shift<=7)
	{
		switch(LSByte)
		{
			case 0:
				if((DDRA&Mask)!=Mask)return 2;
				PORTA=(PORTA&(~Mask))|((Data<<Shift)&Mask);
				ACK=0;
				break;
			case 1:
				if((DDRB&Mask)!=Mask)return 2;
				PORTB=(PORTB&(~Mask))|((Data<<Shift)&Mask);
				ACK=0;
				break;
			case 2:
				if((DDRC&Mask)!=Mask)return 2;
				PORTC=(PORTC&(~Mask))|((Data<<Shift)&Mask);
				ACK=0;
				break;
			case 3:
				if((DDRD&Mask)!=Mask)return 2;
				PORTD=(PORTD&(~Mask))|((Data<<Shift)&Mask);
				ACK=0;
				break;
			case 4:
				if((DDRE&Mask)!=Mask)return 2;
				PORTE=(PORTE&(~Mask))|((Data<<Shift)&Mask);
				ACK=0;
				break;
			case 5:
				if((DDRF&Mask)!=Mask)return 2;
				PORTF=(PORTF&(~Mask))|((Data<<Shift)&Mask);
				ACK=0;
				break;
			default:
				return 1;
		}
	}
	else return 1;
	return ACK;
}

char M128_DIO_fgt(char LSByte, char Mask, char Shift, void *Data_p)
{
	char ACK;
	if(Shift<=7)
	{
		switch(LSByte)
		{
			case 100:
				if((DDRA&Mask)!=0)ACK=2;
				else ACK=0;
				*(char*)Data_p=(PINA&Mask)>>Shift;
				break;
			case 101:
				if((DDRB&Mask)!=0)ACK=2;
				else ACK=0;
				*(char*)Data_p=(PINB&Mask)>>Shift;
				break;
			case 102:
				if((DDRC&Mask)!=0)ACK=2;
				else ACK=0;
				*(char*)Data_p=(PINC&Mask)>>Shift;
				break;
			case 103:
				if((DDRD&Mask)!=0)ACK=2;
				else ACK=0;
				*(char*)Data_p=(PIND&Mask)>>Shift;
				break;
			case 104:
				if((DDRE&Mask)!=0)ACK=2;
				else ACK=0;
				*(char*)Data_p=(PINE&Mask)>>Shift;
				break;
			case 105:
				if((DDRF&Mask)!=0)ACK=2;
				else ACK=0;
				*(char*)Data_p=(PINF&Mask)>>Shift;
				break;
			default:
				return 1;
		}
	}
	else return 1;
	return ACK;
}
