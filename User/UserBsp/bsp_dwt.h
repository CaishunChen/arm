/**
  ******************************************************************************
  * @file    bsp_dwt.h 
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  * @brief   bottom-driven -->   数据观察点与跟踪(DWT)模块.
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
   			 在CM3，CM4中可以有3种跟踪源：ETM, ITM 和DWT，本驱动主要实现
         DWT中的时钟周期（CYCCNT）计数功能，此功能非常重要，可以很方便的
         计算程序执行的时钟周期个数
  @endverbatim      
  ******************************************************************************
  * @attention
  *
  * GNU General Public License (GPL) 
  *
  * <h2><center>&copy; COPYRIGHT 2017 XSLXHN</center></h2>
  ******************************************************************************
  */ 


#ifndef __BSP_DWT_H
#define __BSP_DWT_H

#define  DWT_CYCCNT  *(volatile unsigned int *)0xE0001004
void bsp_InitDWT(void);

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
