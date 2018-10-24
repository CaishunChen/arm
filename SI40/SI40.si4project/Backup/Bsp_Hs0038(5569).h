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
typedef struct
{
    uint32_t LastCapture;
    uint8_t Status;
    uint8_t RxBuf[4];
    uint8_t RepeatCount;
} BSP_HS0038_S_INFO;
/* Exported constants --------------------------------------------------------*/
#if     (defined(PROJECT_ARMFLY_V5_XSL))
#define BSP_HS0038_PORT         GPIOB
#define BSP_HS0038_PIN          GPIO_Pin_0
#define BSP_HS0038_TIMER        TIM3
#define BSP_HS0038_RCC_ENABLE   RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOB , ENABLE);
#elif   (defined(TEST_NRF52_V1))
#define BSP_HS0038_TIMER        4
#define BSP_HS0038_PIN          8
#define BSP_HS0038_PPI_CHANNEL  NRF_PPI_CHANNEL0
#else
#error Please Set Project to Bsp_Hs0038.h
#endif
/* Exported macro ------------------------------------------------------------*/
extern bool Hs0038_Rx_Sem;
extern BSP_HS0038_S_INFO g_tIR;
/* Exported functions --------------------------------------------------------*/
extern void BspHs0038_Init(void);
//-----------------------------------------------------------
#endif
