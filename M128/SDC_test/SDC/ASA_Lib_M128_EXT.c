//Author    : Li Naicheng
//Last Edit : Hsin 2017/08/10
//ASA Lib Gen2

#include "ASA_Lib.h"

//Internal Definitions

//Internal Function Prototypes

//Global Variables

void (*EXT_ISR_Function[8])(void);

//Functions

char M128_EXT_set(char LSByte, char Mask, char Shift, char Data)
{
	char ACK;
	if(Shift<=7)
	{
		switch(LSByte)
		{
			case 200:
				EIMSK=(EIMSK&(~Mask))|((Data<<Shift)&Mask);
				ACK=0;
				break;
			case 201:
				EICRA=(EICRA&(~Mask))|((Data<<Shift)&Mask);
				ACK=0;
				break;
			case 202:
				EICRB=(EICRB&(~Mask))|((Data<<Shift)&Mask);
				ACK=0;
				break;
			case 204:
				if((DDRD&Mask)!=((Data<<Shift)&Mask))ACK=2;
				else ACK=0;
				DDRD=(DDRD&(~Mask))|((Data<<Shift)&Mask);
				break;
			case 205:
				if((DDRE&Mask)!=((Data<<Shift)&Mask))ACK=2;
				else ACK=0;
				DDRE=(DDRE&(~Mask))|((Data<<Shift)&Mask);
				break;
			default:
				return 1;
		}
	}
	else return 1;
	return ACK;
}

char M128_EXT_fpt(char LSByte, char Mask, char Shift, char Data)
{
	char ACK;
	if(Shift<=7)
	{
		switch(LSByte)
		{
			case 203:
				EIFR=((Data<<Shift)&Mask);
				ACK=0;
				break;
			default:
				return 1;
		}
	}
	else return 1;
	return ACK;
}

char M128_EXT_fgt(char LSByte, char Mask, char Shift, void *Data_p)
{
	char ACK;
	if(Shift<=7)
	{
		switch(LSByte)
		{
			case 203:
				*(char*)Data_p=(EIFR&Mask)>>Shift;
				ACK=0;
				break;
			default:
				return 1;
		}
	}
	else return 1;
	return ACK;
}

char M128_EXT_isr(char Number, void (*function)(void))
{
	char ACK;
	switch(Number)
	{
		case 0:
			EXT_ISR_Function[(int)Number]=function;
			ACK=0;
			break;
		case 1:
			EXT_ISR_Function[(int)Number]=function;
			ACK=0;
			break;
		case 2:
			EXT_ISR_Function[(int)Number]=function;
			ACK=0;
			break;
		case 3:
			EXT_ISR_Function[(int)Number]=function;
			ACK=0;
			break;
		case 4:
			EXT_ISR_Function[(int)Number]=function;
			ACK=0;
			break;
		case 5:
			EXT_ISR_Function[(int)Number]=function;
			ACK=0;
			break;
		case 6:
			EXT_ISR_Function[(int)Number]=function;
			ACK=0;
			break;
		case 7:
			EXT_ISR_Function[(int)Number]=function;
			ACK=0;
			break;
		default:
			return 1;
	}
	return ACK;
}

ISR(INT0_vect)
{
	EXT_ISR_Function[0]();
}

ISR(INT1_vect)
{
	EXT_ISR_Function[1]();
}

ISR(INT2_vect)
{
	EXT_ISR_Function[2]();
}

ISR(INT3_vect)
{
	EXT_ISR_Function[3]();
}

ISR(INT4_vect)
{
	EXT_ISR_Function[4]();
}

ISR(INT5_vect)
{
	EXT_ISR_Function[5]();
}

ISR(INT6_vect)
{
	EXT_ISR_Function[6]();
}

ISR(INT7_vect)
{
	EXT_ISR_Function[7]();
}
