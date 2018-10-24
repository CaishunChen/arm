/**
  ******************************************************************************
  * @file    Bsp_WatchDog.c 
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
//------------------------------- Includes -----------------------------------
#include "Bsp_WatchDog.h"
/*
******************************************************************************
* 函数功能: 初始化
* 函 数 名: bsp_InitIwdg
* 功能说明: 独立看门狗时间配置函数
* 形    参：IWDGTime: 0 ---- 0x0FFF
*         独立看门狗时间设置,单位为ms,IWDGTime = 1000 大约就是一秒的
*            时间，这里没有结合TIM5测得实际LSI频率，只是为了操作方便取了
*            一个估计值超过IWDGTime不进行喂狗的话系统将会复位。
*         LSI = 34000左右
*返 回 值: 无
******************************************************************************
*/
static uint8_t BspWatchDogEnable=0;

#if (defined(NRF51)||defined(NRF52))
#include "nrf_drv_wdt.h"
nrf_drv_wdt_channel_id m_channel_id;
/**
 * @brief WDT events handler.
 */
void wdt_event_handler(void)
{
    //NOTE: The max amount of time we can spend in WDT interrupt is two cycles of 32768[Hz] clock - after that, reset occurs
}
#endif

void Bsp_WatchDog_init(uint16_t ms)
{
#if   (defined(STM32F1)||defined(STM32F4))
    /* Check if the system has resumed from IWDG reset */
    if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)//取清复位中断
    {
        /* Clear reset flags */
        RCC_ClearFlag();
    }
    /* Enable write access to IWDG_PR and IWDG_RLR registers */
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

    /* IWDG counter clock: 128/40KHz(LSI)  IWDG->PR  3.2ms*/
    IWDG_SetPrescaler(IWDG_Prescaler_128);
    /* 设置复位时间 最长12位 [0,4096]*/
    //IWDG_SetReload(1250);//复位时间为2000*3.2ms=4s
#if   (defined(STM32F1))
    IWDG_SetReload((ms*32)/10);
#else
    IWDG_SetReload(ms);
#endif
    /* Reload IWDG counter IWDG->KR  0xAAAA*/
    IWDG_ReloadCounter();
    /* Enable IWDG (the LSI oscillator will be enabled by hardware)  IWDG->KR 0xCCCC*/
    IWDG_Enable();
#elif (defined(NRF51)||defined(NRF52))
    uint32_t err_code = NRF_SUCCESS;
    nrf_drv_wdt_config_t config = NRF_DRV_WDT_DEAFULT_CONFIG;
    config.reload_value=ms;
    err_code = nrf_drv_wdt_init(&config, wdt_event_handler);
    APP_ERROR_CHECK(err_code);
    err_code = nrf_drv_wdt_channel_alloc(&m_channel_id);
    APP_ERROR_CHECK(err_code);
    nrf_drv_wdt_enable();
#endif
    //
    BspWatchDogEnable   =  1;
}
void Bsp_WatchDog_Feed(void)
{
    if(BspWatchDogEnable==0)
        return;
#if   (defined(STM32F1)||defined(STM32F4))
    IWDG_ReloadCounter();
#elif (defined(NRF51)||defined(NRF52))
    nrf_drv_wdt_channel_feed(m_channel_id);
#endif
}
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

