/**
  ******************************************************************************
  * @file    Bsp_Rfms.h 
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  * @brief   bottom-driven -->   基于Rfms的数据驱动.
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
#ifndef __BSP_RFMS_H
#define __BSP_RFMS_H
//-------------------加载库函数------------------------------
#include "includes.h"
//-------------------接口宏定义(硬件相关)--------------------
#define RFMS_UARTX      1
#define RFMS_UART_BPS   19200
#define RFMS_REAL_VALUE (ADC_mV[0])
//-------------------接口宏定义(硬件无关)--------------------
//缓存环的尺寸
#define RFMS_BUF_MAX 50
//-------------------接口变量--------------------------------
//均值累加
extern uint32_t Rfms_BufAveSum;
//满足条件的差值次数累加
extern uint16_t Rfms_SubCount;
//差分阈值/连续2次采样满足一定差值的最小值/(EEPROM)
extern uint16_t Rfms_SubThreshold;
//调试信息
extern uint16_t Rfms_Timer1;
extern uint16_t Rfms_SubCounter;
//-------------------接口函数--------------------------------
extern uint8_t Rfms_Init(void);
extern uint16_t Rfms_GetSubThreshold(void);
extern void Rfms_SetSubThreshold(uint16_t value);
extern void Rfms_Count_20ms(void);
extern void Rfms_DebugTestWaveOnOff(uint8_t OnOff);
extern void Rfms_DebugTestOnOff(uint8_t OnOff);
//-------------------------------------------------------------------------------
#endif

