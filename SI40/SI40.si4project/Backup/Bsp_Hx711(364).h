/**
  ******************************************************************************
  * @file    Bsp_Hx711.h 
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  * @brief   bottom-driven -->   基于Hx711的电子秤驱动.
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
  
#ifndef __BSP_HX711_H
#define __BSP_HX711_H
//-------------------加载库函数------------------------------
#include "includes.h"
//-------------------接口宏定义(硬件相关)--------------------
//----------
#if (defined(PROJECT_XKAP_V3))
//---->
//时钟
#if   (defined(STM32F1))
#define BSP_HX711_RCC_ENABLE  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD , ENABLE);
#elif (defined(STM32F4))
#define BSP_HX711_RCC_ENABLE  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOD , ENABLE);
#endif
//端口
#define BSP_HX711_DOUT_PORT   GPIOD
#define BSP_HX711_DOUT_PIN    GPIO_Pin_4
#define BSP_HX711_SCLK_PORT   GPIOD
#define BSP_HX711_SCLK_PIN    GPIO_Pin_5
//<----
#elif (defined(PROJECT_ARMFLY_V5_XSL))
//---->
//时钟
#if   (defined(STM32F1))
#define BSP_HX711_RCC_ENABLE  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC , ENABLE);
#elif (defined(STM32F4))
#define BSP_HX711_RCC_ENABLE  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOC , ENABLE);
#endif
//端口
#define BSP_HX711_DOUT_PORT   GPIOC
#define BSP_HX711_DOUT_PIN    GPIO_Pin_2
#define BSP_HX711_SCLK_PORT   GPIOC
#define BSP_HX711_SCLK_PIN    GPIO_Pin_7
//<----
#elif (defined(XKAP_ICARE_A_M)||defined(XKAP_ICARE_A_S))
//---->
#define BSP_HX711_DOUT_PIN    17
#define BSP_HX711_SCLK_PIN    18
//<----
#elif (defined(XKAP_ICARE_B_M))
//---->
#if   (HARDWARE_SUB_VER==1)
#define BSP_HX711_DOUT_PIN    15
#define BSP_HX711_SCLK_PIN    16
#elif (HARDWARE_SUB_VER==2)
#define BSP_HX711_DOUT_PIN    24
#define BSP_HX711_SCLK_PIN    25
#endif
//<----
#else 
#error Please Set Project to Bsp_Hx711.h
#endif
//----------
//-------------------接口宏定义(硬件无关)--------------------
#define BSP_HX711_REF_VALUE  (0.43)
#if   (defined(STM32F1)||defined(STM32F4))  
//---->
#define BSP_HX711_SCLK_L      GPIO_ResetBits(BSP_HX711_SCLK_PORT , BSP_HX711_SCLK_PIN)
#define BSP_HX711_SCLK_H      GPIO_SetBits(BSP_HX711_SCLK_PORT , BSP_HX711_SCLK_PIN)
#define BSP_HX711_DOUT_R      (BSP_HX711_DOUT_PORT->ODR & GPIO_PIN_LED2)
//<----
#elif(defined(NRF52)||defined(NRF51))
//---->
#define BSP_HX711_SCLK_L      nrf_gpio_pin_write(BSP_HX711_SCLK_PIN,0)
#define BSP_HX711_SCLK_H      nrf_gpio_pin_write(BSP_HX711_SCLK_PIN,1)
#define BSP_HX711_DOUT_R      nrf_gpio_pin_read(BSP_HX711_DOUT_PIN)
//<----
#endif
//-------------------接口函数--------------------------------
extern void BspHx711_Init(void);
extern void BspHx711_100ms(void);
extern void BspHx711_Precise1000ms(void);
extern uint8_t BspHx711_Calibration(void);
extern uint8_t BspHx711_Get_g(uint32_t *value_g);
extern void BspHx711_DebugTestOnOff(uint8_t OnOff);
//-----------------------------------------------------------
#endif
