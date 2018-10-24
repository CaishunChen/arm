/*
***********************************************************************************
*                    作    者: 徐松亮
*                    更新时间: 2015-06-03
***********************************************************************************
*/
//--------------------------------
#ifndef __UCTSK_USER_TIMER_H
#define __UCTSK_USER_TIMER_H
//-------------------加载库函数------------------------------
#include "includes.h"
#include "uctsk_Debug.h"
//-------------------接口宏定义(硬件相关)--------------------
#if   (defined(PROJECT_XKAP_V3)||defined(XKAP_ICARE_B_D_M))
#include "Bsp_PwmOut.h"
#include "Bsp_Key.h"
#include "Bsp_BuzzerMusic.h"
#include "Bsp_IllumSensor.h"
#include "Bsp_TempHumidSensor.h"
#include "uctsk_RFMS.h"
#include "Bsp_Led.h"
#include "uctsk_BluetoothDTU.h"
#define  USER_TIMER_10MS_CH1  BspPwmOut_test_10ms()
#define  USER_TIMER_10MS_CH2  BspKey_Main_10ms()
#define  USER_TIMER_10MS_CH3  Bsp_BuzzerMusic_PlayMusic_10ms()
#define  USER_TIMER_100MS_CH1 BspIllumSensor_DebugTest_100ms()
#define  USER_TIMER_100MS_CH2 BspTempHumidSensor_DebugTest_100ms()
#define  USER_TIMER_100MS_CH3 uctsk_Rfms_DataSaveToTf_100ms()
#define  USER_TIMER_100MS_CH4 Key_Main_DebugTest_100ms()
#define  USER_TIMER_100MS_CH5 BluetoothDtu_100ms()
#if   (defined(XKAP_ICARE_B_D_M))
#define  USER_TIMER_100MS_CH6 BSP_LED_LED1_TOGGLE
#endif
#elif (defined(PROJECT_SPI_SLAVE))
#include "Bsp_Key.h"
#include "Bsp_Led.h"
#include "Bsp_Relay.h"
//#include "uctsk_SpiMaster.h"
#define  USER_TIMER_10MS_CH1  BspKey_Main_10ms()
#define  USER_TIMER_100MS_CH1 BSP_LED_LED1_TOGGLE
#define  USER_TIMER_100MS_CH2 BspRelay_100ms()
#define  USER_TIMER_100MS_CH3 Key_Main_DebugTest_100ms()
//#define  USER_TIMER_100MS_CH2 SpiMaster_DebugTestOnOff(ON)
#elif (defined(PROJECT_ARMFLY_V5_XSL))
#include "Bsp_Key.h"
#include "Bsp_Ds18b20.h"
#define  USER_TIMER_10MS_CH1  BspKey_Main_10ms()
#define  USER_TIMER_100MS_CH1 BspDs18b20_DebugTest_100ms()
#define  USER_TIMER_100MS_CH2 Key_Main_DebugTest_100ms()
#elif (defined(PROJECT_BIB_TEST1))
#include "Bsp_Led.h"
#include "Bsp_Key.h"
#include "Pulse_ADC.h"
#define  USER_TIMER_10MS_CH1  BspKey_Main_10ms()
#define  USER_TIMER_100MS_CH1 BSP_LED_LED1_TOGGLE
#define  USER_TIMER_100MS_CH2 PulseADC_100ms()
#define  USER_TIMER_100MS_CH3 Key_Main_DebugTest_100ms()
#elif (defined(XKAP_ICARE_A_M))
#include "Bsp_Led.h"
#define  USER_TIMER_100MS_CH1 {BSP_LED_LED2_TOGGLE;}
#elif (defined(XKAP_ICARE_B_M))
#include "Bsp_Hx711.h"
#include "Bsp_BuzzerMusic.h"
#include "Bsp_Key.h"
#include "uctsk_HCI.h"
#include "Module_Memory.h"
#include "Bsp_Led.h"
#include "Bsp_Ap3216c.h"
#define  USER_TIMER_10MS_CH1  {Bsp_BuzzerMusic_PlayMusic_10ms();}
#define  USER_TIMER_10MS_CH2  {BspKey_Main_10ms();}
#define  USER_TIMER_100MS_CH1 BspHx711_100ms()
#define  USER_TIMER_100MS_CH2 Hci_100ms()
#define  USER_TIMER_100MS_CH3 BspLed_100ms()
#define  USER_TIMER_100MS_CH4 Key_Main_DebugTest_100ms()
#define  USER_TIMER_100MS_CH5 BspAp3216c_DebugTest_100ms()
#if   (defined(HAIER))
#include "Bsp_NrfBle.h"
#define  USER_TIMER_100MS_CH6 BspNrfBle_Timer(100)
#endif
#elif (defined(PROJECT_NRF5X_BLE))
#include "uctsk_HCI.h"
#include "Bsp_Bmp180.h"
#include "Bsp_Ap3216c.h"
#include "Bsp_Mpu6050.h"
#include "Bsp_Led.h"
#include "Bsp_NrfBle.h"
#include "Bsp_Key.h"
#define  USER_TIMER_100MS_CH1 Hci_Timer(100)
#define  USER_TIMER_100MS_CH2 BspBmp180_DebugTest_100ms()
#define  USER_TIMER_100MS_CH3 BspAp3216c_DebugTest_100ms()
#define  USER_TIMER_100MS_CH4 BspMpu6050_DebugTest_100ms()
#define  USER_TIMER_100MS_CH5 BspLed_100ms()
#define  USER_TIMER_100MS_CH6 BspNrfBle_Timer(100)
#define  USER_TIMER_100MS_CH7 Key_Main_DebugTest_100ms()
#elif (defined(XKAP_ICARE_B_C))
#include "uctsk_HCI.h"
//#include "Bsp_Bmp180.h"
//#include "Bsp_Ap3216c.h"
//#include "Bsp_Mpu6050.h"
#include "Bsp_Led.h"
#include "Bsp_NrfBle.h"
#include "Bsp_Key.h"
#define  USER_TIMER_100MS_CH1 Hci_100ms()
//#define  USER_TIMER_100MS_CH2 BspBmp180_DebugTest_100ms()
//#define  USER_TIMER_100MS_CH3 BspAp3216c_DebugTest_100ms()
//#define  USER_TIMER_100MS_CH4 BspMpu6050_DebugTest_100ms()
#define  USER_TIMER_100MS_CH5 BspLed_100ms()
#define  USER_TIMER_100MS_CH6 BspNrfBle_Timer(100)
#define  USER_TIMER_100MS_CH7 Key_Main_DebugTest_100ms()
#elif (defined(XKAP_ICARE_B_D))
#include "Bsp_Led.h"
#include "Bsp_NrfBle.h"
#define  USER_TIMER_100MS_CH1 BspLed_100ms()
#define  USER_TIMER_100MS_CH2 BspNrfBle_Timer(100)
#elif (defined(PROJECT_TCI_V30))
#include "Bsp_Relay.h"
#include "Bsp_NXP74HC165.h"
#define  USER_TIMER_100MS_CH1 BspRelay_100ms()
#define  USER_TIMER_100MS_CH2 BspNXP74HC165_100ms()
#else 
#error Please Set Project to uctsk_UserTimer.h
#endif
//-------------------接口宏定义(硬件无关)--------------------
//-------------------接口变量--------------------------------
//-------------------接口函数--------------------------------
extern void App_UserTimer10msTaskCreate(void);
extern void App_UserTimer100msTaskCreate(void);
//-----------------------------------------------------------
#endif

