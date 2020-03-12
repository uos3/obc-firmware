#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "packet_base.h"
#include "packet_application.h"
#include "buffer.h"
#include "../utility/byte_plexing.h"


app_info_t app_info_fromfields(uint8_t whofor, bool is_unfinished, bool is_ret, bool is_confirm, bool is_now){
	app_info_t app_info;
	app_info.whofor = (whofor & 0x0F);		 // mask with 11110000 and shift right 4 times
	app_info.unfinished = is_unfinished;
	app_info.ret = is_ret;
	app_info.confirm = is_confirm;
	app_info.now = is_now;
	return app_info;
}

app_header_t app_header_fromfields(APPLICATION_LEN_TYPE data_length, uint8_t whofor, bool is_unfinished, bool is_ret, bool is_confirm, bool is_now){
	app_info_t info;
	app_header_t header;
	APPLICATION_LEN_TYPE correct_endian_length;
	correct_endian_length = data_length;
	info = app_info_fromfields(whofor, is_unfinished, is_ret, is_confirm, is_now);

	#if little_endian
		flip_endian(correct_endian_length, APPLICATION_LEN_LEN);
	#endif

	header.as_struct.info = info;
	header.as_struct.length = correct_endian_length;
	return header;
}

app_header_t app_header_frombytes(uint8_t* data){
	app_header_t header;
	
	memcpy(header.as_bytes, data, APPLICATION_HEADER_LEN);

	#if little_endian
		flip_endian(header.as_struct.length, APPLICATION_LEN_LEN);
	#endif

	return header;
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
	if (data_len > (BLOCK_SIZE - BUFFER_DATA_START_INDEX)){
		return true;
	}
	return false;
}
