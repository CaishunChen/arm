/**
  ******************************************************************************
  * @file    PWMIN.c 
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  ******************************************************************************
  * @attention
  *
  * GNU General Public License (GPL) 
  *
  * <h2><center>&copy; COPYRIGHT 2017 XSLXHN</center></h2>
  ******************************************************************************
  */ 
/*******************************************************************************
说明:
1,基于TIM3(CH1-PC6),做PWM输入
2,对于TCI-V1电路板为光耦输入3.
*******************************************************************************/
//------------------------------------------------------------------------------
#define PWMIN_GLOBAL
#include "includes.h"
//------------------------------------------------------------------------------Private define
//------------------------------------------------------------------------------Private variables
uint16_t PWMIN_CH1_DutyCycle = 0;
uint32_t PWMIN_CH1_Frequency = 0;
//------------------------------------------------------------------------------
void PWMIN_GPIO_Configuration (void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    //-----
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    //-----
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}
void TIM3_PWMIN_Config(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_ICInitTypeDef  TIM_ICInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    //初始化AFIO
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    //GPIO初始化
    GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE); //重映射
    PWMIN_GPIO_Configuration();
    //时钟初始化
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    //中断初始化
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    //复位TIM3
    TIM_DeInit(TIM3);
    //定时器BASE配置:
    TIM_TimeBaseStructure.TIM_Period = 0xFFFF;     //周期0～FFFF
    TIM_TimeBaseStructure.TIM_Prescaler = 0;       //时钟分频
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;   //时钟分割
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//模式
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);//基本初始化
    //输入捕获配置:
    //TIM3配置: PWM输入模式,上升沿有效,CCR2测量频率,CCR1测量占空比
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;//通道选择
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;//上升沿触发
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;//管脚与寄存器对应关系
    //输入预分频.意思是控制多少个输入周期做一次捕获,如果输入的信号频率没有变化
    //,测得的周期也不会变.如果选择4分频,则每4个输入周期才做一次捕获以减少中断.
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV8;
    TIM_ICInitStructure.TIM_ICFilter = 0x0; //滤波设置,经理几个周期跳变认定波形稳定0x0-0xF
    TIM_PWMIConfig(TIM3, &TIM_ICInitStructure);
    //选择输入触发源: TIM经滤波定时器输入2
    TIM_SelectInputTrigger(TIM3, TIM_TS_TI1FP1);
    //选择从模式,上升沿重初始化计数器并触发寄存器的更新
    TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Reset);
    //使能主/从模式
    TIM_SelectMasterSlaveMode(TIM3, TIM_MasterSlaveMode_Enable);
    //使能TIM3
    TIM_Cmd(TIM3, ENABLE);
    //使能TIM捕获/比较2(CC1)中断源
    TIM_ITConfig(TIM3, TIM_IT_CC2, ENABLE);
}
void TIM3_ISRHandler(void)
{
    uint16_t IC2Value = 0;
    CPU_SR cpu_sr;
    //通知uCOSII进入中断
    CPU_CRITICAL_ENTER();
    OSIntNesting++;
    CPU_CRITICAL_EXIT();
    //------------------------用户代码区---Begin
    if(TIM_GetITStatus(TIM3, TIM_IT_CC2) == SET)
    {
        //Clear TIM3 Capture compare interrupt pending bit
        TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);
        //Get the Input Capture value
        IC2Value = TIM_GetCapture1(TIM3);

        if(IC2Value != 0)
        {
            //Duty cycle computation
            PWMIN_CH1_DutyCycle = (TIM_GetCapture2(TIM3) * 1000) / IC2Value;
            //Frequency computation
            PWMIN_CH1_Frequency = 72000000 / IC2Value;
        }
        else
        {
            PWMIN_CH1_DutyCycle = 0;
            PWMIN_CH1_Frequency = 0;
        }
    }
    //------------------------用户代码区---End
    //通知uCOSII退出中断
    OSIntExit();
}
