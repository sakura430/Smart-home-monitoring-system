#ifndef __TCP_SEND_DATA
#define __TCP_SEND_DATA
#include "esp01s_switch.h"

int tcp_send_data(char *name,uint16_t data,char* timestamp);
typedef enum{
	NORMORL,
	TIMEOUT
}TimeFlag;
extern TimeFlag time_flag;
#endif


