//Author    : WANGLIANG
//Last Edit : Hsin 2017/08/10
//ASA Lib Gen2

#include "ASA_Lib.h"

//Internal Definitions

#define initial {{NULL},{NULL},{NULL},{NULL},NULL,NULL}
#define TIM_number_max 10

typedef void (*TIM_function)(void);
typedef struct
{
	TIM_function T_C0_CTC[TIM_number_max];
	TIM_function T_C1_CTC[TIM_number_max];
	TIM_function T_C2_CTC[TIM_number_max];
	TIM_function T_C3_CTC[TIM_number_max];
	TIM_function T_C1_CAPT;
	TIM_function T_C3_CAPT;
}T_CX_int;


//Internal Function Prototypes

//Global Variables

T_CX_int m128_timer_isr =  initial;

//Functions

char M128_TIM_set(char Lsbyte, char Mask, char Shift, char Data)
{

	if(Lsbyte<20 || Lsbyte>226)return 1;
	if(Shift>7 || Shift<0)return 1;

	switch(Lsbyte)
	{
		case 200:DDRA = (DDRA & (~Mask)) | ((Data<<Shift) & Mask);break;
		case 201:DDRB = (DDRB & (~Mask)) | ((Data<<Shift) & Mask);break;
		case 202:DDRC = (DDRC & (~Mask)) | ((Data<<Shift) & Mask);break;
		case 203:DDRD = (DDRD & (~Mask)) | ((Data<<Shift) & Mask);break;
		case 204:DDRE = (DDRE & (~Mask)) | ((Data<<Shift) & Mask);break;
		case 205:DDRF = (DDRF & (~Mask)) | ((Data<<Shift) & Mask);break;

		case 207:TIMSK = (TIMSK & (~Mask)) | ((Data<<Shift) & Mask);break;
		case 208:TIFR = (TIFR & (~Mask)) | ((Data<<Shift) & Mask);break;
		case 209:ETIMSK = (ETIMSK & (~Mask)) | ((Data<<Shift) & Mask);break;
		case 210:ETIFR = (ETIFR & (~Mask)) | ((Data<<Shift) & Mask);break;
		case 211:SFIOR = (SFIOR & (~Mask)) | ((Data<<Shift) & Mask);break;

		//timer0
		case 214:ASSR = (ASSR & (~Mask)) | ((Data<<Shift) & Mask);break;
		case 215:TCCR0 = (TCCR0 & (~Mask)) | ((Data<<Shift) & Mask);break;

		//timer1
		case 217:TCCR1A = (TCCR1A & (~Mask)) | ((Data<<Shift) & Mask);break;
		case 218:TCCR1B = (TCCR1B & (~Mask)) | ((Data<<Shift) & Mask);break;
		case 219:TCCR1C = (TCCR1C & (~Mask)) | ((Data<<Shift) & Mask);break;
		case 220:ACSR = (ACSR & (~Mask)) | ((Data<<Shift) & Mask);break;

		//timer2
		case 222:TCCR2 = (TCCR2 & (~Mask)) | ((Data<<Shift) & Mask);break;

		//timer3
		case 224:TCCR3A = (TCCR3A & (~Mask)) | ((Data<<Shift) & Mask);break;
		case 225:TCCR3B = (TCCR3B & (~Mask)) | ((Data<<Shift) & Mask);break;
		case 226:TCCR3C = (TCCR3C & (~Mask)) | ((Data<<Shift) & Mask);break;

	}


	return 0;
}

char M128_TIM_get(char Lsbyte, char Bytes, void *Data_p)
{
	//code wait to test
	if(Lsbyte<100 || Lsbyte>134)return 1;
	if(Bytes!=1 && Bytes!=2)return 2;

	unsigned char data_char = 0;
	unsigned int data_int = 0;

	switch(Lsbyte)
	{
		//timer0
		case 20:data_char = TCNT0;break;

		//timer1
		case 100:data_int = ICR1;break;

		//timer2
		case 26:data_char = TCNT2;break;

		//timer3
		case 104:data_int = ICR3;break;

	}

	if(Bytes == 1)
	{
		*((unsigned char*)Data_p) = data_char;
	}
	else
	{
		*((unsigned int*)Data_p) = data_int;
	}


	return 0;

}

char M128_TIM_put(char Lsbyte, char Bytes, void *Data_p)
{
	//if(Lsbyte<0 || Lsbyte>34)return 1;

	unsigned char data_char = 0;
	unsigned int data_int = 0;

	if(Bytes == 1)
	{
		data_char = *((unsigned char*)Data_p);
	}
	else if(Bytes == 2)
	{
		data_int = *((unsigned int*)Data_p);
	}
	else
	{
		return 1;
	}

	switch(Lsbyte)
	{
		//timer0
		case 0:OCR0 = data_char;break;
		case 20:TCNT0 = data_char;break;

		//timer1
		case 2:OCR1A = data_int;break;
		case 22:TCNT1 = data_int;break;

		//timer2
		case 10:OCR2 = data_char;break;
		case 26:TCNT2 = data_char;break;

		//timer3
		case 12:OCR3A = data_int;break;
		case 28:TCNT3 = data_int;break;
	}

	return 0;
}

char M128_TIM_fpt(char Lsbyte, char Mask, char Shift, char Data)
{
	if(Lsbyte!=215 ||  Lsbyte!=218 || Lsbyte!=222 || Lsbyte!=225)return 1;
	if(Shift>7 || Shift<0)return 1;

	switch(Lsbyte)
	{
		case 215:TCCR0 = (TCCR0 & (~Mask)) | ((Data<<Shift) & Mask);break;	//TCCR0
		case 218:TCCR1B = (TCCR1B & (~Mask)) | ((Data<<Shift) & Mask);break;	//TCCR1B
		case 222:TCCR2 = (TCCR2 & (~Mask)) | ((Data<<Shift) & Mask);break;	//TCCR2
		case 225:TCCR3B = (TCCR3B & (~Mask)) | ((Data<<Shift) & Mask);break;	//TCC3B
	}

	return 0;
}

char M128_TIM_isr(char Number,  void (*function)(void))
{
	if(Number<0 || Number>5)return 1;

	switch(Number)
	{
		case 0:
				for(int i=0;i<TIM_number_max;i++)
				{
					if(m128_timer_isr.T_C0_CTC[i] == NULL)
					{
						m128_timer_isr.T_C0_CTC[i] = function;
						break;
					}
					else
					{
						if(i == TIM_number_max-1)return 2;
					}

				}
				break;
		case 1:
				for(int i=0;i<TIM_number_max;i++)
				{
					if(m128_timer_isr.T_C1_CTC[i] == NULL)
					{
						m128_timer_isr.T_C1_CTC[i] = function;
						break;
					}
					else
					{
						if(i == TIM_number_max-1)return 2;
					}
				}
				break;
		case 2:
				for(int i=0;i<TIM_number_max;i++)
				{
					if(m128_timer_isr.T_C2_CTC[i] == NULL)
					{
						m128_timer_isr.T_C2_CTC[i] = function;
						break;
					}
					else
					{
						if(i == TIM_number_max-1)return 2;
					}
				}
				break;
		case 3:
				for(int i=0;i<TIM_number_max;i++)
				{
					if(m128_timer_isr.T_C3_CTC[i] == NULL)
					{
						m128_timer_isr.T_C3_CTC[i] = function;
						break;
					}
					else
					{
						if(i == TIM_number_max-1)return 2;
					}
				}
				break;
		case 4:
				if(m128_timer_isr.T_C1_CAPT == NULL)
				{
					m128_timer_isr.T_C1_CAPT = function;
				}
				else
				{
					return 2;
				}
				break;
		case 5:
				if(m128_timer_isr.T_C3_CAPT == NULL)
				{
					m128_timer_isr.T_C3_CAPT = function;
				}
				else
				{
					return 2;
				}
				break;
	}
	return 0;
}

char M128_TIM_isr_remove(char Number,  void (*function)(void))
{
	if(Number<0 || Number>5)return 1;

	switch(Number)
	{
		case 0:
				for(int i=0;i<TIM_number_max;i++)
				{
					if(m128_timer_isr.T_C0_CTC[i] == function)
					{
						m128_timer_isr.T_C0_CTC[i] = NULL;
						break;
					}
				}
				break;
		case 1:
				for(int i=0;i<TIM_number_max;i++)
				{
					if(m128_timer_isr.T_C1_CTC[i] == function)
					{
						m128_timer_isr.T_C1_CTC[i] = NULL;
						break;
					}
				}
				break;
		case 2:
				for(int i=0;i<TIM_number_max;i++)
				{
					if(m128_timer_isr.T_C2_CTC[i] == function)
					{
						m128_timer_isr.T_C2_CTC[i] = NULL;
						break;
					}
				}
				break;
		case 3:
				for(int i=0;i<TIM_number_max;i++)
				{
					if(m128_timer_isr.T_C3_CTC[i] == function)
					{
						m128_timer_isr.T_C3_CTC[i] = NULL;
						break;
					}
				}
				break;
	}


	switch(Number)	//re-arrangement
	{
		case 0:
				for(int i = 0; i < TIM_number_max; ++i)
				{
					if(m128_timer_isr.T_C0_CTC[i] == NULL)
					{
						for(int j = i+1; i < TIM_number_max; ++j)
						{
							if(m128_timer_isr.T_C0_CTC[j] != NULL)
							{
								m128_timer_isr.T_C0_CTC[i] = m128_timer_isr.T_C0_CTC[j];
								m128_timer_isr.T_C0_CTC[j] = NULL;
								break;
							}
						}
					}
				}
				break;
		case 1:
				for(int i = 0; i < TIM_number_max; ++i)
				{
					if(m128_timer_isr.T_C1_CTC[i] == NULL)
					{
						for(int j = i+1; i < TIM_number_max; ++j)
						{
							if(m128_timer_isr.T_C1_CTC[j] != NULL)
							{
								m128_timer_isr.T_C1_CTC[i] = m128_timer_isr.T_C1_CTC[j];
								m128_timer_isr.T_C1_CTC[j] = NULL;
								break;
							}
						}
					}
				}
				break;
		case 2:
				for(int i = 0; i < TIM_number_max; ++i)
				{
					if(m128_timer_isr.T_C2_CTC[i] == NULL)
					{
						for(int j = i+1; i < TIM_number_max; ++j)
						{
							if(m128_timer_isr.T_C2_CTC[j] != NULL)
							{
								m128_timer_isr.T_C2_CTC[i] = m128_timer_isr.T_C2_CTC[j];
								m128_timer_isr.T_C2_CTC[j] = NULL;
								break;
							}
						}
					}
				}
				break;
		case 3:
				for(int i = 0; i < TIM_number_max; ++i)
				{
					if(m128_timer_isr.T_C3_CTC[i] == NULL)
					{
						for(int j = i+1; i < TIM_number_max; ++j)
						{
							if(m128_timer_isr.T_C3_CTC[j] != NULL)
							{
								m128_timer_isr.T_C3_CTC[i] = m128_timer_isr.T_C3_CTC[j];
								m128_timer_isr.T_C3_CTC[j] = NULL;
								break;
							}
						}
					}
				}
				break;
	}

	return 0;
}

ISR(TIMER0_COMP_vect)
{
	for(int i=0;i<9;i++)
	{
		if(m128_timer_isr.T_C0_CTC[i] != NULL)
		{
			m128_timer_isr.T_C0_CTC[i]();
		}
		else
		{
			break;
		}
	}
}

ISR(TIMER1_COMPA_vect)
{
	for(int i=0;i<9;i++)
	{
		if(m128_timer_isr.T_C1_CTC[i] != NULL)
		{
			m128_timer_isr.T_C1_CTC[i]();
		}
		else
		{
			break;
		}
	}
}

ISR(TIMER2_COMP_vect)
{
	for(int i=0;i<9;i++)
	{
		if(m128_timer_isr.T_C2_CTC[i] != NULL)
		{
			m128_timer_isr.T_C2_CTC[i]();
		}
		else
		{
			break;
		}
	}
}

ISR(TIMER3_COMPA_vect)
{
	for(int i=0;i<9;i++)
	{
		if(m128_timer_isr.T_C3_CTC[i] != NULL)
		{
			m128_timer_isr.T_C3_CTC[i]();
		}
		else
		{
			break;
		}
	}
}

ISR(TIMER1_CAPT_vect)
{
	/*
	unsigned int k = 0;
	M128_TIM_get(100, 2, &k);
	printWord(k);
	printString("\r\n");
	*/
}

ISR(TIMER3_CAPT_vect)
{
	/*
	unsigned int k = 0;
	M128_TIM_get(104, 2, &k);
	printWord(k);
	printString("\r\n");
	*/
}
