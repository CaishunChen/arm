/*
***********************************************************************************
*                    作    者: 徐松亮
*                    更新时间: 2015-06-03
***********************************************************************************
*/

#ifndef __BLUETOOTH_BEACON_M_H
#define __BLUETOOTH_BEACON_M_H
//-------------------加载库函数------------------------------
#include "includes.h"
//-------------------接口宏定义(硬件相关)--------------------
#if   (defined(STM32F1))
#define BLUETOOTH_RCC_ENABLE     RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOE , ENABLE)
#elif (defined(STM32F4))
#define BLUETOOTH_RCC_ENABLE     RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOE , ENABLE)
#endif
#define BLUETOOTH_PWR_PORT       GPIOE
#define BLUETOOTH_PWR_PIN        GPIO_Pin_4

#define BLUETOOTH_TX_PORT        GPIOD
#define BLUETOOTH_TX_PIN         GPIO_Pin_8
#define BLUETOOTH_RX_PORT        GPIOD
#define BLUETOOTH_RX_PIN         GPIO_Pin_9
//-------------------接口宏定义(硬件无关)--------------------
//控制LED亮灭
#if   (defined(PROJECT_XKAP_V1))
#define BLUETOOTH_PWR_ON         GPIO_ResetBits(BLUETOOTH_PWR_PORT , BLUETOOTH_PWR_PIN)
#define BLUETOOTH_PWR_OFF        GPIO_SetBits(BLUETOOTH_PWR_PORT , BLUETOOTH_PWR_PIN)
#elif (defined(PROJECT_XKAP_V2))
#define BLUETOOTH_PWR_ON         GPIO_SetBits(BLUETOOTH_PWR_PORT , BLUETOOTH_PWR_PIN)
#define BLUETOOTH_PWR_OFF        GPIO_ResetBits(BLUETOOTH_PWR_PORT , BLUETOOTH_PWR_PIN)
#endif
//-------------------接口变量--------------------------------
//长供电使能
extern uint8_t Bluetooth_PowerOnHold_Enable; 
//连接使能
extern uint8_t Bluetooth_ConnectSign;
//统计数据
extern uint16_t Bluetooth_ChipPowerCount;
extern uint16_t Bluetooth_ChipOkCount;
extern uint16_t Bluetooth_ChipErrCount;
extern uint16_t Bluetooth_ChipNCCount;
//-------------------接口函数--------------------------------
void Bluetooth_RxIrqBufToRBuf0(uint8_t *pRxIqBuf,uint16_t len);
uint8_t Bluetooth_BufferLoopPop(uint8_t *pBuf,uint16_t *plen);
void App_BluetoothTaskCreate(void);
void Bluetooth_DebugTestOnOff(uint8_t OnOff);

#endif


