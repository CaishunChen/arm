/**
  ******************************************************************************
  * @file    Bsp_CpuId.h 
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  * @brief   bottom-driven -->   获取芯片内部CPUID等信息.
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
         无
  @endverbatim      
  ******************************************************************************
  * @attention
  *
  * GNU General Public License (GPL) 
  *
  * <h2><center>&copy; COPYRIGHT 2017 XSLXHN</center></h2>
  ******************************************************************************
  */  
#ifndef __BSP_CPUID_H
#define __BSP_CPUID_H
//-------------------加载库函数------------------------------
#include "includes.h"
//-------------------接口宏定义(硬件相关)--------------------
#if   (defined(STM32F1))
#define BSP_CPUID_ADDR           0x1FFFF7E8
#define BSP_CPUID_FLASHSIZE_ADDR 0x1FFFF7E0
#elif (defined(STM32F4))
#define BSP_CPUID_ADDR           0x1FFF7A10
#define BSP_CPUID_FLASHSIZE_ADDR 0x1FFF7A22
#elif (defined(NRF51)||defined(NRF52))
#define BSP_CPUID_ADDR           NRF_FICR->DEVICEID
#define BSP_CPUID_FLASHSIZE_ADDR ((NRF_FICR->CODESIZE)*(NRF_FICR->CODEPAGESIZE))/1024
#else
#error Please Set Project to Bsp_CpuId.h
#endif
//-------------------接口函数--------------------------------
extern void BspCpuId_Get(uint8_t* pbuf,uint16_t* pflashSize);
extern void Bsp_CpuId_DebugTestOnOff(uint8_t OnOff);
//
extern uint8_t Bsp_CpuId_ReadTemp(int16_t *pTemp);
extern void Bsp_CpuId_ReadTemp_DebugTestOnOff(uint8_t OnOff);
//-------------------------------------------------------------------------------输出函数
#endif
