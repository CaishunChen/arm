﻿/*
***********************************************************************************
*                    作    者: 徐松亮
*                    更新时间: 2015-06-03
***********************************************************************************
*/
//------------------------------- Includes -----------------------------------
#include "Module_OS.h"

#if (defined(STM32F1)||defined(STM32F4)||defined(NRF51)||defined(NRF52))
#include "uctsk_Debug.h"
#include "Bsp_Rtc.h"
#define MODULE_OS_DEBUG_OUT_STR(str) DebugOutStr(str)
#else
#define MODULE_OS_DEBUG_OUT_STR(str)
#endif
//------------------------------- 函数声明 -----------------------------------
#if   (defined(OS_UCOSII))
#elif (defined(OS_UCOSIII))
static void Module_OS_TaskInfo(OS_TCB *pOsTcb);
#endif
//------------------------------- 函数 ---------------------------------------
/*******************************************************************************
* 函数功能: 在Debug中打印出单个任务的信息
*******************************************************************************/
#if   (defined(OS_UCOSIII))
static void Module_OS_TaskInfo(OS_TCB *pOsTcb)
{
    int8_t *pbuf;
    pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    sprintf((char *)pbuf, "%s:\r\n",
            pOsTcb->NamePtr);
    MODULE_OS_DEBUG_OUT_STR(pbuf);
    sprintf((char *)pbuf, "  (.Prio:%02d)(.Stk:%04d/%04d)(.CtxSwCtr:%d)\r\n",
            pOsTcb->Prio,
            pOsTcb->StkUsed,pOsTcb->StkUsed+pOsTcb->StkFree,
            pOsTcb->CtxSwCtr);
    MODULE_OS_DEBUG_OUT_STR(pbuf);
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
}
#elif (defined(OS_FREERTOS))
#include "Bsp_Pwr.h"
// IDLE任务回调函数
void vApplicationIdleHook(void)
{
#ifndef    XKAP_ICARE_B_M 
    Bsp_Pwr_EnterStandby();
#endif
}
// TICK任务回调函数
void vApplicationTickHook(void)
{
}
#endif
/*******************************************************************************
* 函数功能: 在Debug中打印出操作系统信息
*******************************************************************************/
int8_t *ptest;
void Module_OS_Info_DebugTestOnOff(uint8_t OnOff)
{
    int8_t *pbuf;
    static uint8_t s_count=0;
#if (defined(OS_FREERTOS))
    int8_t *pbuf1;
    uint32_t i32;
#endif
    if(OnOff==ON)
    {
        if(s_count==0)
        {
            //首次进入
            pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
            MODULE_OS_DEBUG_OUT_STR("MemInfo:  Num Max Size\r\n");
            sprintf((char *)pbuf, "Mem-256B = %d   %d   %d\r\n", MemManager_Mem_256B_Num,MemManager_Mem_256B_MaxNum,MEM_256B_BLK_NBR);
            MODULE_OS_DEBUG_OUT_STR(pbuf);
            sprintf((char *)pbuf, "Mem-1KB  = %d   %d   %d\r\n", MemManager_Mem_1KB_Num,MemManager_Mem_1KB_MaxNum,MEM_1KB_BLK_NBR);
            MODULE_OS_DEBUG_OUT_STR(pbuf);
            sprintf((char *)pbuf, "Mem-2KB  = %d   %d   %d\r\n", MemManager_Mem_2KB_Basic_Num,MemManager_Mem_2KB_Basic_MaxNum,MEM_2KB_BASIC_BLK_NBR);
            MODULE_OS_DEBUG_OUT_STR(pbuf);
            sprintf((char *)pbuf, "Mem-5KB  = %d   %d   %d\r\n", MemManager_Mem_5KB_Basic_Num,MemManager_Mem_5KB_Basic_MaxNum,MEM_5KB_BASIC_BLK_NBR);
            MODULE_OS_DEBUG_OUT_STR(pbuf);
            sprintf((char *)pbuf, "Mem-10KB = %d   %d   %d\r\n", MemManager_Mem_10KB_Num,MemManager_Mem_10KB_MaxNum,MEM_10KB_BLK_NBR);
            MODULE_OS_DEBUG_OUT_STR(pbuf);
#if   (defined(OS_UCOSII))
#elif (defined(OS_UCOSIII))

            MODULE_OS_DEBUG_OUT_STR("-----OS_XX_Qty\r\n");
            sprintf((char *)pbuf, "OSTaskQty = %3u\r\n", OSTaskQty);
            MODULE_OS_DEBUG_OUT_STR(pbuf);
            sprintf((char *)pbuf, "OSFlagQty = %3u\r\n", OSFlagQty);
            MODULE_OS_DEBUG_OUT_STR(pbuf);
            sprintf((char *)pbuf, "OSMemQty  = %3u\r\n", OSMemQty);
            MODULE_OS_DEBUG_OUT_STR(pbuf);
            sprintf((char *)pbuf, "OSMutexQty= %3u\r\n", OSMutexQty);
            MODULE_OS_DEBUG_OUT_STR(pbuf);
            sprintf((char *)pbuf, "OSSemQty  = %3u\r\n", OSSemQty);
            MODULE_OS_DEBUG_OUT_STR(pbuf);
            sprintf((char *)pbuf, "OSTmrQty  = %3u\r\n", OSTmrQty);
            MODULE_OS_DEBUG_OUT_STR(pbuf);

#elif (defined(OS_FREERTOS)&&(configGENERATE_RUN_TIME_STATS==1))

#endif
            MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
        }
        else if(s_count<10)
        {
            s_count++;
            return;
        }
        s_count=1;
        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
#if   (defined(OS_UCOSII))
        MODULE_OS_DEBUG_OUT_STR("-----OS_Stat\r\n");
        sprintf((char *)pbuf, "CPU Usage = %3u%%\r\n", OSCPUUsage);
        MODULE_OS_DEBUG_OUT_STR(pbuf);
        //sprintf((char *)pbuf, "#Tasks    = %3u\r\n", OSTaskCtr);
        //MODULE_OS_DEBUG_OUT_STR(pbuf);
#elif (defined(OS_UCOSIII))
        MODULE_OS_DEBUG_OUT_STR("-----OS_Stat\r\n");
        sprintf((char *)pbuf, "CPU Usage = %d.%02d%%\r\n", OSStatTaskCPUUsage/100,OSStatTaskCPUUsage%100);
        MODULE_OS_DEBUG_OUT_STR(pbuf);

        MODULE_OS_DEBUG_OUT_STR("-----OS_Task\r\n");
        {
            extern OS_TCB  AppTaskStartTCB;
            extern OS_TCB  OSIdleTaskTCB;
            extern OS_TCB  OSTickTaskTCB;
            extern OS_TCB  OSStatTaskTCB;
            extern OS_TCB  OSTmrTaskTCB;
            extern OS_TCB  AppTaskDebugTCB;
#if   (defined(PROJECT_BASE_STM32F1)||defined(PROJECT_BASE_STM32F4))
            //
            Module_OS_TaskInfo(&AppTaskStartTCB);
            Module_OS_TaskInfo(&OSIdleTaskTCB);
            Module_OS_TaskInfo(&OSTickTaskTCB);
            Module_OS_TaskInfo(&OSStatTaskTCB);
            Module_OS_TaskInfo(&OSTmrTaskTCB);
            //
            Module_OS_TaskInfo(&AppTaskDebugTCB);
#elif (defined(PROJECT_ARMFLY_V5_XSL))
            //
            Module_OS_TaskInfo(&AppTaskStartTCB);
            Module_OS_TaskInfo(&OSIdleTaskTCB);
            Module_OS_TaskInfo(&OSTickTaskTCB);
            Module_OS_TaskInfo(&OSStatTaskTCB);
            Module_OS_TaskInfo(&OSTmrTaskTCB);
            //
            Module_OS_TaskInfo(&AppTaskDebugTCB);
#elif (defined(PROJECT_XKAP_V3)||defined(XKAP_ICARE_B_D_M))
            //
            extern OS_TCB  App_TaskADTCB;
            extern OS_TCB  App_TaskSensorTCB;
            extern OS_TCB  App_TaskHciTCB;
            //extern OS_TCB  App_TaskBluetoothTCB;
            extern OS_TCB  App_TaskUserTimer10msTCB;
            extern OS_TCB  App_TaskUserTimer100msTCB;
            extern OS_TCB  AppTaskRfmsTCB;
            extern OS_TCB  AppTaskRfmsTxTCB;
            extern OS_TCB  AppTaskRfmsTestTCB;
            extern OS_TCB  App_TaskGasModuleTCB;

#if   (HARDWARE_VER!=7)
            extern OS_TCB  App_TaskGsmTestTCB;
            extern OS_TCB  App_TaskGsmSendTCB;
            extern OS_TCB  App_TaskGsmParseTCB;
            extern OS_TCB  App_GprsAppXkapTaskTCB;
#else
            extern OS_TCB  App_TaskUartIcamTCB;
#endif
            Module_OS_TaskInfo(&AppTaskStartTCB);
            Module_OS_TaskInfo(&OSIdleTaskTCB);
            Module_OS_TaskInfo(&OSTickTaskTCB);
            Module_OS_TaskInfo(&OSStatTaskTCB);
            Module_OS_TaskInfo(&OSTmrTaskTCB);
            //
            Module_OS_TaskInfo(&AppTaskRfmsTxTCB);
            Module_OS_TaskInfo(&AppTaskDebugTCB);
            Module_OS_TaskInfo(&App_TaskADTCB);
            Module_OS_TaskInfo(&App_TaskSensorTCB);
            Module_OS_TaskInfo(&App_TaskHciTCB);
            //Module_OS_TaskInfo(&App_TaskBluetoothTCB);
#if   (HARDWARE_VER!=7)
            Module_OS_TaskInfo(&App_TaskGsmTestTCB);
            Module_OS_TaskInfo(&App_TaskGsmSendTCB);
            Module_OS_TaskInfo(&App_TaskGsmParseTCB);
            Module_OS_TaskInfo(&App_GprsAppXkapTaskTCB);
#else
            Module_OS_TaskInfo(&App_TaskUartIcamTCB);
#endif
            Module_OS_TaskInfo(&App_TaskUserTimer10msTCB);
            Module_OS_TaskInfo(&App_TaskUserTimer100msTCB);
            Module_OS_TaskInfo(&AppTaskRfmsTCB);
            Module_OS_TaskInfo(&AppTaskRfmsTestTCB);
            Module_OS_TaskInfo(&App_TaskGasModuleTCB);
#elif (defined(PROJECT_TCI_V30))
#endif
        }
#elif (defined(OS_FREERTOS))
        i32=BspRtc_GetBeginToNowS();
        sprintf((char*)pbuf,"Task(%05ld)           State  Pro    Stack  Num\r\n",i32);
        MODULE_OS_DEBUG_OUT_STR(pbuf);
        ptest = pbuf1 = pbuf;
        vTaskList((char*)pbuf1);
        MODULE_OS_DEBUG_OUT_STR(pbuf1);
        /*
        sprintf((char*)pbuf,"Task     Count    UsageRate\r\n");
        MODULE_OS_DEBUG_OUT_STR(pbuf);
        ptest = pbuf1 = pbuf;
        vTaskGetRunTimeStats((char*)pbuf1);
        MODULE_OS_DEBUG_OUT_STR(pbuf1);
        */
#endif
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
    else
    {
        s_count=0;
    }
}
//----------------------------------------------------------------------------