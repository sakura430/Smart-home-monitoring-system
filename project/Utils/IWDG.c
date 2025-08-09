#include "IWDG.h"
void IWDG_Init(void)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);  // 允许修改
	IWDG_SetPrescaler(IWDG_Prescaler_64); // 设置预分频为64 40kHz/64=625
	IWDG_SetReload(4095);  // 设置重装值		//4096/625=6s
	IWDG_Enable();  // 启动 IWDG
}

