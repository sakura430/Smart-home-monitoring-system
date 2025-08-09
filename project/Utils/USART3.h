#ifndef __USART3_H
#define __USART3_H
#include "stm32f10x.h" 
#include "freertos.h"
#include "task.h"
#include "queue.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
extern QueueHandle_t queueHC05;

void PrintToHc05(char  *format,...);
void USART3_NVIC_Init(void);
void USART3_IRQHandler(void);
void USART3_SendByte(uint8_t Byte);
#endif

