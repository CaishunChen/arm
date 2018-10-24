/**
  ******************************************************************************
  * @file    bsp_Eeprom.h 
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  * @brief   bottom-driven -->   基于IIC的EEPROM.
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
#ifndef __BSP_EEPROM_H
#define __BSP_EEPROM_H
//-------------------加载库函数------------------------------
#include "includes.h"
//-------------------接口宏定义(硬件相关)--------------------
#if   (defined(PROJECT_STLH_V10))
//---->
//铁电使能宏定义(无需繁忙等待)
#define I2C_FM_ENABLE
//器件地址
#define BSP_EEPROM_IIC_ADDR      0xA0
//时钟
#if   (defined(STM32F1))
#define BSP_EEPROM_RCC_ENABLE    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOD, ENABLE);
#elif (defined(STM32F4))
#define BSP_EEPROM_RCC_ENABLE    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOD, ENABLE);
#endif
//端口
#define BSP_EEPROM_SDA_PORT      GPIOB
#define BSP_EEPROM_SDA_PIN       GPIO_Pin_7
#define BSP_EEPROM_SCL_PORT      GPIOB
#define BSP_EEPROM_SCL_PIN       GPIO_Pin_6
#define BSP_EEPROM_WP_PORT       GPIOD
#define BSP_EEPROM_WP_PIN        GPIO_Pin_0
//<----
#else
#error Please Set Project to Bsp_Gps.h
#endif
//-------------------接口宏定义(硬件无关)--------------------
#define BSP_EERPOM_BEGIN_ADDR    (0)
#define BSP_EERPOM_END_ADDR      ((256/8)*1024L-1)
#define BSP_EERPOM_MAX_WRITE_SIZE   (256L)
//
#define BSP_EEPROM_SDA_H         GPIO_SetBits(BSP_EEPROM_SDA_PORT, BSP_EEPROM_SDA_PIN)
#define BSP_EEPROM_SDA_L         GPIO_ResetBits(BSP_EEPROM_SDA_PORT, BSP_EEPROM_SDA_PIN)
#define BSP_EEPROM_SCL_H         GPIO_SetBits(BSP_EEPROM_SCL_PORT, BSP_EEPROM_SCL_PIN)
#define BSP_EEPROM_SCL_L         GPIO_ResetBits(BSP_EEPROM_SCL_PORT, BSP_EEPROM_SCL_PIN)
#ifdef BSP_EEPROM_WP_PORT 
#define BSP_EEPROM_WP_H          GPIO_SetBits(BSP_EEPROM_WP_PORT, BSP_EEPROM_WP_PIN)
#define BSP_EEPROM_WP_L          GPIO_ResetBits(BSP_EEPROM_WP_PORT, BSP_EEPROM_WP_PIN)
#else
#define BSP_EEPROM_WP_H
#define BSP_EEPROM_WP_L
#endif
#define BSP_EEPROM_SDA_R         (BSP_EEPROM_SDA_PORT->IDR & BSP_EEPROM_SDA_PIN)
#ifdef STM32F1
#define BSP_EEPROM_NOP           __nop();__nop();__nop();__nop();__nop();__nop();
#endif
#ifdef STM32F4
#define BSP_EEPROM_NOP           __nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
#endif
//-------------------接口函数--------------------------------
void BspEeprom_Init(void);
uint8_t BspEeprom_Read(uint16_t addr,uint8_t *buf,uint16_t num);
uint8_t BspEeprom_Write(uint16_t addr,uint8_t *buf,uint16_t num);
void BspEeprom_DebugTestOnOff(uint8_t OnOff);
//
#endif
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

