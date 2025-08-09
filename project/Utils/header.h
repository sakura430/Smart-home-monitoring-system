#ifndef __HEADER_H
#define __HEADER_H

#include "USART1.h"
#include "USART2.h"
#include "USART3.h"
#include "GPIO.h"
#include "wifi_connect.h"
#include "tcp_connect.h"
#include "esp01s_switch.h"
#include "tcp_send_data.h"
#include "Delay.h"
#include "OLED.h"
#include "LED.h"
#include "ADC.h"
#include "buzzer.h"
#include "flash.h"
#include "spi.h"
#include "IWDG.h"
#include "RTC.h"
#include "Key.h"
#define LDR_MAX 3000
#define IR_MAX 3000
QueueHandle_t queueUTools, queueESP01s, queueHC05;
QueueSetHandle_t QueueSet;
QueueHandle_t queueTaskStatus;
int network_available = 0;
int local_storage_mode = 0;

LEDMode g_ledModes[3] = {LED_OFF, LED_OFF, LED_OFF};
BuzzerStatus g_buzzerStatus = OFF;
//TimeFlag time_flag = NORMORL;

//int wifi_flag=0;
TaskHandle_t handle1,handle2;
#endif


