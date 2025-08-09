#ifndef __USART1_H
#define __USART1_H
#include "stm32f10x.h" 
#include "freertos.h"
#include "task.h"
#include "queue.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
extern QueueHandle_t queueUTools;

void PrintToUTools(char  *format,...);
void USART1_NVIC_Init(void);
void USART1_IRQHandler(void);
void USART1_SendByte(uint8_t Byte);
#endif

