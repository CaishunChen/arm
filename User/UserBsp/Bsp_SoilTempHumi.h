/**
  ******************************************************************************
  * @file    Bsp_SoilTempHumi.h 
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  * @brief   bottom-driven -->   土壤温湿度传感器(待整合).
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

#ifndef __BSP_SOIL_TEMP_HUMI_H
#define __BSP_SOIL_TEMP_HUMI_H
//-------------------加载库函数------------------------------
#include "includes.h"
//-------------------接口宏定义(硬件相关)--------------------
#if   (defined(PROJECT_STLH_V10))
//---->
#define  BSP_SOIL_TEMP_HUMIDITY_UARTX  (1)
//时钟
#if   (defined(STM32F1))
#define BSP_SOILTEMPHUMI_PWR_ENABLE    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA , ENABLE);
#elif (defined(STM32F4))
#define BSP_SOILTEMPHUMI_PWR_ENABLE    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA , ENABLE);
#endif
//端口
#define BSP_SOILTEMPHUMI_PWR_PORT     GPIOA
#define BSP_SOILTEMPHUMI_PWR_PIN      GPIO_Pin_6
//<----
#else
#error Please Set Project to Bsp_SoilTempHumi.h
#endif
//-------------------接口宏定义(硬件无关)--------------------
#ifdef BSP_SOILTEMPHUMI_PWR_PORT
#define BSP_SOILTEMPHUMI_PWR_ON       GPIO_ResetBits(BSP_SOILTEMPHUMI_PWR_PORT , BSP_SOILTEMPHUMI_PWR_PIN)
#define BSP_SOILTEMPHUMI_PWR_OFF      GPIO_SetBits(BSP_SOILTEMPHUMI_PWR_PORT , BSP_SOILTEMPHUMI_PWR_PIN)
#endif
//-------------------接口变量--------------------------------
//-------------------接口函数--------------------------------
void BspSoilTempHumi_Init(void);
void BspSoilTempHumi_100ms(void);
void BspSoilTempHumi_InterruptRx(uint8_t *pbuf,uint16_t len);
uint8_t BspSoilTempHumi_Read(int16_t *pTemp,uint16_t *pHumi);
void BspSoilTempHumi_DebugTestOnOff(uint8_t OnOff);
//-----------------------------------------------------------
#endif

