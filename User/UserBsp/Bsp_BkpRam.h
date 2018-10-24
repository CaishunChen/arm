/**
  ******************************************************************************
  * @file    Bsp_BkpRam.h 
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  * @brief   bottom-driven -->   芯片内部的备份区Ram.
  * @note    
  * @verbatim
    
 ===============================================================================
                     ##### How to use this driver #####
 ===============================================================================
   1,    适用芯片
         STM      :  STM32F1  STM32F4
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
#ifndef __BSP_BKP_RAM_H
#define __BSP_BKP_RAM_H
//-------------------加载库函数------------------------------
//-------------------接口宏定义(硬件无关)--------------------
#if   (defined(STM32F1))
//---->
#define  BSP_BKPRAM_RTC_CFG      BKP_DR1
#define  BSP_BKPRAM_POWER_CMT    BKP_DR2
#define  BSP_BKPRAM_WAKE_UP      BKP_DR3
#define  BSP_BKPRAM_GPRS_FLOW_H  BKP_DR4
#define  BSP_BKPRAM_GPRS_FLOW_L  BKP_DR5
#define  BSP_BKPRAM_MONTH        BKP_DR6
#define  BSP_BKPRAM_BELL_TIME_H  BKP_DR7
//<----
#elif (defined(STM32F4))
//---->
#define  BSP_BKPRAM_RTC_CFG      RTC_BKP_DR0
#define  BSP_BKPRAM_POWER_CMT    RTC_BKP_DR1
#define  BSP_BKPRAM_WAKE_UP      RTC_BKP_DR2
#define  BSP_BKPRAM_GPRS_FLOW_H  RTC_BKP_DR3
#define  BSP_BKPRAM_GPRS_FLOW_L  RTC_BKP_DR4
#define  BSP_BKPRAM_MONTH        RTC_BKP_DR5
#define  BSP_BKPRAM_BELL_TIME_H  RTC_BKP_DR6
//<----
#endif
//-------------------接口宏定义(硬件相关)--------------------
#if   (defined(STM32F1))
//---->
#define  BSP_BKPRAM_READ(addr)         BKP_ReadBackupRegister(addr)
#define  BSP_BKPRAM_WRITE(addr,value)  {PWR_BackupAccessCmd(ENABLE);BKP_WriteBackupRegister(addr,value);PWR_BackupAccessCmd(DISABLE);}
//<----
#elif (defined(STM32F4))
//---->
#define  BSP_BKPRAM_READ(addr)         RTC_ReadBackupRegister(addr)
#define  BSP_BKPRAM_WRITE(addr,value)  {PWR_BackupAccessCmd(ENABLE);RTC_WriteBackupRegister(addr,value);PWR_BackupAccessCmd(DISABLE);}
#endif
//-------------------接口变量--------------------------------
//-------------------接口函数--------------------------------
//-----------------------------------------------------------
#endif

