/*
***********************************************************************************
*                    作    者: 徐松亮
*                    更新时间: 2015-06-03
***********************************************************************************
*/
#ifndef __MODULE_ERR_H
#define __MODULE_ERR_H
//-------------------加载库函数------------------------------
#include "includes.h"
//-------------------接口宏定义(硬件相关)--------------------
//-------------------接口宏定义(硬件无关)--------------------
enum MODULE_E_ERR
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
};
//-------------------接口变量--------------------------------
//-------------------接口函数--------------------------------
//-----------------------------------------------------------
#endif
