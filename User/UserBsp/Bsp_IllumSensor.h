/**
  ******************************************************************************
  * @file    Bsp_IllumSensor.h 
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  * @brief   bottom-driven -->   照度传感器.
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
#ifndef __BSP_ILLUMSENSOR_H
#define __BSP_ILLUMSENSOR_H
//-------------------加载库函数------------------------------
#include <includes.h>
//-------------------接口宏定义(硬件相关)--------------------
//时钟
#if   (defined(STM32F1))
#define BSP_ILLUM_GPIO_RCC_ENABLE   RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB , ENABLE)
#define BSP_ILLUM_IICX_RCC_ENABLE   RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2,ENABLE)
#elif (defined(STM32F4))
#define BSP_ILLUM_GPIO_RCC_ENABLE   RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOB , ENABLE)
#define BSP_ILLUM_IICX_RCC_ENABLE   RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2,ENABLE)
#endif
//端口
#define BSP_ILLUM_IIC_SDA_PORT      GPIOB
#define BSP_ILLUM_IIC_SDA_PIN       GPIO_Pin_11
#define BSP_ILLUM_IIC_SCL_PORT      GPIOB
#define BSP_ILLUM_IIC_SCL_PIN       GPIO_Pin_10
#if   (defined(STM32F1))
#define BSP_ILLUM_IIC_SDA_REMAP     
#define BSP_ILLUM_IIC_SCL_REMAP     
#elif (defined(STM32F4))
#define BSP_ILLUM_IIC_SDA_REMAP     GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_I2C2);
#define BSP_ILLUM_IIC_SCL_REMAP     GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_I2C2);
#endif
//IIC
#define BSP_ILLUM_IICX  I2C2
//-------------------接口宏定义(硬件无关)--------------------
//-------------------接口函数--------------------------------
extern uint8_t BspIllumSensor_Init(void);
extern uint8_t BspIllumSensor_Read(uint32_t *pIllum);
extern void BspIllumSensor_DebugTest_100ms(void);
extern void BspIllumSensor_DebugTestOnOff(uint8_t OnOff);
//-------------------------------------------------------------------------------
#endif

