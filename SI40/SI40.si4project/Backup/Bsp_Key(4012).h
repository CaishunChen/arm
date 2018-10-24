/**
  ******************************************************************************
  * @file    Bsp_Key.h 
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  * @brief   bottom-driven -->   按键驱动.
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
//-------------------------------------------------------------------------------
#ifndef __BSP_KEY_H
#define __BSP_KEY_H
//-------------------加载库函数------------------------------
#include "includes.h"
//-------------------接口宏定义(硬件相关)--------------------
#if   (defined(PROJECT_XKAP_V3))
//---->
//时钟
#if   (defined(STM32F1))
#define BSP_KEY_RCC_ENABLE RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD , ENABLE);
#elif (defined(STM32F4))
#define BSP_KEY_RCC_ENABLE RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD , ENABLE);
#endif
//端口
#define BSP_KEY1_PORT      GPIOC
#define BSP_KEY1_PIN       GPIO_Pin_3
#define BSP_KEY2_PORT      GPIOC
#define BSP_KEY2_PIN       GPIO_Pin_2
#define BSP_KEY3_PORT      GPIOC
#define BSP_KEY3_PIN       GPIO_Pin_1
#define BSP_KEY4_PORT      GPIOC
#define BSP_KEY4_PIN       GPIO_Pin_0
#define BSP_KEY5_PORT      GPIOD
#define BSP_KEY5_PIN       GPIO_Pin_10
//枚举
enum BSP_KEY_E
{
    BSP_KEY_E_NC=0,
    BSP_KEY_E_KEY1,
    BSP_KEY_E_KEY2,
    BSP_KEY_E_KEY3,
    BSP_KEY_E_KEY4,
    BSP_KEY_E_KEY5,
    BSP_KEY_E_MAX=BSP_KEY_E_KEY5
};
//<----
#elif (defined(XKAP_ICARE_B_D_M))
//---->
//时钟
#if   (defined(STM32F1))
#define BSP_KEY_RCC_ENABLE RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD , ENABLE);
#elif (defined(STM32F4))
#define BSP_KEY_RCC_ENABLE RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD , ENABLE);
#endif
//端口
#define BSP_KEY1_PORT      GPIOD
#define BSP_KEY1_PIN       GPIO_Pin_10
#define BSP_KEY2_PORT      GPIOC
#define BSP_KEY2_PIN       GPIO_Pin_1
#define BSP_KEY3_PORT      GPIOC
#define BSP_KEY3_PIN       GPIO_Pin_0
#define BSP_KEY4_PORT      GPIOC
#define BSP_KEY4_PIN       GPIO_Pin_0
#define BSP_KEY5_PORT      GPIOC
#define BSP_KEY5_PIN       GPIO_Pin_0
//枚举
enum BSP_KEY_E
{
    BSP_KEY_E_NC=0,
    BSP_KEY_E_KEY1,
    BSP_KEY_E_KEY2,
    BSP_KEY_E_KEY3,
    BSP_KEY_E_KEY4,
    BSP_KEY_E_KEY5,
    BSP_KEY_E_MAX=BSP_KEY_E_KEY5
};
//<----
#elif (defined(PROJECT_SPI_SLAVE))
//---->
//时钟
#if   (defined(STM32F1))
#define BSP_KEY_RCC_ENABLE RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD , ENABLE);
#elif (defined(STM32F4))
#define BSP_KEY_RCC_ENABLE RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOD , ENABLE);
#endif
//端口
#define BSP_KEY1_PORT      GPIOD
#define BSP_KEY1_PIN       GPIO_Pin_10
#define BSP_KEY2_PORT      GPIOD
#define BSP_KEY2_PIN       GPIO_Pin_11
//枚举
enum BSP_KEY_E
{
    BSP_KEY_E_NC=0,
    BSP_KEY_E_KEY1,
    BSP_KEY_E_KEY2,
    BSP_KEY_E_MAX=BSP_KEY_E_KEY2
};
//<----
#elif (defined(PROJECT_ARMFLY_V5_XSL))
//---->
//时钟
#if   (defined(STM32F1))
#define BSP_KEY_RCC_ENABLE RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOF | RCC_APB2Periph_GPIOG | RCC_APB2Periph_GPIOH | RCC_APB2Periph_GPIOI , ENABLE);
#elif (defined(STM32F4))
#define BSP_KEY_RCC_ENABLE RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOG | RCC_AHB1Periph_GPIOH | RCC_AHB1Periph_GPIOI, ENABLE);
#endif
//端口
#define BSP_KEY1_PORT      GPIOI
#define BSP_KEY1_PIN       GPIO_Pin_8
#define BSP_KEY2_PORT      GPIOC
#define BSP_KEY2_PIN       GPIO_Pin_13
#define BSP_KEY3_PORT      GPIOI
#define BSP_KEY3_PIN       GPIO_Pin_11
#define BSP_KEY4_PORT      GPIOH
#define BSP_KEY4_PIN       GPIO_Pin_2
#define BSP_KEY5_PORT      GPIOH
#define BSP_KEY5_PIN       GPIO_Pin_3
#define BSP_KEY6_PORT      GPIOF
#define BSP_KEY6_PIN       GPIO_Pin_11
#define BSP_KEY7_PORT      GPIOG
#define BSP_KEY7_PIN       GPIO_Pin_7
#define BSP_KEY8_PORT      GPIOH
#define BSP_KEY8_PIN       GPIO_Pin_15
//枚举
enum BSP_KEY_E
{
    BSP_KEY_E_NC=0,
    BSP_KEY_E_KEY1,
    BSP_KEY_E_KEY2,
    BSP_KEY_E_KEY3,
    BSP_KEY_E_KEY4,
    BSP_KEY_E_KEY5,
    BSP_KEY_E_KEY6,
    BSP_KEY_E_KEY7,
    BSP_KEY_E_KEY8,
    BSP_KEY_E_MAX=BSP_KEY_E_KEY8
};
//<----
#elif (defined(XKAP_ICARE_A_M)||defined(XKAP_ICARE_A_S))
//---->
#define BSP_KEY1_PORT      11
#define BSP_KEY2_PORT      10
#define BSP_KEY3_PORT      9
#define BSP_KEY4_PORT      8
#define BSP_KEY5_PORT      7
#define BSP_KEY6_PORT      3
#define BSP_KEY7_PORT      2
#define BSP_KEY8_PORT      1
#define BSP_KEY9_PORT      0
#define BSP_KEY10_PORT     30
#define BSP_KEY11_PORT     29
#define BSP_KEY12_PORT     12
//枚举
enum BSP_KEY_E
{
    BSP_KEY_E_NC=0,
    BSP_KEY_E_KEY1,
    BSP_KEY_E_KEY2,
    BSP_KEY_E_KEY3,
    BSP_KEY_E_KEY4,
    BSP_KEY_E_KEY5,
    BSP_KEY_E_KEY6,
    BSP_KEY_E_KEY7,
    BSP_KEY_E_KEY8,
    BSP_KEY_E_KEY9,
    BSP_KEY_E_KEY10,
    BSP_KEY_E_KEY11,
    BSP_KEY_E_KEY12,
    BSP_KEY_E_MAX=BSP_KEY_E_KEY12
};
//<----
#elif (defined(XKAP_ICARE_B_M))
//---->
#define BSP_KEY_AD_ENABLE
#define BSP_KEY_AD_THR_RANGE  15
#define BSP_KEY_AD_THR_NULL   937
#define BSP_KEY_AD_THR_2      300/*181*/
#define BSP_KEY_AD_THR_3      470/*365*/
#define BSP_KEY_AD_THR_4      645/*557*/
#define BSP_KEY_AD_THR_5      784/*743*/
#define BSP_KEY_AD_THR_2A4    260/*743*/

//
#if   (HARDWARE_SUB_VER==1)
#define BSP_KEY1_PORT      17
#elif (HARDWARE_SUB_VER==2)
#define BSP_KEY1_PORT      28
#elif (HARDWARE_SUB_VER==3)
#define BSP_KEY1_PORT      15
#endif
//枚举
enum BSP_KEY_E
{
    BSP_KEY_E_NC=0,
    BSP_KEY_E_KEY1,
    BSP_KEY_E_KEY2,
    BSP_KEY_E_KEY3,
    BSP_KEY_E_KEY4,
    BSP_KEY_E_KEY5,    
    BSP_KEY_E_KEY_2A4=24,    
    BSP_KEY_E_MAX=BSP_KEY_E_KEY5+1
};
//<----
#elif (defined(TEST_NRF52_V1))
//---->
//#define BSP_KEY1_PORT      6
//#define BSP_KEY2_PORT      13
//#define BSP_KEY3_PORT      23
//#define BSP_KEY2_PORT      26
#define BSP_KEY1_PORT      28
//枚举
enum BSP_KEY_E
{
    BSP_KEY_E_NC=0,
    BSP_KEY_E_KEY1,
    //BSP_KEY_E_KEY2,
    //BSP_KEY_E_KEY3,
    //BSP_KEY_E_KEY4,
    //BSP_KEY_E_KEY5,
    BSP_KEY_E_MAX=BSP_KEY_E_KEY1
};
//<----
#elif (defined(XKAP_ICARE_B_C))
//---->
#define BSP_KEY1_PORT      30
#define BSP_KEY2_PORT      28
//枚举
enum BSP_KEY_E
{
    BSP_KEY_E_NC=0,
    BSP_KEY_E_KEY1,
    BSP_KEY_E_KEY2,
    BSP_KEY_E_MAX=BSP_KEY_E_KEY2
};
//<----
#elif (defined(PROJECT_NRF5X_BLE))
//---->
//#include "app_button.h"
#include "bsp.h"
#include "app_timer.h"
#define BSP_KEY_ENABLE_NRF_DRIVER
#define APP_TIMER_PRESCALER             0                                           /**< Value of the RTC1 PRESCALER register. */
#define BUTTON_DETECTION_DELAY          APP_TIMER_TICKS(50, APP_TIMER_PRESCALER)    /**< Delay from a GPIOTE event until a button is reported as pushed (in number of timer ticks). */
#define BSP_KEY1_PORT      30
#define BSP_KEY2_PORT      28
//枚举
enum BSP_KEY_E
{
    BSP_KEY_E_NC=0,
    BSP_KEY_E_KEY1,
    BSP_KEY_E_KEY2,
    BSP_KEY_E_MAX=BSP_KEY_E_KEY2
};
//<----
#elif (defined(XKAP_ICARE_A_C))
//---->
//#include "app_button.h"
#include "bsp.h"
#include "app_timer.h"
#define BSP_KEY_ENABLE_NRF_DRIVER
#define APP_TIMER_PRESCALER             0                                           /**< Value of the RTC1 PRESCALER register. */
#define BUTTON_DETECTION_DELAY          APP_TIMER_TICKS(50, APP_TIMER_PRESCALER)    /**< Delay from a GPIOTE event until a button is reported as pushed (in number of timer ticks). */
#define BSP_KEY1_PORT      13
#define BSP_KEY2_PORT      12
#define BSP_KEY3_PORT      11
#define BSP_KEY4_PORT      10
#define BSP_KEY5_PORT      17
enum BSP_KEY_E
{
    BSP_KEY_E_NC=0,
    BSP_KEY_E_KEY1,
    BSP_KEY_E_KEY2,
    BSP_KEY_E_KEY3,
    BSP_KEY_E_KEY4,
    BSP_KEY_E_KEY5,
    BSP_KEY_E_MAX=BSP_KEY_E_KEY5
};
//<----

#else 
#error Please Set Project to Bsp_Key.h
#endif
//-------------------接口宏定义(硬件无关)--------------------
#ifdef   BSP_KEY_AD_ENABLE
#include "uctsk_AD.h"
#endif

#ifdef   BSP_KEY1_PORT
#if   (defined(STM32F1)||defined(STM32F4))
#define BSP_KEY1_PIN_R     (BSP_KEY1_PORT->IDR & BSP_KEY1_PIN)
#elif (defined(NRF51)||defined(NRF52))
#define BSP_KEY1_PIN_R     nrf_gpio_pin_read(BSP_KEY1_PORT)
#endif
#endif
#ifdef   BSP_KEY2_PORT
#if   (defined(STM32F1)||defined(STM32F4))
#define BSP_KEY2_PIN_R     (BSP_KEY2_PORT->IDR & BSP_KEY2_PIN)
#elif (defined(NRF51)||defined(NRF52))
#define BSP_KEY2_PIN_R     nrf_gpio_pin_read(BSP_KEY2_PORT)
#endif
#endif
#ifdef   BSP_KEY3_PORT
#if   (defined(STM32F1)||defined(STM32F4))
#define BSP_KEY3_PIN_R     (BSP_KEY3_PORT->IDR & BSP_KEY3_PIN)
#elif (defined(NRF51)||defined(NRF52))
#define BSP_KEY3_PIN_R     nrf_gpio_pin_read(BSP_KEY3_PORT)
#endif
#endif
#ifdef   BSP_KEY4_PORT
#if   (defined(STM32F1)||defined(STM32F4))
#define BSP_KEY4_PIN_R     (BSP_KEY4_PORT->IDR & BSP_KEY4_PIN)
#elif (defined(NRF51)||defined(NRF52))
#define BSP_KEY4_PIN_R     nrf_gpio_pin_read(BSP_KEY4_PORT)
#endif
#endif
#ifdef   BSP_KEY5_PORT
#if   (defined(STM32F1)||defined(STM32F4))
#define BSP_KEY5_PIN_R     (BSP_KEY5_PORT->IDR & BSP_KEY5_PIN)
#elif (defined(NRF51)||defined(NRF52))
#define BSP_KEY5_PIN_R     nrf_gpio_pin_read(BSP_KEY5_PORT)
#endif
#endif
#ifdef   BSP_KEY6_PORT
#if   (defined(STM32F1)||defined(STM32F4))
#define BSP_KEY6_PIN_R     (BSP_KEY6_PORT->IDR & BSP_KEY6_PIN)
#elif (defined(NRF51)||defined(NRF52))
#define BSP_KEY6_PIN_R     nrf_gpio_pin_read(BSP_KEY6_PORT)
#endif
#endif
#ifdef   BSP_KEY7_PORT
#if   (defined(STM32F1)||defined(STM32F4))
#define BSP_KEY7_PIN_R     (BSP_KEY7_PORT->IDR & BSP_KEY7_PIN)
#elif (defined(NRF51)||defined(NRF52))
#define BSP_KEY7_PIN_R     nrf_gpio_pin_read(BSP_KEY7_PORT)
#endif
#endif
#ifdef   BSP_KEY8_PORT
#if   (defined(STM32F1)||defined(STM32F4))
#define BSP_KEY8_PIN_R     (BSP_KEY8_PORT->IDR & BSP_KEY8_PIN)
#elif (defined(NRF51)||defined(NRF52))
#define BSP_KEY8_PIN_R     nrf_gpio_pin_read(BSP_KEY8_PORT)
#endif
#endif
#ifdef   BSP_KEY9_PORT
#if   (defined(STM32F1)||defined(STM32F4))
#define BSP_KEY9_PIN_R     (BSP_KEY9_PORT->IDR & BSP_KEY9_PIN)
#elif (defined(NRF51)||defined(NRF52))
#define BSP_KEY9_PIN_R     nrf_gpio_pin_read(BSP_KEY9_PORT)
#endif
#endif
#ifdef   BSP_KEY10_PORT
#if   (defined(STM32F1)||defined(STM32F4))
#define BSP_KEY10_PIN_R    (BSP_KEY10_PORT->IDR & BSP_KEY10_PIN)
#elif (defined(NRF51)||defined(NRF52))
#define BSP_KEY10_PIN_R     nrf_gpio_pin_read(BSP_KEY10_PORT)
#endif
#endif
#ifdef   BSP_KEY11_PORT
#if   (defined(STM32F1)||defined(STM32F4))
#define BSP_KEY11_PIN_R    (BSP_KEY11_PORT->IDR & BSP_KEY11_PIN)
#elif (defined(NRF51)||defined(NRF52))
#define BSP_KEY11_PIN_R     nrf_gpio_pin_read(BSP_KEY11_PORT)
#endif
#endif
#ifdef   BSP_KEY12_PORT
#if   (defined(STM32F1)||defined(STM32F4))
#define BSP_KEY12_PIN_R    (BSP_KEY12_PORT->IDR & BSP_KEY12_PIN)
#elif (defined(NRF51)||defined(NRF52))
#define BSP_KEY12_PIN_R     nrf_gpio_pin_read(BSP_KEY12_PORT)
#endif
#endif
#ifdef   BSP_KEY13_PORT
#if   (defined(STM32F1)||defined(STM32F4))
#define BSP_KEY13_PIN_R    (BSP_KEY13_PORT->IDR & BSP_KEY13_PIN)
#elif (defined(NRF51)||defined(NRF52))
#define BSP_KEY13_PIN_R     nrf_gpio_pin_read(BSP_KEY13_PORT)
#endif
#endif
#ifdef   BSP_KEY14_PORT
#if   (defined(STM32F1)||defined(STM32F4))
#define BSP_KEY14_PIN_R    (BSP_KEY14_PORT->IDR & BSP_KEY14_PIN)
#elif (defined(NRF51)||defined(NRF52))
#define BSP_KEY14_PIN_R     nrf_gpio_pin_read(BSP_KEY14_PORT)
#endif
#endif
#ifdef   BSP_KEY15_PORT
#if   (defined(STM32F1)||defined(STM32F4))
#define BSP_KEY15_PIN_R    (BSP_KEY15_PORT->IDR & BSP_KEY15_PIN)
#elif (defined(NRF51)||defined(NRF52))
#define BSP_KEY15_PIN_R     nrf_gpio_pin_read(BSP_KEY15_PORT)
#endif
#endif
#ifdef   BSP_KEY16_PORT
#if   (defined(STM32F1)||defined(STM32F4))
#define BSP_KEY16_PIN_R    (BSP_KEY16_PORT->IDR & BSP_KEY16_PIN)
#elif (defined(NRF51)||defined(NRF52))
#define BSP_KEY16_PIN_R     nrf_gpio_pin_read(BSP_KEY16_PORT)
#endif
#endif
#ifdef   BSP_KEY17_PORT
#if   (defined(STM32F1)||defined(STM32F4))
#define BSP_KEY17_PIN_R    (BSP_KEY17_PORT->IDR & BSP_KEY17_PIN)
#elif (defined(NRF51)||defined(NRF52))
#define BSP_KEY17_PIN_R     nrf_gpio_pin_read(BSP_KEY17_PORT)
#endif
#endif
#ifdef   BSP_KEY18_PORT
#if   (defined(STM32F1)||defined(STM32F4))
#define BSP_KEY18_PIN_R    (BSP_KEY18_PORT->IDR & BSP_KEY18_PIN)
#elif (defined(NRF51)||defined(NRF52))
#define BSP_KEY18_PIN_R     nrf_gpio_pin_read(BSP_KEY18_PORT)
#endif
#endif
#ifdef   BSP_KEY19_PORT
#if   (defined(STM32F1)||defined(STM32F4))
#define BSP_KEY19_PIN_R    (BSP_KEY19_PORT->IDR & BSP_KEY19_PIN)
#elif (defined(NRF51)||defined(NRF52))
#define BSP_KEY19_PIN_R     nrf_gpio_pin_read(BSP_KEY19_PORT)
#endif
#endif
#ifdef   BSP_KEY20_PORT
#if   (defined(STM32F1)||defined(STM32F4))
#define BSP_KEY20_PIN_R    (BSP_KEY20_PORT->IDR & BSP_KEY20_PIN)
#elif (defined(NRF51)||defined(NRF52))
#define BSP_KEY20_PIN_R     nrf_gpio_pin_read(BSP_KEY20_PORT)
#endif
#endif
//-------------------接口变量--------------------------------
//输出方式一(单稳态-只置高,由应用程序置低)
extern uint8_t BspKey_ValueBuf[BSP_KEY_E_MAX];
//输出方式二(单一键值)
// 1
extern uint8_t BspKey_Value;
extern uint8_t BspKey_NewSign;
extern uint16_t BspKey_KeepTimer_ms;
//输出方式三(按键状态)
extern uint8_t BspKey_RealValueBuf[BSP_KEY_E_MAX];
//输出方式四
extern uint16_t BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_MAX];
//信息输出(总按键次数)
extern uint16_t BspKey_Count;
//-------------------接口函数--------------------------------
extern void BspKey_Init(void);
extern void BspKey_Main_1ms(void);
extern void BspKey_Main_10ms(void);
extern void Key_Main_DebugTest_100ms(void);
//注意: 只能被一处调用,因为调用即同步信息
extern uint8_t BspKey_IfPressKey(void);
//
extern void BspKey_DebugTestOnOff(uint8_t OnOff);
//-------------------------------------------------------------------------------
#endif

