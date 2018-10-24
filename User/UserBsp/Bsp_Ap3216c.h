/**
  ******************************************************************************
  * @file    Bsp_Ap3216c.h 
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  * @brief   bottom-driven -->   环境光、接近、红外灯
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
         环境光   (自动滤波50/60Hz)
            范围  440/1760/7038/28152
            输出  16位AD   0-65535
         接近     (高环境光抑制)
            输出  10位AD   0-1023
            模式  4个可编程红外LED
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
#ifndef __BSP_AP3216C_H
#define __BSP_AP3216C_H
//-------------------加载库函数------------------------------
#include "includes.h"
//-------------------接口宏定义(硬件相关)--------------------
#if   (defined(XKAP_ICARE_B_M))
#if		(HARDWARE_SUB_VER==1 || HARDWARE_SUB_VER==2)
#define BSP_AP3216C_INT_PIN               10
#elif	(HARDWARE_SUB_VER==3)
#define BSP_AP3216C_INT_PIN               17
#endif
#define BSP_AP3216C_AUTO
#endif
//-------------------接口宏定义(硬件无关)--------------------
// 器件地址
#define BSP_AP3216C_ADDR                  0x3C
// 寄存器地址-系统
#define BSP_AP3216C_REG_SYS_CONF          0x00
#define BSP_AP3216C_REG_SYS_INT_STA       0x01
#define BSP_AP3216C_REG_SYS_INT_CLR       0x02
#define BSP_AP3216C_REG_SYS_IR_DATA_L     0x0A
#define BSP_AP3216C_REG_SYS_IR_DATA_H     0x0B
#define BSP_AP3216C_REG_SYS_ALS_DATA_L    0x0C
#define BSP_AP3216C_REG_SYS_ALS_DATA_H    0x0D
#define BSP_AP3216C_REG_SYS_PS_DATA_L     0x0E
#define BSP_AP3216C_REG_SYS_PS_DATA_H     0x0F
// 寄存器地址-环境光(ambient light sensor) 
#define BSP_AP3216C_REG_ALS_CONF          0x10
#define BSP_AP3216C_REG_ALS_CAL           0x19
#define BSP_AP3216C_REG_ALS_LOW_THRD_L    0x1A
#define BSP_AP3216C_REG_ALS_LOW_THRD_H    0x1B
#define BSP_AP3216C_REG_ALS_HIGH_THRD_L   0x1C
#define BSP_AP3216C_REG_ALS_HIGH_THRD_H   0x1D
// 寄存器地址-接近(proximity sensor)
#define BSP_AP3216C_REG_PS_CONF           0x20
#define BSP_AP3216C_REG_PS_LED_DRIVER     0x21
#define BSP_AP3216C_REG_PS_INT_FORM       0x22
#define BSP_AP3216C_REG_PS_MEAN_TIME      0x23
#define BSP_AP3216C_REG_PS_LED_WAIT_TIME  0x24
#define BSP_AP3216C_REG_PS_CAL_LOW_L      0x28
#define BSP_AP3216C_REG_PS_CAL_LOW_H      0x29
#define BSP_AP3216C_REG_PS_LOW_THR_L      0x2A
#define BSP_AP3216C_REG_PS_LOW_THR_H      0x2B
#define BSP_AP3216C_REG_PS_HIGH_THR_L     0x2C
#define BSP_AP3216C_REG_PS_HIGH_THR_H     0x2D
//-------------------接口函数--------------------------------
#ifdef BSP_AP3216C_AUTO
extern uint16_t BspAp3216c_Als,BspAp3216c_Ps,BspAp3216c_Ir;
#endif
//-------------------接口函数--------------------------------
uint8_t BspAp3216c_Init(void);
uint8_t BspAp3216c_Read(uint16_t *p_ALS,uint16_t *p_PS,uint16_t *p_IR);
void BspAp3216c_DebugTest_100ms(void);
void BspAp3216c_DebugTestOnOff(uint8_t OnOff);
//-------------------------------------------------------------------------------
#endif

