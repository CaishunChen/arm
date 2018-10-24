/**
  ******************************************************************************
  * @file    Bsp_SoilTempHumi.c 
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
//-------------------加载库函数------------------------------
#include "includes.h"
#include "Bsp_SoilTempHumi.h"
#include "Bsp_Uart.h"
#include "uctsk_Debug.h"

static uint8_t BspSoilTempHumi_DebugTest_Enable=0;
static int16_t Temp=0;
static uint16_t Humi=0;
static uint8_t CommErrConut=0xff;
static uint8_t CommState=0;
/*
*********************************************************************************************************
*   功能说明:
*   形    参:
*   返 回 值:
*********************************************************************************************************
*/

void BspSoilTempHumi_Init(void)
{
    //初始化GPIO
#ifdef BSP_SOILTEMPHUMI_PWR_PORT
    GPIO_InitTypeDef GPIO_InitStructure;
    BSP_SOILTEMPHUMI_PWR_ENABLE;
#if   (defined(STM32F1))
    GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_Out_PP;
#elif (defined(STM32F4))
    GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType   =  GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd    =  GPIO_PuPd_NOPULL;
#endif
    GPIO_InitStructure.GPIO_Pin     =  BSP_SOILTEMPHUMI_PWR_PIN;
    GPIO_Init(BSP_SOILTEMPHUMI_PWR_PORT, &GPIO_InitStructure);
    BSP_SOILTEMPHUMI_PWR_ON;
#endif
    //硬件初始化
    UART_INIT(BSP_SOIL_TEMP_HUMIDITY_UARTX,9600);
}

/*
*********************************************************************************************************
*   功能说明:
*   形    参:
*   返 回 值:
*********************************************************************************************************
*/
void BspSoilTempHumi_InterruptRx(uint8_t *pbuf,uint16_t len)
{
    CommState=2;
    //校验
    //...
    //
    Humi = Count_2ByteToWord(pbuf[3],pbuf[4]);
    Temp = Count_2ByteToWord(pbuf[5],pbuf[6]);
    if(BspSoilTempHumi_DebugTest_Enable==1)
    {
        DebugOutHex("SoilTempHumi:",pbuf,len);
    }
}
/*
*********************************************************************************************************
*   功能说明:
*   形    参:
*   返 回 值:
*********************************************************************************************************
*/
void BspSoilTempHumi_100ms(void)
{
    static uint8_t scnt=0;
    static uint8_t const cmd[8]= {0xFE,0x03,0x00,0x00,0x00,0x02,0xD0,0x04};
    uint8_t *pbuf;
    if(scnt<10)
    {
        scnt++;
        return;
    }
    scnt=0;
    //
    if(CommState==2)
    {
        CommErrConut=0;
    }
    else
    {
        if(CommErrConut!=0xFF)
        {
            CommErrConut++;
        }
    }
    //
    if(BspSoilTempHumi_DebugTest_Enable==1)
    {
        pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        sprintf((char*)pbuf,"SoilTempHumi: temp=%d,humi=%d,ErrCount=%d\r\n",Temp,Humi,CommErrConut);
        DebugOutStr((int8_t*)pbuf);
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
    //
    UART_DMA_Tx(BSP_SOIL_TEMP_HUMIDITY_UARTX,(uint8_t*)cmd,8);
    CommState=1;
}
/*
*********************************************************************************************************
*   功能说明:
*   形    参:
*   返 回 值:
*********************************************************************************************************
*/
uint8_t BspSoilTempHumi_Read(int16_t *pTemp,uint16_t *pHumi)
{
    if(CommErrConut>=10)
    {
        return ERR;
    }
    if(pTemp!=NULL)
    {
        *pTemp=Temp;
    }
    if(pHumi!=NULL)
    {
        *pHumi=Humi;
    }
    return OK;
}
/*
*********************************************************************************************************
*   功能说明:
*   形    参:
*   返 回 值:
*********************************************************************************************************
*/
void BspSoilTempHumi_DebugTestOnOff(uint8_t OnOff)
{
    if(OnOff==ON)
    {
        BspSoilTempHumi_DebugTest_Enable=1;
    }
    else
    {
        BspSoilTempHumi_DebugTest_Enable=0;
    }
}
//-----------------------------------------------------------

