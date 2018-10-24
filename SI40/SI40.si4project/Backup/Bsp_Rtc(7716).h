/**
  ******************************************************************************
  * @file    Bsp_Rtc.h 
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  * @brief   bottom-driven -->   Rtc驱动，包含闹钟.
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
#ifndef __BSP_RTC_H
#define __BSP_RTC_H
//-------------------加载库函数------------------------------
#include "includes.h"
//-------------------接口宏定义(硬件相关)--------------------
#if   (defined(BOOTLOADER)\
   ||defined(PROJECT_BASE_STM32F1)\
   ||defined(PROJECT_BASE_STM32F4)\
   ||defined(PROJECT_ARMFLY_V5_XSL)\
	 ||defined(PROJECT_SPI_SLAVE)\
	 ||defined(PROJECT_BIB_TEST1))
//---->
//普通闹钟
//---闹钟触发外部函数
//#define RTC_ALARM_ON_FUNCTION
//---闹钟关闭外部函数
//#define RTC_ALARM_OFF_FUNCTION
//---闹钟关闭条件外部函数
#define RTC_ALARM_OFF_IF_FUNCTION      NULL
//智能闹钟
//---智能闹钟偏离时间
#define RTC_SMART_ALARM_OFFSET_TIME_S  NULL
//---智能闹钟启动条件
#define RTC_SMART_ALARM_ON_IF_1        NULL
#define RTC_SMART_ALARM_ON_IF_2        NULL
#define RTC_SMART_ALARM_ON_IF_3        NULL
//提醒
//#define RTC_WARN1_OFFSET1_TIME_S      (-5*60)
//#define RTC_WARN1_OFFSET2_TIME_S      (-15*60)
//#define RTC_WARN1_OFFSET3_TIME_S      (-30*60)
//#define RTC_WARN1_ON_FUNCTION           NULL
//#define RTC_WARN2_OFFSET1_TIME_S      (-5*60)
//#define RTC_WARN2_OFFSET2_TIME_S      (-15*60)
//#define RTC_WARN2_OFFSET3_TIME_S      (-30*60)
//#define RTC_WARN2_ON_FUNCTION           NULL
//#define RTC_WARN3_OFFSET1_TIME_S      (-5*60)
//#define RTC_WARN3_OFFSET2_TIME_S      (-15*60)
//#define RTC_WARN3_OFFSET3_TIME_S      (-30*60)
//#define RTC_WARN3_ON_FUNCTION           NULL
//#define RTC_WARN4_OFFSET1_TIME_S      (-5*60)
//#define RTC_WARN4_OFFSET2_TIME_S      (-15*60)
//#define RTC_WARN4_OFFSET3_TIME_S      (-30*60)
//#define RTC_WARN4_ON_FUNCTION           NULL
//自启动
#define RTC_AUTO_ON_FUNCTION
#define RTC_AUTO_OFF_FUNCTION
#define RTC_AUTO_UPDATA_STATE
//月更新
//#define RTC_MONTH_UPDATA_FUNCTION
//<----
#elif (defined(PROJECT_XKAP_V3)||defined(XKAP_ICARE_B_D_M))
//---->
//
#define RTC_1000MS_IN_INTERRUPT			0
//普通闹钟
//---闹钟触发外部函数
#include "Bsp_BuzzerMusic.h"
//#include "Bsp_Led.h"
#define RTC_ALARM_ON_FUNCTION          {Bsp_BuzzerMusic_PlayStop(ON,BSP_BUZZER_MUSIC_LIB_MORNING,90);uctsk_RfmsSetSleepLevelBeforeAlarm();}
//#define RTC_ALARM_ON_FUNCTION          {BSP_LED_LED1_ON;uctsk_RfmsSetSleepLevelBeforeAlarm();}
//---闹钟关闭执行外部函数
#define RTC_ALARM_OFF_FUNCTION         Bsp_BuzzerMusic_PlayStop(OFF,NULL,NULL);
//#define RTC_ALARM_OFF_FUNCTION         {BSP_LED_LED1_OFF;}
//---闹钟关闭条件外部函数
#include "Bsp_Key.h"
#define RTC_ALARM_OFF_IF_FUNCTION      ON==BspKey_IfPressKey()
//智能闹钟
//---智能闹钟偏离时间
#define RTC_SMART_ALARM_OFFSET_TIME_S  (-(24*60))
//---智能闹钟启动条件
#include "uctsk_RFMS.h"
#include "uctsk_Hci.h"
#include "uctsk_GprsNet.h"
#define RTC_SMART_ALARM_ON_IF_1        MODULE_MEMORY_E_SLEEPLEVEL_WAKE==uctsk_CurrentSleepLevel
#define RTC_SMART_ALARM_ON_IF_2        (MODULE_MEMORY_E_SLEEPLEVEL_WAKE==uctsk_CurrentSleepLevel)\
                                       ||(MODULE_MEMORY_E_SLEEPLEVEL_LIGHT==uctsk_CurrentSleepLevel)
#define RTC_SMART_ALARM_ON_IF_3        (MODULE_MEMORY_E_SLEEPLEVEL_WAKE==uctsk_CurrentSleepLevel)\
                                       ||(MODULE_MEMORY_E_SLEEPLEVEL_LIGHT==uctsk_CurrentSleepLevel)\
                                       ||(MODULE_MEMORY_E_SLEEPLEVEL_MIDDLE==uctsk_CurrentSleepLevel)
//提醒
#define RTC_WARN1_OFFSET1_TIME_S       0/*(-5*60)*/
//#define RTC_WARN1_OFFSET2_TIME_S     (-15*60)
//#define RTC_WARN1_OFFSET3_TIME_S     (-30*60)
#define RTC_WARN1_ON_FUNCTION          if(UCTSK_RFMS_E_RUNSTATE_IDLE==uctsk_Rfms_GetRunState())\
                                       {\
                                          Bsp_BuzzerMusic_PlayStop(ON,BSP_BUZZER_MUSIC_LIB_GF,0);\
                                          Hci_s_Bit_hint.bit0=1;\
                                       }
#define RTC_WARN2_OFFSET1_TIME_S       (-5)
//#define RTC_WARN2_OFFSET2_TIME_S      (-15*60)
//#define RTC_WARN2_OFFSET3_TIME_S      (-30*60)
#define RTC_WARN2_ON_FUNCTION          if(UCTSK_RFMS_E_RUNSTATE_IDLE==uctsk_Rfms_GetRunState())\
                                       {\
                                          Bsp_BuzzerMusic_PlayStop(ON,BSP_BUZZER_MUSIC_LIB_GF,0);\
                                          Hci_s_Bit_hint.bit1=1;\
                                       }
#define RTC_WARN3_OFFSET1_TIME_S       (-5)
//#define RTC_WARN3_OFFSET2_TIME_S      (-15*60)
//#define RTC_WARN3_OFFSET3_TIME_S      (-30*60)
#define RTC_WARN3_ON_FUNCTION          if(UCTSK_RFMS_E_RUNSTATE_IDLE==uctsk_Rfms_GetRunState())\
                                       {\
                                          Bsp_BuzzerMusic_PlayStop(ON,BSP_BUZZER_MUSIC_LIB_GF,0);\
                                          Hci_s_Bit_hint.bit2=1;\
                                       }
//#define RTC_WARN4_OFFSET1_TIME_S      (-5*60)
//#define RTC_WARN4_OFFSET2_TIME_S      (-15*60)
//#define RTC_WARN4_OFFSET3_TIME_S      (-30*60)
//#define RTC_WARN4_ON_FUNCTION           NULL
//自启动
#define RTC_AUTO_ON_FUNCTION           if((UCTSK_RFMS_E_RUNSTATE_IDLE==uctsk_Rfms_GetRunState()))\
                                       {\
                                          GprsNet_OnOff(OFF);\
                                          uctsk_Rfms_SetOnOff(ON,1);\
                                          Rtc_Auto_State  =  ON;\
                                       }
#define RTC_AUTO_OFF_FUNCTION          if((uctsk_Rfms_GetRunState()>=UCTSK_RFMS_E_RUNSTATE_START && uctsk_Rfms_GetRunState() <=UCTSK_RFMS_E_RUNSTATE_GET))\
                                       {\
                                          GprsNet_OnOff(ON);\
                                          uctsk_Rfms_SetOnOff(OFF,1);\
                                          Rtc_Auto_State  =  OFF;\
                                       }
#define RTC_AUTO_UPDATA_STATE          if(UCTSK_RFMS_E_RUNSTATE_IDLE==uctsk_Rfms_GetRunState())\
                                       {\
                                          Rtc_Auto_State  =  OFF;\
                                       }\
                                       else\
                                       {\
                                          Rtc_Auto_State  =  ON;\
                                       }
//月更新
#define RTC_MONTH_UPDATA_FUNCTION      GprsNet_FlowWrite(0)
//<----
#elif (defined(PROJECT_TCI_V30))
//---->
#include "Bsp_Led.h"
#define RTC_1000MS_IN_INTERRUPT			1
//普通闹钟
//---闹钟触发外部函数
//#define RTC_ALARM_ON_FUNCTION          NULL
//---闹钟关闭外部函数
//#define RTC_ALARM_OFF_FUNCTION         NULL
//---闹钟关闭条件外部函数
#define RTC_ALARM_OFF_IF_FUNCTION      NULL
//智能闹钟
//---智能闹钟偏离时间
#define RTC_SMART_ALARM_OFFSET_TIME_S  NULL
//---智能闹钟启动条件
#define RTC_SMART_ALARM_ON_IF_1        NULL
#define RTC_SMART_ALARM_ON_IF_2        NULL
#define RTC_SMART_ALARM_ON_IF_3        NULL
//提醒
//#define RTC_WARN1_OFFSET1_TIME_S       (-5*60)
//#define RTC_WARN1_OFFSET2_TIME_S       (-15*60)
//#define RTC_WARN1_OFFSET3_TIME_S       (-30*60)
//#define RTC_WARN1_ON_FUNCTION          NULL
//#define RTC_WARN2_OFFSET1_TIME_S      (-5*60)
//#define RTC_WARN2_OFFSET2_TIME_S      (-15*60)
//#define RTC_WARN2_OFFSET3_TIME_S      (-30*60)
//#define RTC_WARN2_ON_FUNCTION           NULL
//#define RTC_WARN3_OFFSET1_TIME_S      (-5*60)
//#define RTC_WARN3_OFFSET2_TIME_S      (-15*60)
//#define RTC_WARN3_OFFSET3_TIME_S      (-30*60)
//#define RTC_WARN3_ON_FUNCTION           NULL
//#define RTC_WARN4_OFFSET1_TIME_S      (-5*60)
//#define RTC_WARN4_OFFSET2_TIME_S      (-15*60)
//#define RTC_WARN4_OFFSET3_TIME_S      (-30*60)
//#define RTC_WARN4_ON_FUNCTION           NULL
//自启动
#define RTC_AUTO_ON_FUNCTION
#define RTC_AUTO_OFF_FUNCTION
#define RTC_AUTO_UPDATA_STATE
//月更新
//#define RTC_MONTH_UPDATA_FUNCTION
//秒精确函数
#define RTC_PRECISE_1S_FUNCTION1       BSP_LED_LED1_TOGGLE
//<----
#elif (defined(XKAP_ICARE_B_M))
#include "Bsp_Hx711.h"
//---->
#define RTC_1000MS_IN_INTERRUPT			1
//普通闹钟
//---闹钟触发外部函数
//#define RTC_ALARM_ON_FUNCTION
//---闹钟关闭外部函数
//#define RTC_ALARM_OFF_FUNCTION
//---闹钟关闭条件外部函数
#define RTC_ALARM_OFF_IF_FUNCTION      NULL
//智能闹钟
//---智能闹钟偏离时间
#define RTC_SMART_ALARM_OFFSET_TIME_S  NULL
//---智能闹钟启动条件
#define RTC_SMART_ALARM_ON_IF_1        NULL
#define RTC_SMART_ALARM_ON_IF_2        NULL
#define RTC_SMART_ALARM_ON_IF_3        NULL
//秒精确函数
#define RTC_PRECISE_1S_FUNCTION1       BspHx711_Precise1000ms()
//<----
#elif (defined(BASE_NRF52)||defined(XKAP_ICARE_A_M)||defined(XKAP_ICARE_A_S))
//---->
#define RTC_1000MS_IN_INTERRUPT			1
//普通闹钟
//---闹钟触发外部函数
//#define RTC_ALARM_ON_FUNCTION
//---闹钟关闭外部函数
//#define RTC_ALARM_OFF_FUNCTION
//---闹钟关闭条件外部函数
#define RTC_ALARM_OFF_IF_FUNCTION      NULL
//智能闹钟
//---智能闹钟偏离时间
#define RTC_SMART_ALARM_OFFSET_TIME_S  NULL
//---智能闹钟启动条件
#define RTC_SMART_ALARM_ON_IF_1        NULL
#define RTC_SMART_ALARM_ON_IF_2        NULL
#define RTC_SMART_ALARM_ON_IF_3        NULL
//提醒
//#define RTC_WARN1_OFFSET1_TIME_S      (-5*60)
//#define RTC_WARN1_OFFSET2_TIME_S      (-15*60)
//#define RTC_WARN1_OFFSET3_TIME_S      (-30*60)
//#define RTC_WARN1_ON_FUNCTION           NULL
//#define RTC_WARN2_OFFSET1_TIME_S      (-5*60)
//#define RTC_WARN2_OFFSET2_TIME_S      (-15*60)
//#define RTC_WARN2_OFFSET3_TIME_S      (-30*60)
//#define RTC_WARN2_ON_FUNCTION           NULL
//#define RTC_WARN3_OFFSET1_TIME_S      (-5*60)
//#define RTC_WARN3_OFFSET2_TIME_S      (-15*60)
//#define RTC_WARN3_OFFSET3_TIME_S      (-30*60)
//#define RTC_WARN3_ON_FUNCTION           NULL
//#define RTC_WARN4_OFFSET1_TIME_S      (-5*60)
//#define RTC_WARN4_OFFSET2_TIME_S      (-15*60)
//#define RTC_WARN4_OFFSET3_TIME_S      (-30*60)
//#define RTC_WARN4_ON_FUNCTION           NULL
//自启动
//#define RTC_AUTO_ON_FUNCTION
//#define RTC_AUTO_OFF_FUNCTION
//#define RTC_AUTO_UPDATA_STATE
//月更新
//#define RTC_MONTH_UPDATA_FUNCTION
//<----
#elif (defined(BASE_NRF51))
//---->
#include "Bsp_Led.h"
#define RTC_1000MS_IN_INTERRUPT			1
//普通闹钟
//---闹钟触发外部函数
//#define RTC_ALARM_ON_FUNCTION
//---闹钟关闭外部函数
//#define RTC_ALARM_OFF_FUNCTION
//---闹钟关闭条件外部函数
#define RTC_ALARM_OFF_IF_FUNCTION      NULL
//智能闹钟
//---智能闹钟偏离时间
#define RTC_SMART_ALARM_OFFSET_TIME_S  NULL
//---智能闹钟启动条件
#define RTC_SMART_ALARM_ON_IF_1        NULL
#define RTC_SMART_ALARM_ON_IF_2        NULL
#define RTC_SMART_ALARM_ON_IF_3        NULL
//秒精确函数
#define RTC_PRECISE_1S_FUNCTION1       BSP_LED_LED1_TOGGLE
#elif (defined(PROJECT_NRF5X_BLE))
//---->
#include "Bsp_Led.h"
#include "Bsp_Pwr.h"
#include "Bsp_NrfBle.h"
#define RTC_1000MS_IN_INTERRUPT			1
//普通闹钟
//---闹钟触发外部函数
//#define RTC_ALARM_ON_FUNCTION
//---闹钟关闭外部函数
//#define RTC_ALARM_OFF_FUNCTION
//---闹钟关闭条件外部函数
#define RTC_ALARM_OFF_IF_FUNCTION      NULL
//智能闹钟
//---智能闹钟偏离时间
#define RTC_SMART_ALARM_OFFSET_TIME_S  NULL
//---智能闹钟启动条件
#define RTC_SMART_ALARM_ON_IF_1        NULL
#define RTC_SMART_ALARM_ON_IF_2        NULL
#define RTC_SMART_ALARM_ON_IF_3        NULL
//秒精确函数
#define RTC_PRECISE_1S_FUNCTION1       BspPwr_Cmd_timer_1s()
#define RTC_PRECISE_1S_FUNCTION2       BspNrfBle_Timer(1000)
//<----
#elif (defined(XKAP_ICARE_B_C)||defined(XKAP_ICARE_B_D))
//---->
#define RTC_1000MS_IN_INTERRUPT			1
//普通闹钟
//---闹钟触发外部函数
//#define RTC_ALARM_ON_FUNCTION
//---闹钟关闭外部函数
//#define RTC_ALARM_OFF_FUNCTION
//---闹钟关闭条件外部函数
#define RTC_ALARM_OFF_IF_FUNCTION      NULL
//智能闹钟
//---智能闹钟偏离时间
#define RTC_SMART_ALARM_OFFSET_TIME_S  NULL
//---智能闹钟启动条件
#define RTC_SMART_ALARM_ON_IF_1        NULL
#define RTC_SMART_ALARM_ON_IF_2        NULL
#define RTC_SMART_ALARM_ON_IF_3        NULL
//秒精确函数
//<----
#else
#error Please Set Project to Bsp_Rtc.h
#endif
//-------------------接口宏定义(硬件无关)--------------------
#ifdef FACTORY_TIME_YEAR
#define BSP_RTC_DEFAULT_YEAR     (FACTORY_TIME_YEAR)
#define BSP_RTC_DEFAULT_MONTH    (FACTORY_TIME_MONTH)
#define BSP_RTC_DEFAULT_DAY      (FACTORY_TIME_DAY)
#define BSP_RTC_DEFAULT_HOUR     (FACTORY_TIME_HOUR)
#define BSP_RTC_DEFAULT_MINUTE   (FACTORY_TIME_MINUTE)
#define BSP_RTC_DEFAULT_SECOND   (FACTORY_TIME_SECONT)
#else
#define BSP_RTC_DEFAULT_YEAR     2015
#define BSP_RTC_DEFAULT_MONTH    7
#define BSP_RTC_DEFAULT_DAY      10
#define BSP_RTC_DEFAULT_HOUR     0
#define BSP_RTC_DEFAULT_MINUTE   0
#define BSP_RTC_DEFAULT_SECOND   0

#endif
//
#define BSP_RTC_ALARM_MAX_NUM    2
#define BSP_RTC_WARN_MAX_NUM     4
//-------------------接口变量--------------------------------
extern uint8_t BspRtc_Err;
extern uint8_t BspRtc_PowerDown;
extern uint8_t BspRtc_CurrentTimeBuf[6];
extern uint8_t BspRtc_SoftResetTime_s;
//-------------------接口函数--------------------------------
extern void BspRtc_Init(void);
extern void BspRtc_1000ms(void);
//实时时钟
extern uint8_t BspRtc_ReadRealTime(uint8_t *ptime5,struct tm *pCurrentTime_tm,time_t *pUnixTime,uint8_t *ptime6);
extern uint8_t BspRtc_SetRealTime(uint8_t *ptime5,struct tm *pCurrentTime_tm,time_t *pUnixTime,uint8_t *ptime6);
//闹钟
extern void BspRtc_ReadAlarm(uint8_t num,uint16_t *pvalue,uint8_t *palarmMode);
extern void BspRtc_SetAlarm(uint8_t num,uint16_t *pvalue,uint8_t *palarmMode);
//提醒
extern void BspRtc_SetWarn(uint8_t num,uint8_t OnOff,uint16_t time);
extern void BspRtc_ReadWarn(uint8_t num,uint8_t *pOnOff,uint16_t *ptime);
//自启动
extern void BspRtc_SetAuto(uint8_t OnOff,uint16_t BeginTime,uint16_t EndTime);
//制式标志
extern void BspRtc_SetFormat(uint8_t value);
extern void BspRtc_ReadFormat(uint8_t *pvalue);
//获取起始到现在运行的时间
extern uint32_t BspRtc_GetBeginToNowS(void);
//
extern void BspRtc_AlarmConfig(uint8_t AlarmNum,uint8_t *ptime6);
//测试
extern void BspRtc_DebugTestOnOff(uint8_t OnOff);
//--------------------------------
#endif
