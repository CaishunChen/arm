/**
  ******************************************************************************
  * @file    Bsp_TempHumiSensor.h 
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  * @brief   bottom-driven -->   基于IIC的温湿度传感器驱动.
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
#ifndef __BSP_TEMPHUMIDSENSOR_H
#define __BSP_TEMPHUMIDSENSOR_H
//-------------------加载库函数------------------------------
#include "includes.h"
//-------------------接口宏定义(硬件相关)--------------------
#if   (defined(PROJECT_XKAP_V3)||defined(XKAP_ICARE_B_D_M))
//时钟
#if   (defined(STM32F1))
#define BSP_TEMPHUMIDSENSOR_GPIO_RCC_ENABLE  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB , ENABLE)
#define BSP_TEMPHUMIDSENSOR_IICX_RCC_ENABLE  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE)
#elif (defined(STM32F4))
#define BSP_TEMPHUMIDSENSOR_GPIO_RCC_ENABLE  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOB , ENABLE)
#define BSP_TEMPHUMIDSENSOR_IICX_RCC_ENABLE  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE)
#endif
//端口
#define BSP_TEMPHUMIDSENSOR_IIC_SDA_PORT     GPIOB
#define BSP_TEMPHUMIDSENSOR_IIC_SDA_PIN      GPIO_Pin_7
#define BSP_TEMPHUMIDSENSOR_IIC_SCL_PORT     GPIOB
#define BSP_TEMPHUMIDSENSOR_IIC_SCL_PIN      GPIO_Pin_6
#if   (defined(STM32F1))
#define BSP_TEMPHUMIDSENSOR_IIC_SDA_REMAP     
#define BSP_TEMPHUMIDSENSOR_IIC_SCL_REMAP     
#elif (defined(STM32F4))
#define BSP_TEMPHUMIDSENSOR_IIC_SDA_REMAP    GPIO_PinAFConfig(GPIOB,GPIO_PinSource7,GPIO_AF_I2C1);
#define BSP_TEMPHUMIDSENSOR_IIC_SCL_REMAP    GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_I2C1);
#endif
//IIC
#define BSP_TEMPHUMIDSENSOR_IICX             I2C1
//供电
#define BSP_TEMPHUMIDSENSOR_POWER_INIT       BspDac_Init(2)
#define BSP_TEMPHUMIDSENSOR_POWER_ON         BspDac_Write(2,1800)
#define BSP_TEMPHUMIDSENSOR_POWER_OFF        BspDac_Write(2,1)
//<----
#endif
//-------------------接口宏定义(硬件无关)--------------------
#define BSP_TEMPHUMID_ADDR                   0x30
#define BSP_TEMPHUMIDSENSOR_TEMP_CORRENT     (-35)
//-------------------接口函数--------------------------------
extern uint8_t BspTempHumidSensor_Init(void);
extern uint8_t BspTempHumidSensor_Read(int16_t *pTemp,uint16_t *pHumi);
extern void BspTempHumidSensor_DebugTest_100ms(void);
extern void BspTempHumidSensor_DebugTestOnOff(uint8_t OnOff);
//-------------------------------------------------------------------------------
#endif

