#ifndef __ADC_H__
#define __ADC_H__

// STM32外设库头文件
#include "stm32f10x.h" 
#include <stdio.h>
#include <stdarg.h>

void AD_Init(void);

uint16_t AD_GetValue(uint8_t ADC_Channel);


#endif
