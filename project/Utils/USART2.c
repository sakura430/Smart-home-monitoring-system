#include "USART2.h"




void USART2_NVIC_Init() {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    USART_InitTypeDef USART2_InitStruct;
    USART2_InitStruct.USART_BaudRate = 115200;
    USART2_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART2_InitStruct.USART_Parity = USART_Parity_No;
    USART2_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART2_InitStruct.USART_StopBits = USART_StopBits_1;
    USART2_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART2, &USART2_InitStruct);
    USART_Cmd(USART2, ENABLE);

    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);   //优先级分组
    NVIC_InitTypeDef NVIC_InitStruct2;
    NVIC_InitStruct2.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStruct2.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct2.NVIC_IRQChannelPreemptionPriority = 12;
    NVIC_InitStruct2.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_InitStruct2);
}

void USART2_SendByte(uint8_t Byte) {
    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
        ;

    USART_SendData(USART2, Byte);

    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
}

void PrintToEsp01s(char  *format, ...) {
    taskENTER_CRITICAL();

    char strs[100] = {0};
    va_list list;
    va_start(list, format);
    vsprintf(strs, format, list);
    va_end(list);

    for (uint8_t i = 0; strs[i] != '\0'; i++) {
        USART2_SendByte(strs[i]);
    }
    taskEXIT_CRITICAL();
}

void USART2_IRQHandler(void) {//ESP01S发送数据到stm32产生中断
    if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET) {
        char Data = USART_ReceiveData(USART2);
        xQueueSendFromISR(queueESP01s, &Data, NULL);

        static int flag = 0;

        if (flag == 0) {
            xQueueReceiveFromISR(queueESP01s, &Data, NULL);
            //memset(Str2, 0, sizeof(Str2));
            flag = 1;
        }
    }
}

