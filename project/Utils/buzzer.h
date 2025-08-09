#ifndef __BUZZER_H
#define __BUZZER_H
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

typedef enum{
	OFF,
	ON
}BuzzerStatus;
extern BuzzerStatus g_buzzerStatus;
void Buzzer_Init(void);
void Buzzer_Control(BuzzerStatus buzzer_status);

#endif
