/**
  ******************************************************************************
  * @file    Bsp_Dht11.c
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
/* Includes ------------------------------------------------------------------*/
#include "Bsp_Dht11.h"
#include "uctsk_Debug.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
BSP_DHTXX_S_INFO BspDhtXX_s_Info;
static uint8_t BspDhtXX_DebugTest_Enable=0;
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
 * @brief   初始化函数
 * @note    初始化GPIO
 * @param   None
 * @return  None
 */
void BspDhtXX_Init(void)
{
    // GPIO初始化
#if     (defined(STM32F1))
    GPIO_InitTypeDef GPIO_InitStructure;
    BSP_DHTXX_RCC_ENABLE;
    GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_Out_OD;
    GPIO_InitStructure.GPIO_Pin     =  BSP_DHTXX_PIN;
    GPIO_Init(BSP_DHTXX_PORT, &GPIO_InitStructure);
#elif   (defined(STM32F4))
    GPIO_InitTypeDef GPIO_InitStructure;
    BSP_DHTXX_RCC_ENABLE;
    GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType   =  GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd    =  GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Pin     =  BSP_DHTXX_PIN;
    GPIO_Init(BSP_DHTXX_PORT, &GPIO_InitStructure);
#elif   (defined(NRF51)||defined(NRF52))
    
#endif
		nrf_gpio_cfg_output(BSP_DHTXX_PIN);
    BSP_DHTXX_DQ_H;
    // 变量初始化
    memset((char*)&BspDhtXX_s_Info,0,sizeof(BspDhtXX_s_Info));
    // 应用初始化
}
/*
*********************************************************************************************************
*   函 数 名: DHT11_ReadByte
*   功能说明: 向DHT11读取1字节数据
*   形    参: 无
*   返 回 值: 无
*********************************************************************************************************
*/
static uint8_t BspDhtXX_ReadByte(void)
{
    /*
        写数据时序, 见DHT11 page 16
    */
    uint8_t i,k;
    uint8_t read = 0;

    for (i = 0; i < 8; i++)
    {
        read <<= 1;
        /* 等待DQ电平变高 (超时100us) */
        for (k = 0; k < 10; k++)
        {
            if (BSP_DHTXX_DQ_R)
            {
                break;
            }
            Count_DelayUs(10);
        }
        if (k >= 10)
        {
            goto quit;      /* 超时无应答，失败 */
        }

        /* 等待DQ电平变低，统计DQ高电平时长 (超时100us) */
        for (k = 0; k < 10; k++)
        {
            if (!BSP_DHTXX_DQ_R)
            {
                break;
            }
            Count_DelayUs(10);
        }

        if (k > 3)      /* 高脉冲持续时间大于40us ，认为是1，否则是0 */
        {
            read++;
        }
    }

    return read;

quit:
    return 0xFF;
}
/*
*********************************************************************************************************
*   函 数 名: DHT11_ReadData
*   功能说明: 复位DHT11。 拉低DQ为低，持续最少480us，然后等待
*   形    参: 无
*   返 回 值: 0 失败； 1 表示成功
*********************************************************************************************************
*/
void BspDhtXX_ReadData(void)
{
    /*
        时序:
        1. MCU拉低QD持续时间大于 18ms, 然后释放QD = 1
    */
    uint8_t i;
    uint8_t k;
    uint8_t sum;
    static uint8_t si=0;
    /* 1. 主机发送起始信号, DQ拉低时间 ＞ 18ms */
    if(si==0)
    {
        si=1;
        BSP_DHTXX_DQ_L;
        Count_DelayUs(20000);
        //return;
    }
    si=0;
    BSP_DHTXX_DQ_H;

    Count_DelayUs(15);  /* 等待15us */

    /* 2. 等待DQ电平变低 ( 超时100us) */
    for (k = 0; k < 10; k++)
    {
        if (!BSP_DHTXX_DQ_R)
        {
            break;
        }
        Count_DelayUs(10);
    }
    if (k >= 10)
    {
        goto quit;      /* 超时无应答，失败 */
    }

    /* 3.等待DQ电平变高 (超时100us) */
		BSP_DHTXX_DQ_H;
    for (k = 0; k < 10; k++)
    {
        if (BSP_DHTXX_DQ_R)
        {
            break;
        }
        Count_DelayUs(10);
    }
    if (k >= 10)
    {
        goto quit;      /* 超时无应答，失败 */
    }

    /* 4.等待DQ电平变低 (超时100us) */
    for (k = 0; k < 10; k++)
    {
        if (!BSP_DHTXX_DQ_R)
        {
            break;
        }
        Count_DelayUs(10);
    }
    if (k >= 10)
    {
        goto quit;      /* 超时无应答，失败 */
    }

    /* 读40bit 数据 */
    for (i = 0; i < 5; i++)
    {
        BspDhtXX_s_Info.Buf[i] = BspDhtXX_ReadByte();
    }
    Count_DelayUs(100);

    /* 计算校验和 */
    sum = BspDhtXX_s_Info.Buf[0] + BspDhtXX_s_Info.Buf[1] + BspDhtXX_s_Info.Buf[2] + BspDhtXX_s_Info.Buf[3];
    if (sum == BspDhtXX_s_Info.Buf[4])
    {
        BspDhtXX_s_Info.Temp    =   BspDhtXX_s_Info.Buf[2];
        BspDhtXX_s_Info.Hum     =   BspDhtXX_s_Info.Buf[0];
        return;
    }
quit:
    si=0;
}

void BspDhtXX_100ms(void)
{
    static uint8_t si=0;
    char *pbuf;
    si++;
    if(si==250)
    {
        si=0;
    }
    if(si%5==0)
    {
        BspDhtXX_ReadData();
        if(BspDhtXX_DebugTest_Enable==1)
        {
            pbuf    =   MemManager_Get(E_MEM_MANAGER_TYPE_256B);
            sprintf(pbuf,"BspDhtXX Info:Buf-[%02X][%02X][%02X][%02X][%02X] Temp:%d Hum:%d PeriodS:%d\r\n"\
                    ,BspDhtXX_s_Info.Buf[0]\

                    ,BspDhtXX_s_Info.Buf[1]\

                    ,BspDhtXX_s_Info.Buf[2]\

                    ,BspDhtXX_s_Info.Buf[3]\

                    ,BspDhtXX_s_Info.Buf[4]\

                    ,BspDhtXX_s_Info.Temp\

                    ,BspDhtXX_s_Info.Hum\

                    ,BspDhtXX_s_Info.PeriodS\

                   );
            DebugOutStr((int8_t *)pbuf);
            MemManager_Free(E_MEM_MANAGER_TYPE_256B, pbuf);
        }
    }
}
void BspDhtXX_DebugTestOnOff(uint8_t OnOff)
{
    if(OnOff==ON)
    {
        BspDhtXX_DebugTest_Enable=1;
    }
    else
    {
        BspDhtXX_DebugTest_Enable=0;
    }
}

/******************* (C) COPYRIGHT 2011 XSLXHN *****END OF FILE****/

