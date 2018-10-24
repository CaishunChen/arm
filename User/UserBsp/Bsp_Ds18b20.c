/**
  ******************************************************************************
  * @file    Bsp_Ds18b20.c 
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
#include "Bsp_Ds18b20.h"
#include "uctsk_Debug.h"

static uint8_t BspDs18b20_DebugTest_Enable=0;
static uint8_t BspDs18b20_ErrNum=0;
/*
*********************************************************************************************************
*   函 数 名: 初始化
*********************************************************************************************************
*/
void BspDs18b20_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    BSP_DS18B20_RCC_ENABLE;
    BSP_DS18B20_1_H;
    // 配置DQ为开漏输出
#if   (defined(STM32F1))
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_OD;
#elif (defined(STM32F4))
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
#endif
    //
#ifdef BSP_DS18B20_1_PORT
    GPIO_InitStructure.GPIO_Pin = BSP_DS18B20_1_PIN;
    GPIO_Init(BSP_DS18B20_1_PORT, &GPIO_InitStructure);
#endif
#ifdef BSP_DS18B20_2_PORT
    GPIO_InitStructure.GPIO_Pin = BSP_DS18B20_2_PIN;
    GPIO_Init(BSP_DS18B20_2_PORT, &GPIO_InitStructure);
#endif
#ifdef BSP_DS18B20_3_PORT
    GPIO_InitStructure.GPIO_Pin = BSP_DS18B20_3_PIN;
    GPIO_Init(BSP_DS18B20_3_PORT, &GPIO_InitStructure);
#endif
#ifdef BSP_DS18B20_4_PORT
    GPIO_InitStructure.GPIO_Pin = BSP_DS18B20_4_PIN;
    GPIO_Init(BSP_DS18B20_4_PORT, &GPIO_InitStructure);
#endif
}

/*
*********************************************************************************************************
*   函 数 名: 复位DS18B20
*  形    参: 无
*   返 回 值: 0 失败； 1 表示成功
*  说    明: 拉低DQ为低，持续最少480us，然后等待
               复位时序, 见DS18B20 page 15
                 首先主机拉低DQ，持续最少 480us
                 然后释放DQ，等待DQ被上拉电阻拉高，约 15-60us
                 DS18B20 将驱动DQ为低 60-240us， 这个信号叫 presence pulse
                 (在位脉冲,表示DS18B20准备就绪 可以接受命令)
                 如果主机检测到这个低应答信号，表示DS18B20复位成功
*********************************************************************************************************
*/
uint8_t BspDs18b20_Reset(void)
{
    uint8_t i;
    uint16_t k;
    // 禁止全局中断
    DISABLE_INT();
    // 复位，如果失败则返回0
    for (i = 0; i < 1; i++)
    {
        BSP_DS18B20_1_L;
        // 延迟 520uS， 要求这个延迟大于 480us
        Count_SysTickDelayUs(520);
        BSP_DS18B20_1_H;
        // 等待15us
        Count_SysTickDelayUs(15);
        // 检测DQ电平是否为低
        for (k = 0; k < 10; k++)
        {
            if (BSP_DS18B20_IS_LOW)
            {
                break;
            }
            // 等待65us
            Count_SysTickDelayUs(10);
        }
        // 失败
        if (k >= 10)
        {
            continue;
        }

        // 等待DS18B20释放DQ
        for (k = 0; k < 30; k++)
        {
            if (!BSP_DS18B20_IS_LOW)
            {
                break;
            }
            Count_SysTickDelayUs(10);
        }
        // 失败
        if (k >= 30)
        {
            continue;
        }
        break;
    }
    // 使能全局中断
    ENABLE_INT();
    Count_SysTickDelayUs(5);
    //
    if (i >= 1)
    {
        return ERR;
    }
    //
    return OK;
}

/*
*********************************************************************************************************
*   函 数 名: 向DS18B20写入1字节数据
*  说    明: 写数据时序, 见DS18B20 page 16
*********************************************************************************************************
*/
static void BspDs18b20_WriteByte(uint8_t _val)
{
    uint8_t i;

    for (i = 0; i < 8; i++)
    {
        BSP_DS18B20_1_L;
        Count_SysTickDelayUs(2);

        if (_val & 0x01)
        {
            BSP_DS18B20_1_H;
        }
        else
        {
            BSP_DS18B20_1_L;
        }
        Count_SysTickDelayUs(60);
        BSP_DS18B20_1_H;
        Count_SysTickDelayUs(2);
        _val >>= 1;
    }
}

/*
*********************************************************************************************************
*   函 数 名: 向DS18B20读取1字节数据
*********************************************************************************************************
*/
static uint8_t BspDs18b20_ReadByte(void)
{
    uint8_t i;
    uint8_t read = 0;

    for (i = 0; i < 8; i++)
    {
        read >>= 1;

        BSP_DS18B20_1_L;
        Count_SysTickDelayUs(3);
        BSP_DS18B20_1_H;
        Count_SysTickDelayUs(3);

        if (BSP_DS18B20_IS_LOW)
        {
            ;
        }
        else
        {
            read |= 0x80;
        }
        Count_SysTickDelayUs(60);
    }

    return read;
}

/*
*********************************************************************************************************
*   函 数 名: 读温度寄存器的值（原始数据）
*   返 回 值: 温度寄存器数据 （除以16得到 1摄氏度单位, 也就是小数点前面的数字)
*********************************************************************************************************
*/
uint8_t BspDs18b20_ReadTempReg(int32_t *pvalue)
{
    uint8_t temp1, temp2;

    /* 总线复位 */
    if (BspDs18b20_Reset() == ERR)
    {
        BspDs18b20_ErrNum = 1;
        return ERR;
    }
    // 发命令
    BspDs18b20_WriteByte(0xcc);
    // 发转换命令
    BspDs18b20_WriteByte(0x44);
    // ---------------------------------------------
    MODULE_OS_DELAY_MS(100);
    // 总线复位
    if (BspDs18b20_Reset() == ERR)
    {
      BspDs18b20_ErrNum = 2;
      return ERR;
    }
    // 发命令
    BspDs18b20_WriteByte(0xcc);
    BspDs18b20_WriteByte(0xbe);
    // 读温度值低字节
    temp1 = BspDs18b20_ReadByte();
    // 读温度值高字节
    temp2 = BspDs18b20_ReadByte();
    // 返回16位寄存器值
    *pvalue = ((temp2 << 8) | temp1);
    // 转为温度值
    if((*pvalue & 0xF800)==0)
    {
        (*pvalue) *= 625;
    }
    else if((*pvalue & 0xF800)==0xF800)
    {
        (*pvalue) = ~(*pvalue) + 1;
        (*pvalue) *= 625;
    }
    else
    {
        BspDs18b20_ErrNum = 3;
        return ERR;
    }
    if((*pvalue)<(-550000L) || (*pvalue)>1250000L)
    {
        BspDs18b20_ErrNum = 4;
        return ERR;
    }

    //
    return OK;
}

/*
*********************************************************************************************************
*   函 数 名: DS18B20_ReadID
*   功能说明: 读DS18B20的ROM ID， 总线上必须只有1个芯片
*   形    参: _id 存储ID
*   返 回 值: 0 表示失败， 1表示检测到正确ID
*********************************************************************************************************
*/
uint8_t BspDs18b20_ReadID(uint8_t *_id)
{
    uint8_t i;

    /* 总线复位 */
    if (BspDs18b20_Reset() == ERR)
    {
        return 0;
    }

    BspDs18b20_WriteByte(0x33); /* 发命令 */
    for (i = 0; i < 8; i++)
    {
        _id[i] = BspDs18b20_ReadByte();
    }

    BspDs18b20_Reset();     /* 总线复位 */

    return 1;
}

/*
*********************************************************************************************************
*   函 数 名: DS18B20_ReadTempByID
*   功能说明: 读指定ID的温度寄存器的值（原始数据）
*   形    参: 无
*   返 回 值: 温度寄存器数据 （除以16得到 1摄氏度单位, 也就是小数点前面的数字)
*********************************************************************************************************
*/
int16_t BspDs18b20_ReadTempByID(uint8_t *_id)
{
    uint8_t temp1, temp2;
    uint8_t i;
    // 总线复位
    BspDs18b20_Reset();
    // 发命令
    BspDs18b20_WriteByte(0x55);
    for (i = 0; i < 8; i++)
    {
        BspDs18b20_WriteByte(_id[i]);
    }
    // 发转换命令
    BspDs18b20_WriteByte(0x44);
    // 总线复位
    BspDs18b20_Reset();
    // 发命令
    BspDs18b20_WriteByte(0x55);
    for (i = 0; i < 8; i++)
    {
        BspDs18b20_WriteByte(_id[i]);
    }
    BspDs18b20_WriteByte(0xbe);
    // 读温度值低字节
    temp1 = BspDs18b20_ReadByte();
    // 读温度值高字节
    temp2 = BspDs18b20_ReadByte();
    // 返回16位寄存器值
    return ((temp2 << 8) | temp1);
}
//
void BspDs18b20_DebugTest_100ms(void)
{
    uint8_t res;
    uint8_t *pbuf;
    int32_t value;
    static uint8_t s_count=0;
    //
    if(BspDs18b20_DebugTest_Enable==1)
    {
        s_count++;
        if(s_count<10)
        {
            return;
        }
        s_count=0;
        //申请缓存
        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        res=BspDs18b20_ReadTempReg(&value);
        if(res==OK)
        {
            //打印输出
            sprintf((char*)pbuf,"DebugOut: DS18B20-%ld\r\n",value);
            DebugOutStr((int8_t*)pbuf);
        }
        else
        {
            //打印输出
            sprintf((char*)pbuf,"DebugOut: DS18B20-ERR(%d)\r\n",BspDs18b20_ErrNum);
            DebugOutStr((int8_t*)pbuf);
        }
        BspDs18b20_ErrNum = 0;
        //释放缓存
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
}
void BspDs18b20_DebugTestOnOff(uint8_t OnOff)
{
    if(OnOff==ON)
    {
        BspDs18b20_DebugTest_Enable=1;
    }
    else
    {
        BspDs18b20_DebugTest_Enable=0;
    }
}
/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
