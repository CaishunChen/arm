/*
***********************************************************************************
*                    作    者: 徐松亮
*                    更新时间: 2015-06-03
***********************************************************************************
*/
//------------------------------- Includes -----------------------------------
#include "Module_OS.h"


//------------------------------- 函数声明 -----------------------------------
#if (defined(STM32F1)||defined(STM32F4)||defined(NRF51)||defined(NRF52))
#include "uctsk_Debug.h"
#include "Bsp_Rtc.h"
#define MODULE_OS_DEBUG_OUT_STR(str) DebugOutStr(str)
#else
#define MODULE_OS_DEBUG_OUT_STR(str)
#endif
//------------------------------- 函数 ---------------------------------------
/*******************************************************************************
* 函数功能: 在Debug中打印出单个任务的信息
*******************************************************************************/

#if     (defined(OS_UCOSIII))
static void Module_OS_TaskInfo(void *pOsTcb);
static void Module_OS_TaskInfo(void *pOsTcb)
{

    int8_t *pbuf;
    pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
#if     (defined(OS_UCOSIII))
    sprintf((char *)pbuf, "%s:\r\n",
            ((OS_TCB *)pOsTcb)->NamePtr);
    MODULE_OS_DEBUG_OUT_STR(pbuf);
    sprintf((char *)pbuf, "  (.Prio:%02d)(.Stk:%04d/%04d)(.CtxSwCtr:%d)\r\n",
            ((OS_TCB *)pOsTcb)->Prio,
            ((OS_TCB *)pOsTcb)->StkUsed,((OS_TCB *)pOsTcb)->StkUsed+((OS_TCB *)pOsTcb)->StkFree,
            ((OS_TCB *)pOsTcb)->CtxSwCtr);
    MODULE_OS_DEBUG_OUT_STR(pbuf);
#elif   (defined(OS_FREERTOS))
    extern TaskHandle_t AppTaskStartTCB;
    //unsigned portBASE_TYPE uxHighWaterMark;
    //uxHighWaterMark=uxTaskGetStackHighWaterMark( AppTaskStartTCB );
    sprintf((char *)pbuf, "%s:%05ld\r\n",pcTaskGetTaskName(AppTaskStartTCB),uxTaskGetStackHighWaterMark(AppTaskStartTCB));
    MODULE_OS_DEBUG_OUT_STR(pbuf);

    sprintf((char *)pbuf, "%s:\r\n",
            pcTaskGetTaskName(pOsTcb));
    MODULE_OS_DEBUG_OUT_STR(pbuf);
    sprintf((char *)pbuf, "  (.Prio:%02d)(.Stk:%04d)\r\n",
            uxTaskPriorityGet(pOsTcb),
            uxTaskGetStackHighWaterMark(pOsTcb));
    MODULE_OS_DEBUG_OUT_STR(pbuf);
#endif
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
}
#endif

#if (defined(OS_FREERTOS))
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
// 当创建任务，信号量，消息队列时，freertos通过pvPortMalloc()动态申请动态内存失败时调用
void vApplicationMallocFailedHook(void)
{

}
// 任务堆栈溢出回调函数
void vApplicationStackOverflowHook(TaskHandle_t xTask,signed char *pcTaskName)
{
    int8_t *pbuf;
    pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    sprintf((char *)pbuf, "Task stack overflow: %s\r\n",pcTaskName);
    MODULE_OS_DEBUG_OUT_STR(pbuf);
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    while(1);
}
void HardFault_Handler(void)
{
    int8_t *pbuf;
	TaskHandle_t task_handle;
    pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
	task_handle = xTaskGetCurrentTaskHandle();
    sprintf((char *)pbuf, "HardFault_Handler(%s)\r\n",pcTaskGetTaskName(task_handle));
    MODULE_OS_DEBUG_OUT_STR(pbuf);
    {
    	MODULE_MEMORY_S_INFO *ps_info;
		ps_info=(MODULE_MEMORY_S_INFO*)pbuf;
		ps_info->ErrId      =   MODULE_E_ERR_HARDFAULT;
        ps_info->Para       =   uxTaskPriorityGet(task_handle);;
        Module_Memory_App(MODULE_MEMORY_APP_CMD_INFO_W,(uint8_t*)ps_info,NULL);
    }
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    while (1);
}

// -----Test-----
void StackOverflowTest(void)
{
    int16_t i16;
    uint8_t buf[2048];
    buf[0]=buf[0];
    // 对于M3与M4内核的MCU，堆栈生长方向是向下生长的满栈，即高地址是buf[2047],低地址是buf[0]，
    // 否则很可能在回调函数之前就发生硬件错误，所以建议从后向前赋值,
    for(i16=(2048-1); i16>=0; i16--)
    {
        buf[i16]=0x55;
        // 回调函数在任务切换时产生，所以在此加延时函数
        MODULE_OS_DELAY_MS(1);
    }
}
// --------------
#endif
/*******************************************************************************
* 函数功能: 在Debug中打印出操作系统信息
*******************************************************************************/
//int8_t *pbuf;
//uint32_t *pi32;
void Module_OS_Info_DebugTestOnOff(uint8_t OnOff)
{
    int8_t *pbuf;
    static uint8_t s_count=0;
#if (defined(OS_FREERTOS))
    uint32_t *pi32;
    TaskStatus_t *ps;
	const char task_state[]={'r','R','B','S','D'}; 
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
            MODULE_OS_DEBUG_OUT_STR("-----OS_XX_Qty\r\n");
            // 获取任务总数量
            sprintf((char *)pbuf, "OSTaskQty = %3u\r\n", uxTaskGetNumberOfTasks());
            MODULE_OS_DEBUG_OUT_STR(pbuf);
#endif
            MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
        }
        else if(s_count<10)
        {
            s_count++;
            return;
        }
        s_count=1;
#if (defined(OS_UCOSIII))
        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
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
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
#elif (defined(OS_FREERTOS))
#if		(MEM_1KB_BLK_NBR!=0)
		pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_1KB);
#else
        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
#endif
        pi32=(uint32_t*)&pbuf[80];
        ps  =(TaskStatus_t*)&pbuf[88];
        pi32[0]=xPortGetFreeHeapSize();
        pi32[1]=xPortGetMinimumEverFreeHeapSize();
        sprintf((char*)pbuf,"Heap: Total-%05ld MinFree-%05ld CurrentFree-%05ld\r\n",configTOTAL_HEAP_SIZE,pi32[1],pi32[0]);
        MODULE_OS_DEBUG_OUT_STR(pbuf);
        pi32[0]=BspRtc_GetBeginToNowS();
        sprintf((char*)pbuf,"Task(%05ld)           State   ID   Pro    Stack     CPU\r\n",pi32[0]);
        MODULE_OS_DEBUG_OUT_STR(pbuf);
        /*
        ptest = pbuf1 = pbuf;
        vTaskList((char*)pbuf1);
        MODULE_OS_DEBUG_OUT_STR(pbuf1);
        */
        /*
        sprintf((char*)pbuf,"Task     Count    UsageRate\r\n");
        MODULE_OS_DEBUG_OUT_STR(pbuf);
        ptest = pbuf1 = pbuf;
        vTaskGetRunTimeStats((char*)pbuf1);
        MODULE_OS_DEBUG_OUT_STR(pbuf1);
        */
        pbuf[70] = uxTaskGetNumberOfTasks();
		//
		//
        pbuf[70] = uxTaskGetSystemState(ps, pbuf[70], &pi32[0]);
        if(pi32[0]>0)
        {
            for(pbuf[71]=0; pbuf[71]<pbuf[70]; pbuf[71]++)
            {
                pi32[1]=(uint64_t)(ps[ pbuf[71] ].ulRunTimeCounter)*100 / pi32[0];
                if( pi32[1] > 0UL )
                {

                    sprintf((char*)pbuf,"%-24s%-6c%-6d%-8d%-8d%d%%\r\n",ps[ pbuf[71]].pcTaskName,task_state[ps[ pbuf[71] ].eCurrentState],
                            ps[ pbuf[71] ].xTaskNumber,ps[ pbuf[71]].uxCurrentPriority,
                            ps[ pbuf[71] ].usStackHighWaterMark,pi32[1]);
                }
                else
                {
                    /* 任务运行时间不足总运行时间的1%*/
                    sprintf((char*)pbuf,"%-24s%-6c%-6d%-8d%-8dt<1%%\r\n",ps[pbuf[71] ].pcTaskName,task_state[ps[ pbuf[71] ].eCurrentState],
                            ps[ pbuf[71] ].xTaskNumber,ps[ pbuf[71]].uxCurrentPriority,
                            ps[ pbuf[71] ].usStackHighWaterMark);
                }
                MODULE_OS_DEBUG_OUT_STR(pbuf);
				MODULE_OS_DELAY_MS(1);
            }
        }
		/*
        MODULE_OS_DEBUG_OUT_STR("-----OS_Task\r\n");
        //Module_OS_TaskInfo(&AppTaskStartTCB);
        {
            extern TaskHandle_t AppTaskStartTCB;
            //unsigned portBASE_TYPE uxHighWaterMark;
            //uxHighWaterMark=uxTaskGetStackHighWaterMark( AppTaskStartTCB );
            sprintf((char *)pbuf, "%s:%05ld\r\n",pcTaskGetTaskName(AppTaskStartTCB),uxTaskGetStackHighWaterMark(AppTaskStartTCB));
            MODULE_OS_DEBUG_OUT_STR(pbuf);
        }
		*/
#if		(MEM_1KB_BLK_NBR!=0)
		MemManager_Free(E_MEM_MANAGER_TYPE_1KB,pbuf);
#else
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
#endif
#endif
    }
    else
    {
        s_count=0;
    }
}
//----------------------------------------------------------------------------
