#ifndef PACKET_APPLICATION_H
#define PACKET_APPLICATION_H

#define APPLICATION_INFO_LEN 1
#define APPLICATION_LEN_LEN 1
#define APPLICATION_HEADER_LEN APPLICATION_LEN_LEN+APPLICATION_INFO_LEN // 2


typedef struct app_info_t{
	int whofor: 4;
	int unfinished: 1;
	int ret: 1;
	int confirm: 1;
	int now: 1;
} app_info_t;

typedef struct app_commnad_t{
	uint8_t buffer[APPLICATION_HEADER_LEN];
	uint8_t* length_start;
	uint8_t* info_start;
}app_header_t;


app_header_t app_header_constructor();

app_info_t app_info_fromfields(uint8_t whofor, uint8_t is_unfinished, uint8_t is_ret, uint8_t is_confirm, uint8_t is_now);

app_info_t app_info_frombyte(uint8_t byte);

uint8_t app_info_asbyte(app_info_t app_info);

#endif // packet application h
