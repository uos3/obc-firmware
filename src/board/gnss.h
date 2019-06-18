#ifndef __GNSS_H__
#define __GNSS_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/*Intialises GNSS UART connection*/
void GNSS_Init(void);

/*Retrieves longitude, latitude, time and standard deviations */
int GNSS_getData(int32_t *longitude, int32_t *latitude, int32_t *altitude, uint8_t *long_sd, uint8_t *lat_sd, uint8_t *alt_sd,uint16_t *week_num, uint32_t *week_seconds, uint64_t *ex_time);

/*Dynamically sends command to GNSS, not implemented, may be continued after further discussion
bool GNSS_sendCommand(char GNSS_Command, char *GNSS_reply);*/

/*Appends characters received through UART to received string*/
void append(char *append_str, char append_c);

void adjust_decimal(uint32_t factor, char *to_adjust);

void compress(uint8_t *s_d, char s_d_str[]);

unsigned long CalculateBlockCRC32(unsigned long ulCount, /* Number of bytes in the data block */unsigned char *ucBuffer );

unsigned long CRC32Value(int i);

#endif /* __GNSS_H__ */
