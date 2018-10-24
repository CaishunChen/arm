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


#ifdef BSP_LED_LED1_PIN
#ifdef BSP_LED_PWM_ENABLE
static uint8_t Bspled_PwmLed1_level=0;//0-10
static uint8_t sled1_cmt=0;
#endif
#endif

#ifdef BSP_LED_LED2_PIN
#ifdef BSP_LED_PWM_ENABLE
static uint8_t Bspled_PwmLed2_level=0;//0-10
static uint8_t sled2_cmt=0;
#endif
#endif

#ifdef BSP_LED_LED3_PIN
#ifdef BSP_LED_PWM_ENABLE
static uint8_t Bspled_PwmLed3_level=0;//0-10
static uint8_t sled3_cmt=0;
#endif
#endif

#ifdef BSP_LED_LED4_PIN
#ifdef BSP_LED_PWM_ENABLE
static uint8_t Bspled_PwmLed4_level=0;//0-10
static uint8_t sled4_cmt=0;
#endif
#endif

static BSP_LED_E_MODE BspLed_ModeLed[BSP_LED_MAX_NUM]={BSP_LED_E_MODE_NULL};
static uint8_t BspLed_ModeTimerLed[BSP_LED_MAX_NUM]={0};

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
 * @note    用于LED的调光
 * @param   None
 * @return  None
 */
void BspLed_1ms(void)
{
#ifdef BSP_LED_PWM_ENABLE
    if(sled1_cmt<Bspled_PwmLed1_level)
    {
        BSP_LED_LED1_ON;
        sled1_cmt++;
    }
    else if(sled1_cmt<10)
    {
        BSP_LED_LED1_OFF;
        sled1_cmt++;
    }
    else
    {
        sled1_cmt=0;
    }

    if(sled2_cmt<Bspled_PwmLed2_level)
    {
        BSP_LED_LED2_ON;
        sled2_cmt++;
    }
    else if(sled2_cmt<10)
    {
        BSP_LED_LED2_OFF;
        sled2_cmt++;
    }
    else
    {
        sled2_cmt=0;
    }

    if(sled3_cmt<Bspled_PwmLed3_level)
    {
        BSP_LED_LED3_ON;
        sled3_cmt++;
    }
    else if(sled3_cmt<10)
    {
        BSP_LED_LED3_OFF;
        sled3_cmt++;
    }
    else
    {
        sled3_cmt=0;
    }

    if(sled4_cmt<Bspled_PwmLed4_level)
    {
        BSP_LED_LED4_ON;
        sled4_cmt++;
    }
    else if(sled4_cmt<10)
    {
        BSP_LED_LED4_OFF;
        sled4_cmt++;
    }
    else
    {
        sled4_cmt=0;
    }
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
    //
    // 测试

    {
        step++;
        switch(step)
        {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
                BspLed_PwmLed(4,step);
                break;
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
            case 26:
            case 27:
            case 28:
            case 29:
            case 30:
                BspLed_PwmLed(4,30-step);
                break;
            default:
                if(step>80)
                {
                    step=0;
                }
                break;
        }
    }
    // 非调试状态
    if(BspLed_DebugTest_Enable==0)
    {
#ifdef BSP_LED_LED1_PIN
        switch(BspLed_ModeLed1)
        {
        	case BSP_LED_E_MODE_NULL:
				break;
			case BSP_LED_E_MODE_ONCE:
				BspLed_ModeTimerLed1++;
				if(BspLed_ModeTimerLed1<10)
				{
					BSP_LED_LED1_ON;
				}
				else
				{
					BSP_LED_LED1_OFF;
					BspLed_ModeLed1	=	BSP_LED_E_MODE_NULL;
					BspLed_ModeTimerLed1	=	0;
				}
				break;
			case BSP_LED_E_MODE_FLICK:
				BspLed_ModeTimerLed1++;
				if(BspLed_ModeTimerLed1==1)
				{
					BSP_LED_LED1_ON;
				}
				else if(BspLed_ModeTimerLed1==2)
				{
					BSP_LED_LED1_OFF;
				}
				else if(BspLed_ModeTimerLed1>=10)
				{
					BspLed_ModeTimerLed1=0;
				}
				break;
			case BSP_LED_E_MODE_PWM:
				BspLed_ModeTimerLed1++;
				if(BspLed_ModeTimerLed1<=10)
				{
					BspLed_PwmLed(1,step);
				}
				else if(20<BspLed_ModeTimerLed1 && BspLed_ModeTimerLed1<=30)
				{
					BspLed_PwmLed(1,30-step);
				}
				else if(BspLed_ModeTimerLed1>=80)
				{
					BspLed_ModeTimerLed1=0;
				}
				break;
			case BSP_LED_E_MODE_ON:
				BSP_LED_LED1_ON;
				break;	
			case BSP_LED_E_MODE_OFF:
				BSP_LED_LED1_OFF;
				break;
            default:
            	BspLed_ModeTimerLed1=0;
                break;
        }
#endif
    }
    // 调试状态
    else
    {
        si++;
        if(si<5)
            return;
        si=0;
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
 * @brief   控制PWM-LED
 * @note    改写pwm值
 * @param   num         -   LED编号，1起始
            pwmValue    -   0-10
 * @return  None
 */
void BspLed_PwmLed(uint8_t num,uint8_t pwmValue)
{
    switch(num)
    {
#ifdef BSP_LED_LED1_PIN
        case 1:
#ifdef BSP_LED_PWM_ENABLE
            if(pwmValue<=10)
            {
                Bspled_PwmLed1_level=pwmValue;
            }
#else
            if(pwmValue==0)
            {
                BSP_LED_LED1_OFF;
            }
            else if(pwmValue==10)
            {
                BSP_LED_LED1_ON;
            }
            else
            {
                BSP_LED_LED1_TOGGLE;
            }
#endif
            break;
#endif
#ifdef BSP_LED_LED2_PIN
        case 2:
#ifdef BSP_LED_PWM_ENABLE

            if(pwmValue<=10)
            {
                Bspled_PwmLed2_level=pwmValue;
            }
#else
            if(pwmValue==0)
            {
                BSP_LED_LED2_OFF;
            }
            else if(pwmValue==10)
            {
                BSP_LED_LED2_ON;
            }
            else
            {
                BSP_LED_LED2_TOGGLE;
            }
#endif
            break;
#endif
#ifdef BSP_LED_LED3_PIN
        case 3:
#ifdef BSP_LED_PWM_ENABLE

            if(pwmValue<=10)
            {
                Bspled_PwmLed3_level=pwmValue;
            }
#else
            if(pwmValue==0)
            {
                BSP_LED_LED3_OFF;
            }
            else if(pwmValue==10)
            {
                BSP_LED_LED3_ON;
            }
            else
            {
                BSP_LED_LED3_TOGGLE;
            }
#endif
            break;
#endif
#ifdef BSP_LED_LED4_PIN
        case 4:
#ifdef BSP_LED_PWM_ENABLE

            if(pwmValue<=10)
            {
                Bspled_PwmLed4_level=pwmValue;
            }
#else
            if(pwmValue==0)
            {
                BSP_LED_LED4_OFF;
            }
            else if(pwmValue==10)
            {
                BSP_LED_LED4_ON;
            }
            else
            {
                BSP_LED_LED4_TOGGLE;
            }
#endif
            break;
#endif
        default:
            break;
    }
}

/**
 * @brief   控制LED模式
 * @note    模式优先级大于单控
 * @param   num         -   LED编号，1起始
            mode        -   模式
            para        -   参数
 * @return  None
 */
void BspLed_Mode(uint8_t num,BSP_LED_E_MODE mode)
{
    switch(num)
    {
#ifdef BSP_LED_LED1_PIN
        case 1:
            BspLed_ModeLed1 		=   mode;
			BspLed_ModeTimerLed1	=	0;
            break;
#endif
#ifdef BSP_LED_LED2_PIN
        case 2:
            BspLed_ModeLed2 		=   mode;
			BspLed_ModeTimerLed2	=	0;
            break;
#endif
#ifdef BSP_LED_LED3_PIN
        case 3:
            BspLed_ModeLed3 		=   mode;
			BspLed_ModeTimerLed3	=	0;
            break;
#endif
#ifdef BSP_LED_LED4_PIN
        case 4:
            BspLed_ModeLed4 		=   mode;
			BspLed_ModeTimerLed4	=	0;
            break;
#endif
        default:
            break;
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
