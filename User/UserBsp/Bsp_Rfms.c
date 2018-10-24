/**
  ******************************************************************************
  * @file    Bsp_Rfms.c 
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
//------------------------------- Includes --------------------
#include "Bsp_Rfms.h"
#include "uctsk_AD.h"
#include "MemManager.h"
#include "uctsk_Debug.h"
#include "Bsp_CpuFlash.h"
#include "Bsp_Uart.h"
//-------------------------------------------------------------Private define
//------------------------------- 用户变量 --------------------
//均值累加
uint32_t Rfms_BufAveSum=0;
//满足条件的差值次数累加
uint16_t Rfms_SubCount=0;
//差分阈值/连续2次采样满足一定差值的最小值/(EEPROM)
uint16_t Rfms_SubThreshold=0;
//------------------------------- 静态变量 --------------------
//缓存环实体
static uint16_t Rfms_Buf[RFMS_BUF_MAX]= {0};
//测试使能
static uint8_t Rfms_DebugTestWave_Enable=0;
static uint8_t Rfms_DebugTest_Enable=0;
//器件状态 0-无效，1-有效
//static uint8_t Rfms_DeviceState=0;
//------------------------------- 用户函数声明 ----------------
//------------------------------- 照度驱动 ---------------------------------------
uint16_t Rfms_GetSubThreshold(void)
{
   return Rfms_SubThreshold;
}
void Rfms_SetSubThreshold(uint16_t value)
{
   Rfms_SubThreshold=value;
}
//-------------------------------------------------------------------------------
// 函数功能: RFMS自检
//-------------------------------------------------------------------------------
/*
static uint8_t Rfms_SelfCheck(void)
{
    uint8_t i;
    for(i=0; i<10; i++)
    {
        Module_OS_DelayMs(500);
        //查看AD采集值是否在一定范围内，如果在，则说明已经接入RFMS传感器
        if(RFMS_REAL_VALUE>1000 && RFMS_REAL_VALUE<3000)
        {
            return OK;
        }
    }
    return ERR;
}
*/
//-------------------------------------------------------------------------------
// 函数功能: RFMS初始化
//-------------------------------------------------------------------------------
uint8_t Rfms_Init(void)
{
    //初始化端口
    UART_INIT(RFMS_UARTX,RFMS_UART_BPS);
    //初始化参数
    {
        MODULE_MEMORY_S_PARA *pPara;
        uint8_t res;
        pPara=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        Bsp_CpuFlash_Read(BSP_CPU_FLASH_PARA_ADDR,(uint8_t*)pPara,sizeof(MODULE_MEMORY_S_PARA));
        Rfms_SubThreshold=pPara->Rfms_SubThreshold;
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pPara);
    }
    return OK;
    //器件自检
    /*
    if(OK==Rfms_SelfCheck())
    {
        Rfms_DeviceState=1;
        return OK;
    }
    else
    {
        //防警告
        Rfms_DeviceState=Rfms_DeviceState;
        return ERR;
    }
    */
}
//-------------------------------------------------------------------------------
// 函数功能:   求波形面积(Rfms_BufAveSum)与斜率(转变算法为差值)
// 输    出:   Rfms_BufAveSum ---   一定时间内的均值累加
//             Rfms_SubCount  ---   一定时间内的满足条件的差值次数累加
//-------------------------------------------------------------------------------
uint16_t Rfms_Timer1=0;
uint16_t Rfms_SubCounter=0;
void Rfms_Count_20ms(void)
{
    uint8_t res;
    int8_t *pbuf;
    uint16_t adValue=RFMS_REAL_VALUE;
    uint16_t i16;
    uint32_t bufSum;
    static uint16_t s_adValueBak=0;

    static uint32_t s_BufAveSum=0;

    static uint8_t s_SubSign=0;
    //求面积
    //--计算缓存环求和(bufSum)
    Count_BufLoopSum(Rfms_Buf,RFMS_BUF_MAX,adValue,&bufSum);
    //--计算缓存环的平均值(i16)
    i16=bufSum/RFMS_BUF_MAX;
    //--计算缓存环均值累加和(积分面积)
    s_BufAveSum+=abs(i16-1500);
    //满足一定斜率计数
    //--转变算法为差值: a/t与常数b做比对，与a直接与常数bt做比对等同
    i16=abs(adValue-s_adValueBak);
    if(i16>Rfms_SubThreshold && s_SubSign==0)
    {
        Rfms_SubCounter++;
        s_SubSign=1;
    }
    else if(i16<=Rfms_SubThreshold)
    {
        s_SubSign=0;
    }
    s_adValueBak=adValue;
    // 1分钟更新全局变量
    if(Rfms_Timer1>=(50*60))
    {
        Rfms_Timer1=0;
        //赋值一分钟的均值积分面积
        Rfms_BufAveSum=s_BufAveSum;
        s_BufAveSum=0;
        //赋值一分钟的斜率计数
        Rfms_SubCount =Rfms_SubCounter;
        Rfms_SubCounter=0;
    }
    else
    {
        Rfms_Timer1++;
    }
    //调试数据输出(1S输出)
    if(Rfms_DebugTest_Enable==1)
    {
        if(Rfms_Timer1%50==0)
        {
            //申请缓存
            pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
            //
            sprintf((char *)pbuf,"Rfms_BufAveSum-%d Rfms_SubCount-%d Rfms_Timer1-%d Rfms_SubCount-%d\r\n",Rfms_BufAveSum,Rfms_SubCount,Rfms_Timer1,Rfms_SubCounter);
            DebugOutStr(pbuf);
            //释放缓存
            MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
        }
    }
    //调试波形输出
    if(Rfms_DebugTestWave_Enable==1)
    {
        //申请缓存
        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        //
        sprintf((char *)pbuf,"%d\r\n",adValue);
        DebugOutStr(pbuf);
        //释放缓存
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
}
void Rfms_DebugTestWaveOnOff(uint8_t OnOff)
{
    if(OnOff==ON)
    {
        Rfms_DebugTestWave_Enable=1;
    }
    else
    {
        Rfms_DebugTestWave_Enable=0;
    }
}
void Rfms_DebugTestOnOff(uint8_t OnOff)
{
    if(OnOff==ON)
    {
        Rfms_DebugTest_Enable=1;
    }
    else
    {
        Rfms_DebugTest_Enable=0;
    }
}
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
