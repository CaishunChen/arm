/**
  ******************************************************************************
  * @file    Bsp_NXP74HC595.h 
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  * @brief   bottom-driven -->   基于NXP74HC595的LED驱动.
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
#ifndef __BSP_NXP74HC595_H
#define __BSP_NXP74HC595_H
//-------------------加载库函数------------------------------
#include "includes.h"
//-------------------接口宏定义(硬件相关)--------------------
#if   (defined(XXX))
//时钟
#if   (defined(STM32F1))
#define BSP_NXP74HC595_RCC_ENABLE   RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOE , ENABLE);
#elif (defined(STM32F4))
#define BSP_NXP74HC595_RCC_ENABLE   RCC_AHB1PeriphClockCmd( RCC_APB2Periph_GPIOA|RCC_AHB1Periph_GPIOE , ENABLE);
#endif
//端口
#define BSP_NXP74HC595_SER_PORT        GPIOA
#define BSP_NXP74HC595_SER_PIN         GPIO_Pin_0
#define BSP_NXP74HC595_SCLK_PORT       GPIOA
#define BSP_NXP74HC595_SCLK_PIN        GPIO_Pin_0
#define BSP_NXP74HC595_RCLK_PORT       GPIOA
#define BSP_NXP74HC595_RCLK_PIN        GPIO_Pin_0
#define BSP_NXP74HC595_NOP 
#else 
#error Please Set Project to Bsp_NXP74HC595.h
#endif
//-------------------接口宏定义(硬件无关)--------------------
#if   (defined(STM32F1)||defined(STM32F4))
#define BSP_NXP74HC595_SER_SET       GPIO_SetBits(BSP_NXP74HC595_SER_PORT, BSP_NXP74HC595_SER_PIN)
#define BSP_NXP74HC595_SER_CLR       GPIO_ResetBits(BSP_NXP74HC595_SER_PORT, BSP_NXP74HC595_SER_PIN)
#define BSP_NXP74HC595_SCLK_SET      GPIO_SetBits(BSP_NXP74HC595_SCLK_PORT, BSP_NXP74HC595_SCLK_PIN)
#define BSP_NXP74HC595_SCLK_CLR      GPIO_ResetBits(BSP_NXP74HC595_SCLK_PORT, BSP_NXP74HC595_SCLK_PIN)
#define BSP_NXP74HC595_RCLK_SET      GPIO_SetBits(BSP_NXP74HC595_RCLK_PORT, BSP_NXP74HC595_RCLK_PIN)
#define BSP_NXP74HC595_RCLK_CLR      GPIO_ResetBits(BSP_NXP74HC595_RCLK_PORT, BSP_NXP74HC595_RCLK_PIN)
#elif (defined(NRF51)||defined(NRF52))
#define BSP_NXP74HC595_SER_SET       nrf_gpio_pin_write(BSP_NXP74HC595_SER_PIN,1)
#define BSP_NXP74HC595_SER_CLR       nrf_gpio_pin_write(BSP_NXP74HC595_SER_PIN,0)
#define BSP_NXP74HC595_SCLK_SET      nrf_gpio_pin_write(BSP_NXP74HC595_SCLK_PIN,1)
#define BSP_NXP74HC595_SCLK_CLR      nrf_gpio_pin_write(BSP_NXP74HC595_SCLK_PIN,0)
#define BSP_NXP74HC595_RCLK_SET      nrf_gpio_pin_write(BSP_NXP74HC595_RCLK_PIN,1)
#define BSP_NXP74HC595_RCLK_CLR      nrf_gpio_pin_write(BSP_NXP74HC595_RCLK_PIN,0)
#endif
//-------------------接口函数--------------------------------
extern void BspNXP74HC595_Init(void);
extern void BspNXP74HC595_APP(uint8_t lednum,uint8_t ledstate);
extern void BspNXP74HC595_100ms(void);
extern void BspNXP74HC595_DebugTestOnOff(uint8_t OnOff);
#endif

