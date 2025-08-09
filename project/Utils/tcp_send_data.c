#include "tcp_send_data.h"
#include "OLED.h"
#include "Delay.h"
int tcp_send_data(char *name, uint16_t data,char* timestamp) {

    char http[100]={0};
    sprintf(http, "GET /0?%s:%u[%s]?HTTP/1.1", name, data,timestamp);
    PrintToEsp01s("%s", http);

    while (1) {
        QueueSetMemberHandle_t set = xQueueSelectFromSet(QueueSet, portMAX_DELAY);

        if (set == queueESP01s) {
            static char str[MAX_SIZE] = {0};
            char Data;
            Delay_Ms(100);//等待中断发送完所有数据
            UBaseType_t que_msg_num = uxQueueMessagesWaiting(queueESP01s);

            while (que_msg_num > 0) {
                xQueueReceive(queueESP01s, &Data, 0);
                str[strlen(str)] = Data;
                que_msg_num--;
            }

            if (strlen(str) != 0) {
                PrintToUTools("Esp01s :%s", str);
                char *reply = strstr(str,"HTTP/1.1 200 OK");
                if (reply != NULL) {
                    memset(str, 0, sizeof(str));
                    return 1;
                }
                memset(str, 0, sizeof(str));
            }
        }
    }
}






