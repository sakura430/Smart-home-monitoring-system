#include "header.h"

void control_task(void *arg) {
    int switch_command;//0表示切换到本地，1表示切换回网络
    while (1) {
        // 等待切换命令，没收到前一直阻塞
        if (xQueueReceive(queueTaskStatus, &switch_command, portMAX_DELAY) == pdPASS) {
            switch (switch_command) {
            case 0: // 切换到本地模式
                if (!local_storage_mode) {
                    local_storage_mode = 1;
                    PrintToUTools("Switching to local storage\r\n");
                    vTaskSuspend(handle1);
                    vTaskResume(handle2);
                }

                break;

            case 1: // 切换回网络模式
                if (local_storage_mode) {
                    local_storage_mode = 0;
                    PrintToUTools("Switching back to network\r\n");
                    vTaskSuspend(handle2);
                    vTaskResume(handle1);
                }

                break;
            }
        }
    }
}
//Todo:将存储的本地数据上传到云端
void local_task(void *arg) {
	//先挂起自己，断网重连失败时才会被唤醒
    vTaskSuspend(handle2);
    OLED_UpdateDisplay("task2 start");
    PrintToUTools("task2 start\r\n");
    W25Q64_Erase_Sector(0x000000); // 擦除扇区0
    W25Q64_Erase_Sector(0x001000);// 擦除扇区1
    W25Q64_Erase_Sector(0x002000);// 擦除扇区2
    int base_address = 0x000000;
    int num = 0;

    while (1) {
		IWDG_ReloadCounter();

        // 每处理10个数据点检查一次网络状态
        if (num >= 10) {
            num = 0;

            // 尝试连接到WiFi
            if (quick_wifi_check()) {
                int switch_command = 1; // 切换到upload_task的信号
                xQueueSend(queueTaskStatus, &switch_command, 0);

                // 进入等待切换状态
                OLED_UpdateDisplay("Network back!");
                PrintToUTools("Network restored!\r\n");

                // 等待主控任务处理切换
                vTaskSuspend(handle2);
            }
        }

        LED_Control(GREEN, LED_FAST_BLINK);
        char buf[40] = {0};
		time_t now=MyRTC_ReadTime();
		char timestamp[20];
		strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S",localtime(&now));
        if (num % 2 == 0) {
            uint16_t  ret  =  AD_GetValue(ADC_Channel_1);//光敏电阻

            if (ret > LDR_MAX) {
                LED_Control(BLUE, LED_ON);
            } else {
                LED_Control(BLUE, LED_OFF);
            }

            sprintf(buf, "+LDR:%u,%d[%s]", ret, num,timestamp);
        } else {
            uint16_t  ret  =  AD_GetValue(ADC_Channel_0);//红外线感应

            if (ret > IR_MAX) {
                Buzzer_Control(ON);
                LED_Control(RED, LED_FAST_BLINK);
            } else {
                LED_Control(RED, LED_OFF);
                Buzzer_Control(OFF);
            }

            sprintf(buf, "+IR:%u,%d[%s]", ret, num,timestamp);
        }

        PrintToUTools("%s\r\n", buf);
        W25Q64_Write_String(base_address, buf, sizeof(buf));
        OLED_UpdateDisplay(buf);
        base_address += sizeof(buf);
        memset(buf, 0, sizeof(buf));

        num++;

        // 循环地址空间
        if (base_address >= 0x003000) {
            base_address = 0x000000;
        }

        Delay_Ms(1000);
    }
}
//Todo:外部中断按键，重启程序
void upload_task(void *arg) {
    wifi_connect();//wifi连接
    tcp_connect();//tcp连接
    esp01s_open();//开启透传模式
    int num = 0;//发送数据的编号
    LED_Control(GREEN, LED_SLOW_BLINK);//正常上传数据，绿灯慢闪
	
    while (1) { 
        IWDG_ReloadCounter();//重载计数值
        char buf[40] = {0};//存储数据信息
		time_t now=MyRTC_ReadTime();
		char timestamp[20];
		strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S",localtime(&now));
        if (num % 2 == 0) {
            uint16_t  ret  =  AD_GetValue(ADC_Channel_1);//光敏电阻

            if (ret > LDR_MAX) {
                LED_Control(BLUE, LED_ON);
            } else {
                LED_Control(BLUE, LED_OFF);
            }
            tcp_send_data("LDR", ret,timestamp);//发送数据
            sprintf(buf, "LDR:%u,%d[%s]", ret, num,timestamp);
        } else {
            uint16_t  ret  =  AD_GetValue(ADC_Channel_0);//红外线感应

            if (ret > IR_MAX) {
                Buzzer_Control(ON);
                LED_Control(RED, LED_FAST_BLINK);
            } else {
                LED_Control(RED, LED_OFF);
                Buzzer_Control(OFF);
            }

			tcp_send_data("IR", ret,timestamp);
            sprintf(buf, "IR:%u,%d[%s]", ret, num,timestamp);
        }

        PrintToUTools("%s", buf);
        OLED_UpdateDisplay(buf);
        Delay_Ms(1000);
        num++;

        if (num == 50) {
            esp01s_close();
            LED_Control(GREEN, LED_ON);
        }
    }
}

int main(void) {
    OLED_Init();
    AD_Init();
    USART1_NVIC_Init();
    USART2_NVIC_Init();
    USART3_NVIC_Init();
    LED_Init();
    TIM_Init();
    Buzzer_Init();
    SPI1_Init();
	MyRTC_Init();
	Key_Init();
    Delay_Ms(2000);
	//判断程序是正常启动还是由看门狗重启
    if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET) {
        OLED_UpdateDisplay("reset by IWDG");
        RCC_ClearFlag();
    } else {
        OLED_UpdateDisplay("Normal boot");
    }
	
    LED_Control(GREEN, LED_ON); //上电绿灯亮
    OLED_UpdateDisplay("PleaseWait");
    queueUTools = xQueueCreate(MAX_SIZE, sizeof(char));//接收串口工具传来的信息
    queueESP01s = xQueueCreate(MAX_SIZE, sizeof(char));//接收ESP01S传来的信息
    queueHC05 = xQueueCreate(MAX_SIZE, sizeof(char));//接收HC05传来的信息
    QueueSet = xQueueCreateSet(MAX_SIZE * 3);
    xQueueAddToSet(queueUTools, QueueSet);
    xQueueAddToSet(queueESP01s, QueueSet);
    xQueueAddToSet(queueHC05, QueueSet);
    queueTaskStatus = xQueueCreate(1, sizeof(int));//一个标记位判断任务状态
    xTaskCreate(upload_task, "Upload", 256, NULL, 3, &handle1);
    xTaskCreate(local_task, "Local", 256, NULL, 3, &handle2);
    xTaskCreate(control_task, "Control", 128, NULL, 4, NULL);//先启动
    vTaskStartScheduler();

    while (1) {
    }
}
