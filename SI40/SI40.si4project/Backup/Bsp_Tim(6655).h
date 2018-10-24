/**
  ******************************************************************************
  * @file    Bsp_Tim.h
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  * @brief   bottom-driven -->   硬件定时器驱动.
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
#ifndef __BSP_TIM_H
#define __BSP_TIM_H
//-------------------加载库函数------------------------------
#include "includes.h"
//-------------------接口宏定义(硬件相关)--------------------
#if   (defined(PROJECT_XKAP_V3)||defined(XKAP_ICARE_B_D_M))
#include "Bsp_Uart.h"
#include "uctsk_RFMS.h"
#include "Module_SdFat.h"
#define  BSP_TIM_CALL_FUNCTION1  UART_1ms_IRQ()
#define  BSP_TIM_CALL_FUNCTION2  uctsk_Rfms_1mS()
#define  BSP_TIM_CALL_FUNCTION3  SdFat_1msPro()
#elif (defined(PROJECT_TCI_V30))
#include "Bsp_HBridge.h"
#include "Bsp_StepperMotor.h"
#define  BSP_TIM_CALL_FUNCTION1  BspHBridge_1ms()
#define  BSP_TIM_CALL_FUNCTION2  BspStepperMotor_1ms()
#elif (defined(BOOTLOADER)||defined(PROJECT_BASE_STM32F1)||defined(PROJECT_BASE_STM32F4))
#elif (defined(BASE_NRF51)||defined(BASE_NRF52))
#include "nrf_drv_timer.h"
#elif (defined(TEST_NRF52_V1))
#include "nrf_drv_timer.h"
#include "Bsp_Hs0038.h"
#include "Bsp_InfraredEmitter.h"
#include "Bsp_BuzzerMusic.h"
#define BSP_TIMER_CH_A      BSP_HS0038_TIMER
#define BSP_TIMER_CH_B      BSP_INFRARED_38K_TIMER
#define BSP_TIMER_CH_C      BSP_10US_TIMER
#define BSP_TIMER_CH_D      BSP_BUZZER_MUSIC_TIMER
#define BSP_TIMER_CH_E      250
#elif (defined(PROJECT_SPI_SLAVE))
#include "Bsp_Uart.h"
#include "uctsk_SpiSlave.h"
#define  BSP_TIM_CALL_FUNCTION1  UART_1ms_IRQ()
#define  BSP_TIM_CALL_FUNCTION2  Spi_Slave_1ms()
#elif (defined(PROJECT_ARMFLY_V5_XSL))
#include "Bsp_Uart.h"
#include "bsp_touch.h"
#define  BSP_TIM_CALL_FUNCTION1  UART_1ms_IRQ()
#define  BSP_TIM_CALL_FUNCTION2  TOUCH_Scan()
#define  BSP_TIM_CALL_FUNCTION3  Count_Transplant_1ms()
#elif (defined(XKAP_ICARE_A_M))
#include "nrf_drv_timer.h"
#include "Bsp_Key.h"
#include "Bsp_NrfRfEsb.h"
#include "Bsp_Uart.h"
#define  BSP_TIM_CALL_FUNCTION1  BspKey_Main_1ms()
#define  BSP_TIM_CALL_FUNCTION2  Bsp_NrfRfEsb_1ms()
#define  BSP_TIM_CALL_FUNCTION3  UART_1ms_IRQ()
#elif (defined(XKAP_ICARE_A_S))
#include "nrf_drv_timer.h"
#include "Bsp_Key.h"
#include "Bsp_NrfRfEsb.h"
#include "uctsk_AD.h"
#include "Bsp_Uart.h"
#define  BSP_TIM_CALL_FUNCTION1  BspKey_Main_1ms()
#define  BSP_TIM_CALL_FUNCTION2  Bsp_NrfRfEsb_1ms()
#define  BSP_TIM_CALL_FUNCTION3  AD_ReadValue(NULL,NULL,NULL)
#define  BSP_TIM_CALL_FUNCTION4  UART_1ms_IRQ()
#elif (defined(XKAP_ICARE_A_C))
#include "nrf_drv_timer.h"
//#include "Bsp_Key.h"
//#include "Bsp_NrfRfEsb.h"
//#define  BSP_TIM_CALL_FUNCTION1  BspKey_Main_1ms()
//#define  BSP_TIM_CALL_FUNCTION2  Bsp_NrfRfEsb_1ms()
#elif (defined(XKAP_ICARE_B_M))
#include "Bsp_Led.h"
#include "nrf_drv_timer.h"
//#include "Bsp_NrfRfEsb.h"
#include "Bsp_Uart.h"
#define  BSP_TIM_CALL_FUNCTION1  BspLed_1ms()
//#define  BSP_TIM_CALL_FUNCTION2  Bsp_NrfRfEsb_1ms()
#define  BSP_TIM_CALL_FUNCTION3  UART_1ms_IRQ()
#elif (defined(XKAP_ICARE_B_C)||defined(PROJECT_NRF5X_BLE))
#include "nrf_drv_timer.h"
#include "Bsp_Key.h"
#define  BSP_TIM_CALL_FUNCTION1  BspKey_Main_1ms()
#elif (defined(XKAP_ICARE_B_D))
#include "nrf_drv_timer.h"
#include "Bsp_Uart.h"
#define  BSP_TIM_CALL_FUNCTION3  UART_1ms_IRQ()
#else
#error Please Set Project to Bsp_Tim.h
#endif
//-------------------接口宏定义(硬件无关)--------------------
// 延时启动定时器，为了等待任务硬件初始化完毕
#define  BSP_TIM_START_DELAY_MS  10
#if ((BSP_TIMER_CH_A==BSP_TIMER_CH_B) || (BSP_TIMER_CH_A==BSP_TIMER_CH_C) || (BSP_TIMER_CH_A==BSP_TIMER_CH_D) || (BSP_TIMER_CH_A==BSP_TIMER_CH_E) \
    ||(BSP_TIMER_CH_B==BSP_TIMER_CH_C) || (BSP_TIMER_CH_B==BSP_TIMER_CH_D) || (BSP_TIMER_CH_B==BSP_TIMER_CH_E)\
    ||(BSP_TIMER_CH_C==BSP_TIMER_CH_D) || (BSP_TIMER_CH_C==BSP_TIMER_CH_E)\
    ||(BSP_TIMER_CH_D==BSP_TIMER_CH_E))
#endif
//-------------------接口函数--------------------------------
extern volatile uint32_t Tim_1ms_Count;
extern void TIM_Configuration(uint16_t ms);
extern void TIM2_ISRHandler(void);
//-----------------------------------------------------------
#endif
