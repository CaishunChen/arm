/**
  ******************************************************************************
  * @file    Bsp_Led.c
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

/* Includes ------------------------------------------------------------------*/
#include "Bsp_Led.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint8_t BspLed_DebugTest_Enable=0;
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @brief   LED初始化函数
 * @note    初始化GPIO
 * @param   None
 * @return  None
 */
void BspLed_Init(void)
{
#if   (defined(STM32F1)||defined(STM32F4))
    GPIO_InitTypeDef GPIO_InitStructure;
    BSP_LED_RCC_ENABLE;
#if   (defined(STM32F1))
    GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_Out_PP;
#elif (defined(STM32F4))
    GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType   =  GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd    =  GPIO_PuPd_NOPULL;
#endif
    //
#ifdef BSP_LED_LED1_PORT
    GPIO_InitStructure.GPIO_Pin     =  BSP_LED_LED1_PIN;
    GPIO_Init(BSP_LED_LED1_PORT, &GPIO_InitStructure);
    BSP_LED_LED1_OFF;
#endif
#ifdef BSP_LED_LED2_PORT
    GPIO_InitStructure.GPIO_Pin     =  BSP_LED_LED2_PIN;
    GPIO_Init(BSP_LED_LED2_PORT, &GPIO_InitStructure);
    BSP_LED_LED2_OFF;
#endif
#ifdef BSP_LED_LED3_PORT
    GPIO_InitStructure.GPIO_Pin     =  BSP_LED_LED3_PIN;
    GPIO_Init(BSP_LED_LED3_PORT, &GPIO_InitStructure);
    BSP_LED_LED3_OFF;
#endif
#ifdef BSP_LED_LED4_PORT
    GPIO_InitStructure.GPIO_Pin     =  BSP_LED_LED4_PIN;
    GPIO_Init(BSP_LED_LED4_PORT, &GPIO_InitStructure);
    BSP_LED_LED4_OFF;
#endif
#elif(defined(NRF52)||defined(NRF51))
#ifdef BSP_LED_LED1_PIN
    nrf_gpio_cfg_output(BSP_LED_LED1_PIN);
    BSP_LED_LED1_OFF;
#endif
#ifdef BSP_LED_LED2_PIN
    nrf_gpio_cfg_output(BSP_LED_LED2_PIN);
    BSP_LED_LED2_OFF;
#endif
#ifdef BSP_LED_LED3_PIN
    nrf_gpio_cfg_output(BSP_LED_LED3_PIN);
    BSP_LED_LED3_OFF;
#endif
#ifdef BSP_LED_LED4_PIN
    nrf_gpio_cfg_output(BSP_LED_LED4_PIN);
    BSP_LED_LED4_OFF;
#endif
#endif
}

/**
 * @brief   周期执行函数
 * @note    用于LED的Debug测试,开启Debug相关指令后会循环点亮
 * @param   None
 * @return  None
 */
void BspLed_100ms(void)
{
    static uint8_t si=0;
    static uint8_t step=0;
    si++;
    if(si<5)
        return;
    si=0;
    if(BspLed_DebugTest_Enable==1)
    {
        switch(step)
        {
            case 0:
#ifdef BSP_LED_LED1_PIN
                BSP_LED_LED1_ON;
                step++;
                break;
            case 1:
                BSP_LED_LED1_OFF;
                step++;
#else
                step=2;
#endif
#ifdef BSP_LED_LED2_PIN
            case 2:
                BSP_LED_LED2_ON;
                step++;
                break;
            case 3:
                BSP_LED_LED2_OFF;
                step++;
#else
                step=4;
#endif
#ifdef BSP_LED_LED3_PIN
            case 4:
                BSP_LED_LED3_ON;
                step++;
                break;
            case 5:
                BSP_LED_LED3_OFF;
                step++;
#else
                step=6;
#endif
#ifdef BSP_LED_LED4_PIN
            case 6:
                BSP_LED_LED4_ON;
                step++;
                break;
            case 7:
                BSP_LED_LED4_OFF;
                step++;
#endif
            default:
                step=0;
                break;
        }
    }
}
/**
 * @brief   Debug接口函数
 * @note    开启Debug相关指令后会循环点亮，关闭Debug后所有LED全部关闭
 * @param   OnOff-判断指令进入与退出
 * @return  None
 */
void BspLed_DebugTestOnOff(uint8_t OnOff)
{
    if(OnOff==ON)
    {
        BspLed_DebugTest_Enable=1;
    }
    else
    {
        BspLed_DebugTest_Enable=0;
#ifdef BSP_LED_LED1_PIN
        BSP_LED_LED1_OFF;
#endif
#ifdef BSP_LED_LED2_PIN
        BSP_LED_LED2_OFF;
#endif
#ifdef BSP_LED_LED3_PIN
        BSP_LED_LED3_OFF;
#endif
#ifdef BSP_LED_LED4_PIN
        BSP_LED_LED4_OFF;
#endif
    }
}

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
