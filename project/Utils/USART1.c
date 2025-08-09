#include "USART1.h"


void USART1_NVIC_Init() {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    USART_InitTypeDef USART1_InitStruct;
    USART1_InitStruct.USART_BaudRate = 115200;
    USART1_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART1_InitStruct.USART_Parity = USART_Parity_No;
    USART1_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART1_InitStruct.USART_StopBits = USART_StopBits_1;
    USART1_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART1, &USART1_InitStruct);
    USART_Cmd(USART1, ENABLE);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);   //优先级分组
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 12;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_InitStruct);
}

void USART1_SendByte(uint8_t Byte) {
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
        ;

    USART_SendData(USART1, Byte);

    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
}
void PrintToUTools(char  *format, ...) {
    taskENTER_CRITICAL();
    char strs[100] = {0};
    va_list list;
    va_start(list, format);
    vsprintf(strs, format, list);
    va_end(list);

    for (uint8_t i = 0; strs[i] != '\0'; i++) {
        USART1_SendByte(strs[i]);
    }

    taskEXIT_CRITICAL();
}

void USART1_IRQHandler(void) {//串口工具发送数据到stm32产生中断
    if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET) {
        char Data = USART_ReceiveData(USART1);
		xQueueSendFromISR(queueUTools, &Data,NULL);
    }
}
