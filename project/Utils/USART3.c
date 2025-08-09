#include "USART3.h"

void USART3_NVIC_Init() {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    USART_InitTypeDef USART3_InitStruct;
    USART3_InitStruct.USART_BaudRate = 9600;
    USART3_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART3_InitStruct.USART_Parity = USART_Parity_No;
    USART3_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART3_InitStruct.USART_StopBits = USART_StopBits_1;
    USART3_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART3, &USART3_InitStruct);
    USART_Cmd(USART3, ENABLE);

    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);   //优先级分组
    NVIC_InitTypeDef NVIC_InitStruct3;
    NVIC_InitStruct3.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStruct3.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct3.NVIC_IRQChannelPreemptionPriority = 12;
    NVIC_InitStruct3.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_InitStruct3);
}
void USART3_SendByte(uint8_t Byte) {
    while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET)
        ;

    USART_SendData(USART3, Byte);

    while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
}
void PrintToHc05(char  *format, ...) {
    taskENTER_CRITICAL();

    char strs[100] = {0};
    va_list list;
    va_start(list, format);
    vsprintf(strs, format, list);
    va_end(list);

    for (uint8_t i = 0; strs[i] != '\0'; i++) {
        USART3_SendByte(strs[i]);
    }

    taskEXIT_CRITICAL();
}
void USART3_IRQHandler(void) {//HC05发送数据到stm32产生中断
    if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET) {

        char Data = USART_ReceiveData(USART3);
		xQueueSendFromISR(queueHC05, &Data, NULL);

    }
}

