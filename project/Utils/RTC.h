#ifndef _RTC_H
#define _RTC_H
#include "stm32f10x.h"
#include <time.h>
void MyRTC_Init(void);
void MyRTC_SetTime(void);
time_t MyRTC_ReadTime(void);
typedef struct{  
    int year;
	int mon; 
	int mday;
    int hour; 
    int min; 
	int sec;		
}rtc_time;
extern rtc_time rt;
#endif

