/**
  ******************************************************************************
  * @file    Bsp_GpioIIC.h 
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  * @brief   bottom-driven -->   GPIO仿IIC时序.
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
#ifndef __BSP_GPIO_IIC_H
#define __BSP_GPIO_IIC_H
//-------------------加载库函数------------------------------
#include "includes.h"
//-------------------接口宏定义(硬件相关)--------------------
#if   (defined(PROJECT_ARMFLY_V5_XSL))
//---->
//时钟
#if   (defined(STM32F1))
#define BSP_GPIO_IIC_RCC_ENABLE  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOH , ENABLE);
#elif (defined(STM32F4))
#define BSP_GPIO_IIC_RCC_ENABLE  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOH , ENABLE);
#endif
//端口
#define BSP_GPIO_IIC_SDA_PORT GPIOH
#define BSP_GPIO_IIC_SDA_PIN  GPIO_Pin_5
#define BSP_GPIO_IIC_SCL_PORT GPIOH
#define BSP_GPIO_IIC_SCL_PIN  GPIO_Pin_4
//<----
#else
#error Please Set Project to Bsp_GpioIIC.h
#endif
//-------------------接口宏定义(硬件无关)--------------------
#define BSP_GPIO_IIC_WR	      0
#define BSP_GPIO_IIC_RD	      1
#define BSP_GPIO_IIC_SDA_H    GPIO_SetBits(BSP_GPIO_IIC_SDA_PORT, BSP_GPIO_IIC_SDA_PIN)
#define BSP_GPIO_IIC_SDA_L    GPIO_ResetBits(BSP_GPIO_IIC_SDA_PORT, BSP_GPIO_IIC_SDA_PIN)
#define BSP_GPIO_IIC_SCL_H    GPIO_SetBits(BSP_GPIO_IIC_SCL_PORT, BSP_GPIO_IIC_SCL_PIN)
#define BSP_GPIO_IIC_SCL_L    GPIO_ResetBits(BSP_GPIO_IIC_SCL_PORT, BSP_GPIO_IIC_SCL_PIN)
#define BSP_GPIO_IIC_SDA_R    (BSP_GPIO_IIC_SDA_PORT->IDR & BSP_GPIO_IIC_SDA_PIN)
#define BSP_GPIO_IIC_SCL_R    (BSP_GPIO_IIC_SCL_PORT->IDR & BSP_GPIO_IIC_SCL_PIN)
//-------------------接口函数--------------------------------
void BspGpioIIC_Init(void);
void BspGpioIIC_Start(void);
void BspGpioIIC_Stop(void);
void BspGpioIIC_SendByte(uint8_t _ucByte);
uint8_t BspGpioIIC_ReadByte(void);
uint8_t BspGpioIIC_WaitAck(void);
void BspGpioIIC_Ack(void);
void BspGpioIIC_NAck(void);
uint8_t BspGpioIIC_CheckDevice(uint8_t _Address);
//
#endif
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

