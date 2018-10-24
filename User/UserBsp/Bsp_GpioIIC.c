/**
  ******************************************************************************
  * @file    Bsp_GpioIIC.c 
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
#include "Bsp_GpioIIC.h"
/*
*********************************************************************************************************
*   功能说明: 配置I2C总线的GPIO，采用模拟IO的方式实现
*********************************************************************************************************
*/
void BspGpioIIC_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    BSP_GPIO_IIC_RCC_ENABLE;
#if   (defined(STM32F1))
    GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_Out_OD;
#elif (defined(STM32F4))
    GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType   =  GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd    =  GPIO_PuPd_NOPULL;
#endif
    GPIO_InitStructure.GPIO_Pin     =  BSP_GPIO_IIC_SDA_PIN;
    GPIO_Init(BSP_GPIO_IIC_SDA_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin     =  BSP_GPIO_IIC_SCL_PIN;
    GPIO_Init(BSP_GPIO_IIC_SCL_PORT, &GPIO_InitStructure);
    // 给一个停止信号, 复位I2C总线上的所有设备到待机模式
    BspGpioIIC_Stop();
}

/*
*********************************************************************************************************
*   功能说明: I2C总线位延迟，最快400KHz
*********************************************************************************************************
*/
static void BspGpioIIC_Delay(void)
{
    uint8_t i;

    /*　
        CPU主频168MHz时，在内部Flash运行, MDK工程不优化。用台式示波器观测波形。
        循环次数为5时，SCL频率 = 1.78MHz (读耗时: 92ms, 读写正常，但是用示波器探头碰上就读写失败。时序接近临界)
        循环次数为10时，SCL频率 = 1.1MHz (读耗时: 138ms, 读速度: 118724B/s)
        循环次数为30时，SCL频率 = 440KHz， SCL高电平时间1.0us，SCL低电平时间1.2us

        上拉电阻选择2.2K欧时，SCL上升沿时间约0.5us，如果选4.7K欧，则上升沿约1us

        实际应用选择400KHz左右的速率即可
    */
    for (i = 0; i < 30; i++);
}

/*
*********************************************************************************************************
*   功能说明: CPU发起I2C总线启动信号
*********************************************************************************************************
*/
void BspGpioIIC_Start(void)
{
    BSP_GPIO_IIC_SDA_H;
    BSP_GPIO_IIC_SCL_H;
    BspGpioIIC_Delay();
    BSP_GPIO_IIC_SDA_L;
    BspGpioIIC_Delay();
    BSP_GPIO_IIC_SCL_L;
    BspGpioIIC_Delay();
}

/*
*********************************************************************************************************
*   功能说明: CPU发起I2C总线停止信号
*********************************************************************************************************
*/
void BspGpioIIC_Stop(void)
{
    /* 当SCL高电平时，SDA出现一个上跳沿表示I2C总线停止信号 */
    BSP_GPIO_IIC_SDA_L;
    BSP_GPIO_IIC_SCL_H;
    BspGpioIIC_Delay();
    BSP_GPIO_IIC_SDA_H;
    BspGpioIIC_Delay();
}

/*
*********************************************************************************************************
*   功能说明: CPU向I2C总线设备发送8bit数据
*********************************************************************************************************
*/
void BspGpioIIC_SendByte(uint8_t _ucByte)
{
    uint8_t i;
    // 先发送字节的高位bit7
    for (i = 0; i < 8; i++)
    {
        if (_ucByte & 0x80)
        {
            BSP_GPIO_IIC_SDA_H;
        }
        else
        {
            BSP_GPIO_IIC_SDA_L;
        }
        BspGpioIIC_Delay();
        BSP_GPIO_IIC_SCL_H;
        BspGpioIIC_Delay();
        BSP_GPIO_IIC_SCL_L;
        if (i == 7)
        {
            BSP_GPIO_IIC_SDA_H;
        }
        _ucByte <<= 1;  /* 左移一个bit */
        BspGpioIIC_Delay();
    }
}

/*
*********************************************************************************************************
*   功能说明: CPU从I2C总线设备读取8bit数据
*   返 回 值: 读到的数据
*********************************************************************************************************
*/
uint8_t BspGpioIIC_ReadByte(void)
{
    uint8_t i;
    uint8_t value;

    // 读到第1个bit为数据的bit7
    value = 0;
    for (i = 0; i < 8; i++)
    {
        value <<= 1;
        BSP_GPIO_IIC_SCL_H;
        BspGpioIIC_Delay();
        if (BSP_GPIO_IIC_SDA_R)
        {
            value++;
        }
        BSP_GPIO_IIC_SCL_L;
        BspGpioIIC_Delay();
    }
    return value;
}

/*
*********************************************************************************************************
*   功能说明: CPU产生一个时钟，并读取器件的ACK应答信号
*   返 回 值: 返回0表示正确应答，1表示无器件响应
*********************************************************************************************************
*/
uint8_t BspGpioIIC_WaitAck(void)
{
    uint8_t re;

    BSP_GPIO_IIC_SDA_H;
    BspGpioIIC_Delay();
    BSP_GPIO_IIC_SCL_H;
    BspGpioIIC_Delay();
    if (BSP_GPIO_IIC_SDA_R)
    {
        re = 1;
    }
    else
    {
        re = 0;
    }
    BSP_GPIO_IIC_SCL_L;
    BspGpioIIC_Delay();
    return re;
}

/*
*********************************************************************************************************
*   功能说明: CPU产生一个ACK信号
*********************************************************************************************************
*/
void BspGpioIIC_Ack(void)
{
    BSP_GPIO_IIC_SDA_L;
    BspGpioIIC_Delay();
    BSP_GPIO_IIC_SCL_H;
    BspGpioIIC_Delay();
    BSP_GPIO_IIC_SCL_L;
    BspGpioIIC_Delay();
    BSP_GPIO_IIC_SDA_H;
}

/*
*********************************************************************************************************
*   功能说明: CPU产生1个NACK信号
*********************************************************************************************************
*/
void BspGpioIIC_NAck(void)
{
    BSP_GPIO_IIC_SDA_H;
    BspGpioIIC_Delay();
    BSP_GPIO_IIC_SCL_H;
    BspGpioIIC_Delay();
    BSP_GPIO_IIC_SCL_L;
    BspGpioIIC_Delay();
}

/*
*********************************************************************************************************
*   函 数 名: i2c_CheckDevice
*   功能说明: 检测I2C总线设备，CPU向发送设备地址，然后读取设备应答来判断该设备是否存在
*   形    参:  _Address：设备的I2C总线地址
*   返 回 值: 返回值 0 表示正确， 返回1表示未探测到
*********************************************************************************************************
*/
uint8_t BspGpioIIC_CheckDevice(uint8_t _Address)
{
    uint8_t ucAck;

    if (BSP_GPIO_IIC_SDA_R && BSP_GPIO_IIC_SCL_R)
    {
        BspGpioIIC_Start();
        // 发送设备地址+读写控制bit（0 = w， 1 = r) bit7 先传
        BspGpioIIC_SendByte(_Address | BSP_GPIO_IIC_WR);
        ucAck = BspGpioIIC_WaitAck();
        BspGpioIIC_Stop();
        return ucAck;
    }
    return 1;
}
