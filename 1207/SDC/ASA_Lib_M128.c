//Author    : Hsin
//Last Edit : Hsin 2017/08/10
//ASA Lib Gen1

#include "ASA_Lib.h"
#include "ASA_Core_M128.h"

//Internal Definitions

#define DEFAULTUARTBAUD 38400

//Internal Function Prototypes

int stdio_putchar(char c, FILE *stream);
int stdio_getchar(FILE *stream);

//Global Variables

static FILE STDIO_BUFFER = FDEV_SETUP_STREAM(stdio_putchar, stdio_getchar, _FDEV_SETUP_RW);

//Functions

int stdio_putchar(char c, FILE *stream) 
{ 
    if (c == '\n') 
        stdio_putchar('\r',stream); 
		
    while((UCSR0A&(1<<UDRE0))==0); 
	
    UDR0 = c; 
	
    return 0; 
}

int stdio_getchar(FILE *stream)
{
	int UDR_Buff;
	
    while((UCSR0A&(1<<RXC0))==0);
	
	UDR_Buff = UDR0;
	
	stdio_putchar(UDR_Buff,stream);
    
	return UDR_Buff;
}

char ASA_M128_set(void)
{
	unsigned int baud;

	baud = F_CPU/16/DEFAULTUARTBAUD-1;
	UBRR0H = (unsigned char)(baud>>8);
	UBRR0L = (unsigned char)baud;
	
	UCSR0B |= (1<<RXEN0) | (1<<TXEN0);
	UCSR0C |= (3<<UCSZ00);

	ADDR_DDR |= (0x07 << ADDR0);

	stdout = &STDIO_BUFFER;
	stdin = &STDIO_BUFFER;
	
	return 0;
}