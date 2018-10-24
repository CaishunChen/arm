/*
***********************************************************************************
*                    作    者: 徐松亮
*                    更新时间: 2015-06-03
***********************************************************************************
*/
#ifndef __UCTSK_RFMS_H
#define __UCTSK_RFMS_H
//-------------------加载库函数------------------------------
#include "includes.h"
#include "Module_Memory.h"
//-------------------数据类型--------------------------------
enum UCTSK_RFMS_E_RUNSTATE
{
    UCTSK_RFMS_E_RUNSTATE_IDLE=0,
    UCTSK_RFMS_E_RUNSTATE_START,
    UCTSK_RFMS_E_RUNSTATE_START_DELAY,
    UCTSK_RFMS_E_RUNSTATE_GET,
    UCTSK_RFMS_E_RUNSTATE_STOP,
};
enum UCTSK_RFMS_E_ERR
{
    UCTSK_RFMS_E_ERR_RES1           =  0xFA,
    UCTSK_RFMS_E_ERR_RES2           =  0xFB,  
    UCTSK_RFMS_E_ERR_MOVE_TOO_MANY  =  0xFC,
    UCTSK_RFMS_E_ERR_SIGNAL_HIGN    =  0xFD,
    UCTSK_RFMS_E_ERR_SIGNAL_LOW     =  0xFE,
    UCTSK_RFMS_E_ERR_SIGNAL_NO      =  0xFF,
};
#define ARITHMETIC_BODY_MOVE_90S_PER_3600S_NUM  40
#define ARITHMETIC_BODY_MOVE_LEVEL_WAKE         202
#define ARITHMETIC_BODY_MOVE_LEVEL_LIGHT        155
#define ARITHMETIC_BODY_MOVE_LEVEL_MIDDLE       120
// 室内活动监测
typedef struct UCTSK_S_MOVE_POWER
{
    uint8_t Num;
    //
    uint8_t Time[6];
    //
    uint32_t Data[6];
} UCTSK_S_MOVE_POWER;
//-------------------接口宏定义(配置相关)--------------------
#define UCTSK_RFMS_ENABLE_KEEP_BODY_MOVE
#define UCTSK_RFMS_KEEP_BODY_MOVE_TIME   (-12)
//-------------------接口宏定义(硬件相关)--------------------
#define UCTSK_RFMS_UARTX               1
#define UCTSK_RFMS_UART_BPS            (19200)
/*
//时钟
#if   (defined(STM32F1))
#define UCTSK_RFMS_POWER_RCC_ENABLE    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA , ENABLE);
#elif (defined(STM32F4))
#define UCTSK_RFMS_POWER_RCC_ENABLE    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA , ENABLE);
#endif
//端口
#define UCTSK_RFMS_POWER_PORT          GPIOA
#define UCTSK_RFMS_POWER_PIN           GPIO_Pin_6
//电源开启
#define UCTSK_RFMS_POWER_PIN_ON        GPIO_SetBits(UCTSK_RFMS_POWER_PORT , UCTSK_RFMS_POWER_PIN)
//电源关闭
#define UCTSK_RFMS_POWER_PIN_OFF       GPIO_ResetBits(UCTSK_RFMS_POWER_PORT , UCTSK_RFMS_POWER_PIN)
*/
//开启
#define UCTSK_RFMS_ON         uctsk_Rfms_Tx((uint8_t*)UCTSK_RFMS_POWER_ON,sizeof(UCTSK_RFMS_POWER_ON));
//关闭
#define UCTSK_RFMS_OFF        uctsk_Rfms_Tx((uint8_t*)UCTSK_RFMS_POWER_OFF,sizeof(UCTSK_RFMS_POWER_OFF));
//-------------------接口宏定义(硬件无关)--------------------
//用于数据去直流
#define UCTSK_RFMS_DATA_DC    0x1FF
//评分算法
//睡眠结构
#define WEIGHTSLEEPSTRCT               56    /*52*/
#define STDSLEEPSTRUCTURE              0.6f  /*0.57f*/ /*0.65f*/
//
#define WEIGHTSLEEPSTRCT_HEAD          41
#define STDSLEEPSTRUCTURE_HEAD         0.6f  
#define WEIGHTSLEEPSTRCT_TAIL          15
#define STDSLEEPSTRUCTURE_TAIL         0.4f  
//入睡时长
#define STDINTOPERIOD                  9
#define WEIGHTSLEEPINTOPERIOD          12    /*13*/
//体动次数相关得分
#define STDBODYMOVEMENTCOUNT_HI        12    /*16*/
#define WEIGHTBODYMOVEMENTCOUNT        8
#define STDBODYMOVEMENTCOUNT_LOW       8     /*10*/
//唤醒状态相关得分
#define WEIGHTAWAKESTATE               3
//睡眠习惯
#define STDSARTTIMEDIS                 30    /*10*/    /*15*/
#define WEIGHTSTARTTIMEDIS             10    /*12*/
//睡眠环境-温度
#define STDSLEEPTEMP_HI                24
#define WEIGHTSLEEPTEMP                7
#define STDSLEEPTEMP_LOW               20
//睡眠环境-湿度
#define STDSLEEPHUM_HI                 65
#define WEIGHTSLEEPHUM                 3
#define STDSLEEPHUM_LOW                45
//总睡时长系数
#define STDTOTALSLEEPPERIOD_HI         480
#define STDTOTALSLEEPPERIOD_LOW        390
//
#define AGERATIO                        (1)
//-------------------接口变量--------------------------------
extern uint32_t uctsk_Rfms_TxNum;
extern uint32_t uctsk_Rfms_RxNum;
extern uint32_t uctsk_Rfms_MonitorRxNum;
extern uint16_t uctsk_Rfms_RxValue;
extern uint8_t uctsk_CurrentSleepLevel;
extern uint8_t  uctsk_Rfms_Err;
extern uint16_t Uctsk_Rfms_RxZeroCmtInfo;
//---当前缓存是否为ExtFlash中的数据
extern uint8_t uctsk_Rfms_ExtFlashReadSign;
extern MODULE_MEMORY_S_DAY_INFO         uctsk_Rfms_s_DayInfo;
extern MODULE_MEMORY_S_DAY_SLEEPLEVEL   uctsk_Rfms_s_DaySleepLevel;
extern MODULE_MEMORY_S_DAY_BREATH       uctsk_Rfms_s_DayBreath;
extern MODULE_MEMORY_S_DAY_HEARTRATE    uctsk_Rfms_s_HeartRate;
extern MODULE_MEMORY_S_DAY_BODYMOVE     uctsk_Rfms_s_BodyMove;
//
extern uint8_t Uctsk_Rfms_RunFileData_DebugTest_Enable;
extern uint32_t UctskRfms_TestfileDataNum;
extern uint32_t UctskRfms_TestfileDataAllByteNum;
extern uint8_t uctsk_Rfms_PowerSavingOff;
extern uint16_t Uctsk_Rfms_PowerSavingTimer_s;
extern uint16_t UctskRfms_BufferLoopAllErrCmt;
extern uint16_t UctskRfms_BufferLoopMaxErrCmt;
//
extern uint8_t uctsk_Rfms_Hardware,uctsk_Rfms_Software;
//
extern uint32_t   uctsk_Rfms_NoBodyThresholdValue;
extern uint32_t   uctsk_Rfms_NoBodyThresholdVTimerS;
//-------------------接口函数--------------------------------
//创建任务
extern void App_RfmsTaskCreate(void);
extern void App_RfmsTaskTestCreate(void);
extern void App_RfmsTaskTxCreate(void);
//获取设置运行状态
extern uint8_t uctsk_Rfms_GetRunState(void);
extern void uctsk_Rfms_Power(uint8_t OnOff);
extern void uctsk_Rfms_SetRunState(uint8_t state);
//数据中断
extern void uctsk_Rfms_RxIrq(uint8_t *pbuf,uint16_t len);
//时间中断
extern void uctsk_Rfms_1mS(void);
//设置传感器开关
extern void uctsk_Rfms_PowerOnOff(uint8_t OnOff);
extern void uctsk_Rfms_SetOnOff(uint8_t OnOff,uint8_t ManualOrAuto);
//调试
extern void uctsk_Rfms_DataStream_DebugTestOnOff(uint8_t OnOff);
extern void uctsk_Rfms_WaveStream_DebugTestOnOff(uint8_t OnOff);
extern void uctsk_Rfms_FinalData_DebugTestOnOff(uint8_t OnOff);
extern void uctsk_Rfms_RunFileData_DebugTestOnOff(uint8_t OnOff);
//数据转换
extern void uctsk_Rfms_Interpolation_Proc(uint8_t* source, uint8_t* parameter,uint8_t* destination, uint8_t length ,uint8_t min,float coeff, uint8_t endMark);
extern void uctsk_Rfms_Average_Proc(uint8_t* valueArray, uint8_t average,uint16_t* coordirateArray,uint16_t centerCoordinate,uint16_t limit ,uint8_t length,uint8_t endMark, float step);
extern void uctsk_Rfms_DataSaveToTf_100ms(void);
extern void uctsk_RfmsSetSleepLevelBeforeAlarm(void);
extern uint8_t uctsk_Rfms_BufferLoop1Pop(uint8_t *pbuf,uint16_t *plen);
//体动
extern uint8_t uctsk_Rfms_KeepBodyMoveIndex(struct tm *p_tm,uint8_t *p_buf,uint8_t *index);
extern uint8_t uctsk_Rfms_KeepBodyMove_GetFinalInfo(struct tm *p_tm,uint8_t *pOutbuf);
//-----------------------------------------------------------
#endif

