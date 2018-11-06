/*===============ASA Series Library===============*/

/*****ReleaseDate*****/
//----/--/--

/*****Feature*****/

/*===Supported Module===*/
//ASA M128

/*================================================*/

#ifndef __ASA_H__
#define __ASA_H__

#include <avr\io.h>
#include <avr\interrupt.h>
#include <stdio.h>
#include <util\delay.h>

/**M128**/

char ASA_M128_set(void);

char M128_AComp_set(char LSByte, char Mask, char Shift, char Data);
char M128_AComp_fpt(char LSByte, char Mask, char Shift, char Data);
char M128_AComp_fgt(char LSByte, char Mask, char Shift, void *Data_p);
char M128_Acomp_isr(char Number, void (*function)(void));

char M128_DIO_set(char LSByte, char Mask, char Shift, char Data);
char M128_DIO_put(char LSByte, char Bytes, void *Data_p);
char M128_DIO_get(char LSByte, char Bytes, void *Data_p);
char M128_DIO_fpt(char LSByte, char Mask, char Shift, char Data);
char M128_DIO_fgt(char LSByte, char Mask, char Shift, void *Data_p);

char M128_EXT_set(char LSByte, char Mask, char Shift, char Data);
char M128_EXT_fpt(char LSByte, char Mask, char Shift, char Data);
char M128_EXT_fgt(char LSByte, char Mask, char Shift, void *Data_p);
char M128_EXT_isr(char Number, void (*function)(void));

char M128_TIM_set(char Lsbyte, char Mask, char Shift, char Data);
char M128_TIM_put(char LSbyte, char Bytes, void *Data_p);
char M128_TIM_get(char LSByte, char Bytes, void *Data_p);
char M128_TIM_fpt(char Lsbyte, char Mask, char Shift, char Data);
char M128_TIM_isr(char Number,  void (*function)(void));
char M128_TIM_isr_remove(char Number,  void (*function)(void));

/**SDC00**/
char ASA_SDC00_set(char ASA_ID, char LSByte, char Mask, char shift, char Data);
char ASA_SDC00_put(char ASA_ID, char LSByte, char Bytes, void *Data_p);
char ASA_SDC00_get(char ASA_ID, char LSByte, char Bytes, void *Data_p);

#endif
