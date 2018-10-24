/**
  ******************************************************************************
  * @file    Bsp_CpuId.c 
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
//------------------------------- Includes -----------------------------------
#include "Bsp_CpuId.h"
#include "uctsk_Debug.h"

#if   (defined(NRF51)||defined(NRF52))
#include "nrf_temp.h"
#endif
//----------------------------------------------------------------------------
/*******************************************************************************
函数功能: 读取芯片唯一ID
*******************************************************************************/
void BspCpuId_Get(uint8_t* pbuf,uint16_t* pflashSize)
{
    if(pbuf!=NULL)
    {
#if   (defined(STM32F1)||defined(STM32F4))
        pbuf[0] =  *(vu8*)(BSP_CPUID_ADDR+0);
        pbuf[1] =  *(vu8*)(BSP_CPUID_ADDR+1);
        pbuf[2] =  *(vu8*)(BSP_CPUID_ADDR+2);
        pbuf[3] =  *(vu8*)(BSP_CPUID_ADDR+3);
        pbuf[4] =  *(vu8*)(BSP_CPUID_ADDR+4);
        pbuf[5] =  *(vu8*)(BSP_CPUID_ADDR+5);
        pbuf[6] =  *(vu8*)(BSP_CPUID_ADDR+6);
        pbuf[7] =  *(vu8*)(BSP_CPUID_ADDR+7);
        pbuf[8] =  *(vu8*)(BSP_CPUID_ADDR+8);
        pbuf[9] =  *(vu8*)(BSP_CPUID_ADDR+9);
        pbuf[10]=  *(vu8*)(BSP_CPUID_ADDR+10);
        pbuf[11]=  *(vu8*)(BSP_CPUID_ADDR+11);
#elif (defined(NRF51)||defined(NRF52))
        pbuf[0] =  BSP_CPUID_ADDR[0]>>0;
        pbuf[1] =  BSP_CPUID_ADDR[0]>>8;
        pbuf[2] =  BSP_CPUID_ADDR[0]>>16;
        pbuf[3] =  BSP_CPUID_ADDR[0]>>24;
        pbuf[4] =  BSP_CPUID_ADDR[1]>>0;
        pbuf[5] =  BSP_CPUID_ADDR[1]>>8;
        pbuf[6] =  BSP_CPUID_ADDR[1]>>16;
        pbuf[7] =  BSP_CPUID_ADDR[1]>>24;
        pbuf[8] =  0;
        pbuf[9] =  0;
        pbuf[10]=  0;
        pbuf[11]=  0;
#endif
    }
    if(pflashSize!=NULL)
    {
#if   (defined(STM32F1)||defined(STM32F4))
        *pflashSize=*(vu16*)(BSP_CPUID_FLASHSIZE_ADDR);
#elif (defined(NRF51))
        *pflashSize=BSP_CPUID_FLASHSIZE_ADDR;
#elif (defined(NRF52))
        *pflashSize=BSP_CPUID_FLASHSIZE_ADDR;
#endif
    }
}
void Bsp_CpuId_DebugTestOnOff(uint8_t OnOff)
{
    uint8_t *pbuf;
    uint16_t i16;
    OnOff=OnOff;
    //申请缓存
    pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    //
    BspCpuId_Get(pbuf,&i16);
    DebugOutHex("CpuID:",pbuf,12);
    sprintf((char*)pbuf,"Cpu FlashSize:%d kB\r\n",i16);
    DebugOutStr((int8_t*)pbuf);
    //释放缓存
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
}
//-----------------------------------------------------------
/*
函数功能: CPU温度采集
*/
uint8_t Bsp_CpuId_ReadTemp(int16_t *pTemp)
{
#if   (defined(STM32F1)||defined(STM32F4))
    return OK;
#elif (defined(NRF51)||defined(NRF52))
    int32_t volatile temp;
    NRF_TEMP->TASKS_START = 1;
    while (NRF_TEMP->EVENTS_DATARDY == 0)
    {
        ;
    }
    NRF_TEMP->EVENTS_DATARDY = 0;
    temp = (nrf_temp_read() / 4);
    NRF_TEMP->TASKS_STOP = 1;
    *pTemp = (int16_t)temp*10;
    return OK;
#endif
}

void Bsp_CpuId_ReadTemp_DebugTestOnOff(uint8_t OnOff)
{
    uint8_t *pbuf;
    int16_t i16;
    static uint8_t s_count=0;
    if(OnOff==ON)
    {
        s_count++;
        if(s_count<10)
        {
            return;
        }
        s_count=0;
        //申请缓存
        pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        //
        Bsp_CpuId_ReadTemp(&i16);
        sprintf((char*)pbuf,"Cpu Temp:%d \r\n",i16/10);
        DebugOutStr((int8_t*)pbuf);
        //释放缓存
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
    else if(OnOff==OFF)
    {
        s_count = 0;
    }
}
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
