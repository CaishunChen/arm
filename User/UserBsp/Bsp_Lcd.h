/**
  ******************************************************************************
  * @file    Bsp_Lcd.h 
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  * @brief   bottom-driven -->   简易LCD驱动(类似12864等).
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
#ifndef __BSP_LCD_H
#define __BSP_LCD_H
//-------------------加载库函数------------------------------
#include "includes.h"
//-------------------接口宏定义(硬件相关)--------------------
#if   (defined(PROJECT_STLH_V10))
//---->
//时钟
#if   (defined(STM32F1))
//#define BSP_LCD_RCC_ENABLE    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB , ENABLE);
#define BSP_LCD_RCC_ENABLE    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOE , ENABLE);
#elif (defined(STM32F4))
#define BSP_LCD_RCC_ENABLE    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB , ENABLE);
#endif
//端口
//#define BSP_LCD_CS_PORT       GPIOA
//#define BSP_LCD_CS_PIN        GPIO_Pin_15
//#define BSP_LCD_SDI_PORT      GPIOB
//#define BSP_LCD_SDI_PIN       GPIO_Pin_4
//#define BSP_LCD_SCLK_PORT     GPIOB
//#define BSP_LCD_SCLK_PIN      GPIO_Pin_3

#define BSP_LCD_CS_PORT       GPIOE
#define BSP_LCD_CS_PIN        GPIO_Pin_9
#define BSP_LCD_SDI_PORT      GPIOE
#define BSP_LCD_SDI_PIN       GPIO_Pin_8
#define BSP_LCD_SCLK_PORT     GPIOE
#define BSP_LCD_SCLK_PIN      GPIO_Pin_7
//<----
#endif
//-----------------------------------------------------------
//-------------------接口宏定义(硬件无关)--------------------
#define BSP_LCD_CS_H          GPIO_SetBits(BSP_LCD_CS_PORT , BSP_LCD_CS_PIN)
#define BSP_LCD_CS_L          GPIO_ResetBits(BSP_LCD_CS_PORT , BSP_LCD_CS_PIN)
#define BSP_LCD_SDI_H         GPIO_SetBits(BSP_LCD_SDI_PORT , BSP_LCD_SDI_PIN)
#define BSP_LCD_SDI_L         GPIO_ResetBits(BSP_LCD_SDI_PORT , BSP_LCD_SDI_PIN)
#define BSP_LCD_SCLK_H        GPIO_SetBits(BSP_LCD_SCLK_PORT , BSP_LCD_SCLK_PIN)
#define BSP_LCD_SCLK_L        GPIO_ResetBits(BSP_LCD_SCLK_PORT , BSP_LCD_SCLK_PIN)
//-------------------接口函数--------------------------------
extern void BspLcd_Init(void);
extern void BspLcd_Test(void);
extern void	(*MenuFun)(void);
extern void BspLcd_100ms(void);
extern int ftostr(float fNum,char str[],int dotsize);
//-----------------------------------------------------------
#endif
