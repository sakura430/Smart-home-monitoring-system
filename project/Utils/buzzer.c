#include "buzzer.h"

void Buzzer_Init(void){
	GPIO_InitTypeDef GPIO_InitStructure;
    // 使能 GPIOC 时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);   
    // 配置 LED 引脚为推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);  
    // 初始关闭蜂鸣器
    GPIO_SetBits(GPIOB, GPIO_Pin_9);
}


void Buzzer_Control(BuzzerStatus buzzer_status){
		if(buzzer_status == ON){
			GPIO_ResetBits(GPIOB, GPIO_Pin_9);			
		}else{
			GPIO_SetBits(GPIOB, GPIO_Pin_9);	
		}
}


