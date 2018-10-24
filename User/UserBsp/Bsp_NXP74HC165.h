/**
  ******************************************************************************
  * @file    Bsp_NXP74HC165.h 
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  * @brief   bottom-driven -->   基于NXP74HC165的按键驱动.
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
#ifndef __BSP_NXP74HC165_H
#define __BSP_NXP74HC165_H
//-------------------加载库函数------------------------------
#include "includes.h"
//-------------------接口宏定义(硬件相关)--------------------
#if   (defined(PROJECT_TCI_V30))
//---->
//数量
#define BSP_NXP74HC165_NUM          1
//时钟
#if   (defined(STM32F1))
#define BSP_NXP74HC165_RCC_ENABLE   RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOE , ENABLE);
#elif (defined(STM32F4))
#define BSP_NXP74HC165_RCC_ENABLE   RCC_AHB1PeriphClockCmd( RCC_APB2Periph_GPIOA|RCC_AHB1Periph_GPIOE , ENABLE);
#endif
//端口
#define BSP_NXP74HC165_SH_PORT      GPIOE
#define BSP_NXP74HC165_SH_PIN       GPIO_Pin_9
#define BSP_NXP74HC165_CLK_PORT     GPIOA
#define BSP_NXP74HC165_CLK_PIN      GPIO_Pin_0
#define BSP_NXP74HC165_QH_PORT      GPIOE
#define BSP_NXP74HC165_QH_PIN       GPIO_Pin_10
//<----
#else 
#error Please Set Project to Bsp_NXP74HC165.h
#endif
//-------------------接口宏定义(硬件无关)--------------------
#if   (defined(STM32F1)||defined(STM32F4))
#define BSP_NXP74HC165_SH_SET       GPIO_SetBits(BSP_NXP74HC165_SH_PORT, BSP_NXP74HC165_SH_PIN)
#define BSP_NXP74HC165_SH_CLR       GPIO_ResetBits(BSP_NXP74HC165_SH_PORT, BSP_NXP74HC165_SH_PIN)
#define BSP_NXP74HC165_CLK_SET      GPIO_SetBits(BSP_NXP74HC165_CLK_PORT, BSP_NXP74HC165_CLK_PIN)
#define BSP_NXP74HC165_CLK_CLR      GPIO_ResetBits(BSP_NXP74HC165_CLK_PORT, BSP_NXP74HC165_CLK_PIN)
#define BSP_NXP74HC165_QH_R		   (BSP_NXP74HC165_QH_PORT->IDR & BSP_NXP74HC165_QH_PIN)
#define NXP74HC165_NOP      
#elif (defined(NRF51)||defined(NRF52))
#define BSP_NXP74HC165_SH_SET       nrf_gpio_pin_write(BSP_NXP74HC165_SH_PIN,1)
#define BSP_NXP74HC165_SH_CLR       nrf_gpio_pin_write(BSP_NXP74HC165_SH_PIN,0)
#define BSP_NXP74HC165_CLK_SET      nrf_gpio_pin_write(BSP_NXP74HC165_CLK_PIN,1)
#define BSP_NXP74HC165_CLK_CLR      nrf_gpio_pin_write(BSP_NXP74HC165_CLK_PIN,0)
#define BSP_NXP74HC165_QH_R         nrf_gpio_pin_read(BSP_NXP74HC165_QH_PIN)
#endif
//-------------------接口函数--------------------------------
extern void BspNXP74HC165_Init(void);
extern uint32_t BspNXP74HC165_read(void);
extern void BspNXP74HC165_100ms(void);
extern void BspNXP74HC165_DebugTestOnOff(uint8_t OnOff);
//
#endif

