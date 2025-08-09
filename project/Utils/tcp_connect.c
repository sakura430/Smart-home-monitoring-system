#include "tcp_connect.h"
#include "Delay.h"
int tcp_connect(void) {
    PrintToEsp01s("AT+CIPSTART=\"TCP\",\"47.115.220.165\",9001\r\n");
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
				char *reply_to_tcp = strstr(str, "CONNECT\r\n\r\nOK");
                if (reply_to_tcp != NULL) {
                    OLED_UpdateDisplay("TcpConnectOK");
                    return 1;
                }	
				char *reply_to_tcp2= strstr(str, "ERROR\r\nCLOSED");
                if (reply_to_tcp2 != NULL) {
                    OLED_UpdateDisplay("TcpConnectFail");
                    OLED_UpdateDisplay("ServerERROR");
                }
				char *reply_to_tcp3= strstr(str,"ALREADY CONNECTED");
                if (reply_to_tcp3 != NULL) {
                    OLED_UpdateDisplay("TcpConnectFail");
                    OLED_UpdateDisplay("ServerERROR");
                }
            }
        }
    }
}

