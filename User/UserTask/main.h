/**
  ******************************************************************************
  * @file    main.h
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  * @brief   Top-Level  -->   Main.
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
         无
  @endverbatim
  ******************************************************************************
  * @attention
  *
  * GNU General Public License (GPL)
  *
  * <h2><center>&copy; COPYRIGHT 2017 XSLXHN</center></h2>
  ******************************************************************************
  */
#ifndef __MAIN_H
#define __MAIN_H
/* Includes ------------------------------------------------------------------*/
#if (defined(STM32F1)||defined(STM32F4))
#include "Bsp_Tim.h"
#include "Bsp_Rtc.h"
#include "uctsk_Sensor.h"
#include "IAP.h"
#include "uctsk_Debug.h"
#include "Bsp_WatchDog.h"
#include "Bsp_CpuFlash.h"
#include "Module_Memory.h"
#include "SEGGER_RTT.h"
#elif(defined(NRF51)||defined(NRF52))
#include "nrf_drv_clock.h"
#include "Bsp_Pwr.h"
#include "Bsp_Rtc.h"
#include "Bsp_Tim.h"
#include "Module_Memory.h"
#include "SEGGER_RTT.h"
#endif

#ifdef STM32F1
#include "HW_Config.h"
#endif
#ifdef STM32F4
#include "Bsp_Uart.h"
#endif
//-------------------接口宏定义(硬件相关)--------------------
#if (defined(PROJECT_BASE_STM32F1))
//---->
//初始化(1-20)
//#define MAIN_BSP_INIT_X
//任务(1-20)
//#define MAIN_TASK_X
//<----
#elif (defined(PROJECT_BASE_STM32F4))
//---->
//初始化(1-20)
//#define MAIN_BSP_INIT_X
//任务(1-20)
//#define MAIN_TASK_X
//<----
#elif (defined(PROJECT_TCI_V30))
//---->
#include "uctsk_UserTimer.h"
#include "Bsp_Led.h"
#include "Bsp_StepperMotor.h"
#include "Bsp_NXP74HC165.h"
//初始化(1-20)
#define MAIN_BSP_INIT_1    BspLed_Init()
#define MAIN_BSP_INIT_2    BspStepperMotor_Init()
#define MAIN_BSP_INIT_3    BspNXP74HC165_Init()
#define MAIN_BSP_INIT_4    TIM_Configuration(1);
//任务(1-20)
#define MAIN_TASK_1        App_UserTimer100msTaskCreate()
//<----
#elif (defined(PROJECT_XKAP_V3)||defined(XKAP_ICARE_B_D_M))
//---->
#include "Language.h"
#include "Bsp_Led.h"
#include "Bsp_Key.h"
#include "uctsk_AD.h"
#include "uctsk_RFMS.h"
#include "uctsk_GprsNet.h"
#include "uctsk_GprsAppXkap.h"
#include "uctsk_HCI.h"
#include "uctsk_UserTimer.h"
#include "Bsp_IllumSensor.h"
#include "Bsp_TempHumidSensor.h"
//#include "Bsp_Twi.h"
#include "Bsp_WatchDog.h"
//#include "uctsk_GasModule.h"
#include "uctsk_UartIcam.h"
#include "uctsk_BluetoothDTU.h"
#include "SEGGER_RTT.h"
//初始化
#define MAIN_BSP_INIT_1    Language_Init()
#define MAIN_BSP_INIT_2    BspLed_Init()
#define MAIN_BSP_INIT_3    BspKey_Init()
#define MAIN_BSP_INIT_4    ADC_Configuration()
#define MAIN_BSP_INIT_5    BspPwmOut_Init()
#define MAIN_BSP_INIT_6    BspIllumSensor_Init()
//#define MAIN_BSP_INIT_6    BspTwi_Init()
#define MAIN_BSP_INIT_7    App_Hci_Init()
#define MAIN_BSP_INIT_8    BluetoothDtu_Init()
#define MAIN_BSP_INIT_9    TIM_Configuration(1);
#if		(HARDWARE_VER==0xF1)
#include "Bsp_Esp8266.h"
#define MAIN_BSP_INIT_10   BspEsp8266_Init()
#endif
//#define MAIN_BSP_INIT_7    Bsp_WatchDog_init(5000)
//任务
#if   	(HARDWARE_VER==7)
#define MAIN_TASK_1        App_UartIcamTaskCreate()
#elif	(HARDWARE_VER==0xF1)
#else
#define MAIN_TASK_1        App_GsmParseTaskCreate()
#define MAIN_TASK_2        App_GsmSendTaskCreate()
#define MAIN_TASK_3        App_GsmTestTaskCreate()
#define MAIN_TASK_4        App_GprsAppXkapTaskCreate()
#endif

#define MAIN_TASK_5        App_HciTaskCreate()
#define MAIN_TASK_6        App_SensorTaskCreate()
#define MAIN_TASK_7        App_ADTaskCreate()
#define MAIN_TASK_8        App_UserTimer10msTaskCreate()
#define MAIN_TASK_9        App_UserTimer100msTaskCreate()
//#define MAIN_TASK_10       App_GasModuleTaskCreate()
#define MAIN_TASK_11       App_RfmsTaskCreate()
#define MAIN_TASK_12       App_RfmsTaskTestCreate()
#define MAIN_TASK_13       App_RfmsTaskTxCreate()
//<----
#elif (defined(PROJECT_SPI_SLAVE))
//---->
#include "uctsk_UserTimer.h"
#include "uctsk_SpiSlave.h"
#include "uctsk_AD.h"
#include "Bsp_Key.h"
#include "Bsp_Led.h"
//初始化(1-20)
#define MAIN_BSP_INIT_1    BspKey_Init()
#define MAIN_BSP_INIT_2    BspLed_Init()
#define MAIN_BSP_INIT_3    TIM_Configuration(1);
//任务(1-20)
#define MAIN_TASK_1        App_SpiTaskCreate()
#define MAIN_TASK_2        App_ADTaskCreate()
#define MAIN_TASK_3        App_UserTimer10msTaskCreate()
#define MAIN_TASK_4        App_UserTimer100msTaskCreate()
//<----
#elif (defined(PROJECT_ARMFLY_V5_XSL))
//---->
#include "Bsp_Led.h"
#include "Bsp_Key.h"
#include "Bsp_GpioIIC.h"
#include "Bsp_FsmcSram.h"
#include "Bsp_NorFlash.h"
#include "Bsp_Ds18b20.h"
#include "uctsk_UserTimer.h"
#include "uctsk_HCI.h"
#include "Language.h"
//初始化(1-20)
#define MAIN_BSP_INIT_1    Language_Init()
#define MAIN_BSP_INIT_2    BspLed_Init()
#define MAIN_BSP_INIT_3    BspKey_Init()
#define MAIN_BSP_INIT_4    BspGpioIIC_Init()
#define MAIN_BSP_INIT_5    BspFsmcSram_Init()
#define MAIN_BSP_INIT_6    BspNorFlash_Init()
#define MAIN_BSP_INIT_7    BspDs18b20_Init()
#define MAIN_BSP_INIT_8    TIM_Configuration(1);
//任务(1-20)
#define MAIN_TASK_1        App_UserTimer10msTaskCreate()
#define MAIN_TASK_2        App_UserTimer100msTaskCreate()
#define MAIN_TASK_3        App_HciTaskCreate()
//<----
#elif (defined(BASE_NRF51)||defined(BASE_NRF52))
//---->
#include "Bsp_Led.h"
#include "uctsk_Debug.h"
#include "SEGGER_RTT.h"
#include "Bsp_NrfBle.h"
//初始化(1-20)
#define MAIN_BSP_INIT_1    BspLed_Init()
#define MAIN_BSP_INIT_2    BspNrfBle_Init()
//任务(1-20)
//#define MAIN_TASK_X
//<----
#elif (defined(TEST_NRF52_V1))
//---->
#include "Bsp_Led.h"
#include "uctsk_Debug.h"
#include "SEGGER_RTT.h"
#include "Bsp_NrfBle.h"
#include "uctsk_UserTimer.h"
#include "Bsp_Key.h"
#include "Bsp_Twi.h"
#include "Bsp_Ap3216c.h"
#include "Bsp_Hx711.h"
#include "uctsk_AD.h"
#include "Bsp_HBridge.h"
#include "Bsp_Hs0038.h"
#include "Bsp_InfraredEmitter.h"
#include "Bsp_StepperMotor.h"
#include "Bsp_Dht11.h"
#include "Bsp_Esp8266.h"
#include "uctsk_GprsNet.h"
//#include "uctsk_GprsAppXkap.h"
//初始化(1-20)
#define MAIN_BSP_INIT_1    BspLed_Init()
#define MAIN_BSP_INIT_2    BspTwi_Init()
#define MAIN_BSP_INIT_3    BspKey_Init()
//#define MAIN_BSP_INIT_4    TIM_Configuration(1);
#define MAIN_BSP_INIT_5    BspNrfBle_Init()
#define MAIN_BSP_INIT_6    BspAp3216c_Init()
#define MAIN_BSP_INIT_7    BspHx711_Init()
#define MAIN_BSP_INIT_8    ADC_Configuration()
#define MAIN_BSP_INIT_9    BspHBridge_Init()
#define MAIN_BSP_INIT_10   BspHs0038_Init()		//与 BspNrfBle_Init()冲突
//#define MAIN_BSP_INIT_11    Bsp_Infrared_Emitter_Init()
#define MAIN_BSP_INIT_12   BspStepperMotor_Init()
#define MAIN_BSP_INIT_13   BspDhtXX_Init()
#if		(SOFTWARE_VER==0)
#define MAIN_BSP_INIT_14   BspEsp8266_Init()
#endif
//任务(1-20)
#define MAIN_TASK_1		   App_UserTimer100msTaskCreate()
#if		(SOFTWARE_VER==1)
#define MAIN_TASK_2        App_GsmParseTaskCreate()
#define MAIN_TASK_3        App_GsmSendTaskCreate()
#define MAIN_TASK_4        App_GsmTestTaskCreate()
#endif
#elif (defined(XKAP_ICARE_A_M))
//---->
#include "Bsp_Led.h"
#include "Bsp_Key.h"
#include "uctsk_Debug.h"
#include "Bsp_NrfRfEsb.h"
#include "uctsk_UserTimer.h"
#include "uctsk_GprsNet.h"
#include "uctsk_GprsAppXkap.h"
#include "SEGGER_RTT.h"
//初始化(1-20)
#define MAIN_BSP_INIT_1    BspLed_Init()
#define MAIN_BSP_INIT_2    BspKey_Init()
#define MAIN_BSP_INIT_3    Bsp_NrfRfEsb_Init()
#define MAIN_BSP_INIT_4    TIM_Configuration(1);
//任务(1-20)
#define MAIN_TASK_1        App_UserTimer100msTaskCreate()
#define MAIN_TASK_2        App_GsmParseTaskCreate()
#define MAIN_TASK_3        App_GsmSendTaskCreate()
#define MAIN_TASK_4        App_GsmTestTaskCreate()
#define MAIN_TASK_5        App_GprsAppXkapTaskCreate()
//<----
#elif (defined(XKAP_ICARE_A_S))
//---->
#include "Bsp_Led.h"
#include "Bsp_Key.h"
#include "Bsp_NrfRfEsb.h"
#include "uctsk_Debug.h"
#include "uctsk_AD.h"
//初始化(1-20)
#define MAIN_BSP_INIT_1    BspLed_Init()
#define MAIN_BSP_INIT_2    BspKey_Init()
#define MAIN_BSP_INIT_3    Bsp_NrfRfEsb_Init()
#define MAIN_BSP_INIT_4    TIM_Configuration(1);
//任务(1-20)
#define MAIN_TASK_1        App_ADTaskCreate()
//<----
#elif (defined(XKAP_ICARE_A_C))
//---->
#include "Bsp_Led.h"
#include "Bsp_Key.h"
#include "Bsp_NrfRfEsb.h"
#include "uctsk_Debug.h"
//初始化(1-20)
#define MAIN_BSP_INIT_1    BspLed_Init()
#define MAIN_BSP_INIT_2    BspKey_Init()
#define MAIN_BSP_INIT_3    Bsp_NrfRfEsb_Init()
//#define MAIN_BSP_INIT_4    TIM_Configuration(1);
//任务(1-20)
//<----
#elif (defined(XKAP_ICARE_B_M))
//---->
#include "Bsp_Led.h"
#include "Bsp_Key.h"
#include "Bsp_PwmOut.h"
#include "uctsk_Debug.h"
#include "uctsk_UserTimer.h"
#include "Bsp_Hx711.h"
#include "SEGGER_RTT.h"
#include "Bsp_Twi.h"
//#include "uctsk_AD.h"
#if   (defined(HAIER))
#include "Bsp_NrfBle.h"
#elif	(HARDWARE_SUB_VER==1||HARDWARE_SUB_VER==2)
#include "Bsp_Uart.h"
#include "Bsp_NrfRfEsb.h"
#include "uctsk_GprsNet.h"
#include "uctsk_GprsAppXkap.h"
#elif	(HARDWARE_SUB_VER==3)
#include "Bsp_Esp8266.h"
#include "uctsk_GprsAppXkap.h"
#endif
//初始化(1-20)
#define MAIN_BSP_INIT_1    BspLed_Init()
#define MAIN_BSP_INIT_2    BspKey_Init()
#define MAIN_BSP_INIT_3    BspHx711_Init()
#define MAIN_BSP_INIT_4    BspAp3216c_Init()
//#define MAIN_BSP_INIT_4    ADC_Configuration()
#if   (defined(HAIER))
#define MAIN_BSP_INIT_5    BspNrfBle_Init()
#elif	(HARDWARE_SUB_VER==1||HARDWARE_SUB_VER==2)
#define MAIN_BSP_INIT_5    Bsp_NrfRfEsb_Init()
#define MAIN_BSP_INIT_6    TIM_Configuration(1);
#elif	(HARDWARE_SUB_VER==3)
#define MAIN_BSP_INIT_6    TIM_Configuration(1);
#define MAIN_BSP_INIT_7    BspEsp8266_Init()
#endif
//

//任务(1-20)
#define MAIN_TASK_1        App_UserTimer100msTaskCreate()
#if   (defined(HAIER))
#elif	(HARDWARE_SUB_VER==1||HARDWARE_SUB_VER==2)
#define MAIN_TASK_2        App_UserTimer10msTaskCreate()
#define MAIN_TASK_3        App_GsmParseTaskCreate()
#define MAIN_TASK_4        App_GsmSendTaskCreate()
#define MAIN_TASK_5        App_GsmTestTaskCreate()
#define MAIN_TASK_6        App_GprsAppXkapTaskCreate()
#elif	(HARDWARE_SUB_VER==3)
#define MAIN_TASK_2        App_UserTimer10msTaskCreate()
#define MAIN_TASK_3        App_GprsAppXkapTaskCreate()
#endif
//<----
#elif (defined(PROJECT_NRF5X_BLE))
//---->
#include "Bsp_Led.h"
#include "uctsk_Debug.h"
#include "SEGGER_RTT.h"
#include "Bsp_Key.h"
#include "uctsk_UserTimer.h"
#include "Bsp_Twi.h"
#include "Bsp_NrfBle.h"
//初始化(1-20)
#define MAIN_BSP_INIT_1    BspLed_Init()
#define MAIN_BSP_INIT_2    BspKey_Init()
#define MAIN_BSP_INIT_3    BspTwi_Init()
#define MAIN_BSP_INIT_4    BspNrfBle_Init()
//任务(1-20)
#define MAIN_TASK_1        App_UserTimer100msTaskCreate()
//<----
#elif (defined(XKAP_ICARE_B_C))
//---->
#include "Bsp_Led.h"
#include "uctsk_Debug.h"
#include "SEGGER_RTT.h"
#include "Bsp_Key.h"
#include "uctsk_UserTimer.h"
#include "Bsp_NrfBle.h"
//初始化(1-20)
#define MAIN_BSP_INIT_1    BspLed_Init()
#define MAIN_BSP_INIT_2    BspKey_Init()
#define MAIN_BSP_INIT_3    BspNrfBle_Init()
//任务(1-20)
#define MAIN_TASK_1        App_UserTimer100msTaskCreate()
//<----
#elif (defined(XKAP_ICARE_B_D))
//---->
#include "Bsp_Led.h"
#include "uctsk_Debug.h"
#include "SEGGER_RTT.h"
#include "uctsk_UserTimer.h"
#include "Bsp_NrfBle.h"
//初始化(1-20)
#define MAIN_BSP_INIT_1    BspLed_Init()
#define MAIN_BSP_INIT_2    BspNrfBle_Init()
//任务(1-20)
#define MAIN_TASK_1        App_UserTimer100msTaskCreate()
//<----
#else
//---->
#error Please Set Project to main.h
//<----
#endif
//-----------------------------------------------------------
#endif

