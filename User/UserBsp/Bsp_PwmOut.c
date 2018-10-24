/**
  ******************************************************************************
  * @file    Bsp_PwmOut.c 
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
1,基于TIM4(CH3-PB8,CH4-PB9),做PWM输出,用于4-20mA
2,电压-电流关系:10.385~3.873V对应4~20mA
*******************************************************************************/
//------------------------------------------------------------------------------
#include "includes.h"
#include "Bsp_PwmOut.h"

#if (defined(NRF51)||defined(NRF52))
#include "Bsp_Led.h"
#include "app_pwm.h"
#include "nrf_delay.h"
APP_PWM_INSTANCE(PWM1,1);                   // Create the instance "PWM1" using TIMER1.
static volatile bool ready_flag;            // A flag indicating PWM status.
void pwm_ready_callback(uint32_t pwm_id)    // PWM callback function
{
    ready_flag = true;
}
#endif
//------------------------------------------------------------------------------Private define

//------------------------------------------------------------------------------Private variables
static uint8_t PwmOut_DebugTest_Enable=0;
static uint8_t PwmOut_DutyBakBuf[2];
//------------------------------------------------------------------------------Private function
//------------------------------------------------------------------------------
void BspPwmOut_Init (void)
{
#if   (defined(STM32F1)||defined(STM32F4))
    GPIO_InitTypeDef GPIO_InitStructure;

    //使能时钟
    BSP_PWMOUT_GPIO_RCC_ENABLE;
    //GPIO重映射
    BSP_PWMOUT1_REMAP;
    BSP_PWMOUT2_REMAP;
    //初始化GPIO
    {
#if   (defined(STM32F1))
        GPIO_AFIODeInit();
        //重映射
        GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
#elif (defined(STM32F4))
        GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_100MHz;
        GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_AF;
        GPIO_InitStructure.GPIO_OType   =  GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd    =  GPIO_PuPd_NOPULL;
#endif
        GPIO_InitStructure.GPIO_Pin =  BSP_PWMOUT1_PIN;
        GPIO_Init(BSP_PWMOUT1_PORT, &GPIO_InitStructure);
        GPIO_InitStructure.GPIO_Pin =  BSP_PWMOUT2_PIN;
        GPIO_Init(BSP_PWMOUT2_PORT, &GPIO_InitStructure);
    }
    /* -----------------------------------------------------------------------
        TIM4 Configuration: generate 4 PWM signals with 4 different duty cycles:
        TIM4CLK = 72 MHz, Prescaler = 0x0, TIM4 counter clock = 72 MHz
        TIM4 ARR Register = 999 => TIM4 Frequency = TIM4 counter clock/(ARR + 1)
        TIM4 Frequency = 72 KHz.
        TIM4 Channel1 duty cycle = (TIM4_CCR1/ TIM4_ARR)* 100 = 50%
        TIM4 Channel2 duty cycle = (TIM4_CCR2/ TIM4_ARR)* 100 = 37.5%
        TIM4 Channel3 duty cycle = (TIM4_CCR3/ TIM4_ARR)* 100 = 25%
        TIM4 Channel4 duty cycle = (TIM4_CCR4/ TIM4_ARR)* 100 = 12.5%
        ----------------------------------------------------------------------- */
    {
        TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
        TIM_OCInitTypeDef  TIM_OCInitStructure;
        memset((char*)&TIM_OCInitStructure,0,sizeof(TIM_OCInitTypeDef));
        memset((char*)&TIM_TimeBaseStructure,0,sizeof(TIM_TimeBaseInitTypeDef));
        //定时器初始化
        BSP_PWMOUT_TIM_RCC_ENABLE;
        TIM_DeInit(BSP_PWMOUT_TIMER_X);
        // Time base configuration
        TIM_TimeBaseStructure.TIM_Period        = (SystemCoreClock/1000)-1;
        TIM_TimeBaseStructure.TIM_Prescaler     = 0;
        TIM_TimeBaseStructure.TIM_ClockDivision = 0;
        TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
        TIM_TimeBaseInit(BSP_PWMOUT_TIMER_X, &TIM_TimeBaseStructure);
        /* PWM1 Mode configuration: Channel3 */
        TIM_OCInitStructure.TIM_OCMode          = TIM_OCMode_PWM1;
        //-----???-----为什么用High导致屏幕白屏
        //TIM_OCInitStructure.TIM_OCPolarity      = TIM_OCPolarity_Low; //TIM_OCPolarity :TIM 输出比较极性高
        TIM_OCInitStructure.TIM_OCPolarity      = TIM_OCPolarity_High;
        //
        TIM_OCInitStructure.TIM_OutputState     = TIM_OutputState_Enable;
        TIM_OCInitStructure.TIM_Pulse           = (((SystemCoreClock/1000)-1)*1)/256;
        //TIM_OCInitStructure.TIM_Pulse           = 0;
        TIM_OC1Init(BSP_PWMOUT_TIMER_X, &TIM_OCInitStructure);
        TIM_OC1PreloadConfig(BSP_PWMOUT_TIMER_X, TIM_OCPreload_Enable);
        TIM_OC2Init(BSP_PWMOUT_TIMER_X, &TIM_OCInitStructure);
        TIM_OC2PreloadConfig(BSP_PWMOUT_TIMER_X, TIM_OCPreload_Enable);
        /* TIM enable counter */
        //TIM_ARRPreloadConfig(BSP_PWMOUT_TIMER_X, ENABLE);
        TIM_Cmd(BSP_PWMOUT_TIMER_X, ENABLE);
        // TIM Main Output Enable
        //TIM_CtrlPWMOutputs(BSP_PWMOUT_TIMER_X, ENABLE);
    }
#elif (defined(NRF51)||defined(NRF52))
    ret_code_t err_code;
    // 1-channel PWM, 200Hz, output on DK LED pins.
    static app_pwm_config_t pwm1_cfg = APP_PWM_DEFAULT_CONFIG_1CH(5000L, BSP_LED_LED1_PIN);
    // Switch the polarity of the second channel.
    //pwm1_cfg.pin_polarity[0] = APP_PWM_POLARITY_ACTIVE_HIGH;
    // Initialize and enable PWM.
    err_code = app_pwm_init(&PWM1,&pwm1_cfg,pwm_ready_callback);
    APP_ERROR_CHECK(err_code);
    app_pwm_enable(&PWM1);
    BspPwmOut_Write(NULL,NULL,10);
#endif
}
/*******************************************************************************
* 函数功能: 设置PWM
* 参    数: num   -  1起始
*           freq  -  频率
*           duty  -  占空比(0-255)
* 返 回 值: 转换后的值
*******************************************************************************/
void BspPwmOut_Write(uint8_t num,uint32_t freq,uint8_t duty)
{
#if   (defined(STM32F1)||defined(STM32F4))
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    uint32_t i32;
    //
    memset((char*)&TIM_OCInitStructure,0,sizeof(TIM_OCInitTypeDef));
    memset((char*)&TIM_TimeBaseStructure,0,sizeof(TIM_TimeBaseInitTypeDef));
    //
    num=num;
    // -----XSL-----
    //在168000000MHz的主频下,下面配置占空比不好使(有效范围从0-255变为0-99)原因未知
    //所以暂时不考虑频率的准确性,以降频方式来暂时解决此问题
    if(SystemCoreClock==168000000)
    {
        i32 =  SystemCoreClock/4;
    }
    else
    {
        i32 =  SystemCoreClock;
    }
    //设置频率
    TIM_TimeBaseStructure.TIM_Period        = (i32/freq)-1;
    if(SystemCoreClock==168000000)
    {
        TIM_TimeBaseStructure.TIM_Prescaler     = 4;
    }
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit(BSP_PWMOUT_TIMER_X, &TIM_TimeBaseStructure);
    //设置占空比
    TIM_OCInitStructure.TIM_OCMode          = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState     = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse           = (((i32/freq)-1)*duty)/256;
    TIM_OCInitStructure.TIM_OCPolarity      = TIM_OCPolarity_High;
    TIM_OC1Init(BSP_PWMOUT_TIMER_X, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(BSP_PWMOUT_TIMER_X, TIM_OCPreload_Enable);
    //
    TIM_OC2Init(BSP_PWMOUT_TIMER_X, &TIM_OCInitStructure);
    TIM_OC2PreloadConfig(BSP_PWMOUT_TIMER_X, TIM_OCPreload_Enable);
    //
    PwmOut_DutyBakBuf[0]=duty;
#elif (defined(NRF51)||defined(NRF52))
    //ready_flag = false;
    /* Set the duty cycle - keep trying until PWM is ready... */
    while (app_pwm_channel_duty_set(&PWM1, 0, (duty*100/255)) == NRF_ERROR_BUSY);
    /* ... or wait for callback. */
    //while (!ready_flag);
    //APP_ERROR_CHECK(app_pwm_channel_duty_set(&PWM1, 1, value));
#endif
}
void BspPwmOut_test_10ms(void)
{
    static uint16_t s16=0;
    uint8_t dutyBak=0;
    if(PwmOut_DebugTest_Enable==0)
        return;
    if(s16<(255*2))
    {
        s16+=5;
    }
    else
    {
        s16=0;
    }
    dutyBak=PwmOut_DutyBakBuf[0];
    if(s16<255)
    {
        BspPwmOut_Write(1,1000,s16);
    }
    else
    {
        BspPwmOut_Write(1,1000,(255*2)-s16);
    }
    PwmOut_DutyBakBuf[0]=dutyBak;
}
void BspPwmOut_DebugTestOnOff(uint8_t OnOff)
{
    if(OnOff==ON)
    {
        PwmOut_DebugTest_Enable=1;
    }
    else
    {
        PwmOut_DebugTest_Enable=0;
        BspPwmOut_Write(1,1000,PwmOut_DutyBakBuf[0]);
    }
}
