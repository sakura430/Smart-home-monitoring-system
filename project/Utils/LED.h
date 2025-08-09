#ifndef __LED_H
#define __LED_H
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"


typedef enum{
	RED,
	GREEN,
	BLUE
}LEDColor;
typedef enum {
    LED_ON,
	LED_OFF,
    LED_FAST_BLINK,
    LED_SLOW_BLINK
} LEDMode;
// 全局变量存储每个 LED 的当前模式
extern LEDMode g_ledModes[3];
void LED_Init(void);
void TIM_Init(void);
void TIM2_IRQHandler(void);
void LED_Control(LEDColor led_color,LEDMode led_mode);
#endif



