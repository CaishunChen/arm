/**
  ******************************************************************************
  * @file    Module_Err.h 
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  * @brief   Module -->   Err.
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

#ifndef __MODULE_ERR_H
#define __MODULE_ERR_H
//-------------------加载库函数------------------------------
#include "includes.h"
//-------------------接口宏定义(硬件相关)--------------------
//-------------------接口宏定义(硬件无关)--------------------
typedef enum
{
   //无错误
   MODULE_E_ERR_NULL    =  0,
   MODULE_E_ERR_NONE,
   //驱动层错误
   MODULE_E_ERR_BSP     =  1000,
   MODULE_E_ERR_RTC_VBAT,
   //系统层错误
   MODULE_E_ERR_SYS     =  2000,
   //模块层错误
   MODULE_E_ERR_MODULE  =  3000,
   MODULE_E_ERR_MEM,
   MODULE_E_ERR_CRC,
   //应用层错误
   MODULE_E_ERR_APP     =  4000, 
#if   (defined(PROJECT_XKAP_V3)||defined(XKAP_ICARE_B_D_M))
   //测量时间过短
   MODULE_E_ERR_SLEEP_MEASURN_SHORT,
   //体动过多
   MODULE_E_ERR_SLEEP_MOVE_TOO_MANY,
   //深睡过多
   MODULE_E_ERR_SLEEP_DEEP_TOO_MANY,
   //无信号
   MODULE_E_ERR_SLEEP_NO_SIGN,
   //蓝牙连接失败
   MODULE_E_ERR_BLUETOOTH_CONNECT,
   //蓝牙数据传输错误
   MODULE_E_ERR_BLUETOOTH_TRANSFER,
#endif
}MODULE_E_ERR;

typedef struct MODULE_S_ERR
{
    uint32_t UnixTime;
	uint16_t ErrId;
	uint16_t Para;
} MODULE_S_ERR;

//-------------------接口变量--------------------------------
//-------------------接口函数--------------------------------
//-----------------------------------------------------------
#endif
