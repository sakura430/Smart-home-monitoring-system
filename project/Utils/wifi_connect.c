#include "wifi_connect.h"
#include "flash.h"
#include "esp01s_switch.h"
#include "Delay.h"
#include "IWDG.h"
//wifi_connect中的延时要用vTaskDelay，Delay_Ms可能会卡住
int wifi_connect(void) {
    static char str_wifi[40] = {0};//存储WiFi信息
    PrintToEsp01s("+++");//退出透传
    vTaskDelay(1000);
    char WifiName[20] = {0};
    char WifiPassword[20] = {0};
    PrintToEsp01s("AT+CWJAP_DEF?\r\n");//查询是否已联网query

    while (1) {
        QueueSetMemberHandle_t set = xQueueSelectFromSet(QueueSet, portMAX_DELAY);

        if (set == queueUTools) {
            char str[MAX_SIZE] = {0};
            char Data = 0;//用于接收队列中的数据
             vTaskDelay(100);//等待中断多发送一些数据
			//查询队列中有多少个数据，全取出来
            UBaseType_t que_msg_num = uxQueueMessagesWaiting(queueUTools);
            while (que_msg_num > 0) {
                xQueueReceive(queueUTools, &Data, 0);
                str[strlen(str)] = Data;
                que_msg_num--;
            }

            if (strlen(str) != 0) {
                PrintToEsp01s("%s", str);
                PrintToUTools("UTools :%s", str);
                memset(str, 0, sizeof(str));
            }
        } else if (set == queueESP01s) {
            char str[MAX_SIZE] = {0};
            char Data = 0;//用于接收队列中的数据
             vTaskDelay(100);//等待中断多发送一些数据
            UBaseType_t que_msg_num = uxQueueMessagesWaiting(queueESP01s);
            while (que_msg_num > 0) {
                xQueueReceive(queueESP01s, &Data, 0);
                str[strlen(str)] = Data;
                que_msg_num--;
            }

            if (strlen(str) != 0) {
                PrintToUTools("Esp01s :%s", str);
				//对于查询的回复
				//现在未连接网络，那就去本地flash中找wifi信息
                char *reply_to_query = strstr(str, "No AP\r\n\r\nOK");
                if (reply_to_query != NULL) {
                    int status = 0;
                    int address = 0x0800F000;
                    status = *((__IO uint32_t *)(address));

                    if (status != 0xFFFFFFFF) {
                        // flash中有内容
                        char buffer[40] = {0};
                        readFlash(address + 8, buffer, 40);
                        PrintToUTools("read:%s \r\n",  buffer);

                        if (buffer[0] != 0) {
                            sscanf(buffer, "!%[^=]=%[^!]!", WifiName, WifiPassword);
                            PrintToEsp01s("AT+CWJAP_DEF=\"%s\",\"%s\"\r\n", WifiName, WifiPassword);
                        }
                    } else {
                        LED_Control(RED, LED_SLOW_BLINK); //红灯慢闪等待wifi
                        OLED_UpdateDisplay("WaitWifi");
                        PrintToUTools("flash is empty\r\n");
                    }
                    continue;
                }
				//对于查询的回复
				//现在已连接网络
				char *reply_to_query2 = strstr(str, "+CWJAP_DEF:");
                if (reply_to_query2 != NULL) {
                    OLED_UpdateDisplay("ConnectWifiOK");
                    LED_Control(RED, LED_OFF);
					IWDG_Init();
                    return 1;
                }
				//对于联网的回复
				//wifi密码错误或连接超时
                char *reply_to_connect = strstr(str, "+CWJAP:1"); 
                if (reply_to_connect != NULL) {
                    PrintToUTools("Esp01s :wifi connect fail\r\n");
                    OLED_UpdateDisplay("WifiConnectFail");
                    continue;
                }
				//Todo：改变wifi名称后会直接切换到本地传输，无法更改flash中wifi信息
                //解决方案：外接按键，长按3秒flash复位
				
				//对于联网的回复
				//wifi账号错误/网络断开
                char *reply_to_connect2 = strstr(str, "+CWJAP:3");
                if (reply_to_connect2 != NULL) {
                    PrintToUTools("Esp01s :wifi connect fail\r\n");
                    OLED_UpdateDisplay("WifiConnectFail");
					//查看flash中是否有数据来区分首次配网和断网重连
					//没数据说明是首次配网，有数据是断网重连
					//解决首次配网手机发送wifi信息时账号写错的问题
                    int status = 0;
                    int address = 0x0800F000;
                    status = *((__IO uint32_t *)(address));
                    if (status == 0xFFFFFFFF) {//账号写错阻塞等待
                        continue;
                    } else {//断网重连，唤醒控制任务，退出wifi_connect函数
						int switch_cmd = 0;
                        xQueueSend(queueTaskStatus, &switch_cmd, 0);
                        return 0;
                    }
                }
				//对于联网的回复
				//网络连接成功
				//此处不担心误判，如果ESP01S回复No AP\r\n\r\nOK
				//虽然也有OK,但直接在上面处理完continue了，不会到达此处
                char *reply_to_connect3 = strstr(str, "OK");
                if (reply_to_connect3 != NULL) {
					//联网成功通知手机
                    PrintToHc05("{\"status\":0, \"wifi_name\":\"%s\"}", WifiName);
					//通过蓝牙联网成功就即时更新本地flash中wifi信息
                    if (strlen(str_wifi) != 0) {
                        int address = 0x0800F000;
                        erasePage(address);
                        changeFlashFor32(address, 6);
                        changeFlashFor32(address + 4, strlen(str_wifi));
                        PrintToUTools("str_wifi :%s\r\n", str_wifi);

                        for (int i = 0; i < sizeof(str_wifi); i += 4) {
                            changeFlashFor32(address + 8 + i, *(int *)(str_wifi + i));
                        }
                    }


                    OLED_UpdateDisplay("ConnectWifiOK");
                    LED_Control(RED, LED_OFF);
                    IWDG_Init();
					//开启看门狗，看门狗需要在联网成功后开启
					//不然会导致正在等手机发送wifi信息但程序超时重启了
                    return 1;
                }
            }
        } else if (set == queueHC05) {
            char str[MAX_SIZE] = {0};
            char Data = 0;//用于接收队列中的数据
            vTaskDelay(100);//等待中断多发送一些数据
            UBaseType_t que_msg_num = uxQueueMessagesWaiting(queueHC05);
            while (que_msg_num > 0) {
                xQueueReceive(queueHC05, &Data, 0);
                str[strlen(str)] = Data;
                que_msg_num--;
            }

            if (strlen(str) != 0) {
				//先清空wifi账号密码数组
				//防止前边输错了，导致错乱
                memset(WifiName, 0, sizeof(WifiName));
                memset(WifiPassword, 0, sizeof(WifiPassword));
                sscanf(str, "!%[^=]=%[^!]!", WifiName, WifiPassword);
                strncpy(str_wifi, str, strlen(str));
                PrintToUTools("wifi:%s\r\n", str);
                PrintToUTools("WifiName:%s\r\n", WifiName);
                PrintToUTools("WifiPassword:%s\r\n", WifiPassword);
                PrintToEsp01s("AT+CWJAP_DEF=\"%s\",\"%s\"\r\n", WifiName, WifiPassword);
            }
        }
    }
}
//快速网络检测，用于往外接flash传输数据时恢复网络
int quick_wifi_check(void) {
    char WifiName[20] = {0};
    char WifiPassword[20] = {0};
    PrintToEsp01s("AT+CWJAP_DEF?\r\n");
	//非阻塞方法联网，每次最多等待1500ms
    for (int i = 0; i < 3; i++) {
        QueueSetMemberHandle_t set = xQueueSelectFromSet(QueueSet, 500);

        if (set == queueESP01s) {
            char str[MAX_SIZE] = {0};
            char Data = 0;
			//quick_wifi_check中要用占用Cpu的Delay_Ms,不能用vTaskDelay,会卡住
            Delay_Ms(100);
            UBaseType_t que_msg_num = uxQueueMessagesWaiting(queueESP01s);

            while (que_msg_num > 0) {
                xQueueReceive(queueESP01s, &Data, 0);
                str[strlen(str)] = Data;
                que_msg_num--;
            }

            if (strlen(str) != 0) {
                PrintToUTools("Esp01s :%s", str);
                char *reply_to_query = strstr(str, "No AP\r\n\r\nOK");

                if (reply_to_query != NULL) {
                    OLED_UpdateDisplay("WaitWifi");
                    int status = 0;
                    int address = 0x0800F000;
                    status = *((__IO uint32_t *)(address));
                    if (status != 0xFFFFFFFF) {
                        // 有内容
                        char buffer[40] = {0};
                        readFlash(address + 8, buffer, 40);
                        PrintToUTools("read:%s \r\n",  buffer);

                        if (buffer[0] != 0) {
                            sscanf(buffer, "!%[^=]=%[^!]!", WifiName, WifiPassword);
                            PrintToEsp01s("AT+CWJAP_DEF=\"%s\",\"%s\"\r\n", WifiName, WifiPassword);
                        }
                    }

                    continue;
                }
				
                char *reply_to_connect = strstr(str, "OK");
                if (reply_to_connect != NULL) {
                    OLED_UpdateDisplay("ConnectWifiOK");
                    LED_Control(RED, LED_OFF);
					IWDG_Init();
                    return 1;
                }
            }
        }
    }

    return 0;
}
