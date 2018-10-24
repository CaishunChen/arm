/**
  ******************************************************************************
  * @file    Bsp_HBridge0.h 
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  * @brief   bottom-driven -->   基于HBridge的直流电机驱动.
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
#ifndef __H_BRIDGE_H
#define __H_BRIDGE_H
/* Includes ------------------------------------------------------------------*/
#include "includes.h"
/* Exported constants --------------------------------------------------------*/
/* Transplant define ---------------------------------------------------------*/
#if	(defined(PROJECT_TCI_V30))
//---->
//时钟
#if   (defined(STM32F1))
#define BSP_HBRIDGE_RCC_ENABLE    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOG , ENABLE);
#elif (defined(STM32F4))
#define BSP_HBRIDGE_RCC_ENABLE    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOG , ENABLE);
#endif
//端口
#define HBRIDGE_1_P_GPIOX  GPIOA
#define HBRIDGE_1_P_PINX   GPIO_Pin_7
#define HBRIDGE_1_N_GPIOX  GPIOG
#define HBRIDGE_1_N_PINX   GPIO_Pin_9
#define HBRIDGE_2_P_GPIOX  GPIOC
#define HBRIDGE_2_P_PINX   GPIO_Pin_7
#define HBRIDGE_2_N_GPIOX  GPIOC
#define HBRIDGE_2_N_PINX   GPIO_Pin_6
#define HBRIDGE_3_P_GPIOX  GPIOD
#define HBRIDGE_3_P_PINX   GPIO_Pin_6
#define HBRIDGE_3_N_GPIOX  GPIOD
#define HBRIDGE_3_N_PINX   GPIO_Pin_5
#define HBRIDGE_4_P_GPIOX  GPIOG
#define HBRIDGE_4_P_PINX   GPIO_Pin_12
#define HBRIDGE_4_N_GPIOX  GPIOG
#define HBRIDGE_4_N_PINX   GPIO_Pin_13
//数量
#define HBRIDGE_MAX_NUM    4

//<----
#elif (defined(TEST_NRF52_V1))
//---->
#define HBRIDGE_1_P_PINX   26
#define HBRIDGE_1_N_PINX   23
#define HBRIDGE_MAX_NUM    1
//<----
#else
#error Please Set Project to Bsp_HBridge.h
#endif
//-------------------接口宏定义(硬件无关)--------------------
//-------------------接口变量--------------------------------
extern long BspHBridge_1msTimerBuf[HBRIDGE_MAX_NUM]; 
//-------------------接口函数--------------------------------
extern void BspHBridge_Init(void);
extern void BspHBridge_1ms(void);
//-----------------------------------------------------------
#endif

