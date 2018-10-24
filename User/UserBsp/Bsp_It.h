/**
  ******************************************************************************
  * @file    Bsp_It.h 
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  * @brief   bottom-driven -->   中断入口.
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
             我们只需要添加需要的中断函数即可。一般中断函数名是固定的，除非您修改了启动文件中的函数名.
         启动文件是汇编语言文件，定了每个中断的服务函数，这些函数使用了WEAK 关键字，表示弱定义，因此如
         果我们在c文件中重定义了该服务函数（必须和它同名），那么启动文件的中断函数将自动无效。这也就
         函数重定义的概念。
  @endverbatim      
  ******************************************************************************
  * @attention
  *
  * GNU General Public License (GPL) 
  *
  * <h2><center>&copy; COPYRIGHT 2017 XSLXHN</center></h2>
  ******************************************************************************
  */ 

#ifndef __BSP_IT_H
#define __BSP_IT_H

//支持C++编译器
#ifdef __cplusplus
 extern "C" {
#endif
//-------------------加载库函数------------------------------
#include "includes.h"
//-------------------接口函数--------------------------------
void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
//void SysTick_Handler(void);  这个ISR被移到 bsp_timer.c
//-----------------------------------------------------------
#ifdef __cplusplus
}
#endif
//-----------------------------------------------------------
#endif

