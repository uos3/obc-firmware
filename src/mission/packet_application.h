#ifndef PACKET_APPLICATION_H
#define PACKET_APPLICATION_H

#include <stdbool.h>
#include <stdint.h>
// defined in TMTC. 4.6 has a 2 byte length, changed to 4 
#define APPLICATION_LEN_TYPE uint32_t
#define APPLICATION_LEN_LEN sizeof(APPLICATION_LEN_TYPE)
#define APPLICATION_INFO_LEN 1
#define APPLICATION_HEADER_LEN APPLICATION_LEN_LEN+APPLICATION_INFO_LEN // 


// pack 1 sets the correct length for structs, no more 4+1 = 8
#pragma pack(1)
// to conform with the TMTC byte order...
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
typedef struct app_info_t{
	unsigned char whofor: 4;
	unsigned char unfinished: 1;
	unsigned char ret: 1;
	unsigned char confirm: 1;
	unsigned char now: 1;
} app_info_t ;
#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
typedef struct app_info_t{
	unsigned char now: 1;
	unsigned char confirm: 1;
	unsigned char ret: 1;
	unsigned char unfinished: 1;
	unsigned char whofor: 4;
} app_info_t ;
#endif

// application layer header struct primitive, not to be used directly
#pragma pack(1)
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
typedef struct app_header_struct_t{
	APPLICATION_LEN_TYPE length;
	app_info_t info;
}app_header_struct_t;
#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
typedef struct app_header_struct_t{
	app_info_t info;
	APPLICATION_LEN_TYPE length;
}app_header_struct_t;
#endif

// application layer header
typedef union app_header_t{
	app_header_struct_t as_struct;
	uint8_t as_bytes[APPLICATION_HEADER_LEN];
} app_header_t;

// constructs an application layer information struct from fields 
app_info_t app_info_fromfields(uint8_t whofor, bool is_unfinished, bool is_ret, bool is_confirm, bool is_now);

// constructs an application header from data, and corrects endian
app_header_t app_header_frombytes(uint8_t* data);

// constructs an application header from fields, and sets endian to the standard
app_header_t app_header_fromfields(APPLICATION_LEN_TYPE data_length, uint8_t whofor, bool is_unfinished, bool is_ret, bool is_confirm, bool is_now);

// checks to see if the application 'unfinished' flag should be set
bool app_is_unfinished(uint32_t data_len);

bool app_retrieve_header(app_header_t* header, uint8_t* data_array, uint8_t data_array_length, uint8_t header_start_index);

#endif // packet application h
