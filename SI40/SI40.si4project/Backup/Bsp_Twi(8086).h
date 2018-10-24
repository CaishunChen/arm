/**
  ******************************************************************************
  * @file    Bsp_Twi.h 
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  * @brief   bottom-driven -->   Twi(IIC)驱动.
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
#ifndef __BSP_TWI_H
#define __BSP_TWI_H
//-------------------加载库函数------------------------------
#include "includes.h"
//-------------------接口宏定义(硬件相关)--------------------
#if   (defined(XKAP_ICARE_B_C)||defined(PROJECT_NRF5X_BLE))
//---->
#define BSP_TWI_MODE_HARDWARE
//#define BSP_TWI_MODE_SOFTWARE
#define BSP_TWI_SDA_PIN             9
#define BSP_TWI_SCL_PIN             10
#define BSP_TWI_X                   1	/*0-1*/
#elif(defined(XKAP_ICARE_B_M))
//#define BSP_TWI_MODE_HARDWARE
#define BSP_TWI_MODE_SOFTWARE
#define BSP_TWI_MUTEX_DISABLE
#if   (HARDWARE_SUB_VER==1)
#define BSP_TWI_SDA_PIN             23
#define BSP_TWI_SCL_PIN             21
#elif (HARDWARE_SUB_VER==2)
#define BSP_TWI_SDA_PIN             11
#define BSP_TWI_SCL_PIN             9
#endif
#define BSP_TWI_X                   1	/*0-1*/
//<----
#elif (defined(PROJECT_XKAP_V3)||defined(XKAP_ICARE_B_D_M))
//---->
#define BSP_TWI_MODE_HARDWARE
//#define BSP_TWI_MODE_SOFTWARE
//时钟
#if   (defined(STM32F1))
#define BSP_TWI_GPIO_RCC_ENABLE     RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB , ENABLE)
#define BSP_TWI_IICX_RCC_ENABLE     RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE)
#elif (defined(STM32F4))
#define BSP_TWI_GPIO_RCC_ENABLE     RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOB , ENABLE)
#define BSP_TWI_IICX_RCC_ENABLE     RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE)
#endif
//端口
#define BSP_TWI_SDA_PORT            GPIOB
#define BSP_TWI_SDA_PIN             GPIO_Pin_7
#define BSP_TWI_SCL_PORT            GPIOB
#define BSP_TWI_SCL_PIN             GPIO_Pin_6
#if   (defined(STM32F1))
#define BSP_TWI_SDA_REMAP     
#define BSP_TWI_SCL_REMAP     
#elif (defined(STM32F4))
#define BSP_TWI_SDA_REMAP           GPIO_PinAFConfig(GPIOB,GPIO_PinSource7,GPIO_AF_I2C1);
#define BSP_TWI_SCL_REMAP           GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_I2C1);
#endif
//IIC
#define BSP_TWI_X                   I2C1
//<----
#else
#error Please Set Project to Bsp_Twi.h
#endif
//-------------------接口宏定义(硬件无关)--------------------
#define BSP_TWI_ERROR_COUNT         2000
#define BSP_TWI_WR	      	0
#define BSP_TWI_RD	      	1
#ifdef	BSP_TWI_MODE_SOFTWARE
#if		(defined(STM32F1)||defined(STM32F4))
#define BSP_TWI_SDA_H    	GPIO_SetBits(BSP_TWI_SDA_PORT, BSP_TWI_SDA_PIN)
#define BSP_TWI_SDA_L    	GPIO_ResetBits(BSP_TWI_SDA_PORT, BSP_TWI_SDA_PIN)
#define BSP_TWI_SCL_H    	GPIO_SetBits(BSP_TWI_SCL_PORT, BSP_TWI_SCL_PIN)
#define BSP_TWI_SCL_L    	GPIO_ResetBits(BSP_TWI_SCL_PORT, BSP_TWI_SCL_PIN)
#define BSP_TWI_SDA_R    	(BSP_TWI_SDA_PORT->IDR & BSP_TWI_SDA_PIN)
#define BSP_TWI_SCL_R    	(BSP_TWI_SCL_PORT->IDR & BSP_TWI_SCL_PIN)
#define BspTwi_Delay()		__nop();__nop();__nop();__nop();
#elif	(defined(NRF51)||defined(NRF52))
#define BSP_TWI_SDA_H    	nrf_gpio_pin_write(BSP_TWI_SDA_PIN,1)
#define BSP_TWI_SDA_L    	nrf_gpio_pin_write(BSP_TWI_SDA_PIN,0)
#define BSP_TWI_SCL_H    	nrf_gpio_pin_write(BSP_TWI_SCL_PIN,1)
#define BSP_TWI_SCL_L    	nrf_gpio_pin_write(BSP_TWI_SCL_PIN,0)
#define BSP_TWI_SDA_R    	nrf_gpio_pin_read(BSP_TWI_SDA_PIN)
#define BSP_TWI_SCL_R    	nrf_gpio_pin_read(BSP_TWI_SCL_PIN)
#define BspTwi_Delay()		__nop();
#endif
#endif
//-------------------接口函数--------------------------------
uint8_t BspTwi_Init(void);
uint8_t BspTwi_SearchDevice(uint8_t *p_num,uint8_t *p_buf);
uint8_t BspTwi_Read_nByte(uint8_t device_addr,uint8_t addr,uint8_t *buf,uint16_t num);
uint8_t BspTwi_Write_nByte(uint8_t device_addr,uint8_t *buf,uint16_t num);
uint8_t BspTwi_Write1_nByte(uint8_t device_addr,uint8_t addr,uint8_t *buf,uint16_t num);
void BspTwi_DebugTestOnOff(uint8_t OnOff);
//-------------------------------------------------------------------------------
#endif

