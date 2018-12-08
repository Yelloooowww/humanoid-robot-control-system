//Author    : Li Naicheng
//Last Edit : Hsin 2017/08/10
//ASA Lib Gen2

#include "ASA_Lib.h"

//Internal Definitions

//Internal Function Prototypes

//Global Variables

void (*AComp_ISR_Function[1])(void);

//Functions

char M128_AComp_set(char LSByte, char Mask, char Shift, char Data)
{
	char ACK;
	if(Shift<=7)
	{
		switch(LSByte)
		{
			case 200:
				ACSR=(ACSR&(~Mask))|((Data<<Shift)&Mask);
				ACK=0;
				break;
			case 201:
				SFIOR=(SFIOR&(~Mask))|((Data<<Shift)&Mask);
				ACK=0;
				break;
			case 202:
				if((ADMUX&Mask)!=((Data<<Shift)&Mask))ACK=3;
				else ACK=0;
				ADMUX=(ADMUX&(~Mask))|((Data<<Shift)&Mask);
				break;
			case 203:
				if((ADCSRA&Mask)!=((Data<<Shift)&Mask))ACK=3;
				else ACK=0;
				ADCSRA=(ADCSRA&(~Mask))|((Data<<Shift)&Mask);
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

char M128_AComp_fpt(char LSByte, char Mask, char Shift, char Data)
{
	char ACK;
	if(Shift<=7)
	{
		switch(LSByte)
		{
			case 200:
				ACSR=(ACSR&(~Mask))|((Data<<Shift)&Mask);
				ACK=0;
				break;
			default:
				return 1;
		}
	}
	else return 1;
	return ACK;
}

char M128_AComp_fgt(char LSByte, char Mask, char Shift, void *Data_p)
{
	char ACK;
	if(Shift<=7)
	{
		switch(LSByte)
		{
			case 200:
				*((char*)Data_p)=(ACSR&Mask)>>Shift;
				ACK=0;
				break;
			default:
				return 1;
		}
	}
	else return 1;
	return ACK;
}

char M128_Acomp_isr(char Number, void (*function)(void))
{
	char ACK;
	switch(Number)
	{
		case 0:
			AComp_ISR_Function[(int)Number]=function;
			ACK=0;
			break;
		default:
			return 1;
	}
	return ACK;
}

ISR(ANALOG_COMP_vect)
{
	AComp_ISR_Function[0]();
}
