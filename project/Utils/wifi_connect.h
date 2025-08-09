#ifndef __WIFI_CONNECT_H
#define __WIFI_CONNECT_H
#include "USART1.h"
#include "USART2.h"
#include "USART3.h"
#include "GPIO.h"
#include "OLED.h"
#include "LED.h"

extern QueueHandle_t queueUTools, queueESP01s, queueHC05,Queue_Key;
extern QueueSetHandle_t QueueSet;
extern QueueHandle_t queueTaskStatus;
#define MAX_SIZE 100
int wifi_connect(void);
int quick_wifi_check(void);
extern int task_flag;
extern int wifi_flag;
extern TaskHandle_t handle1,handle2;
#endif

