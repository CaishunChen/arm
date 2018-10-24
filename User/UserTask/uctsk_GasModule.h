/*
***********************************************************************************
*                    作    者: 徐松亮
*                    更新时间: 2015-06-03
***********************************************************************************
*/

#ifndef __GasModule_H
#define __GasModule_H
//-------------------加载库函数------------------------------
#include "includes.h"
//-------------------接口宏定义(硬件相关)--------------------
#define GASMODULE_UART_CH     2
#define GASMODULE_UART_BPS    9600
//-------------------接口宏定义(硬件无关)--------------------
//------------------ 数据类型 -------------------------------
typedef struct GASMODULE_S_INFO
{
   uint8_t  DateTime[6];         // 时间
   uint16_t RxNum;               // 接收计数
   uint8_t  ModuleState;         // 模块状态
   uint8_t  finish   :1;
   uint8_t  res2     :1;
   uint8_t  res3     :1;
   uint8_t  res4     :1;
   uint8_t  res5     :1;
   uint8_t  res6     :1;
   uint8_t  res7     :1;
   uint8_t  res8     :1;
   uint8_t  res[2];
   uint16_t Formaldehyde_ppb;    // 甲醛
   uint16_t Alcohol_ppb;         // 乙醇
   uint16_t eCO2_ppm;            // eCO2
   uint16_t TVOC_ppb;            // TVOC
   uint32_t SensitiveResistance; // 敏感电阻
}GASMODULE_S_INFO;
//-------------------接口变量--------------------------------
extern GASMODULE_S_INFO GasModule_s_Info;
//-------------------接口函数--------------------------------
void GasModule_RxIrqBufToRBuf0(uint8_t *pRxIqBuf,uint16_t len);
void App_GasModuleTaskCreate(void);
void GasModule_DebugTestOnOff(uint8_t OnOff);

#endif


