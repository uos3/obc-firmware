#include <stdint.h>

#include "packet_base.h"
#include "packet_application.h"
#include "buffer.h"

uint8_t app_info_asbyte(app_info_t app_info){
	uint8_t byte;
	byte = 0;
	byte |= ((uint8_t) app_info.whofor & 0x0F) << 4;
	byte |= ((uint8_t) app_info.unfinished & 0x01) << 3;
	byte |= ((uint8_t) app_info.ret &0x01) << 2;
	byte |= ((uint8_t) app_info.confirm &0x01) << 1;
	byte |= ((uint8_t) app_info.now &0x01);
	return byte;
}

app_info_t app_info_frombyte(uint8_t byte){
	app_info_t app_info;
	app_info.whofor 			= ((byte & 0xF0) >> 4); // mask with 11110000 and shift right 4 times
	app_info.unfinished 	= ((byte & 0x08) >> 3); // 00001000
	app_info.ret 		= ((byte & 0x04) >> 2); // 00000100
	app_info.confirm 		= ((byte & 0x02) >> 1); // 00000010
	app_info.now 		=  (byte & 0x01);		// 00000001
	return app_info;
}

app_info_t app_info_fromfields(uint8_t whofor, uint8_t is_unfinished, uint8_t is_ret, uint8_t is_confirm, uint8_t is_now){
	app_info_t app_info;
	app_info.whofor = (whofor & 0x0F);		 // mask with 11110000 and shift right 4 times
	app_info.unfinished = ((is_unfinished & 0x08) >> 3); // 00001000
	app_info.ret = ((is_ret & 0x04) >> 2);   // 00000100
	app_info.confirm = ((is_confirm & 0x02) >> 1);	// 00000010
	app_info.now = (is_now & 0x01); // 00000001
	return app_info;
}


app_header_t app_header_constructor(){
	app_header_t app_header;
	app_header.length_start = &app_header.buffer;
	app_header.info_start = &app_header.length_start + APPLICATION_LEN_LEN;
	return app_header;
}
