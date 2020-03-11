#include <stdint.h>
#include <string.h>

#include "packet_base.h"
#include "packet_application.h"
#include "buffer.h"
#include "../utility/byte_plexing.h"

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
	app_info.whofor 	= ((byte & 0xF0) >> 4); // mask with 11110000 and shift right 4 times
	app_info.unfinished = ((byte & 0x08) >> 3); // 00001000
	app_info.ret 		= ((byte & 0x04) >> 2); // 00000100
	app_info.confirm 	= ((byte & 0x02) >> 1); // 00000010
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

bool app_retrieve_header(app_header_t* header, uint8_t* data_array, uint8_t data_array_length, uint8_t header_start_index){
	if (header_start_index > (data_array_length-1 - sizeof(app_header_t))){
		return false;
	}
	memcpy(header->as_bytes, data_array+header_start_index, sizeof(app_header_t));
	// check endian
	#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
		flip_endian(cast_asptr(header->as_struct.length), APPLICATION_LEN_LEN);
	#endif
	return true;
}

bool app_is_unfinished(uint32_t data_len){
	if (data_len > buffer_get_free_length()){
		return true;
	}
	return false;
}
