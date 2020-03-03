#ifndef PACKET_APPLICATION_H
#define PACKET_APPLICATION_H

typedef struct app_info_t{
	int whofor: 4;
	int unfinished: 1;
	int ret: 1;
	int confirm: 1;
	int now: 1;
} app_info_t;

#endif // packet application h
