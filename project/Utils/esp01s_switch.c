#include "esp01s_switch.h"
#include "Delay.h"
int esp01s_open(void) {
    PrintToEsp01s("AT+CIPMODE=1\r\n");//开启透传模式(准备收发TCP数据)
    while (1) {
        QueueSetMemberHandle_t set = xQueueSelectFromSet(QueueSet, portMAX_DELAY);

        if (set == queueESP01s) {
            char str[MAX_SIZE] = {0};
            char Data;
            Delay_Ms(100);
            UBaseType_t que_msg_num = uxQueueMessagesWaiting(queueESP01s);

            while (que_msg_num > 0) {
                xQueueReceive(queueESP01s, &Data, 0);
                str[strlen(str)] = Data;
                que_msg_num--;
            }

            if (strlen(str) != 0) {
                PrintToUTools("Esp01s :%s", str);
                char *temp[3] = {"AT+CIPMODE=1\r\n\r\nOK",
                                 "AT+CIPSEND\r\n\r\nOK",
                                 "AT+CIPSEND\r\n"};//开始准备发送数据
                static int i = 0;
                for (; i < 2; i++) {
                    char *ret=strstr(str,temp[i]);
					if (ret != NULL) {
                        if (i == 1) {
                            PrintToUTools("switch -> ON\r\n");
							OLED_UpdateDisplay("Esp01sOpen");
                            return 1;
                        }
                        PrintToEsp01s("%s", temp[i + 2]);
                        break;
                    }
                }
            }
        }
    }
}

int esp01s_close(void) {
	PrintToEsp01s("+++");
	vTaskDelay(1000);
	PrintToEsp01s("AT+CIPMODE=0\r\n");
    while (1) {
        QueueSetMemberHandle_t set = xQueueSelectFromSet(QueueSet, portMAX_DELAY);
        if (set == queueESP01s) {
			char str[MAX_SIZE] = {0};
            char Data;
            Delay_Ms(100);
            UBaseType_t que_msg_num = uxQueueMessagesWaiting(queueESP01s);
            while (que_msg_num > 0) {
                xQueueReceive(queueESP01s, &Data, 0);
                str[strlen(str)] = Data;
                que_msg_num--;
            }
            if (strlen(str) != 0) {
                PrintToUTools("Esp01s :%s", str);
                char *temp[3] = {"AT+CIPMODE=0\r\n\r\nOK",
                                 "AT+CIPCLOSE\r\nCLOSED",
                                 "AT+CIPCLOSE\r\n"
                                };
                static int i = 0;
                for (; i < 2; i++) {
                    char *ret=strstr(str,temp[i]);
					if (ret != NULL) {
                        if (i == 1) {
                            PrintToUTools("switch -> OFF\r\n");
							OLED_UpdateDisplay("Esp01sClose");
                            return 1;
                        }
                        PrintToEsp01s("%s", temp[i + 2]);
                        break;
                    }
                }
            }
        }
    }
}
