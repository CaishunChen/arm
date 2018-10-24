/*
***********************************************************************************
*                    作    者: 徐松亮
*                    更新时间: 2015-06-03
***********************************************************************************
*/
#ifndef __HCI_H
#define __HCI_H
//-------------------加载库函数------------------------------
#include "includes.h"
#include "Bsp_Pwr.h"
//-----------------------------------------------------------
//-------------------接口宏定义(硬件相关)--------------------
#if (defined(PROJECT_XKAP_V3)||defined(XKAP_ICARE_B_D_M))
//使能EMWIN
#define HCI_EMWIN_ENABLE
#if   (HARDWARE_VER==3)
#define HCI_KEY_ENTER   BSP_KEY_E_KEY4
#define HCI_KEY_ESC     BSP_KEY_E_KEY3
#define HCI_KEY_LEFT    BSP_KEY_E_KEY1
#define HCI_KEY_RIGHT   BSP_KEY_E_KEY2
#define HCI_KEY_TOP     BSP_KEY_E_KEY5
#else
#define HCI_KEY_ENTER   BSP_KEY_E_KEY2
#define HCI_KEY_ESC     BSP_KEY_E_KEY1
#define HCI_KEY_LEFT    BSP_KEY_E_KEY3
#define HCI_KEY_RIGHT   BSP_KEY_E_KEY4
#define HCI_KEY_TOP     BSP_KEY_E_KEY5
#endif
#elif (defined(PROJECT_ARMFLY_V5_XSL))
#define HCI_KEY_1     	BSP_KEY_E_KEY1
#define HCI_KEY_2     	BSP_KEY_E_KEY2
#define HCI_KEY_3     	BSP_KEY_E_KEY3
#define HCI_KEY_ENTER   BSP_KEY_E_KEY4
#define HCI_KEY_UP     	BSP_KEY_E_KEY5
#define HCI_KEY_LEFT    BSP_KEY_E_KEY6
#define HCI_KEY_RIGHT   BSP_KEY_E_KEY7
#define HCI_KEY_DOWM    BSP_KEY_E_KEY8
#elif (defined(XKAP_ICARE_A_M)||defined(XKAP_ICARE_A_S))
#elif (defined(XKAP_ICARE_B_M))
#define HCI_KEY_ENTER   BSP_KEY_E_KEY1
#define HCI_KEY_SOS_1   BSP_KEY_E_KEY3
#define HCI_KEY_SOS_2   BSP_KEY_E_KEY2
#define HCI_KEY_SOS_3   BSP_KEY_E_KEY5
#define HCI_KEY_SOS_4   BSP_KEY_E_KEY4
#elif (defined(XKAP_ICARE_B_C)||defined(PROJECT_NRF5X_BLE))
#define HCI_KEY_1       BSP_KEY_E_KEY2
#define HCI_KEY_2       BSP_KEY_E_KEY1
#elif (defined(XKAP_ICARE_B_D))
#elif (defined(PROJECT_BIB_TEST1))
#define HCI_KEY_MOVE    BSP_KEY_E_KEY1
#define HCI_KEY_SET     BSP_KEY_E_KEY2
#define HCI_KEY_ESC     BSP_KEY_E_KEY3
#else 
#error Please Set Project to uctsk_HCI.h
#endif
//
#if   (defined(PROJECT_XKAP_V3)||defined(XKAP_ICARE_B_D_M))
//-------------------接口宏定义(硬件无关)--------------------
#ifdef HCI_EMWIN_ENABLE
#include "EmWinXslTask.h"
#include "EmWinXslApp.h"
#include "QR_XslTest.h"
#else
#define LCD_GetXSize()	(320)
#endif
#define HCI_SLEEP_LIGHT_LEVEL0   0
#define HCI_SLEEP_LIGHT_LEVEL1   1
#define HCI_SLEEP_LIGHT_LEVEL2   5
//#define HCI_SLEEP_LIGHT_LEVEL3   20
//#define HCI_SLEEP_LIGHT_LEVEL4   100
//
//条件节电计时
#define HCI_ENTER_POWER_STOP_TIME_S    60
//低压节电计时
#define HCI_ENTER_BAT_LOW_TIME_S       10
//0x00BBGGRR
#define HCI_GUI_COLOR_BACKGROUND_UP    Count_4ByteToLong(0,0,0,255)
#define HCI_GUI_COLOR_BACKGROUND_DOWN  Count_4ByteToLong(0,0,255,255)
//-------------------接口变量--------------------------------
extern INT16_S_BIT Hci_s_Bit_hint;
extern BSP_PWR_E_POWER  Hci_Power;
//-------------------接口函数--------------------------------
// 启动任务
void App_HciTaskCreate(void);
void App_Hci_Init(void);
// 未启动任务
//-----------------------------------------------------------
#elif (defined(PROJECT_ARMFLY_V5_XSL))
// 启动任务
void App_HciTaskCreate(void);
#elif (defined(XKAP_ICARE_A_M)||defined(XKAP_ICARE_A_S))
#elif (defined(XKAP_ICARE_B_M))
extern void Hci_100ms(void);
extern void Hci_Rx_Parse(uint8_t *pbuf,uint8_t len);
#elif (defined(XKAP_ICARE_B_C))
extern void Hci_100ms(void);
#elif (defined(PROJECT_NRF5X_BLE))
extern void Hci_Timer(uint16_t ms);
#elif (defined(XKAP_ICARE_B_D))
extern void Hci_100ms(void);
#elif (defined(PROJECT_BIB_TEST1))
extern void App_HciTaskCreate(void);
#endif
#endif


