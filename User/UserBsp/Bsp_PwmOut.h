/**
  ******************************************************************************
  * @file    Bsp_PwmOut.h 
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  * @brief   bottom-driven -->   Pwm输出驱动.
  * @note    
  * @verbatim
    
 ===============================================================================
                     ##### How to use this driver #####
 ===============================================================================
   1,    适用芯片
         STM      :  STM32F1  STM32F4
         Nordic   :  Nrf51    Nrf52
   2,    移植步骤
   3,    验证方法
   4,    使用方法
   5,    其他说明
  @endverbatim      
  ******************************************************************************
  * @attention
  *
  * GNU General Public License (GPL) 
  *
  * <h2><center>&copy; COPYRIGHT 2017 XSLXHN</center></h2>
  ******************************************************************************
  */
#ifndef __BSP_PWMOUT_H
#define __BSP_PWMOUT_H
//-------------------加载库函数------------------------------
//-------------------接口宏定义(硬件相关)--------------------
#if   (defined(STM32F1)||defined(STM32F4))   
//---->
//端口
#define BSP_PWMOUT1_PORT            GPIOC
#define BSP_PWMOUT1_PIN             GPIO_Pin_6
#define BSP_PWMOUT2_PORT            GPIOC
#define BSP_PWMOUT2_PIN             GPIO_Pin_7
#define BSP_PWMOUT_TIMER_X          TIM3
//时钟
#define BSP_PWMOUT_TIM_RCC_ENABLE   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE)
#define BSP_PWMOUT_TIM_RCC_DISABLE  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, DISABLE)
#if   (defined(STM32F1))
#define BSP_PWMOUT_GPIO_RCC_ENABLE  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE)
#elif (defined(STM32F4))
#define BSP_PWMOUT_GPIO_RCC_ENABLE  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOC , ENABLE)
#endif
//重映射
#if   (defined(STM32F1))
#define BSP_PWMOUT1_REMAP
#define BSP_PWMOUT2_REMAP
#elif (defined(STM32F4))
#define BSP_PWMOUT1_REMAP           GPIO_PinAFConfig(BSP_PWMOUT1_PORT,GPIO_PinSource6,GPIO_AF_TIM3)
#define BSP_PWMOUT2_REMAP           GPIO_PinAFConfig(BSP_PWMOUT2_PORT,GPIO_PinSource7,GPIO_AF_TIM3)
#endif
//<----
#elif (defined(NRF51)||defined(NRF52))
//---->
//<----
#endif
//-------------------接口宏定义(硬件无关)--------------------
//-------------------接口函数--------------------------------
void BspPwmOut_Init(void);
void BspPwmOut_Write(uint8_t num,uint32_t freq,uint8_t duty);
void BspPwmOut_test_10ms(void);
void BspPwmOut_DebugTestOnOff(uint8_t OnOff);
//-------------------------------------------------------------------------------输出函数
#endif

