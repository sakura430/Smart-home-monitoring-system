#include "RTC.h"
#include "OLED.h"
rtc_time rt={2025,6,25,18,00,00};
void MyRTC_Init(void){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);		//开启PWR的时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP, ENABLE);		//开启BKP的时钟
	
	/*备份寄存器访问使能*/
	PWR_BackupAccessCmd(ENABLE);	//使用PWR开启对备份寄存器的访问
	
//	//BKP_WriteBackupRegister(BKP_DR1, 0x6666);
//	uint16_t ret =BKP_ReadBackupRegister(BKP_DR1);
//	uint16_t ret2 =BKP_ReadBackupRegister(BKP_DR2);
//	while(1){
//		OLED_ShowHexNum(2,1,ret,4);
//		OLED_ShowHexNum(3,1,ret2,4);
//	}
	
	if (BKP_ReadBackupRegister(BKP_DR1) != 0x6666)			//通过写入备份寄存器的标志位，判断RTC是否是第一次配置
															//if成立则执行第一次的RTC配置
	{
		RCC_LSEConfig(RCC_LSE_ON);							//开启LSE时钟
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) != SET);	//等待LSE准备就绪
		
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);				//选择RTCCLK来源为LSE
		RCC_RTCCLKCmd(ENABLE);								//RTCCLK使能
		
		RTC_WaitForSynchro();								//等待同步
		RTC_WaitForLastTask();								//等待上一次操作完成
		
		RTC_SetPrescaler(32768 - 1);						//设置RTC预分频器，预分频后的计数频率为1Hz
		RTC_WaitForLastTask();								//等待上一次操作完成
		
		MyRTC_SetTime();									//设置时间，调用此函数，全局数组里时间值刷新到RTC硬件电路

		BKP_WriteBackupRegister(BKP_DR1, 0x6666);			//在备份寄存器写入自己规定的标志位，用于判断RTC是不是第一次执行配置
		
	}
	else													//RTC不是第一次配置
	{
		RTC_WaitForSynchro();								//等待同步
		RTC_WaitForLastTask();								//等待上一次操作完成
	}
}
void MyRTC_SetTime(void)
{
	
	time_t time_cnt;		//定义秒计数器数据类型
	struct tm time_date;	//定义日期时间数据类型
	
	time_date.tm_year = rt.year - 1900;		//将数组的时间赋值给日期时间结构体
	time_date.tm_mon = rt.mon - 1;
	time_date.tm_mday = rt.mday;
	time_date.tm_hour = rt.hour;
	time_date.tm_min = rt.min;
	time_date.tm_sec = rt.sec;
	
	time_cnt = mktime(&time_date) - 8 * 60 * 60;	//调用mktime函数，将日期时间转换为秒计数器格式
													//- 8 * 60 * 60为东八区的时区调整
	
	RTC_SetCounter(time_cnt);						//将秒计数器写入到RTC的CNT中
	RTC_WaitForLastTask();							//等待上一次操作完成
}
time_t MyRTC_ReadTime(void){
	time_t time_cnt;		//定义秒计数器数据类型
	//struct tm time_date;	//定义日期时间数据类型	
	time_cnt = RTC_GetCounter() + 8 * 60 * 60;		//读取RTC的CNT，获取当前的秒计数器
	return time_cnt;												//+ 8 * 60 * 60为东八区的时区调整
//	time_date = *localtime(&time_cnt);				//使用localtime函数，将秒计数器转换为日期时间格式
//	rt.year = time_date.tm_year + 1900;		//将日期时间结构体赋值给数组的时间
//	rt.mon = time_date.tm_mon + 1;
//	rt.mday = time_date.tm_mday;
//	rt.hour = time_date.tm_hour;
//	rt.min = time_date.tm_min;
//	rt.sec = time_date.tm_sec;
}












