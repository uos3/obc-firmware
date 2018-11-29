#ifndef __GNSS_H__
#define __GNSS_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void GNSS_Init(void);

bool GNSS_getData(char *received_Message);

bool GNSS_sendCommand(char GNSS_Command, char *GNSS_reply);

void append(char *append_str, char append_c);

unsigned long CalculateBlockCRC32(unsigned long ulCount, unsigned char *ucBuffer);


#endif /* __GNSS_H__ */