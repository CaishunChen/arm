/**
  ******************************************************************************
  * @file    Bsp_Tim.c 
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
#include "Bsp_Tim.h"
//-------------------------------------------------------------------------------
uint32_t Tim_1ms_Count;
static uint8_t TimStartDelayTimer=0;
#if	(defined(OS_FREERTOS))
uint32_t ulHighFrequencyTimerTicks;
#endif

static void Bsp_Tim_Handle(void);
/*******************************************************************************
* Function Name  : TIM2_IRQHandler
* Description    : This function handles TIM2 Handler.
*******************************************************************************/
#if   (defined(STM32F1)||defined(STM32F4))
void TIM2_ISRHandler(void)
{
    if ( TIM_GetITStatus(TIM2 , TIM_IT_Update) != RESET )
    {
        TIM_ClearITPendingBit(TIM2 , TIM_FLAG_Update);
        //
        if(TimStartDelayTimer<BSP_TIM_START_DELAY_MS)
        {
           TimStartDelayTimer++;
           return;
        }
        //
        Bsp_Tim_Handle();
    }
}
#elif (defined(NRF51)||defined(NRF52))
static void Bsp_Tim_EventHandler(nrf_timer_event_t event_type, void* p_context)
{
    switch (event_type)
    {
        case NRF_TIMER_EVENT_COMPARE0:
            //
           if(TimStartDelayTimer<BSP_TIM_START_DELAY_MS)
            {
               TimStartDelayTimer++;
               return;
            }
            //
            Bsp_Tim_Handle();
            break;
        default:
            //Do nothing.
            break;
    }
}
#endif
//
/*******************************************************************************
* Function Name  : TIM_Configuration
* Description    : TIM_Configuration program.
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void TIM_Configuration(uint16_t ms)
{
#if   (defined(STM32F1)||defined(STM32F4))
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    //时钟配置
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
    TIM_DeInit(TIM2);
    //自动重装载寄存器周期的值(计数值)
    TIM_TimeBaseStructure.TIM_Period=(10-1)*ms;
    //累计 TIM_Period个频率后产生一个更新或者中断
    //时钟预分频数   例如：时钟频率=72MHZ/(时钟预分频+1)
#if   (defined(STM32F1))
    TIM_TimeBaseStructure.TIM_Prescaler= SystemCoreClock/10000;
#elif (defined(STM32F4))
    TIM_TimeBaseStructure.TIM_Prescaler= (SystemCoreClock/2)/10000;
#endif
    //采样分频
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    //向上计数模式
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Down;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    //清除溢出中断标志
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
    TIM_Cmd(TIM2, ENABLE);
    //中断配置
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
#elif (defined(NRF51)||defined(NRF52))
    uint32_t err_code;
    uint32_t time_ticks;
    const uint32_t time_ms = ms;
    const nrf_drv_timer_t S_TIMER = NRF_DRV_TIMER_INSTANCE(0);
    // 装载默认配置
    nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
    // 设定中断句柄
    err_code = nrf_drv_timer_init(&S_TIMER, &timer_cfg, Bsp_Tim_EventHandler);
    APP_ERROR_CHECK(err_code);
    // 设定时间
    time_ticks = nrf_drv_timer_ms_to_ticks(&S_TIMER, time_ms);
    // 自动清空
    nrf_drv_timer_extended_compare(
        &S_TIMER, NRF_TIMER_CC_CHANNEL0, time_ticks, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);

    nrf_drv_timer_enable(&S_TIMER);
#endif
}

static void Bsp_Tim_Handle(void)
{
   //
            if(TimStartDelayTimer<BSP_TIM_START_DELAY_MS)
            {
               TimStartDelayTimer++;
               return;
            }
            //
            if(Tim_1ms_Count<0xFFFFFFFF)Tim_1ms_Count++;
				    //
#if	(defined(OS_FREERTOS))
						ulHighFrequencyTimerTicks+=20;
#endif
				    //
#ifdef   BSP_TIM_CALL_FUNCTION1
            BSP_TIM_CALL_FUNCTION1;
#endif
#ifdef   BSP_TIM_CALL_FUNCTION2
            BSP_TIM_CALL_FUNCTION2;
#endif
#ifdef   BSP_TIM_CALL_FUNCTION3
            BSP_TIM_CALL_FUNCTION3;
#endif
#ifdef   BSP_TIM_CALL_FUNCTION4
            BSP_TIM_CALL_FUNCTION4;
#endif
#ifdef   BSP_TIM_CALL_FUNCTION5
            BSP_TIM_CALL_FUNCTION5;
#endif
}
/********************************************************************************
      END FILE
********************************************************************************/
