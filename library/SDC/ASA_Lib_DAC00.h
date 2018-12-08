#ifndef __ASA_DAC00_H__
#define __ASA_DAC00_H__

char ASA_DAC00_set(char ASA_ID, char LSByte, char Mask, char shift, char Data);
char ASA_DAC00_put(char ASA_ID, char LSByte, char Bytes, void *Data_p);
char ASA_DAC00_get(char ASA_ID, char LSByte, char Bytes, void *Data_p);

#endif
