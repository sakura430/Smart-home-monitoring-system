#ifndef __USART2_H
#define __USART2_H
#include "stm32f10x.h" 
#include "freertos.h"
#include "task.h"
#include "queue.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
extern QueueHandle_t queueESP01s;

void PrintToEsp01s(char  *format,...);
void USART2_NVIC_Init(void);
void USART2_IRQHandler(void);
void USART2_SendByte(uint8_t Byte);
#endif

