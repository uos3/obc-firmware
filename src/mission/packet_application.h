#ifndef PACKET_APPLICATION_H
#define PACKET_APPLICATION_H

// defined in TMTC. 4.6 has a 2 byte length, changed to 4 
#define APPLICATION_LEN_TYPE uint32_t
#define APPLICATION_LEN_LEN sizeof(APPLICATION_LEN_TYPE)
#define APPLICATION_INFO_LEN 1
#define APPLICATION_HEADER_LEN APPLICATION_LEN_LEN+APPLICATION_INFO_LEN // 5

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
typedef struct app_header_struct_t{
	APPLICATION_LEN_TYPE length;
	app_info_t info;
}app_header_struct_t;

// application layer header
typedef union app_header_t{
	app_header_struct_t as_struct;
	uint8_t as_bytes[APPLICATION_HEADER_LEN];
} app_header_t;

app_header_t app_header_constructor();

app_info_t app_info_fromfields(uint8_t whofor, uint8_t is_unfinished, uint8_t is_ret, uint8_t is_confirm, uint8_t is_now);

app_info_t app_info_frombyte(uint8_t byte);

uint8_t app_info_asbyte(app_info_t app_info);

#endif // packet application h
