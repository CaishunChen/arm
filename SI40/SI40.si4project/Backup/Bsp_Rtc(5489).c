/**
  ******************************************************************************
  * @file    Bsp_Rtc.c 
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  ******************************************************************************
  * @attention
  *
  * GNU General Public License (GPL) 
  *
  * <h2><center>&copy; COPYRIGHT 2017 XSLXHN</center></h2>
  ******************************************************************************
  */ 
/*******************************************************************************
*                               UNIX时间戳
*本文件实现基于RTC的日期功能，提供年月日的读写。
* RTC中保存的时间格式，是格式的。即一个32bit的time_t变量（实为u32）
*
* ANSI-C的标准库中，提供了两种表示时间的数据型：
* time_t:
*       UNIX时间戳（从1970-1-1起到某时间经过的秒数）
*   typedef unsigned int time_t;
* struct tm:
*       Calendar格式（年月日形式）
*       tm结构如下：
*       struct tm {
*              int tm_sec;   // 秒 0 to 60
*              int tm_min;   // 分 0 to 59
*              int tm_hour;  // 时 0 to 23
*              int tm_mday;  // 日 1 to 31
*              int tm_mon;   // 月 0 to 11
*              int tm_year;  // 年   since 1900
*              int tm_wday;  // 星期 since Sunday, 0 to 6
*              int tm_yday;  // 从元旦起的天数 days since January 1, 0 to 365
*              int tm_isdst; // 夏令时
*              ...
*        }
*   其中wday，yday可以自动产生，软件直接读取
*   mon的取值为0-11
*   ***注意***：
*   tm_year:在time.h库中定义为1900年起的年份，即2008年应表示为2008-1900=108
*   这种表示方法对用户来说不是十分友好，与现实有较大差异。
*   所以在本文件中，屏蔽了这种差异。
*   即外部调用本文件的函数时，tm结构体类型的日期，tm_year即为2008
*   注意：若要调用系统库time.c中的函数，需要自行将tm_year-=1900
* 成员函数说明：
* struct tm Time_ConvUnixToCalendar(time_t t);
*   输入一个Unix时间戳（time_t），返回Calendar格式日期
* time_t Time_ConvCalendarToUnix(struct tm t);
*   输入一个Calendar格式日期，返回Unix时间戳（time_t）
* time_t Time_GetUnixTime(void);
*   从RTC取当前时间的Unix时间戳值
* struct tm Time_GetCalendarTime(void);
*   从RTC取当前时间的日历时间
* void Time_SetUnixTime(time_t);
*   输入UNIX时间戳格式时间，设置为当前RTC时间
* void Time_SetCalendarTime(struct tm t);
*   输入Calendar格式时间，设置为当前RTC时间
*
* 外部调用实例：
* 定义一个Calendar格式的日期变量：
* struct tm now;
* now.tm_year = 2008;
* now.tm_mon = 11;      //12月
* now.tm_mday = 20;
* now.tm_hour = 20;
* now.tm_min = 12;
* now.tm_sec = 30;
*
* 获取当前日期时间：
* tm_now = Time_GetCalendarTime();
* 然后可以直接读tm_now.tm_wday获取星期数
*
* 设置时间：
* Step1. tm_now.xxx = xxxxxxxxx;
* Step2. Time_SetCalendarTime(tm_now);
*
* 计算两个时间的差
* struct tm t1,t2;
* t1_t = Time_ConvCalendarToUnix(t1);
* t2_t = Time_ConvCalendarToUnix(t2);
* dt = t1_t - t2_t;
* dt就是两个时间差的秒数
* dt_tm = mktime(dt);   //注意dt的年份匹配，ansi库中函数为相对年份，注意超限
* 另可以参考相关资料，调用ansi-c库的格式化输出等功能，ctime，strftime等
*

STM32F1:
 RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
 PWR_BackupAccessCmd(ENABLE);
*******************************************************************************/
//------------------------------- Includes -----------------------------------
#include "Bsp_Rtc.h"
#if   (defined(STM32F1)||defined(STM32F4))
#include "uctsk_Debug.h"
#include "Bsp_CpuFlash.h"
#include "Module_Memory.h"
#include "Bsp_Pwr.h"
#include "Bsp_BkpRam.h"
#elif (defined(NRF51)||defined(NRF52))
#include "uctsk_Debug.h"
#include "nrf_drv_clock.h"
#include "nrf_drv_rtc.h"
#include "Bsp_Led.h"
#endif
//------------------------------------------------------------
uint8_t BspRtc_Err = 0;        //硬件晶振错误
uint8_t BspRtc_PowerDown = 0;  //硬件掉电错误
uint8_t BspRtc_CurrentTimeBuf[6];
//-------------------------------------------------------------
//------------------------------- 用户变量 --------------------
struct tm CurrentDate;  //当前时间
//闹钟(高字节表示小时(0~23),低字节表示分钟(0~59))
static uint16_t   Rtc_Alarm[BSP_RTC_ALARM_MAX_NUM]= {0xFFFF,0xFFFF};
//闹钟模式(0-关闭,1-定时,2-智能)
static uint8_t    Rtc_AlarmMode[BSP_RTC_ALARM_MAX_NUM]= {0};
// 闹钟触发位图
static uint8_t    Rtc_AlarmTriggerBitMap=0;
//提醒
static uint8_t    Rtc_Warn_OnOff[BSP_RTC_WARN_MAX_NUM]= {OFF,OFF,OFF,OFF};
static uint16_t   Rtc_Warn_Time[BSP_RTC_WARN_MAX_NUM]= {0,0,0,0};
//自启动
static uint8_t    Rtc_Auto_OnOff    =  OFF;
static uint16_t   Rtc_Auto_BeginTime=  0;
static uint16_t   Rtc_Auto_EndTime  =  0;
static uint8_t    Rtc_Auto_State    =  OFF;
//12/24格式
static uint8_t    Rtc_Format=0;
//闹钟运行位图
static uint8_t    Rtc_AlarmRunBitMap=  0;
static uint8_t    Rtc_AlarmRunState =  OFF;
//运行计时
static uint32_t   Rtc_BeginToNow_s  =  0;
//月备份
static uint8_t    Rtc_MonthBak      =  0;
#if (defined(NRF51)||defined(NRF52))
uint32_t   Rtc_Timer_s = 0;
#if  (!defined(SOFTDEVICE_PRESENT)) 
#define COMPARE_COUNTERTIME  (3UL)
const nrf_drv_rtc_t rtc = NRF_DRV_RTC_INSTANCE(0);
static void rtc_handler(nrf_drv_rtc_int_type_t int_type);
static void rtc_handler(nrf_drv_rtc_int_type_t int_type)
{
    static uint8_t si=0;
    // 3秒周期
    if (int_type == NRF_DRV_RTC_INT_COMPARE0)
    {
        //BSP_LED_LED2_TOGGLE;
    }
    // 8Hz Tick
    else if (int_type == NRF_DRV_RTC_INT_TICK)
    {
        si++;
        if(si>=8)
        {
            si=0;
            BspRtc_1000ms();
            // RTC运行
            Rtc_Timer_s++;
        }
    }
}
#endif
#elif (defined(STM32F1))
void RTC_IRQHandler(void)
{
   static uint8_t Display;
   if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
   {
      BspRtc_1000ms();
      //
      PWR_BackupAccessCmd(ENABLE);
      RTC_ClearITPendingBit(RTC_IT_SEC);
      PWR_BackupAccessCmd(DISABLE);
   }
}
#elif (defined(STM32F4))
void RTC_WKUP_IRQHandler(void)
{
   MODULE_OS_DISABLE_INT;
	if(RTC_GetITStatus(RTC_IT_WUT) != RESET)
	{
		BspRtc_1000ms();
		//
		PWR_BackupAccessCmd(ENABLE);
		RTC_ClearITPendingBit(RTC_IT_WUT);
		EXTI_ClearITPendingBit(EXTI_Line22);
      PWR_BackupAccessCmd(DISABLE);
	} 
   MODULE_OS_ENABLE_INT;
}
#endif
//------------------------------- 用户函数声明 ----------------
//
static time_t Time_GetUnixTime(void);
static struct tm Time_GetCalendarTime(void);
static void Time_SetUnixTime(time_t t_unix);
static void Time_SetCalendarTime(struct tm t_tm);
//
static void BspRtc_Config(void);
static void BspRtc_WakeUpConfig(void);
/*******************************************************************************
* Function Name  : Time_GetUnixTime()
* Description    : 从RTC取当前时间的Unix时间戳值
* Input          : None
* Output         : None
* Return         : time_t t
*******************************************************************************/
static time_t Time_GetUnixTime(void)
{
#if   (defined(STM32F1))
    return (time_t)RTC_GetCounter();
#elif (defined(STM32F4))
    struct tm t_tm;
    t_tm=Time_GetCalendarTime();
    return Count_Time_ConvCalendarToUnix(t_tm);
#elif (defined(NRF51)||defined(NRF52))
    return Rtc_Timer_s;
#endif
}
/*******************************************************************************
* Function Name  : Time_GetCalendarTime()
* Description    : 从RTC取当前时间的日历时间（struct tm）
* Input          : None
* Output         : None
* Return         : time_t t
*******************************************************************************/
static struct tm Time_GetCalendarTime(void)
{
    struct tm t_tm;
#if   (defined(STM32F1))
    time_t t_t;
    t_t = (time_t)RTC_GetCounter();
    t_tm = Count_Time_ConvUnixToCalendar(t_t);
    return t_tm;
#elif (defined(STM32F4))
    RTC_DateTypeDef  RTC_DateStructure;
    RTC_TimeTypeDef  RTC_TimeStructure;
    RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);
    RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
    // STM32-BUG 不加此语句，时钟0点日期持续半分钟不跳变.
    RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);
    t_tm.tm_year  =  RTC_DateStructure.RTC_Year+2000;
    t_tm.tm_mon   =  RTC_DateStructure.RTC_Month-1;
    t_tm.tm_mday  =  RTC_DateStructure.RTC_Date;
    //t_tm.tm_wday  =  RTC_DateStructure.RTC_WeekDay;
    //t_tm.tm_wday  =  Count_YYMMDDToWeek((uint8_t)(t_tm.tm_year-2000),(uint8_t)(t_tm.tm_mon),(uint8_t)(t_tm.tm_mday));
    t_tm.tm_wday  =  Count_ZellerWeek(t_tm.tm_year,t_tm.tm_mon+1,t_tm.tm_mday);
    t_tm.tm_hour  =  RTC_TimeStructure.RTC_Hours;
    t_tm.tm_min   =  RTC_TimeStructure.RTC_Minutes;
    t_tm.tm_sec   =  RTC_TimeStructure.RTC_Seconds;
    return t_tm;
#elif (defined(NRF51)||defined(NRF52))
    time_t t_t;
    t_t = (time_t)Rtc_Timer_s;
    t_tm = Count_Time_ConvUnixToCalendar(t_t);
    return t_tm;
#endif
}
/*******************************************************************************
* Function Name  : Time_SetUnixTime()
* Description    : 将给定的Unix时间戳写入RTC
* Input          : time_t t
* Output         : None
* Return         : None
*******************************************************************************/
static void Time_SetUnixTime(time_t t_unix)
{
#if   (defined(STM32F1))
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    PWR_BackupAccessCmd(ENABLE);
    //
    RTC_WaitForLastTask();
    RTC_SetCounter((u32)t_unix);
    RTC_WaitForLastTask();
    //
    BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
#elif (defined(STM32F4))
    struct tm t_tm;
    t_tm = Count_Time_ConvUnixToCalendar(t_unix);
    Time_SetCalendarTime(t_tm);
#elif (defined(NRF51)||defined(NRF52))
    Rtc_Timer_s = (uint32_t)t_unix;
#endif
}
/*******************************************************************************
* Function Name  : Time_SetCalendarTime()
* Description    : 将给定的Calendar格式时间转换成UNIX时间戳写入RTC
* Input          : struct tm t
* Output         : None
* Return         : None
*******************************************************************************/
static void Time_SetCalendarTime(struct tm t_tm)
{
#if   (defined(STM32F1)||defined(NRF51)||defined(NRF52))
    Time_SetUnixTime(Count_Time_ConvCalendarToUnix(t_tm));
    return;
#elif (defined(STM32F4))
    RTC_DateTypeDef  RTC_DateStructure,RTC_DateStructure1;
    RTC_TimeTypeDef  RTC_TimeStructure;
    PWR_BackupAccessCmd(ENABLE);
    RTC_DateStructure.RTC_Year      =  t_tm.tm_year-2000;
    RTC_DateStructure.RTC_Month     =  t_tm.tm_mon+1;
    RTC_DateStructure.RTC_Date      =  t_tm.tm_mday;
    //RTC_DateStructure.RTC_WeekDay   =  Count_YYMMDDToWeek((uint8_t)(t_tm.tm_year-2000),(uint8_t)(t_tm.tm_mon),(uint8_t)(t_tm.tm_mday));
    RTC_DateStructure.RTC_WeekDay   =  Count_ZellerWeek(t_tm.tm_year,t_tm.tm_mon+1,t_tm.tm_mday);
    while(1)
    {
        RTC_SetDate(RTC_Format_BIN, &RTC_DateStructure);
        RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure1);
        if(RTC_DateStructure.RTC_Year==RTC_DateStructure1.RTC_Year
           || RTC_DateStructure.RTC_Month==RTC_DateStructure1.RTC_Month
           || RTC_DateStructure.RTC_Date==RTC_DateStructure1.RTC_Date)
        {
            break;
        }
    }
    RTC_TimeStructure.RTC_Hours     =  t_tm.tm_hour;
    RTC_TimeStructure.RTC_Minutes   =  t_tm.tm_min;
    RTC_TimeStructure.RTC_Seconds   =  t_tm.tm_sec;
    RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure);
    PWR_BackupAccessCmd(DISABLE);
#endif
}
//---------------------------------------------------------------------------------------
/*******************************************************************************
* Function Name  : RTC_Configuration
* Description    : 来重新配置RTC和BKP，仅在检测到后备寄存器数据丢失时使用
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void BspRtc_Config(void)
{
#if   (defined(STM32F1))
    //启用PWR和BKP的时钟（from APB1）
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    //后备域解锁
    PWR_BackupAccessCmd(ENABLE);
    //备份寄存器模块复位
    BKP_DeInit();
    //外部32.768K
    RCC_LSEConfig(RCC_LSE_ON);
    //等待 LSE 准备好
    while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
    //RTC时钟源配置成LSE（外部32.768K）
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
    //RTC开启
    RCC_RTCCLKCmd(ENABLE);
    //开启后需要等待APB1时钟与RTC时钟同步，才能读写寄存器
    RTC_WaitForSynchro();
    //读写寄存器前，要确定上一个操作已经结束
    RTC_WaitForLastTask();
    //设置RTC分频器，使RTC时钟为1Hz
    //RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1)
    RTC_SetPrescaler(32767);
    //等待寄存器写入完成
    RTC_WaitForLastTask();
    //
    {
        struct tm t_tm;
        t_tm.tm_year  =  BSP_RTC_DEFAULT_YEAR;
        t_tm.tm_mon   =  BSP_RTC_DEFAULT_MONTH-1;
        t_tm.tm_mday  =  BSP_RTC_DEFAULT_DAY;
        t_tm.tm_hour  =  BSP_RTC_DEFAULT_HOUR;
        t_tm.tm_min   =  BSP_RTC_DEFAULT_MINUTE;
        t_tm.tm_sec   =  BSP_RTC_DEFAULT_SECOND;
        Time_SetCalendarTime(t_tm);
    }
    //
    BSP_BKPRAM_WRITE(BSP_BKPRAM_RTC_CFG, 0xA5A5);
    //上锁
    PWR_BackupAccessCmd(DISABLE);
#elif (defined(STM32F4))
    RTC_InitTypeDef  RTC_InitStructure;
    /* 使能 PWR 时钟 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    /* 允许访问 RTC */
    PWR_BackupAccessCmd(ENABLE);
    /* 选择LSI做为RTC时钟源 */
#if 1
    RCC_LSEConfig(RCC_LSE_ON);
    while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
#else
    RCC_LSICmd(ENABLE);
    while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
#endif
    /* 使能RTC时钟 */
    RCC_RTCCLKCmd(ENABLE);
    /* 等待 RTC APB 寄存器同步 */
    RTC_WaitForSynchro();
    //
    RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
    RTC_InitStructure.RTC_SynchPrediv  = 0xFF;
    RTC_InitStructure.RTC_HourFormat   = RTC_HourFormat_24;
    RTC_Init(&RTC_InitStructure);
    //
    //RTC_OutputConfig(RTC_Output_Disable, RTC_Output_AlarmA);
    //
    {
        struct tm t_tm;
        t_tm.tm_year  =  BSP_RTC_DEFAULT_YEAR;
        t_tm.tm_mon   =  BSP_RTC_DEFAULT_MONTH-1;
        t_tm.tm_mday  =  BSP_RTC_DEFAULT_DAY;
        t_tm.tm_hour  =  BSP_RTC_DEFAULT_HOUR;
        t_tm.tm_min   =  BSP_RTC_DEFAULT_MINUTE;
        t_tm.tm_sec   =  BSP_RTC_DEFAULT_SECOND;
        Time_SetCalendarTime(t_tm);
    }
    //
    BSP_BKPRAM_WRITE(BSP_BKPRAM_RTC_CFG, 0xA5A5);
    PWR_BackupAccessCmd(DISABLE);
#elif ((defined(NRF51)||defined(NRF52))&&(!defined(SOFTDEVICE_PRESENT)))
    // -----rtc_config
    {
        uint32_t err_code;
        //Initialize RTC instance
        nrf_drv_rtc_config_t config = NRF_DRV_RTC_DEFAULT_CONFIG;
        config.prescaler = 4095;
        err_code = nrf_drv_rtc_init(&rtc, &config, rtc_handler);
        APP_ERROR_CHECK(err_code);
        //Enable tick event & interrupt
        nrf_drv_rtc_tick_enable(&rtc,true);
        //Set compare channel to trigger interrupt after COMPARE_COUNTERTIME seconds
        err_code = nrf_drv_rtc_cc_set(&rtc,0,COMPARE_COUNTERTIME * 8,true);
        APP_ERROR_CHECK(err_code);
        //Power on RTC instance
        nrf_drv_rtc_enable(&rtc);
    }
    //-----
#endif
}
//-------------------------------------------------------------
void BspRtc_Init(void)
{
    // 预防警告
    Rtc_Alarm[0]=Rtc_Alarm[0];
    Rtc_AlarmMode[0]=Rtc_AlarmMode[0];
    Rtc_AlarmTriggerBitMap=Rtc_AlarmTriggerBitMap;
    Rtc_Warn_OnOff[0]=Rtc_Warn_OnOff[0];
    Rtc_Warn_Time[0]=Rtc_Warn_Time[0];
    Rtc_Auto_OnOff=Rtc_Auto_OnOff;
    Rtc_Auto_BeginTime=Rtc_Auto_BeginTime;
    Rtc_Auto_EndTime=Rtc_Auto_EndTime;
    Rtc_Auto_State=Rtc_Auto_State;
    Rtc_Format=Rtc_Format;
    Rtc_AlarmRunBitMap=Rtc_AlarmRunBitMap;
    Rtc_AlarmRunState=Rtc_AlarmRunState;
    Rtc_MonthBak=Rtc_MonthBak;
    //
#if (defined(STM32F1)||defined(STM32F4))
    if (BSP_BKPRAM_READ(BSP_BKPRAM_RTC_CFG) != 0xA5A5)
    {
        //初始化RTC配置
        BspRtc_Config();
        BspRtc_PowerDown=1;
    }
    BspRtc_WakeUpConfig();
    // 获取时间
    CurrentDate  = Time_GetCalendarTime();
    // 获取闹钟时间
    {
        MODULE_MEMORY_S_PARA *pPara;
        uint8_t err;
        pPara=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        Bsp_CpuFlash_Read(BSP_CPU_FLASH_PARA_ADDR,(uint8_t*)pPara,sizeof(MODULE_MEMORY_S_PARA));
        for(err=0; err<BSP_RTC_ALARM_MAX_NUM; err++)
        {
            Rtc_Alarm[err]       =  pPara->RtcAlarm[err];
            Rtc_AlarmMode[err]   =  pPara->RtcAlarmMode[err];
        }
        //提醒
        for(err=0; err<BSP_RTC_WARN_MAX_NUM; err++)
        {
            Rtc_Warn_OnOff[err]  =  pPara->RtcWarnOnOff[err];
            Rtc_Warn_Time[err]   =  pPara->RtcWarnTime[err];
        }
        //自启动
        Rtc_Auto_OnOff  =  pPara->RtcAutoOnOff;
        Rtc_Auto_BeginTime =  pPara->RtcAutoBeginTime;
        Rtc_Auto_EndTime   =  pPara->RtcAutoEndTime;
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pPara);
    }
    //月
    Rtc_MonthBak = BSP_BKPRAM_READ(BSP_BKPRAM_MONTH);
#elif (defined(NRF51)||defined(NRF52))
    //注册定时器
    BspRtc_Config();
    BspRtc_WakeUpConfig();
    BspRtc_CurrentTimeBuf[0]   =   (uint8_t)(BSP_RTC_DEFAULT_YEAR-2000);
    BspRtc_CurrentTimeBuf[1]   =   (uint8_t)BSP_RTC_DEFAULT_MONTH;
    BspRtc_CurrentTimeBuf[2]   =   (uint8_t)BSP_RTC_DEFAULT_DAY;
    BspRtc_CurrentTimeBuf[3]   =   (uint8_t)BSP_RTC_DEFAULT_HOUR;
    BspRtc_CurrentTimeBuf[4]   =   (uint8_t)BSP_RTC_DEFAULT_MINUTE;
    BspRtc_CurrentTimeBuf[5]   =   (uint8_t)BSP_RTC_DEFAULT_SECOND;
    BspRtc_SetRealTime(NULL,NULL,NULL,BspRtc_CurrentTimeBuf);
    CurrentDate  = Time_GetCalendarTime();
#endif
}
/*******************************************************************************
函数功能: 设置闹钟
参    数: 无
返 回 值: 无
*******************************************************************************/
void BspRtc_SetAlarm(uint8_t num,uint16_t *pvalue,uint8_t *palarmMode)
{
#if (defined(STM32F1)||defined(STM32F4))
    MODULE_MEMORY_S_PARA *pPara;
    if(num<1||num>BSP_RTC_ALARM_MAX_NUM)
    {
        return;
    }
    pPara=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    Bsp_CpuFlash_Read(BSP_CPU_FLASH_PARA_ADDR,(uint8_t*)pPara,sizeof(MODULE_MEMORY_S_PARA));
    if(pvalue!=NULL)
    {
        pPara->RtcAlarm[num-1]  =  *pvalue;
        Rtc_Alarm[num-1]        =  *pvalue;
    }
    if(palarmMode!=NULL)
    {
        pPara->RtcAlarmMode[num-1]  =  *palarmMode;
        Rtc_AlarmMode[num-1]        =  *palarmMode;
    }
    Bsp_CpuFlash_Write(BSP_CPU_FLASH_PARA_ADDR,(uint8_t*)pPara,sizeof(MODULE_MEMORY_S_PARA));
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pPara);
    //
    Rtc_AlarmTriggerBitMap=0;
#else
    num=num;
    pvalue=pvalue;
    palarmMode=palarmMode;
#endif
}
/*******************************************************************************
函数功能: 读取闹钟
参    数: 无
返 回 值: 无
*******************************************************************************/
void BspRtc_ReadAlarm(uint8_t num,uint16_t *pvalue,uint8_t *palarmMode)
{
#if (defined(STM32F1)||defined(STM32F4))
    if(num<1||num>BSP_RTC_ALARM_MAX_NUM)
    {
        return;
    }
    if(pvalue!=NULL)
    {
        *pvalue       =  Rtc_Alarm[num-1];
    }
    if(palarmMode!=NULL)
    {
        *palarmMode   =  Rtc_AlarmMode[num-1];
    }
#else
    num=num;
    pvalue=pvalue;
    palarmMode=palarmMode;
#endif
}
/*******************************************************************************
函数功能: 设置时钟表达方式
参    数: 无
返 回 值: 无
*******************************************************************************/
void BspRtc_SetFormat(uint8_t value)
{
#if (defined(STM32F1)||defined(STM32F4))
    MODULE_MEMORY_S_PARA *pPara;
    pPara=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    Bsp_CpuFlash_Read(BSP_CPU_FLASH_PARA_ADDR,(uint8_t*)pPara,sizeof(MODULE_MEMORY_S_PARA));
    pPara->RtcFormat=value;
    Bsp_CpuFlash_Write(BSP_CPU_FLASH_PARA_ADDR,(uint8_t*)pPara,sizeof(MODULE_MEMORY_S_PARA));
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pPara);
    Rtc_Format=value;
#else
    value=value;
#endif
}
/*******************************************************************************
函数功能: 读取时钟表达方式
参    数: 无
返 回 值: 无
*******************************************************************************/
void BspRtc_ReadFormat(uint8_t *pvalue)
{
#if (defined(STM32F1)||defined(STM32F4))
    *pvalue=Rtc_Format;
#else
    pvalue=pvalue;
#endif
}
/*******************************************************************************
函数功能: 设置提醒
参    数: 无
返 回 值: 无
*******************************************************************************/
void BspRtc_SetWarn(uint8_t num,uint8_t OnOff,uint16_t time)
{
#if (defined(STM32F1)||defined(STM32F4))
    MODULE_MEMORY_S_PARA *pPara;
    if(num<1||num>BSP_RTC_WARN_MAX_NUM)
    {
        return;
    }
    pPara=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    Bsp_CpuFlash_Read(BSP_CPU_FLASH_PARA_ADDR,(uint8_t*)pPara,sizeof(MODULE_MEMORY_S_PARA));
    pPara->RtcWarnOnOff[num-1] =  OnOff;
    pPara->RtcWarnTime[num-1] =  time;
    Bsp_CpuFlash_Write(BSP_CPU_FLASH_PARA_ADDR,(uint8_t*)pPara,sizeof(MODULE_MEMORY_S_PARA));
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pPara);
    Rtc_Warn_OnOff[num-1]=  OnOff;
    Rtc_Warn_Time[num-1] =  time;
#else
    num=num;
    OnOff=OnOff;
    time=time;
#endif
}
/*******************************************************************************
函数功能: 读取提醒
参    数: 无
返 回 值: 无
*******************************************************************************/
void BspRtc_ReadWarn(uint8_t num,uint8_t *pOnOff,uint16_t *ptime)
{
#if (defined(STM32F1)||defined(STM32F4))
    if(num<1||num>BSP_RTC_WARN_MAX_NUM)
    {
        return;
    }
    *pOnOff =  Rtc_Warn_OnOff[num-1];
    *ptime  =  Rtc_Warn_Time[num-1];
#else
    num=num;
    pOnOff=pOnOff;
    ptime=ptime;
#endif
}
/*******************************************************************************
函数功能: 设置自启动
参    数: OnOff BeginTime EndTime
返 回 值: void
*******************************************************************************/
void BspRtc_SetAuto(uint8_t OnOff,uint16_t BeginTime,uint16_t EndTime)
{
#if (defined(STM32F1)||defined(STM32F4))
    MODULE_MEMORY_S_PARA *pPara;
    pPara=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_R,(uint8_t*)pPara,NULL);
    pPara->RtcAutoOnOff       =  OnOff;
    pPara->RtcAutoBeginTime   =  BeginTime;
    pPara->RtcAutoEndTime     =  EndTime;
    Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_W,(uint8_t*)pPara,NULL);
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pPara);
    Rtc_Auto_OnOff      =  OnOff;
    Rtc_Auto_BeginTime  =  BeginTime;
    Rtc_Auto_EndTime    =  EndTime;
    if(Rtc_Auto_OnOff==OFF)
    {
        Rtc_Auto_State=ON;
    }
#else
    OnOff=OnOff;
    BeginTime=BeginTime;
    EndTime=EndTime;
#endif
}
/*******************************************************************************
函数功能: 读取自启动
参    数: *pOnOff *pBeginTime *pEndTime
返 回 值: ON/OFF
*******************************************************************************/
uint8_t BspRtc_ReadAuto(uint8_t *pOnOff,uint16_t *pBeginTime,uint16_t *pEndTime)
{
#if (defined(STM32F1)||defined(STM32F4))
    if(pOnOff!=NULL)
    {
        *pOnOff       =  Rtc_Auto_OnOff;
    }
    if(pBeginTime!=NULL)
    {
        *pBeginTime   =  Rtc_Auto_BeginTime;
    }
    if(pEndTime!=NULL)
    {
        *pEndTime     =  Rtc_Auto_EndTime;
    }
    return Rtc_Auto_State;
#else
    pOnOff=pOnOff;
    pBeginTime=pBeginTime;
    pEndTime=pEndTime;
    return OFF;
#endif
}
//--------------------------------------------------------------
void BspRtc_1000ms(void)
{
    uint8_t i,j;
    int32_t i32=0,j32=0;
    struct tm *ptm;
    static uint16_t secondBak=0;
    static uint8_t count=0;
    static uint32_t AlarmTimerSBuf[BSP_RTC_ALARM_MAX_NUM]= {0};
#ifdef   RTC_PRECISE_1S_FUNCTION1
    RTC_PRECISE_1S_FUNCTION1;
#endif
#ifdef   RTC_PRECISE_1S_FUNCTION2
    RTC_PRECISE_1S_FUNCTION2;
#endif
#ifdef   RTC_PRECISE_1S_FUNCTION3
    RTC_PRECISE_1S_FUNCTION3;
#endif
#ifdef   RTC_PRECISE_1S_FUNCTION4
    RTC_PRECISE_1S_FUNCTION4;
#endif
#ifdef   RTC_PRECISE_1S_FUNCTION5
    RTC_PRECISE_1S_FUNCTION5;
#endif
#if   (defined(RTC_WARN1_OFFSET1_TIME_S)||defined(RTC_WARN1_OFFSET2_TIME_S)||defined(RTC_WARN1_OFFSET3_TIME_S))
    // 提醒触发位图
    static uint8_t Rtc_WarnTriggerBitMap[BSP_RTC_WARN_MAX_NUM]= {0};
#endif
    i=i;
    j=j;
    i32=i32;
    j32=j32;
    ptm=ptm;
    secondBak=secondBak;
    count=count;
    AlarmTimerSBuf[0]=AlarmTimerSBuf[0];
    // 运行计时
    Rtc_BeginToNow_s++;
    //----------监控错误
    CurrentDate  = Time_GetCalendarTime();
    BspRtc_CurrentTimeBuf[0]=CurrentDate.tm_year-2000;
    BspRtc_CurrentTimeBuf[1]=CurrentDate.tm_mon+1;
    BspRtc_CurrentTimeBuf[2]=CurrentDate.tm_mday;
    BspRtc_CurrentTimeBuf[3]=CurrentDate.tm_hour;
    BspRtc_CurrentTimeBuf[4]=CurrentDate.tm_min;
    BspRtc_CurrentTimeBuf[5]=CurrentDate.tm_sec;
    //
    if(CurrentDate.tm_sec!=secondBak)
    {
        secondBak = CurrentDate.tm_sec;
        if(BspRtc_Err==1)
        {
            if(count==0)
            {
                BspRtc_Err=0;
            }
            else
            {
                count--;
            }
        }
    }
    else
    {
        if(count>=10)
        {
            BspRtc_Err=1;
        }
        else
        {
            count++;
        }
    }
    //----------
    //月更新
#ifdef   RTC_MONTH_UPDATA_FUNCTION
    if(Rtc_MonthBak!=CurrentDate.tm_mon)
    {
        RTC_MONTH_UPDATA_FUNCTION;
        Rtc_MonthBak=CurrentDate.tm_mon;
        BSP_BKPRAM_WRITE(BSP_BKPRAM_MONTH,Rtc_MonthBak);
    }
#endif
    //----------自启动
#ifdef   RTC_AUTO_ON_FUNCTION
    if(Rtc_Auto_OnOff==ON)
    {
        //当前时间(时分秒)
        i32 = CurrentDate.tm_hour*3600+CurrentDate.tm_min*60+CurrentDate.tm_sec;
        //起始时间
        j32 = (Rtc_Auto_BeginTime>>8)*3600+(Rtc_Auto_BeginTime&0x00FF)*60;
        j32 = i32-j32;
        if((j32==0 || j32==1)&&Rtc_Auto_State==OFF)
        {
            RTC_AUTO_ON_FUNCTION;
        }
        //结束时间
        j32 = (Rtc_Auto_EndTime>>8)*3600+(Rtc_Auto_EndTime&0x00FF)*60;
        j32 = i32-j32;
        if((j32==0 || j32==1)&&Rtc_Auto_State==ON)
        {
            RTC_AUTO_OFF_FUNCTION;
        }
        //更新Rtc_Auto_State
        RTC_AUTO_UPDATA_STATE;
    }
#endif
#if (defined(STM32F1)||defined(STM32F4))
    //申请缓存
    ptm = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    //----------提醒
    for(i=0; i<BSP_RTC_WARN_MAX_NUM; i++)
    {
        if(Rtc_Warn_OnOff[i]!=ON)
        {
            continue;
        }
        //转换闹钟时间为时间戳
        ptm->tm_year =  CurrentDate.tm_year;
        ptm->tm_mon  =  CurrentDate.tm_mon;
        ptm->tm_mday =  CurrentDate.tm_mday;
        ptm->tm_hour =  (uint8_t)(Rtc_Warn_Time[i]>>8);
        ptm->tm_min  =  (uint8_t)(Rtc_Warn_Time[i]);
        ptm->tm_sec  =  0x00;
        i32 = Count_TimeCompare(&CurrentDate,ptm);
        //触发
#if   (defined(RTC_WARN1_OFFSET1_TIME_S)||defined(RTC_WARN1_OFFSET2_TIME_S)||defined(RTC_WARN1_OFFSET3_TIME_S))
        if(i==0)
        {
#ifdef RTC_WARN1_OFFSET1_TIME_S
            j=0;
            if(RTC_WARN1_OFFSET1_TIME_S<=i32 && i32<3 && ((Rtc_WarnTriggerBitMap[i]&(1<<j))==0))
            {
                //触发标志置位
                Rtc_WarnTriggerBitMap[i]  |= (1<<j);
                //运行
                RTC_WARN1_ON_FUNCTION;
            }
            else if(i32>=3)
            {
                Rtc_WarnTriggerBitMap[i]  &= (~(1<<j));
            }
#endif
            //
#ifdef RTC_WARN1_OFFSET2_TIME_S
            j=1;
            if(RTC_WARN1_OFFSET2_TIME_S<=i32 && i32<0 && ((Rtc_WarnTriggerBitMap[i]&(1<<j))==0))
            {
                //触发标志置位
                Rtc_WarnTriggerBitMap[i]  |= (1<<j);
                //运行
                RTC_WARN1_ON_FUNCTION;
            }
            else if(i32>0)
            {
                Rtc_WarnTriggerBitMap[i]  &= (~(1<<j));
            }
#endif
#ifdef RTC_WARN1_OFFSET3_TIME_S
            j=2;
            if(RTC_WARN1_OFFSET3_TIME_S<=i32 && i32<0 && ((Rtc_WarnTriggerBitMap[i]&(1<<j))==0))
            {
                //触发标志置位
                Rtc_WarnTriggerBitMap[i]  |= (1<<j);
                //运行
                RTC_WARN1_ON_FUNCTION;
            }
            else if(i32>0)
            {
                Rtc_WarnTriggerBitMap[i]  &= (~(1<<j));
            }
#endif
        }
#endif
#if   (defined(RTC_WARN2_OFFSET1_TIME_S)||defined(RTC_WARN2_OFFSET2_TIME_S)||defined(RTC_WARN2_OFFSET3_TIME_S))
        if(i==1)
        {
#ifdef RTC_WARN2_OFFSET1_TIME_S
            j=0;
            if(RTC_WARN2_OFFSET1_TIME_S<=i32 && i32<0 && ((Rtc_WarnTriggerBitMap[i]&(1<<j))==0))
            {
                //触发标志置位
                Rtc_WarnTriggerBitMap[i]  |= (1<<j);
                //运行
                RTC_WARN2_ON_FUNCTION;
            }
            else if(i32>0)
            {
                Rtc_WarnTriggerBitMap[i]  &= (~(1<<j));
            }
#endif
            //
#ifdef RTC_WARN2_OFFSET2_TIME_S
            j=1;
            if(RTC_WARN2_OFFSET2_TIME_S<=i32 && i32<0 && ((Rtc_WarnTriggerBitMap[i]&(1<<j))==0))
            {
                //触发标志置位
                Rtc_WarnTriggerBitMap[i]  |= (1<<j);
                //运行
                RTC_WARN2_ON_FUNCTION;
            }
            else if(i32>0)
            {
                Rtc_WarnTriggerBitMap[i]  &= (~(1<<j));
            }
#endif
#ifdef RTC_WARN2_OFFSET3_TIME_S
            j=2;
            if(RTC_WARN2_OFFSET3_TIME_S<=i32 && i32<0 && ((Rtc_WarnTriggerBitMap[i]&(1<<j))==0))
            {
                //触发标志置位
                Rtc_WarnTriggerBitMap[i]  |= (1<<j);
                //运行
                RTC_WARN2_ON_FUNCTION;
            }
            else if(i32>0)
            {
                Rtc_WarnTriggerBitMap[i]  &= (~(1<<j));
            }
#endif
        }
#endif
#if   (defined(RTC_WARN3_OFFSET1_TIME_S)||defined(RTC_WARN3_OFFSET2_TIME_S)||defined(RTC_WARN3_OFFSET3_TIME_S))
        if(i==2)
        {
#ifdef RTC_WARN3_OFFSET1_TIME_S
            j=0;
            if(RTC_WARN3_OFFSET1_TIME_S<=i32 && i32<0 && ((Rtc_WarnTriggerBitMap[i]&(1<<j))==0))
            {
                //触发标志置位
                Rtc_WarnTriggerBitMap[i]  |= (1<<j);
                //运行
                RTC_WARN3_ON_FUNCTION;
            }
            else if(i32>0)
            {
                Rtc_WarnTriggerBitMap[i]  &= (~(1<<j));
            }
#endif
            //
#ifdef RTC_WARN3_OFFSET2_TIME_S
            j=1;
            if(RTC_WARN3_OFFSET2_TIME_S<=i32 && i32<0 && ((Rtc_WarnTriggerBitMap[i]&(1<<j))==0))
            {
                //触发标志置位
                Rtc_WarnTriggerBitMap[i]  |= (1<<j);
                //运行
                RTC_WARN3_ON_FUNCTION;
            }
            else if(i32>0)
            {
                Rtc_WarnTriggerBitMap[i]  &= (~(1<<j));
            }
#endif
#ifdef RTC_WARN3_OFFSET3_TIME_S
            j=2;
            if(RTC_WARN3_OFFSET3_TIME_S<=i32 && i32<0 && ((Rtc_WarnTriggerBitMap[i]&(1<<j))==0))
            {
                //触发标志置位
                Rtc_WarnTriggerBitMap[i]  |= (1<<j);
                //运行
                RTC_WARN3_ON_FUNCTION;
            }
            else if(i32>0)
            {
                Rtc_WarnTriggerBitMap[i]  &= (~(1<<j));
            }
#endif
        }
#endif
#if   (defined(RTC_WARN4_OFFSET1_TIME_S)||defined(RTC_WARN4_OFFSET2_TIME_S)||defined(RTC_WARN4_OFFSET3_TIME_S))
        if(i==3)
        {
#ifdef RTC_WARN4_OFFSET1_TIME_S
            j=0;
            if(RTC_WARN4_OFFSET1_TIME_S<=i32 && i32<0 && ((Rtc_WarnTriggerBitMap[i]&(1<<j))==0))
            {
                //触发标志置位
                Rtc_WarnTriggerBitMap[i]  |= (1<<j);
                //运行
                RTC_WARN4_ON_FUNCTION;
            }
            else if(i32>0)
            {
                Rtc_WarnTriggerBitMap[i]  &= (~(1<<j));
            }
#endif
            //
#ifdef RTC_WARN4_OFFSET2_TIME_S
            j=1;
            if(RTC_WARN4_OFFSET2_TIME_S<=i32 && i32<0 && ((Rtc_WarnTriggerBitMap[i]&(1<<j))==0))
            {
                //触发标志置位
                Rtc_WarnTriggerBitMap[i]  |= (1<<j);
                //运行
                RTC_WARN4_ON_FUNCTION;
            }
            else if(i32>0)
            {
                Rtc_WarnTriggerBitMap[i]  &= (~(1<<j));
            }
#endif
#ifdef RTC_WARN4_OFFSET3_TIME_S
            j=2;
            if(RTC_WARN4_OFFSET3_TIME_S<=i32 && i32<0 && ((Rtc_WarnTriggerBitMap[i]&(1<<j))==0))
            {
                //触发标志置位
                Rtc_WarnTriggerBitMap[i]  |= (1<<j);
                //运行
                RTC_WARN4_ON_FUNCTION;
            }
            else if(i32>0)
            {
                Rtc_WarnTriggerBitMap[i]  &= (~(1<<j));
            }
#endif
        }
#endif
    }
    //----------闹钟
    for(i=0; i<BSP_RTC_ALARM_MAX_NUM; i++)
    {
        //闹钟未使能
        if(Rtc_AlarmMode[i]==0)
        {
            continue;
        }
        //小时容错
        j=Rtc_Alarm[i]>>8;
        if(j>23)
        {
            continue;
        }
        //分钟容错
        j=(uint8_t)Rtc_Alarm[i];
        if(j>59)
        {
            continue;
        }
        //定时闹钟条件判断
        //转换闹钟时间为时间戳
        ptm->tm_year =  CurrentDate.tm_year;
        ptm->tm_mon  =  CurrentDate.tm_mon;
        ptm->tm_mday =  CurrentDate.tm_mday;
        ptm->tm_hour =  (uint8_t)(Rtc_Alarm[i]>>8);
        ptm->tm_min  =  (uint8_t)(Rtc_Alarm[i]);
        ptm->tm_sec  =  0x00;
        i32 = Count_TimeCompare(&CurrentDate,ptm);
        //条件判定---定时闹钟 || 智能闹钟
        //定时闹钟 && 时差3秒 && 闹钟未触发
        if((  (Rtc_AlarmMode[i]==1) && (0<=i32) && (i32<=3) && ((Rtc_AlarmTriggerBitMap&(1<<i))==0))
           //测量时 智能闹钟 && 时差(---24...16---8---0) && 闹钟未触发 && 觉醒
           ||((Rtc_AlarmMode[i]==2) && \
              (RTC_SMART_ALARM_OFFSET_TIME_S<=i32) && (i32<=(RTC_SMART_ALARM_OFFSET_TIME_S/3)*2) && \
              ((Rtc_AlarmTriggerBitMap&(1<<i))==0) && (RTC_SMART_ALARM_ON_IF_1))
           //测量时 智能闹钟 && 时差(---24---16...8---0) && 闹钟未触发 && (觉醒||浅睡)
           ||((Rtc_AlarmMode[i]==2) && \
              ((RTC_SMART_ALARM_OFFSET_TIME_S/3)*2<=i32) && (i32<=(RTC_SMART_ALARM_OFFSET_TIME_S/3)*1) && \
              ((Rtc_AlarmTriggerBitMap&(1<<i))==0) && (RTC_SMART_ALARM_ON_IF_2))
           //测量时 智能闹钟 && 时差(---24---16---8...0) && 闹钟未触发 && (觉醒||浅睡||中睡)
           ||((Rtc_AlarmMode[i]==2) && \
              ((RTC_SMART_ALARM_OFFSET_TIME_S/3)*1<=i32) && (i32<=0) && \
              ((Rtc_AlarmTriggerBitMap&(1<<i))==0) && (RTC_SMART_ALARM_ON_IF_3))
           //智能闹钟为定时闹钟
           ||((Rtc_AlarmMode[i]==2) && (0<=i32) && (i32<=3) && ((Rtc_AlarmTriggerBitMap&(1<<i))==0))
          )
        {
            //触发标志置位
            Rtc_AlarmTriggerBitMap  |= (1<<i);
            //运行标志置位
            Rtc_AlarmRunBitMap      |= (1<<i);
            //清空关闭条件
            if(RTC_ALARM_OFF_IF_FUNCTION)
            {
                i32=0;
            }
            //计时器清零
            AlarmTimerSBuf[i]=0;
        }
        // 90秒闹钟关闭
        if(i32>=90)
        {
            //智能闹钟,则24分钟后触发标志复位
            if(Rtc_AlarmMode[i]==2&&(i32>=(24*60)))
            {
                Rtc_AlarmTriggerBitMap  &= (~(1<<i));
            }
            //普通闹钟,则90秒后触发标志复位
            else if(Rtc_AlarmMode[i]==1&&i32>=90)
            {
                Rtc_AlarmTriggerBitMap  &= (~(1<<i));
            }
            //运行标志复位
            Rtc_AlarmRunBitMap      &= (~(1<<i));
        }
        // 闹钟正在运行 & 关闭条件满足
        if((Rtc_AlarmRunBitMap&(1<<i))!=0)
        {
            //
            if(Rtc_AlarmMode[i]==2)
            {
                AlarmTimerSBuf[i]++;
                if(AlarmTimerSBuf[i]>=90)
                {
                    //运行标志复位
                    Rtc_AlarmRunBitMap &= (~(1<<i));
                    AlarmTimerSBuf[i]=0;
                }
            }
        }

    }
    if(RTC_ALARM_OFF_IF_FUNCTION)
    {
        //运行标志复位
        Rtc_AlarmRunBitMap = 0;
    }
    //执行(只要有一个闹钟条件满足,则就要执行)
    if(Rtc_AlarmRunBitMap!=0)
    {
#ifdef   RTC_ALARM_ON_FUNCTION
        RTC_ALARM_ON_FUNCTION;
#endif
        //闹钟响铃标志置位
        Rtc_AlarmRunState  =  ON;
    }
    else if(Rtc_AlarmRunState  ==  ON)
    {
#ifdef   RTC_ALARM_OFF_FUNCTION
        RTC_ALARM_OFF_FUNCTION;
#endif
        Rtc_AlarmRunState  =  OFF;
    }
    //释放缓存
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,ptm);
#endif
}
//-------------------------------------------------------------------------------以下为定制接口函数
//-------------------------------------------------------------------------------
// 函数功能: 读取实时时间设置
// 入口参数: *time 短5位时间变量 标准节构2时间
// 返回参数: 调用函数后，可读取时间节构1和节构2
// 修改说明：
// 修改时间：
//-------------------------------------------------------------------------------
uint8_t BspRtc_ReadRealTime(uint8_t *ptime5,struct tm *pCurrentTime_tm,time_t *pUnixTime,uint8_t *ptime6)
{
    if(ptime5!=NULL)
    {
        struct tm tm1;
        //读取当前时间
        tm1=Time_GetCalendarTime();
        Count_TimeTypeConvert(1,&tm1,ptime5);
    }
    if(pCurrentTime_tm!=NULL)
    {
        *pCurrentTime_tm=Time_GetCalendarTime();
    }
    if(pUnixTime!=NULL)
    {
        *pUnixTime = Time_GetUnixTime();
    }
    if(ptime6!=NULL)
    {
        struct tm tm1;
        //读取当前时间
        tm1=Time_GetCalendarTime();
        ptime6[0]=tm1.tm_year-2000;
        ptime6[1]=tm1.tm_mon+1;
        ptime6[2]=tm1.tm_mday;
        ptime6[3]=tm1.tm_hour;
        ptime6[4]=tm1.tm_min;
        ptime6[5]=tm1.tm_sec;
    }
    return OK;
}
uint32_t BspRtc_GetBeginToNowS(void)
{
    return Rtc_BeginToNow_s;
}
//-------------------------------------------------------------------------------
// 函数名称: INT8U SetRtc(INT8U *time)
// 函数功能: 实时时间设置
// 入口参数: *time 时间
// 返回参数: 无
// 修改说明：
// 修改时间：
//-------------------------------------------------------------------------------
uint8_t BspRtc_SetRealTime(uint8_t *ptime5,struct tm *pCurrentTime_tm,time_t *pUnixTime,uint8_t *ptime6)
{
    struct tm *ptm;
    uint8_t res1=OK;
    //申请缓存
    ptm=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    //转换5字节短时间time为tm格式
    if(ptime5!=NULL)
    {
        Count_TimeTypeConvert(2,ptm,ptime5);
        Time_SetCalendarTime(*ptm);
    }
    else if(pCurrentTime_tm!=NULL)
    {
        memcpy((char*)ptm,(char*)pCurrentTime_tm,sizeof(struct tm));
        Time_SetCalendarTime(*ptm);
    }
    else if(pUnixTime!=NULL)
    {
        Time_SetUnixTime(*pUnixTime);
    }
    else if(ptime6!=NULL)
    {
        ptm->tm_year =  ptime6[0]+2000;
        ptm->tm_mon  =  ptime6[1]-1;
        ptm->tm_mday =  ptime6[2];
        ptm->tm_hour =  ptime6[3];
        ptm->tm_min  =  ptime6[4];
        ptm->tm_sec  =  ptime6[5];
        Time_SetCalendarTime(*ptm);
    }
    else
    {
        res1 = ERR;
    }
    //释放缓存
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,ptm);
    return res1;
}
static void BspRtc_WakeUpConfig(void)
{   
#if   (defined(STM32F1))   
    PWR_BackupAccessCmd(ENABLE);
    RTC_ITConfig(RTC_IT_SEC, ENABLE);
    RTC_WaitForLastTask();
    PWR_BackupAccessCmd(DISABLE);
#elif (defined(STM32F4))
    NVIC_InitTypeDef NVIC_InitStructure; 
    EXTI_InitTypeDef EXTI_InitStructure;
    //
    PWR_BackupAccessCmd(ENABLE);
    //
    EXTI_ClearITPendingBit(EXTI_Line22);
    EXTI_InitStructure.EXTI_Line = EXTI_Line22;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    //
    NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);  
    //
    RTC_WakeUpCmd(DISABLE);
    // 配置RTC唤醒时钟源: CK_SPRE (1Hz) */
    RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
    RTC_SetWakeUpCounter(0x0);
    // 清除WakeUp中断标志
    RTC_ClearITPendingBit(RTC_IT_WUT);
    // 清除Line22中断标志
    EXTI_ClearITPendingBit(EXTI_Line22);
    // 使能RTC唤醒中断
    RTC_ITConfig(RTC_IT_WUT, ENABLE);
    // 使能唤醒计数
    RTC_WakeUpCmd(ENABLE);
    PWR_BackupAccessCmd(DISABLE);
#endif    
}
void BspRtc_AlarmConfig(uint8_t AlarmNum,uint8_t *ptime6)
{
#if (defined(STM32F1)||defined(NRF51)||defined(NRF52))
    AlarmNum = AlarmNum;
    ptime6 = ptime6;
#elif (defined(STM32F4))
    EXTI_InitTypeDef EXTI_InitStructure;
    RTC_AlarmTypeDef RTC_AlarmStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    if(AlarmNum<1||AlarmNum>2)
    {
        return;
    }
    // 打开闹钟权限
    PWR_BackupAccessCmd(ENABLE);
    if(AlarmNum==1)
    {
        RTC_AlarmCmd(RTC_Alarm_A,DISABLE);
        // 设置闹钟A屏蔽
        // --- 屏蔽日期/星期/天
        //RTC_AlarmStructure.RTC_AlarmMask             =  RTC_AlarmMask_All;
        RTC_AlarmStructure.RTC_AlarmMask             =  RTC_AlarmMask_DateWeekDay;
        // ---
        //RTC_AlarmStructure.RTC_AlarmDateWeekDaySel   =  RTC_AlarmDateWeekDaySel_Date;
        //RTC_AlarmStructure.RTC_AlarmDateWeekDay      =  RTC_Weekday_Monday;
        RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours   =  ptime6[3];
        RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes =  ptime6[4];
        RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds =  ptime6[5];
        RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStructure);
        // 使能中断
        RTC_ITConfig(RTC_IT_ALRA, ENABLE);
        RTC_AlarmCmd(RTC_Alarm_A, ENABLE);
    }
    else if(AlarmNum==2)
    {
        RTC_AlarmCmd(RTC_Alarm_B,DISABLE);
        // 设置闹钟A屏蔽
        RTC_AlarmStructure.RTC_AlarmMask             =  RTC_AlarmMask_DateWeekDay;
        RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours   =  ptime6[3];
        RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes =  ptime6[4];
        RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds =  ptime6[5];
        RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_B, &RTC_AlarmStructure);
        // 使能中断
        RTC_ITConfig(RTC_IT_ALRB, ENABLE);
        RTC_AlarmCmd(RTC_Alarm_B, ENABLE);
    }
    //
    PWR_BackupAccessCmd(DISABLE);
    // 外中断配置
    EXTI_ClearITPendingBit(EXTI_Line17);
    EXTI_InitStructure.EXTI_Line     =  EXTI_Line17;
    EXTI_InitStructure.EXTI_Mode     =  EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger  =  EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd  =  ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    // 中断优先级配置
    NVIC_InitStructure.NVIC_IRQChannel                    =  RTC_Alarm_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority  =  0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority         =  0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                 =  ENABLE;
    NVIC_Init(&NVIC_InitStructure);
#endif
}
//-------------------------------------------------------------------------------
// 函数功能: 设置系统时间
// 入口参数:
// 返回参数:
// 修改说明：
// 修改时间：
// 注    意:
//-------------------------------------------------------------------------------
void BspRtc_DebugTestOnOff(uint8_t OnOff)
{
    uint8_t *pbuf;
    uint16_t len;
    uint8_t i;
    uint8_t d;
    struct tm *ptm;
    OnOff=OnOff;
    //申请缓存
    pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    ptm = (struct tm *)&pbuf[128];
    //显示当前时间
    BspRtc_ReadRealTime(NULL,ptm,NULL,NULL);
    sprintf((char*)pbuf,"Time : %04d-%02d-%02d %02d:%02d:%02d(%d)\r\n",ptm->tm_year,ptm->tm_mon+1,ptm->tm_mday,ptm->tm_hour,ptm->tm_min,ptm->tm_sec,ptm->tm_wday);
    DebugOut((int8_t*)pbuf,strlen((char*)pbuf));
    //显示当前闹钟
    for(i=0; i<BSP_RTC_ALARM_MAX_NUM; i++)
    {
        if(Rtc_Alarm[i]==0xFFFF)
        {
            sprintf((char*)pbuf,"Alarm%d: NC\r\n",i+1);
        }
        else
        {
            sprintf((char*)pbuf,"Alarm%d: %02d:%02d\r\n",i+1,Rtc_Alarm[i]>>8,Rtc_Alarm[i]&0x00FF);
        }
        DebugOut((int8_t*)pbuf,strlen((char*)pbuf));
    }
    MODULE_OS_DELAY_MS(100);
    //sprintf((char*)pbuf,"Set Time: YYMMDDhhmmss\r\n");
    //DebugOut((int8_t*)pbuf,strlen((char*)pbuf));
    DebugInClear();
    while(1)
    {
        //打印提示信息
        sprintf((char*)pbuf,"Set Time: YYMMDDhhmmss\r\n");
        DebugOut((int8_t*)pbuf,strlen((char*)pbuf));
        //输入14个字符(buf)(XXXX XX XX XX XX XX)
        for(i=0; i<14; i++)
        {
            DebugIn((int8_t*)&d,&len);
            if(len==1 && d==27)
            {
                goto goto_RtcDebugSetTimeTest;
            }
            else if(len==1 && d=='\r')
            {
                break;
            }
            else if(len==1&&d>='0'&&d<='9')
            {
                pbuf[i]=d-'0';
            }
            else
            {
                break;
            }
        }
        if(i==0 && d=='\r')break;
        if(i!=14)continue;
        //将字符信息付给tm
        ptm->tm_year  =  pbuf[0]*1000+pbuf[1]*100+pbuf[2]*10+pbuf[3];
        ptm->tm_mon   =  pbuf[4]*10+pbuf[5];
        ptm->tm_mday  =  pbuf[6]*10+pbuf[7];
        ptm->tm_hour  =  pbuf[8]*10+pbuf[9];
        ptm->tm_min   =  pbuf[10]*10+pbuf[11];
        ptm->tm_sec   =  pbuf[12]*10+pbuf[13];
        //验证数据有效性
        //---年
        if(ptm->tm_year<2000 || ptm->tm_year>2035)continue;
        //---月
        if(ptm->tm_mon<1&&ptm->tm_mon>12)  continue;
        //---日
        if (ptm->tm_mon == 1 || ptm->tm_mon == 3 || ptm->tm_mon == 5
            || ptm->tm_mon == 7 || ptm->tm_mon == 8 || ptm->tm_mon == 10
            || ptm->tm_mon == 12)
        {
            if(ptm->tm_mday<1 || ptm->tm_mday>31)continue;
        }
        else if(ptm->tm_mon == 4 || ptm->tm_mon == 6 || ptm->tm_mon == 9
                || ptm->tm_mon == 11)
        {
            if(ptm->tm_mday<1 || ptm->tm_mday>30)continue;
        }
        else if(ptm->tm_mon == 2)
        {
            if(ptm->tm_year%4==0)
            {
                if(ptm->tm_mday<1 || ptm->tm_mday>29)continue;
            }
            else
            {
                if(ptm->tm_mday<1 || ptm->tm_mday>28)continue;
            }
        }
        //---时
        if(ptm->tm_hour>23) continue;
        //---分
        if(ptm->tm_min>59)  continue;
        //---秒
        if(ptm->tm_sec>59)  continue;
        //
        ptm->tm_mon = ptm->tm_mon-1;
        //设置
        BspRtc_SetRealTime(NULL,ptm,NULL,NULL);
        //打印提示信息
        sprintf((char*)pbuf,"\r\nSet Time OK!\r\n");
        DebugOut((int8_t*)pbuf,strlen((char*)pbuf));
        //
        break;
    }
    DebugInClear();
    while(1)
    {
        //打印提示信息
        sprintf((char*)pbuf,"Set Alarm Time: hhmm\r\n");
        DebugOut((int8_t*)pbuf,strlen((char*)pbuf));
        //输入4个字符(buf)(XX XX)
        for(i=0; i<4; i++)
        {
            DebugIn((int8_t*)&d,&len);
            if(len==1 && d==27)
            {
                goto goto_RtcDebugSetTimeTest;
            }
            else if(len==1 && d=='\r')
            {
                break;
            }
            else if(len==1&&d>='0'&&d<='9')
            {
                pbuf[i]=d-'0';
            }
            else
            {
                break;
            }
        }
        if(i==0 && d=='\r')
        {
            sprintf((char*)pbuf,"Set Alarm Cancel!\r\n");
            DebugOut((int8_t*)pbuf,strlen((char*)pbuf));
            break;
        }
        if(i!=4)continue;
        //将字符信息付给tm
        ptm->tm_hour  =  pbuf[0]*10+pbuf[1];
        ptm->tm_min   =  pbuf[2]*10+pbuf[3];
        //验证数据有效性
        //---时
        if(ptm->tm_hour>23 || ptm->tm_min>59)
        {
            len = 0;
            pbuf[0]=0;
            BspRtc_SetAlarm(1,&len,pbuf);
            sprintf((char*)pbuf,"Set Alarm ERR!\r\n");
            DebugOut((int8_t*)pbuf,strlen((char*)pbuf));
        }
        else
        {
            //设置
            len=ptm->tm_hour;
            len<<=8;
            len+=ptm->tm_min;
            pbuf[0]=1;
            BspRtc_SetAlarm(1,&len,pbuf);
            sprintf((char*)pbuf,"Set Alarm OK!\r\n");
            DebugOut((int8_t*)pbuf,strlen((char*)pbuf));
        }
        //
        break;
    }
goto_RtcDebugSetTimeTest:
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
}
//---------------------END-------------------------------------------------------

