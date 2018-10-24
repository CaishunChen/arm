/*
***********************************************************************************
*                    作    者: 徐松亮
*                    更新时间: 2015-06-03
***********************************************************************************
*/
/*********************************************************************************
说明: 空气质量传感模块
**********************************************************************************/
//================================
#include "includes.h"
#include "uctsk_GasModule.h"
#include "uctsk_Debug.h"
#include "Bsp_Uart.h"
#include "Bsp_Rtc.h"
//================================
//------------------------------- 数据类型 ---------------------------------------
//------------------------------- 静态常量 ---------------------------------------
const uint8_t GasModule_TxCmd[]= {0x61,0x02,0x01,0x65};
//------------------------------- 静态变量 ---------------------------------------
MODULE_OS_TASK_TAB(App_TaskGasModuleTCB);
MODULE_OS_TASK_STK(App_TaskGasModuleStk,APP_TASK_GASMODULE_STK_SIZE);
//测试使能
static uint8_t GasModule_DebugTest_Enable=0;
GASMODULE_S_INFO GasModule_s_Info;
//------------------------------- 静态函数 ---------------------------------------
static void uctsk_GasModule(void *pvParameters);
//------------------------------- 引用变量 ---------------------------------------
extern struct tm CurrentDate;
//--------------------------------------------------------------------------------
/*******************************************************************************
函数功能: Bluetooth任务创建
*******************************************************************************/
void  App_GasModuleTaskCreate (void)
{
   MODULE_OS_TASK_CREATE("App Task GasModule",\
                          uctsk_GasModule,\
                          APP_TASK_GASMODULE_PRIO,\
                          App_TaskGasModuleStk,\
                          APP_TASK_GASMODULE_STK_SIZE,\
                          App_TaskGasModuleTCB,\
                          NULL);
}
/*******************************************************************************
函数功能: Sensor任务实体
*******************************************************************************/
static void uctsk_GasModule (void *pvParameters)
{
    //
    memset((char*)&GasModule_s_Info,0,sizeof(GASMODULE_S_INFO));
    //初始化硬件
    UART_INIT(GASMODULE_UART_CH,GASMODULE_UART_BPS);
    MODULE_OS_DELAY_MS(2000);
    for(;;)
    {
        UART_DMA_Tx(GASMODULE_UART_CH,(uint8_t *)GasModule_TxCmd,sizeof(GasModule_TxCmd));
        MODULE_OS_DELAY_MS(2000);
        if(GasModule_DebugTest_Enable==ON)
        {
           char *pbuf;
           pbuf   =  MemManager_Get(E_MEM_MANAGER_TYPE_256B);
           sprintf(pbuf,"%04d-%02d-%02d %02d:%02d:%02d\r\n",\
            GasModule_s_Info.DateTime[0]+2000,\
            GasModule_s_Info.DateTime[1],\
            GasModule_s_Info.DateTime[2],\
            GasModule_s_Info.DateTime[3],\
            GasModule_s_Info.DateTime[4],\
            GasModule_s_Info.DateTime[5]\
            );
           DebugOutStr((int8_t*)pbuf);
           sprintf(pbuf,"Rx Num:%d\r\n",GasModule_s_Info.RxNum);
           DebugOutStr((int8_t*)pbuf);
           sprintf(pbuf,"ModuleState:%d\r\n",GasModule_s_Info.ModuleState);
           DebugOutStr((int8_t*)pbuf);
           sprintf(pbuf,"SensitiveResistance:%d\r\n",GasModule_s_Info.SensitiveResistance);
           DebugOutStr((int8_t*)pbuf);
           sprintf(pbuf,"Formaldehyde_ppb:%d\r\n",GasModule_s_Info.Formaldehyde_ppb);
           DebugOutStr((int8_t*)pbuf);
           sprintf(pbuf,"Alcohol_ppb:%d\r\n",GasModule_s_Info.Alcohol_ppb);
           DebugOutStr((int8_t*)pbuf);
           sprintf(pbuf,"eCO2_ppm:%d\r\n",GasModule_s_Info.eCO2_ppm);
           DebugOutStr((int8_t*)pbuf);
           sprintf(pbuf,"TVOC_ppb:%d\r\n",GasModule_s_Info.TVOC_ppb);
           DebugOutStr((int8_t*)pbuf);
           MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
        }
    }
}
void GasModule_RxIrqBufToRBuf0(uint8_t *pRxIqBuf,uint16_t len)
{
    if(len!=19)
        return;
    if((pRxIqBuf[0]!=0x73) || (pRxIqBuf[18]!=0x65))
        return;
    GasModule_s_Info.RxNum++;
    GasModule_s_Info.finish               =  0;
    GasModule_s_Info.SensitiveResistance  =  Count_4ByteToLong(pRxIqBuf[1],pRxIqBuf[2],pRxIqBuf[3],pRxIqBuf[4]);
    GasModule_s_Info.ModuleState          =  pRxIqBuf[5];
    GasModule_s_Info.Formaldehyde_ppb     =  10*Count_2ByteToWord(pRxIqBuf[6],pRxIqBuf[7]);
    GasModule_s_Info.Alcohol_ppb          =  10*Count_2ByteToWord(pRxIqBuf[8],pRxIqBuf[9]);
    GasModule_s_Info.eCO2_ppm             =  Count_2ByteToWord(pRxIqBuf[10],pRxIqBuf[11]);
    GasModule_s_Info.TVOC_ppb             =  Count_2ByteToWord(pRxIqBuf[12],pRxIqBuf[13]);
    BspRtc_ReadRealTime(NULL,NULL,NULL,GasModule_s_Info.DateTime);
    GasModule_s_Info.finish               =  1;
}
void GasModule_DebugTestOnOff(uint8_t OnOff)
{
    if(OnOff==ON)
    {
        GasModule_DebugTest_Enable=1;
    }
    else
    {
        GasModule_DebugTest_Enable=0;
    }
}
