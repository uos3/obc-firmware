#ifndef __GNSS_H__
#define __GNSS_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/*Intialises GNSS UART connection*/
void GNSS_Init(void);

/*Retrieves longitude, latitude, time and standard deviations */
bool GNSS_getData(int32_t *longitude, int32_t *latitude, int32_t *altitude, uint32_t *long_sd, uint32_t *lat_sd, uint32_t *alt_sd);

/*Dynamically sends command to GNSS, not implemented, may be continued after further discussion*/
bool GNSS_sendCommand(char GNSS_Command, char *GNSS_reply);

/*Appends characters received through UART to received string*/
void append(char *append_str, char append_c);

void adjust_decimal(uint32_t factor, char *to_adjust);

/*Calculates checksum for implementation of GNSS_sendCommand*/
unsigned long CalculateBlockCRC32(unsigned long ulCount, unsigned char *ucBuffer);

int32_t FloatToInt(float in);
int8_t FloatToInt_8(float in);


#endif /* __GNSS_H__ */