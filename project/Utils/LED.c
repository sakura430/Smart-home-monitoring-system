#include"led.h"

// LED GPIO 引脚定义
#define RED_LED_PIN    GPIO_Pin_12
#define GREEN_LED_PIN  GPIO_Pin_13
#define BLUE_LED_PIN   GPIO_Pin_14
#define LED_GPIO_PORT  GPIOB

// LED 初始化函数
void LED_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 使能 GPIOC 时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    // 配置 LED 引脚为推挽输出
    GPIO_InitStructure.GPIO_Pin = RED_LED_PIN | GREEN_LED_PIN | BLUE_LED_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LED_GPIO_PORT, &GPIO_InitStructure);
    
    // 初始关闭所有 LED
    GPIO_SetBits(LED_GPIO_PORT, RED_LED_PIN | GREEN_LED_PIN | BLUE_LED_PIN);
}

// 定时器初始化 (用于 LED 闪烁)
void TIM_Init(void) {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    
    // 使能 TIM2 时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    
    // 定时器配置：1ms 中断
    TIM_TimeBaseStructure.TIM_Period = 1000 - 1;          // 自动重装载值
    TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;         // 72MHz/72 = 1MHz, 1us
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    
    // 使能 TIM2 中断
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    
    // 启动定时器
    TIM_Cmd(TIM2, ENABLE);
    
    // 配置 NVIC
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);   //优先级分组
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 12;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

// LED 控制函数
void LED_Control(LEDColor led_color, LEDMode led_mode) {
    // 更新 LED 模式
    g_ledModes[led_color] = led_mode;
    
    // 根据模式立即更新 LED 状态
    switch(led_mode) {
        case LED_OFF:
            // 关闭 LED
            GPIO_SetBits(LED_GPIO_PORT, 
                        (led_color == RED) ? RED_LED_PIN : 
                        (led_color == GREEN) ? GREEN_LED_PIN : BLUE_LED_PIN);
            break;
            
        case LED_ON:
            // 打开 LED
            GPIO_ResetBits(LED_GPIO_PORT, 
                          (led_color == RED) ? RED_LED_PIN : 
                          (led_color == GREEN) ? GREEN_LED_PIN : BLUE_LED_PIN);
            break;
            
        case LED_FAST_BLINK:
        case LED_SLOW_BLINK:
            // 闪烁模式由定时器中断处理
            break;
    }
}

// 定时器中断处理函数 (处理 LED 闪烁)
void TIM2_IRQHandler(void) {
    static uint16_t fast_counter = 0;
    static uint16_t slow_counter = 0;
    
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
        
        // 快闪计数 (200ms 周期)
        fast_counter = (fast_counter + 1) % 200;
        
        // 慢闪计数 (1000ms 周期)
        slow_counter = (slow_counter + 1) % 2000;
        
        // 处理每个 LED 的闪烁
        for (int i = 0; i < 3; i++) {
            if (g_ledModes[i] == LED_FAST_BLINK) {
                // 快闪逻辑：100ms 开，100ms 关
                if (fast_counter < 100) {
                    GPIO_ResetBits(LED_GPIO_PORT, 
                                  (i == RED) ? RED_LED_PIN : 
                                  (i == GREEN) ? GREEN_LED_PIN : BLUE_LED_PIN);
                } else {
                    GPIO_SetBits(LED_GPIO_PORT, 
                                (i == RED) ? RED_LED_PIN : 
                                (i == GREEN) ? GREEN_LED_PIN : BLUE_LED_PIN);
                }
            }
            else if (g_ledModes[i] == LED_SLOW_BLINK) {
                // 慢闪逻辑：500ms 开，500ms 关
                if (slow_counter < 500) {
                    GPIO_ResetBits(LED_GPIO_PORT, 
                                  (i == RED) ? RED_LED_PIN : 
                                  (i == GREEN) ? GREEN_LED_PIN : BLUE_LED_PIN);
                } else {
                    GPIO_SetBits(LED_GPIO_PORT, 
                                (i == RED) ? RED_LED_PIN : 
                                (i == GREEN) ? GREEN_LED_PIN : BLUE_LED_PIN);
                }
            }
        }
    }
}

//// 主函数
//int main(void) {
//    // 初始化系统时钟
//    SystemInit();
//    
//    // 初始化 LED
//    LED_Init();
//    
//    // 初始化定时器
//    TIM_Init();
//    
//    // 测试 LED 控制
//    LED_Control(RED, LED_ON);          // 红灯常亮
//    LED_Control(GREEN, LED_FAST_BLINK); // 绿灯快闪
//    LED_Control(BLUE, LED_SLOW_BLINK);  // 蓝灯慢闪
//    
//    while (1) {
//        // 主循环可以执行其他任务
//        // LED 控制由中断处理
//    }
//}



