/**
  ******************************************************************************
  * @file    bsp_Ds18b20.h 
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  * @brief   bottom-driven -->   基于DS18B20的温度传感器.
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

#ifndef _BSP_DS18B20_H
#define _BSP_DS18B20_H
//-------------------加载库函数------------------------------
#include "includes.h"
//-------------------接口宏定义(硬件相关)--------------------
#if (defined(PROJECT_ARMFLY_V5_XSL))
//---->
//时钟
#if   (defined(STM32F1))
#define BSP_DS18B20_RCC_ENABLE   RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB , ENABLE);
#elif (defined(STM32F4))
#define BSP_DS18B20_RCC_ENABLE   RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOB , ENABLE);
#endif
//端口
#define BSP_DS18B20_1_PORT       GPIOB
#define BSP_DS18B20_1_PIN        GPIO_Pin_1
//<----
#else
#error Please Set Project to Bsp_Led.h
#endif
//-------------------接口宏定义(硬件无关)--------------------
#if 0
// 库函数方式
#define BSP_DS18B20_1_L     GPIO_ResetBits(PORT_DQ, PIN_DQ)
#define BSP_DS18B20_1_H     GPIO_SetBits(PORT_DQ, PIN_DQ)
#define DQ_IS_LOW()       (GPIO_ReadInputDataBit(PORT_DQ, PIN_DQ) == Bit_RESET)
#else
// 直接操作寄存器，提高速度
#define BSP_DS18B20_1_L     BSP_DS18B20_1_PORT->BSRRH = BSP_DS18B20_1_PIN
#define BSP_DS18B20_1_H     BSP_DS18B20_1_PORT->BSRRL = BSP_DS18B20_1_PIN
#define BSP_DS18B20_IS_LOW  ((BSP_DS18B20_1_PORT->IDR & BSP_DS18B20_1_PIN) == 0)
#endif
//-------------------接口函数--------------------------------
void BspDs18b20_Init(void);
uint8_t BspDs18b20_ReadTempReg(int32_t *value);
int16_t BspDs18b20_ReadTempByID(uint8_t *_id);
uint8_t BspDs18b20_ReadID(uint8_t *_id);
void BspDs18b20_DebugTest_100ms(void);
void BspDs18b20_DebugTestOnOff(uint8_t OnOff);
//-----------------------------------------------------------
#endif

