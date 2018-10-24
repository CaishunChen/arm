/**
  ******************************************************************************
  * @file    Bsp_StepperMotor.h
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  * @brief   bottom-driven -->   基于HS0038的NEC红外遥控解码接收.
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP_HS0038_H
#define __BSP_HS0038_H
/* Includes ------------------------------------------------------------------*/
#include "includes.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#if     (defined(PROJECT_ARMFLY_V5_XSL))
/*待调试*/
#define BSP_HS0038_PORT         GPIOB
#define BSP_HS0038_PIN          GPIO_Pin_0
#define BSP_HS0038_TIMER        TIM3
#define BSP_HS0038_RCC_ENABLE   RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOB , ENABLE);RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE)
#define BSP_HS0038_AF_CONFIG   	GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF_TIM3)
#elif   (defined(TEST_NRF52_V1))
#define BSP_HS0038_TIMER        3
#define BSP_HS0038_PIN          8
#else
#error Please Set Project to Bsp_Hs0038.h
#endif
/* Exported macro ------------------------------------------------------------*/
extern uint8_t BspHs0038_Value;
extern uint8_t BspHs0038_NewSign;
/* Exported functions --------------------------------------------------------*/
extern void BspHs0038_Init(void);
extern void BspHs0038_100ms(void);
extern void BspHs0038_DebugTestOnOff(uint8_t OnOff);
//-----------------------------------------------------------
#endif
