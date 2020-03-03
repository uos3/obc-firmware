#include "packet_base.h"
#include "packet_auth.h"
#include "packet_transport.h"
#include "packet_application.h"
#include "buffer.h"

void packet_store_data(uint8_t whofor, uint8_t is_unfinished, uint8_t data, uint32_t data_len){
	app_header_t header;
	app_info_t info;
	header = app_header_constructor();
	info = app_info_fromfields(whofor, is_unfinished, 0, 0, 0);
	*header.info_start = app_info_asbyte(info);
}
