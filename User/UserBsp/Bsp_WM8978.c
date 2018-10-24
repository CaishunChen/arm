/**
  ******************************************************************************
  * @file    Bsp_WM8978.c 
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
#include "Bsp_WM8978.h"
#include "Bsp_GpioIIC.h"
#include "Bsp_FsmcSram.h"
#include "Bsp_Key.h"
#include "uctsk_Debug.h"
/*
***********************************************************************************
*
*   重要提示:
*   1、wm8978_ 开头的函数是操作WM8978寄存器，操作WM8978寄存器是通过I2C模拟总线进行的
*   2、I2S_ 开头的函数是操作STM32  I2S相关寄存器
*   3、实现录音或放音应用，需要同时操作WM8978和STM32的I2S。
*   4、部分函数用到的形参的定义在ST固件库中，比如：I2S_Standard_Phillips、I2S_Standard_MSB、I2S_Standard_LSB
*             I2S_MCLKOutput_Enable、I2S_MCLKOutput_Disable
*             I2S_AudioFreq_8K、I2S_AudioFreq_16K、I2S_AudioFreq_22K、I2S_AudioFreq_44K、I2S_AudioFreq_48
*             I2S_Mode_MasterTx、I2S_Mode_MasterRx
*   5、注释中 pdf 指的是 wm8978.pdf 数据手册，wm8978de寄存器很多，用到的寄存器会注释pdf文件的页码，便于查询
*
***********************************************************************************
*/
//------------------------------- 用户变量 --------------------
/*
    wm8978寄存器缓存
    由于WM8978的I2C两线接口不支持读取操作，因此寄存器值缓存在内存中，当写寄存器时同步更新缓存，读寄存器时
    直接返回缓存中的值。
    寄存器MAP 在WM8978.pdf 的第67页，寄存器地址是7bit， 寄存器数据是9bit
*/
static uint16_t wm8978_RegCash[] =
{
    0x000, 0x000, 0x000, 0x000, 0x050, 0x000, 0x140, 0x000,
    0x000, 0x000, 0x000, 0x0FF, 0x0FF, 0x000, 0x100, 0x0FF,
    0x0FF, 0x000, 0x12C, 0x02C, 0x02C, 0x02C, 0x02C, 0x000,
    0x032, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
    0x038, 0x00B, 0x032, 0x000, 0x008, 0x00C, 0x093, 0x0E9,
    0x000, 0x000, 0x000, 0x000, 0x003, 0x010, 0x010, 0x100,
    0x100, 0x002, 0x001, 0x001, 0x039, 0x039, 0x039, 0x039,
    0x001, 0x001
};
enum
{
    STA_IDLE = 0,       // 待机状态
    STA_RECORDING,      // 录音状态
    STA_PLAYING,        // 放音状态
    STA_STOP_I2S,       // 临时状态,通知I2S中断服务程序准备停止
};
typedef struct
{
    uint8_t ucFmtIdx;   // 音频格式: 标准，位长，采样频率
    uint8_t ucVolume;   // 当前放音音量
    uint8_t ucMicGain;  // 当前MIC增益
    int16_t *pAudio;    // 音频数据指针
    uint32_t uiCursor;  // 播放位置
    uint8_t ucStatus;   // 录音机状态(0-待机 1-录音 2-放音)
} REC_T;
//录音机状态结构体
REC_T g_tRec;
static uint8_t  BspWm897_ItCmd=0;
static int16_t *pBspWm8978_pbuf1=NULL,*pBspWm8978_pbuf2=NULL;
//------------------------ 用户函数声明 -------------------------
static uint16_t BspWm8978_ReadReg(uint8_t _ucRegAddr);
static uint8_t BspWm8978_WriteReg(uint8_t _ucRegAddr, uint16_t _usValue);
static void BspWm8978_I2S_ModeConfig(uint16_t _usStandard, uint16_t _usWordLen, uint32_t _uiAudioFreq, uint16_t _usMode);
//---------------------------------------------------------------------------------------------------- 输入函数
typedef struct
{
    // RIFF 资源交换文件格式
    int8_t     Riff_Id[4];          // "RIFF"
    uint32_t   Riff_Size;           // 文件总字节数-8
    int8_t     Riff_Format[4];      // "WAVE"
    // Format
    int8_t     Fmt_Id[4];           // "fmt "
    uint32_t   Fmt_Size;            // 16/18 18则最后又附加信息
    uint16_t   Fmt_Tag;             // 编码方式 一般为0x0001
    uint16_t   Fmt_Channels;        // 声道数目 1-单声道 2-双声道
    uint32_t   Fmt_SamplesPerSec;   // 采样频率
    // Fact
    // Data
}WAV_S_HEAD;
/*************************************************************************
*   函 数 名: wm8978_ReadReg
*   功能说明: 从cash中读回读回wm8978寄存器
*   形    参:  _ucRegAddr ： 寄存器地址
*   返 回 值: 无
*************************************************************************/
static uint16_t BspWm8978_ReadReg(uint8_t _ucRegAddr)
{
    return wm8978_RegCash[_ucRegAddr];
}
/*************************************************************************
*   函 数 名: wm8978_WriteReg
*   功能说明: 写wm8978寄存器
*   形    参:  _ucRegAddr ： 寄存器地址
*             _usValue ：寄存器值
*   返 回 值: 无
*************************************************************************/
static uint8_t BspWm8978_WriteReg(uint8_t _ucRegAddr, uint16_t _usValue)
{
    uint8_t ucAck;
    BspGpioIIC_Start();
    BspGpioIIC_SendByte(WM8978_SLAVE_ADDRESS | BSP_GPIO_IIC_WR);
    ucAck = BspGpioIIC_WaitAck();
    if (ucAck == 1)
    {
        return 0;
    }
    // 发送控制字节1
    BspGpioIIC_SendByte(((_ucRegAddr << 1) & 0xFE) | ((_usValue >> 8) & 0x1));
    ucAck = BspGpioIIC_WaitAck();
    if (ucAck == 1)
    {
        return 0;
    }
    // 发送控制字节2
    BspGpioIIC_SendByte(_usValue & 0xFF);
    ucAck = BspGpioIIC_WaitAck();
    if (ucAck == 1)
    {
        return 0;
    }
    // 发送STOP
    BspGpioIIC_Stop();
    wm8978_RegCash[_ucRegAddr] = _usValue;
    return 1;
}
/*************************************************************************
*  功能说明:   配置STM32的I2S外设工作模式
*  形    参:   _usStandard :  接口标准
*                             I2S_Standard_Phillips
*                             I2S_Standard_MSB
*                             I2S_Standard_LSB
*              _usMCLKOutput: 主时钟输出
*                             I2S_MCLKOutput_Enable
*                             I2S_MCLKOutput_Disable
*              _usAudioFreq : 采样频率，
*                             I2S_AudioFreq_8K
*                             I2S_AudioFreq_16K
*                             I2S_AudioFreq_22K
*                             I2S_AudioFreq_44K
                              I2S_AudioFreq_48
*              _usMode :      CPU I2S的工作模式
*                             I2S_Mode_MasterTx
*                             I2S_Mode_MasterRx
*                             I2S_Mode_SlaveTx
*                             I2S_Mode_SlaveRx
*  说    明:   安富莱开发板硬件不支持 I2S_Mode_SlaveTx、I2S_Mode_SlaveRx 模式，
*              这需要WM8978连接外部振荡器
*  返 回 值:   无
*************************************************************************/
static void BspWm8978_I2S_ModeConfig(uint16_t _usStandard, uint16_t _usWordLen, uint32_t _uiAudioFreq, uint16_t _usMode)
{
    I2S_InitTypeDef I2S_InitStructure;
    if ((_usMode == I2S_Mode_SlaveTx) && (_usMode == I2S_Mode_SlaveRx))
    {
        return;
    }
    /*
    For I2S mode, make sure that either:
        - I2S PLL is configured using the functions RCC_I2SCLKConfig(RCC_I2S2CLKSource_PLLI2S),
        RCC_PLLI2SCmd(ENABLE) and RCC_GetFlagStatus(RCC_FLAG_PLLI2SRDY).
    */
    {
        uint32_t n = 0;
        FlagStatus status = RESET;

        RCC_I2SCLKConfig(RCC_I2S2CLKSource_PLLI2S);
        RCC_PLLI2SCmd(ENABLE);

        for (n = 0; n < 500; n++)
        {
            status = RCC_GetFlagStatus(RCC_FLAG_PLLI2SRDY);
            if (status == 1)
            {
                break;
            }
        }
    }
    // 打开 I2S2 APB1 时钟
    BSP_WM8978_IIS_RCC_ENABLE;
    // 复位 SPI2 外设到缺省状态
    SPI_I2S_DeInit(BSP_WM8978_IIS_X);
    // I2S2 外设配置
    //if (_usMode == I2S_Mode_MasterTx)
    {
        I2S_StructInit(&I2S_InitStructure);
        I2S_InitStructure.I2S_Mode = I2S_Mode_MasterTx;  // 配置I2S工作模式
        I2S_InitStructure.I2S_Standard = _usStandard;    // 接口标准
        I2S_InitStructure.I2S_DataFormat = _usWordLen;   // 数据格式，16bit
        I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Enable; // 主时钟模式
        I2S_InitStructure.I2S_AudioFreq = _uiAudioFreq;  // 音频采样频率
        I2S_InitStructure.I2S_CPOL = I2S_CPOL_Low;
        I2S_Init(BSP_WM8978_IIS_X, &I2S_InitStructure);
        // Configure the I2Sx_ext (the second instance) in Slave Receiver Mode
        I2S_FullDuplexConfig(BSP_WM8978_IIS_EXT_X, &I2S_InitStructure);
        // 使能 SPI2/I2S2 外设
        I2S_Cmd(BSP_WM8978_IIS_X, ENABLE);
        // Enable the I2Sx_ext peripheral for Full Duplex mode
        I2S_Cmd(BSP_WM8978_IIS_EXT_X, ENABLE);
    }
    // 禁止I2S2 TXE中断(发送缓冲区空)，需要时再打开
    SPI_I2S_ITConfig(BSP_WM8978_IIS_X, SPI_I2S_IT_TXE, DISABLE);
    // 禁止I2S2 RXNE中断(接收不空)，需要时再打开
    SPI_I2S_ITConfig(BSP_WM8978_IIS_EXT_X, SPI_I2S_IT_RXNE, DISABLE);
    // 这一段代码用于测试I2S双向传输功能
#if 0
    while (1)
    {
        static uint16_t usData;

        if (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == SET)
        {
            SPI_I2S_SendData(SPI2, 0x55);
        }

        if (SPI_I2S_GetFlagStatus(I2S2ext, SPI_I2S_FLAG_RXNE) == SET)
        {
            usData = SPI_I2S_ReceiveData(I2S2ext);

            printf("%02X ", usData);
        }
    }
#endif
}
//---------------------------------------------------------------------------------------------------- 静态函数
/*************************************************************************
*   功能说明: 复位wm8978，所有的寄存器值恢复到缺省值
*************************************************************************/
static void BspWm8978_Reset(void)
{
    /* wm8978寄存器缺省值 */
    const uint16_t reg_default[] =
    {
        0x000, 0x000, 0x000, 0x000, 0x050, 0x000, 0x140, 0x000,
        0x000, 0x000, 0x000, 0x0FF, 0x0FF, 0x000, 0x100, 0x0FF,
        0x0FF, 0x000, 0x12C, 0x02C, 0x02C, 0x02C, 0x02C, 0x000,
        0x032, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
        0x038, 0x00B, 0x032, 0x000, 0x008, 0x00C, 0x093, 0x0E9,
        0x000, 0x000, 0x000, 0x000, 0x003, 0x010, 0x010, 0x100,
        0x100, 0x002, 0x001, 0x001, 0x039, 0x039, 0x039, 0x039,
        0x001, 0x001
    };
    uint8_t i;

    BspWm8978_WriteReg(0x00, 0);
    for (i = 0; i < sizeof(reg_default) / 2; i++)
    {
        wm8978_RegCash[i] = reg_default[i];
    }
}
/*************************************************************************
*   功能说明: 配置I2C GPIO，并检查I2C总线上的WM8978是否正常
*   返 回 值: OK / ERR
*************************************************************************/
uint8_t BspWm8978_Init(void)
{
    uint8_t res;
    //-----IIC初始化
    if (BspGpioIIC_CheckDevice(WM8978_SLAVE_ADDRESS) == 0)
    {
        res = OK;
    }
    else
    {
        res = ERR;
        return res;
    }
    BspWm8978_Reset();
    //-----IIS初始化
    // 配置I2S中断通道
    {
        NVIC_InitTypeDef NVIC_InitStructure;
        NVIC_InitStructure.NVIC_IRQChannel = SPI2_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
    }
    // 配置I2S2 GPIO口线
    {
        GPIO_InitTypeDef GPIO_InitStructure;
        BSP_WM8978_IIS_RCC_ENABLE;
        BSP_WM8978_IIS_GPIO_RCC_ENABLE;
#if   (defined(STM32F1))
        GPIO_InitStructure.GPIO_Speed  =  GPIO_Speed_2MHz;
        GPIO_InitStructure.GPIO_Mode   =  GPIO_Mode_AF_PP;
#elif (defined(STM32F4))
        GPIO_InitStructure.GPIO_Speed  =  GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode   =  GPIO_Mode_AF;
        GPIO_InitStructure.GPIO_OType  =  GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd   =  GPIO_PuPd_NOPULL;
#endif
        GPIO_InitStructure.GPIO_Pin     =  BSP_WM8978_IIS_LRC_PIN;
        GPIO_Init(BSP_WM8978_IIS_LRC_PORT, &GPIO_InitStructure);
        GPIO_InitStructure.GPIO_Pin     =  BSP_WM8978_IIS_BLCK_PIN;
        GPIO_Init(BSP_WM8978_IIS_BLCK_PORT, &GPIO_InitStructure);
        GPIO_InitStructure.GPIO_Pin     =  BSP_WM8978_IIS_MLCK_PIN;
        GPIO_Init(BSP_WM8978_IIS_MLCK_PORT, &GPIO_InitStructure);
        GPIO_InitStructure.GPIO_Pin     =  BSP_WM8978_IIS_ADCDAT_PIN;
        GPIO_Init(BSP_WM8978_IIS_ADCDAT_PORT, &GPIO_InitStructure);
        GPIO_InitStructure.GPIO_Pin     =  BSP_WM8978_IIS_DACDAT_PIN;
        GPIO_Init(BSP_WM8978_IIS_DACDAT_PORT, &GPIO_InitStructure);
#if   (defined(STM32F1))
#elif (defined(STM32F4))
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF_SPI2);
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2);
        GPIO_PinAFConfig(GPIOI, GPIO_PinSource2, GPIO_AF_SPI3);
        GPIO_PinAFConfig(GPIOC, GPIO_PinSource3, GPIO_AF_SPI2);
        GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_SPI2);
#endif
    }
    // 禁止I2S2 TXE中断(发送缓冲区空)，需要时再打开
    SPI_I2S_ITConfig(BSP_WM8978_IIS_X, SPI_I2S_IT_TXE, DISABLE);
    // 禁止I2S2 RXNE中断(接收不空)，需要时再打开
    SPI_I2S_ITConfig(BSP_WM8978_IIS_EXT_X, SPI_I2S_IT_RXNE, DISABLE);
    //-----
    return res;
}
//---------------------------------------------------------------------------------------------------- 输出函数
/*************************************************************************
*   功能说明: 读回耳机通道的音量.
*   返 回 值: 当前音量值
*************************************************************************/
uint8_t BspWm8978_ReadEarVolume(void)
{
    return (uint8_t)(BspWm8978_ReadReg(52) & 0x3F );
}
/*************************************************************************
*   功能说明:   修改耳机输出音量
*   形    参:   _ucLeftVolume ：左声道音量值, 0-63
*                 _ucLRightVolume : 右声道音量值,0-63
*   返 回 值:   无
*************************************************************************/
void BspWm8978_SetEarVolume(uint8_t _ucVolume)
{
    uint16_t regL;
    uint16_t regR;

    if (_ucVolume > 0x3F)
    {
        _ucVolume = 0x3F;
    }
    regL = _ucVolume;
    regR = _ucVolume;
    // (LOUT1)先更新左声道缓存值
    BspWm8978_WriteReg(52, regL | 0x00);
    // (ROUT1)再同步更新左右声道的音量
    BspWm8978_WriteReg(53, regR | 0x100);
    // 0x180表示 在音量为0时再更新，避免调节音量出现的“嘎哒”声
}
/*************************************************************************
*   功能说明: 读回扬声器通道的音量.
*   返 回 值: 当前音量值
*************************************************************************/
uint8_t BspWm8978_ReadSpkVolume(void)
{
    return (uint8_t)(BspWm8978_ReadReg(54) & 0x3F );
}
/*************************************************************************
*   功能说明:   修改扬声器输出音量
*   形    参:   _ucLeftVolume ：左声道音量值, 0-63
*                 _ucLRightVolume : 右声道音量值,0-63
*************************************************************************/
void BspWm8978_SetSpkVolume(uint8_t _ucVolume)
{
    uint16_t regL;
    uint16_t regR;

    if (_ucVolume > 0x3F)
    {
        _ucVolume = 0x3F;
    }
    regL = _ucVolume;
    regR = _ucVolume;
    // (LOUT2)先更新左声道缓存值
    BspWm8978_WriteReg(54, regL | 0x00);
    // (ROUT2)再同步更新左右声道的音量
    BspWm8978_WriteReg(55, regR | 0x100);
    // 在音量为0时再更新，避免调节音量出现的“嘎哒”声
}
/*************************************************************************
*   功能说明: 输出静音.
*   形    参:  _ucMute ：1是静音，0是不静音.
*************************************************************************/
void BspWm8978_OutMute(uint8_t _ucMute)
{
    uint16_t usRegValue;
    // 静音
    if (_ucMute == 1)
    {
        usRegValue = BspWm8978_ReadReg(52);
        usRegValue |= (1u << 6);
        BspWm8978_WriteReg(52, usRegValue);

        usRegValue = BspWm8978_ReadReg(53);
        usRegValue |= (1u << 6);
        BspWm8978_WriteReg(53, usRegValue);

        usRegValue = BspWm8978_ReadReg(54);
        usRegValue |= (1u << 6);
        BspWm8978_WriteReg(54, usRegValue);

        usRegValue = BspWm8978_ReadReg(55);
        usRegValue |= (1u << 6);
        BspWm8978_WriteReg(55, usRegValue);
    }
    // 取消静音
    else
    {
        usRegValue = BspWm8978_ReadReg(52);
        usRegValue &= ~(1u << 6);
        BspWm8978_WriteReg(52, usRegValue);

        usRegValue = BspWm8978_ReadReg(53);
        usRegValue &= ~(1u << 6);
        BspWm8978_WriteReg(53, usRegValue);

        usRegValue = BspWm8978_ReadReg(54);
        usRegValue &= ~(1u << 6);
        BspWm8978_WriteReg(54, usRegValue);

        usRegValue = BspWm8978_ReadReg(55);
        usRegValue &= ~(1u << 6);
        BspWm8978_WriteReg(55, usRegValue);
    }
}

/*************************************************************************
*   功能说明: 设置MIC增益
*   形    参:  _ucGain ：音量值, 0-63
*************************************************************************/
void BspWm8978_SetMicGain(uint8_t _ucGain)
{
    if (_ucGain > GAIN_MAX)
    {
        _ucGain = GAIN_MAX;
    }

    /* PGA 音量控制  R45， R46   pdf 19页
        Bit8    INPPGAUPDATE
        Bit7    INPPGAZCL       过零再更改
        Bit6    INPPGAMUTEL     PGA静音
        Bit5:0  增益值，010000是0dB
    */
    BspWm8978_WriteReg(45, _ucGain);
    BspWm8978_WriteReg(46, _ucGain | (1 << 8));
}
/*************************************************************************
*   功能说明: 设置Line输入通道的增益
*   形    参:  _ucGain ：音量值, 0-7. 7最大，0最小。 可衰减可放大。
*************************************************************************/
void BspWm8978_SetLineGain(uint8_t _ucGain)
{
    uint16_t usRegValue;

    if (_ucGain > 7)
    {
        _ucGain = 7;
    }

    /*
        Mic 输入信道的增益由 PGABOOSTL 和 PGABOOSTR 控制
        Aux 输入信道的输入增益由 AUXL2BOOSTVO[2:0] 和 AUXR2BOOSTVO[2:0] 控制
        Line 输入信道的增益由 LIP2BOOSTVOL[2:0] 和 RIP2BOOSTVOL[2:0] 控制
    */
    /*  pdf 21页，R47（左声道），R48（右声道）, MIC 增益控制寄存器
        R47 (R48定义与此相同)
        B8      PGABOOSTL   = 1,   0表示MIC信号直通无增益，1表示MIC信号+20dB增益（通过自举电路）
        B7      = 0， 保留
        B6:4    L2_2BOOSTVOL = x， 0表示禁止，1-7表示增益-12dB ~ +6dB  （可以衰减也可以放大）
        B3      = 0， 保留
        B2:0`   AUXL2BOOSTVOL = x，0表示禁止，1-7表示增益-12dB ~ +6dB  （可以衰减也可以放大）
    */

    usRegValue = BspWm8978_ReadReg(47);
    // 将Bit6:4清0   1000 1111
    usRegValue &= 0x8F;
    usRegValue |= (_ucGain << 4);
    // 写左声道输入增益控制寄存器
    BspWm8978_WriteReg(47, usRegValue);

    usRegValue = BspWm8978_ReadReg(48);
    // 将Bit6:4清0   1000 1111
    usRegValue &= 0x8F;
    usRegValue |= (_ucGain << 4);
    // 写右声道输入增益控制寄存器
    BspWm8978_WriteReg(48, usRegValue);
}

/*************************************************************************
*   功能说明: 关闭wm8978，进入低功耗模式
*************************************************************************/
void BspWm8978_PowerDown(void)
{
    // 硬件复位WM8978所有寄存器到缺省状态
    BspWm8978_Reset();
}

/*************************************************************************
*   功能说明: 配置WM8978的音频接口(I2S)
*   形    参:
*             _usStandard : 接口标准，I2S_Standard_Phillips, I2S_Standard_MSB 或 I2S_Standard_LSB
*             _ucWordLen : 字长，16、24、32  （丢弃不常用的20bit格式）
*             _usMode : CPU I2S的工作模式，I2S_Mode_MasterTx、I2S_Mode_MasterRx、
*                       安富莱开发板硬件不支持 I2S_Mode_SlaveTx、I2S_Mode_SlaveRx 模式，这需要WM8978连接
*                       外部振荡器
*************************************************************************/
void BspWm8978_CfgAudioIF(uint16_t _usStandard, uint8_t _ucWordLen)
{
    uint16_t usReg;

    /* pdf 67页，寄存器列表 */

    /*  REG R4, 音频接口控制寄存器
        B8      BCP  = X, BCLK极性，0表示正常，1表示反相
        B7      LRCP = x, LRC时钟极性，0表示正常，1表示反相
        B6:5    WL = x， 字长，00=16bit，01=20bit，10=24bit，11=32bit （右对齐模式只能操作在最大24bit)
        B4:3    FMT = x，音频数据格式，00=右对齐，01=左对齐，10=I2S格式，11=PCM
        B2      DACLRSWAP = x, 控制DAC数据出现在LRC时钟的左边还是右边
        B1      ADCLRSWAP = x，控制ADC数据出现在LRC时钟的左边还是右边
        B0      MONO    = 0，0表示立体声，1表示单声道，仅左声道有效
    */
    usReg = 0;
    if (_usStandard == I2S_Standard_Phillips)   /* I2S飞利浦标准 */
    {
        usReg |= (2 << 3);
    }
    else if (_usStandard == I2S_Standard_MSB)   /* MSB对齐标准(左对齐) */
    {
        usReg |= (1 << 3);
    }
    else if (_usStandard == I2S_Standard_LSB)   /* LSB对齐标准(右对齐) */
    {
        usReg |= (0 << 3);
    }
    else    /* PCM标准(16位通道帧上带长或短帧同步或者16位数据帧扩展为32位通道帧) */
    {
        usReg |= (3 << 3);;
    }

    if (_ucWordLen == 24)
    {
        usReg |= (2 << 5);
    }
    else if (_ucWordLen == 32)
    {
        usReg |= (3 << 5);
    }
    else
    {
        usReg |= (0 << 5);      /* 16bit */
    }
    BspWm8978_WriteReg(4, usReg);

    /* R5  pdf 57页 */


    /*
        R6，时钟产生控制寄存器
        MS = 0,  WM8978被动时钟，由MCU提供MCLK时钟
    */
    BspWm8978_WriteReg(6, 0x000);
}



/*************************************************************************
*   函 数 名: wm8978_CfgInOut
*   功能说明: 配置wm8978音频通道
*   形    参:
*           _InPath : 音频输入通道配置
*           _OutPath : 音频输出通道配置
*   返 回 值: 无
*************************************************************************/
void BspWm8978_CfgAudioPath(uint16_t _InPath, uint16_t _OutPath)
{
    uint16_t usReg;

    /* 查看WM8978数据手册的 REGISTER MAP 章节， 第67页 */

    if ((_InPath == IN_PATH_OFF) && (_OutPath == OUT_PATH_OFF))
    {
        BspWm8978_PowerDown();
        return;
    }

    /* --------------------------- 第1步：根据输入通道参数配置寄存器 -----------------------*/
    /*
        R1 寄存器 Power manage 1
        Bit8    BUFDCOPEN,  Output stage 1.5xAVDD/2 driver enable
        Bit7    OUT4MIXEN, OUT4 mixer enable
        Bit6    OUT3MIXEN, OUT3 mixer enable
        Bit5    PLLEN   .不用
        Bit4    MICBEN  ,Microphone Bias Enable (MIC偏置电路使能)
        Bit3    BIASEN  ,Analogue amplifier bias control 必须设置为1模拟放大器才工作
        Bit2    BUFIOEN , Unused input/output tie off buffer enable
        Bit1:0  VMIDSEL, 必须设置为非00值模拟放大器才工作
    */
    usReg = (1 << 3) | (3 << 0);
    if (_OutPath & OUT3_4_ON)   /* OUT3和OUT4使能输出到GSM模块 */
    {
        usReg |= ((1 << 7) | (1 << 6));
    }
    if ((_InPath & MIC_LEFT_ON) || (_InPath & MIC_RIGHT_ON))
    {
        usReg |= (1 << 4);
    }
    BspWm8978_WriteReg(1, usReg);   /* 写寄存器 */

    /*
        R2 寄存器 Power manage 2
        Bit8    ROUT1EN,    ROUT1 output enable 耳机右声道输出使能
        Bit7    LOUT1EN,    LOUT1 output enable 耳机左声道输出使能
        Bit6    SLEEP,      0 = Normal device operation   1 = Residual current reduced in device standby mode
        Bit5    BOOSTENR,   Right channel Input BOOST enable 输入通道自举电路使能. 用到PGA放大功能时必须使能
        Bit4    BOOSTENL,   Left channel Input BOOST enable
        Bit3    INPGAENR,   Right channel input PGA enable 右声道输入PGA使能
        Bit2    INPGAENL,   Left channel input PGA enable
        Bit1    ADCENR,     Enable ADC right channel
        Bit0    ADCENL,     Enable ADC left channel
    */
    usReg = 0;
    if (_OutPath & EAR_LEFT_ON)
    {
        usReg |= (1 << 7);
    }
    if (_OutPath & EAR_RIGHT_ON)
    {
        usReg |= (1 << 8);
    }
    if (_InPath & MIC_LEFT_ON)
    {
        usReg |= ((1 << 4) | (1 << 2));
    }
    if (_InPath & MIC_RIGHT_ON)
    {
        usReg |= ((1 << 5) | (1 << 3));
    }
    if (_InPath & LINE_ON)
    {
        usReg |= ((1 << 4) | (1 << 5));
    }
    if (_InPath & MIC_RIGHT_ON)
    {
        usReg |= ((1 << 5) | (1 << 3));
    }
    if (_InPath & ADC_ON)
    {
        usReg |= ((1 << 1) | (1 << 0));
    }
    BspWm8978_WriteReg(2, usReg);   /* 写寄存器 */

    /*
        R3 寄存器 Power manage 3
        Bit8    OUT4EN,     OUT4 enable
        Bit7    OUT3EN,     OUT3 enable
        Bit6    LOUT2EN,    LOUT2 output enable
        Bit5    ROUT2EN,    ROUT2 output enable
        Bit4    0,
        Bit3    RMIXEN,     Right mixer enable
        Bit2    LMIXEN,     Left mixer enable
        Bit1    DACENR,     Right channel DAC enable
        Bit0    DACENL,     Left channel DAC enable
    */
    usReg = 0;
    if (_OutPath & OUT3_4_ON)
    {
        usReg |= ((1 << 8) | (1 << 7));
    }
    if (_OutPath & SPK_ON)
    {
        usReg |= ((1 << 6) | (1 << 5));
    }
    if (_OutPath != OUT_PATH_OFF)
    {
        usReg |= ((1 << 3) | (1 << 2));
    }
    if (_InPath & DAC_ON)
    {
        usReg |= ((1 << 1) | (1 << 0));
    }
    BspWm8978_WriteReg(3, usReg);   /* 写寄存器 */

    /*
        R44 寄存器 Input ctrl

        Bit8    MBVSEL,         Microphone Bias Voltage Control   0 = 0.9 * AVDD   1 = 0.6 * AVDD
        Bit7    0
        Bit6    R2_2INPPGA,     Connect R2 pin to right channel input PGA positive terminal
        Bit5    RIN2INPPGA,     Connect RIN pin to right channel input PGA negative terminal
        Bit4    RIP2INPPGA,     Connect RIP pin to right channel input PGA amplifier positive terminal
        Bit3    0
        Bit2    L2_2INPPGA,     Connect L2 pin to left channel input PGA positive terminal
        Bit1    LIN2INPPGA,     Connect LIN pin to left channel input PGA negative terminal
        Bit0    LIP2INPPGA,     Connect LIP pin to left channel input PGA amplifier positive terminal
    */
    usReg = 0 << 8;
    if (_InPath & LINE_ON)
    {
        usReg |= ((1 << 6) | (1 << 2));
    }
    if (_InPath & MIC_RIGHT_ON)
    {
        usReg |= ((1 << 5) | (1 << 4));
    }
    if (_InPath & MIC_LEFT_ON)
    {
        usReg |= ((1 << 1) | (1 << 0));
    }
    BspWm8978_WriteReg(44, usReg);  /* 写寄存器 */


    /*
        R14 寄存器 ADC Control
        设置高通滤波器（可选的） pdf 24、25页,
        Bit8    HPFEN,  High Pass Filter Enable高通滤波器使能，0表示禁止，1表示使能
        BIt7    HPFAPP, Select audio mode or application mode 选择音频模式或应用模式，0表示音频模式，
        Bit6:4  HPFCUT，Application mode cut-off frequency  000-111选择应用模式的截止频率
        Bit3    ADCOSR, ADC oversample rate select: 0=64x (lower power) 1=128x (best performance)
        Bit2    0
        Bit1    ADC right channel polarity adjust:  0=normal  1=inverted
        Bit0    ADC left channel polarity adjust:  0=normal 1=inverted
    */
    if (_InPath & ADC_ON)
    {
        usReg = (1 << 3) | (0 << 8) | (4 << 0);     /* 禁止ADC高通滤波器, 设置截至频率 */
    }
    else
    {
        usReg = 0;
    }
    BspWm8978_WriteReg(14, usReg);  /* 写寄存器 */

    /* 设置陷波滤波器（notch filter），主要用于抑制话筒声波正反馈，避免啸叫.  暂时关闭
        R27，R28，R29，R30 用于控制限波滤波器，pdf 26页
        R7的 Bit7 NFEN = 0 表示禁止，1表示使能
    */
    if (_InPath & ADC_ON)
    {
        usReg = (0 << 7);
        BspWm8978_WriteReg(27, usReg);  /* 写寄存器 */
        usReg = 0;
        BspWm8978_WriteReg(28, usReg);  /* 写寄存器,填0，因为已经禁止，所以也可不做 */
        BspWm8978_WriteReg(29, usReg);  /* 写寄存器,填0，因为已经禁止，所以也可不做 */
        BspWm8978_WriteReg(30, usReg);  /* 写寄存器,填0，因为已经禁止，所以也可不做 */
    }

    /* 自动增益控制 ALC, R32  - 34  pdf 19页 */
    {
        usReg = 0;      /* 禁止自动增益控制 */
        BspWm8978_WriteReg(32, usReg);
        BspWm8978_WriteReg(33, usReg);
        BspWm8978_WriteReg(34, usReg);
    }

    /*  R35  ALC Noise Gate Control
        Bit3    NGATEN, Noise gate function enable
        Bit2:0  Noise gate threshold:
    */
    usReg = (3 << 1) | (7 << 0);        /* 禁止自动增益控制 */
    BspWm8978_WriteReg(35, usReg);

    /*
        Mic 输入信道的增益由 PGABOOSTL 和 PGABOOSTR 控制
        Aux 输入信道的输入增益由 AUXL2BOOSTVO[2:0] 和 AUXR2BOOSTVO[2:0] 控制
        Line 输入信道的增益由 LIP2BOOSTVOL[2:0] 和 RIP2BOOSTVOL[2:0] 控制
    */
    /*  pdf 21页，R47（左声道），R48（右声道）, MIC 增益控制寄存器
        R47 (R48定义与此相同)
        B8      PGABOOSTL   = 1,   0表示MIC信号直通无增益，1表示MIC信号+20dB增益（通过自举电路）
        B7      = 0， 保留
        B6:4    L2_2BOOSTVOL = x， 0表示禁止，1-7表示增益-12dB ~ +6dB  （可以衰减也可以放大）
        B3      = 0， 保留
        B2:0`   AUXL2BOOSTVOL = x，0表示禁止，1-7表示增益-12dB ~ +6dB  （可以衰减也可以放大）
    */
    usReg = 0;
    if ((_InPath & MIC_LEFT_ON) || (_InPath & MIC_RIGHT_ON))
    {
        usReg |= (1 << 8);  /* MIC增益取+20dB */
    }
    if (_InPath & AUX_ON)
    {
        usReg |= (3 << 0);  /* Aux增益固定取3，用户可以自行调整 */
    }
    if (_InPath & LINE_ON)
    {
        usReg |= (3 << 4);  /* Line增益固定取3，用户可以自行调整 */
    }
    BspWm8978_WriteReg(47, usReg);  /* 写左声道输入增益控制寄存器 */
    BspWm8978_WriteReg(48, usReg);  /* 写右声道输入增益控制寄存器 */

    /* 数字ADC音量控制，pdf 27页
        R15 控制左声道ADC音量，R16控制右声道ADC音量
        Bit8    ADCVU  = 1 时才更新，用于同步更新左右声道的ADC音量
        Bit7:0  增益选择； 0000 0000 = 静音
                           0000 0001 = -127dB
                           0000 0010 = -12.5dB  （0.5dB 步长）
                           1111 1111 = 0dB  （不衰减）
    */
    usReg = 0xFF;
    BspWm8978_WriteReg(15, usReg);  /* 选择0dB，先缓存左声道 */
    usReg = 0x1FF;
    BspWm8978_WriteReg(16, usReg);  /* 同步更新左右声道 */

    /* 通过 wm8978_SetMicGain 函数设置mic PGA增益 */

    /*  R43 寄存器  AUXR – ROUT2 BEEP Mixer Function
        B8:6 = 0

        B5   MUTERPGA2INV,  Mute input to INVROUT2 mixer
        B4   INVROUT2,  Invert ROUT2 output 用于扬声器推挽输出
        B3:1 BEEPVOL = 7;   AUXR input to ROUT2 inverter gain
        B0   BEEPEN = 1;    Enable AUXR beep input

    */
    usReg = 0;
    if (_OutPath & SPK_ON)
    {
        usReg |= (1 << 4);  /* ROUT2 反相, 用于驱动扬声器 */
    }
    if (_InPath & AUX_ON)
    {
        usReg |= ((7 << 1) | (1 << 0));
    }
    BspWm8978_WriteReg(43, usReg);

    /* R49  Output ctrl
        B8:7    0
        B6      DACL2RMIX,  Left DAC output to right output mixer
        B5      DACR2LMIX,  Right DAC output to left output
        B4      OUT4BOOST,  0 = OUT4 output gain = -1; DC = AVDD / 2；1 = OUT4 output gain = +1.5；DC = 1.5 x AVDD / 2
        B3      OUT3BOOST,  0 = OUT3 output gain = -1; DC = AVDD / 2；1 = OUT3 output gain = +1.5；DC = 1.5 x AVDD / 2
        B2      SPKBOOST,   0 = Speaker gain = -1;  DC = AVDD / 2 ; 1 = Speaker gain = +1.5; DC = 1.5 x AVDD / 2
        B1      TSDEN,   Thermal Shutdown Enable  扬声器热保护使能（缺省1）
        B0      VROI,   Disabled Outputs to VREF Resistance
    */
    usReg = 0;
    if (_InPath & DAC_ON)
    {
        usReg |= ((1 << 6) | (1 << 5));
    }
    if (_OutPath & SPK_ON)
    {
        usReg |=  ((1 << 2) | (1 << 1));    /* SPK 1.5x增益,  热保护使能 */
    }
    if (_OutPath & OUT3_4_ON)
    {
        usReg |=  ((1 << 4) | (1 << 3));    /* BOOT3  BOOT4  1.5x增益 */
    }
    BspWm8978_WriteReg(49, usReg);

    /*  REG 50    (50是左声道，51是右声道，配置寄存器功能一致) pdf 40页
        B8:6    AUXLMIXVOL = 111    AUX用于FM收音机信号输入
        B5      AUXL2LMIX = 1       Left Auxilliary input to left channel
        B4:2    BYPLMIXVOL          音量
        B1      BYPL2LMIX = 0;      Left bypass path (from the left channel input boost output) to left output mixer
        B0      DACL2LMIX = 1;      Left DAC output to left output mixer
    */
    usReg = 0;
    if (_InPath & AUX_ON)
    {
        usReg |= ((7 << 6) | (1 << 5));
    }
    if ((_InPath & LINE_ON) || (_InPath & MIC_LEFT_ON) || (_InPath & MIC_RIGHT_ON))
    {
        usReg |= ((7 << 2) | (1 << 1));
    }
    if (_InPath & DAC_ON)
    {
        usReg |= (1 << 0);
    }
    BspWm8978_WriteReg(50, usReg);
    BspWm8978_WriteReg(51, usReg);

    /*  R56 寄存器   OUT3 mixer ctrl
        B8:7    0
        B6      OUT3MUTE,   0 = Output stage outputs OUT3 mixer;  1 = Output stage muted – drives out VMID.
        B5:4    0
        B3      BYPL2OUT3,  OUT4 mixer output to OUT3  (反相)
        B4      0
        B2      LMIX2OUT3,  Left ADC input to OUT3
        B1      LDAC2OUT3,  Left DAC mixer to OUT3
        B0      LDAC2OUT3,  Left DAC output to OUT3
    */
    usReg = 0;
    if (_OutPath & OUT3_4_ON)
    {
        usReg |= (1 << 3);
    }
    BspWm8978_WriteReg(56, usReg);

    /* R57 寄存器       OUT4 (MONO) mixer ctrl
        B8:7    0
        B6      OUT4MUTE,   0 = Output stage outputs OUT4 mixer  1 = Output stage muted – drives outVMID.
        B5      HALFSIG,    0 = OUT4 normal output  1 = OUT4 attenuated by 6dB
        B4      LMIX2OUT4,  Left DAC mixer to OUT4
        B3      LDAC2UT4,   Left DAC to OUT4
        B2      BYPR2OUT4,  Right ADC input to OUT4
        B1      RMIX2OUT4,  Right DAC mixer to OUT4
        B0      RDAC2OUT4,  Right DAC output to OUT4
    */
    usReg = 0;
    if (_OutPath & OUT3_4_ON)
    {
        usReg |= ((1 << 4) |  (1 << 1));
    }
    BspWm8978_WriteReg(57, usReg);


    /* R11, 12 寄存器 DAC数字音量
        R11     Left DAC Digital Volume
        R12     Right DAC Digital Volume
    */
    if (_InPath & DAC_ON)
    {
        BspWm8978_WriteReg(11, 255);
        BspWm8978_WriteReg(12, 255 | 0x100);
    }
    else
    {
        BspWm8978_WriteReg(11, 0);
        BspWm8978_WriteReg(12, 0 | 0x100);
    }

    /*  R10 寄存器 DAC Control
        B8  0
        B7  0
        B6  SOFTMUTE,   Softmute enable:
        B5  0
        B4  0
        B3  DACOSR128,  DAC oversampling rate: 0=64x (lowest power) 1=128x (best performance)
        B2  AMUTE,      Automute enable
        B1  DACPOLR,    Right DAC output polarity
        B0  DACPOLL,    Left DAC output polarity:
    */
    if (_InPath & DAC_ON)
    {
        BspWm8978_WriteReg(10, 0);
    }
    ;
}

/*************************************************************************
*   功能说明: 设置陷波滤波器（notch filter），主要用于抑制话筒声波正反馈，避免啸叫
*   形    参:  NFA0[13:0] and NFA1[13:0]
*************************************************************************/
void BspWm8978_NotchFilter(uint16_t _NFA0, uint16_t _NFA1)
{
    uint16_t usReg;

    /*  page 26
        A programmable notch filter is provided. This filter has a variable centre frequency and bandwidth,
        programmable via two coefficients, a0 and a1. a0 and a1 are represented by the register bits
        NFA0[13:0] and NFA1[13:0]. Because these coefficient values require four register writes to setup
        there is an NFU (Notch Filter Update) flag which should be set only when all four registers are setup.
    */
    usReg = (1 << 7) | (_NFA0 & 0x3F);
    BspWm8978_WriteReg(27, usReg);  /* 写寄存器 */

    usReg = ((_NFA0 >> 7) & 0x3F);
    BspWm8978_WriteReg(28, usReg);  /* 写寄存器 */

    usReg = (_NFA1 & 0x3F);
    BspWm8978_WriteReg(29, usReg);  /* 写寄存器 */

    usReg = (1 << 8) | ((_NFA1 >> 7) & 0x3F);
    BspWm8978_WriteReg(30, usReg);  /* 写寄存器 */
}

/*************************************************************************
*   功能说明: 控制WM8978的GPIO1引脚输出0或1
*   形    参:  _ucValue ：GPIO1输出值，0或1
*************************************************************************/
void BspWm8978_CtrlGPIO1(uint8_t _ucValue)
{
    uint16_t usRegValue;

    /* R8， pdf 62页 */
    if (_ucValue == 0) /* 输出0 */
    {
        usRegValue = 6; /* B2:0 = 110 */
    }
    else
    {
        usRegValue = 7; /* B2:0 = 111 */
    }
    BspWm8978_WriteReg(8, usRegValue);
}



/*
*********************************************************************************************************
*                        下面的代码是和STM32 I2S硬件相关的
*********************************************************************************************************
*/

/*************************************************************************
*  功能说明:   配置I2S工作模式，启动I2S发送缓冲区空中断，开始放音
*  形    参:   _usStandard    :  接口标准
*                                I2S_Standard_Phillips
*                                I2S_Standard_MSB
*                                I2S_Standard_LSB
*              _usMCLKOutput  :  主时钟输出
*                                I2S_MCLKOutput_Enable
*                                I2S_MCLKOutput_Disable
*              _uiAudioFreq   :  采样频率
*                                I2S_AudioFreq_8K
*                                I2S_AudioFreq_16K
*                                I2S_AudioFreq_22K
*                                I2S_AudioFreq_44K
*                                I2S_AudioFreq_48
*************************************************************************/
void BspWm8978_StartPlay(uint16_t _usStandard, uint16_t _usWordLen,  uint32_t _uiAudioFreq)
{
    // 配置I2S为主发送模式，即STM32提供主时钟，I2S数据口是发送方向(放音)
    BspWm8978_I2S_ModeConfig(_usStandard, _usWordLen, _uiAudioFreq, I2S_Mode_MasterTx);
    // 使能发送中断
    SPI_I2S_ITConfig(BSP_WM8978_IIS_X, SPI_I2S_IT_TXE, ENABLE);
}

/*************************************************************************
*  功能说明:   配置I2S工作模式，启动I2S接收缓冲区不空中断，开始录音。
*              在调用本函数前，请先配置WM8978录音通道.
*  形    参:   _usStandard    :  接口标准
*                                I2S_Standard_Phillips
*                                I2S_Standard_MSB
*                                I2S_Standard_LSB
*              _usMCLKOutput  :  主时钟输出
*                                I2S_MCLKOutput_Enable
*                                I2S_MCLKOutput_Disable
*              _uiAudioFreq   :  采样频率，
*                                I2S_AudioFreq_8K、
*                                I2S_AudioFreq_16K、
*                                I2S_AudioFreq_22K、
*                                I2S_AudioFreq_44K、
*                                I2S_AudioFreq_48
*************************************************************************/
void BspWm8978_StartRecord(uint16_t _usStandard, uint16_t _usWordLen, uint32_t _uiAudioFreq)
{
    // 配置I2S为主发送模式，即STM32提供主时钟，I2S数据口是发送方向(放音)
    BspWm8978_I2S_ModeConfig(_usStandard, _usWordLen, _uiAudioFreq, I2S_Mode_MasterTx);
    // 使能接收中断
    SPI_I2S_ITConfig(BSP_WM8978_IIS_EXT_X, SPI_I2S_IT_RXNE, ENABLE);
    // 使能发送中断
    SPI_I2S_ITConfig(BSP_WM8978_IIS_X, SPI_I2S_IT_TXE, ENABLE);
    SPI_I2S_SendData(BSP_WM8978_IIS_X, 0);
}
/*************************************************************************
*   功能说明: 停止I2S工作
*************************************************************************/
void BspWm8978_Stop(void)
{
    // 禁止I2S2 TXE中断(发送缓冲区空)，需要时再打开
    SPI_I2S_ITConfig(BSP_WM8978_IIS_X, SPI_I2S_IT_TXE, DISABLE);
    // 禁止I2S2 RXNE中断(接收不空)，需要时再打开
    SPI_I2S_ITConfig(BSP_WM8978_IIS_EXT_X, SPI_I2S_IT_RXNE, DISABLE);
    // 禁能 SPI2/I2S2 外设
    I2S_Cmd(BSP_WM8978_IIS_X, DISABLE);
    // 关闭 I2S2 APB1 时钟
    BSP_WM8978_IIS_RCC_DISABLE;
}
/*
*********************************************************************************************************
*   函 数 名: I2S_CODEC_DataTransfer
*   功能说明: I2S数据传输函数, 被SPI2 (I2S)中断服务程序调用。录音和放音均在此函数处理。
*   形    参：无
*   返 回 值: 无
*********************************************************************************************************
*/
void I2S_CODEC_DataTransfer(void)
{
    uint16_t usData;

    /* 只录左声道(安富莱STM32开发板MIC安装在左声道), 放音时左右声道相同 */
    if (g_tRec.ucStatus == STA_RECORDING)   /* 录音状态 */
    {
        if (SPI_I2S_GetFlagStatus(BSP_WM8978_IIS_EXT_X, SPI_I2S_FLAG_RXNE) == SET)
        {
            usData = SPI_I2S_ReceiveData(BSP_WM8978_IIS_EXT_X);
#if 1
            // 录双通道,保存音频数据
            g_tRec.pAudio[g_tRec.uiCursor] = usData;
            if (++g_tRec.uiCursor >= EXT_SRAM_SIZE / 2)
            {
                g_tRec.uiCursor = EXT_SRAM_SIZE / 2;
                /* 禁止I2S2 RXNE中断(接收不空)，需要时再打开 */
                SPI_I2S_ITConfig(BSP_WM8978_IIS_EXT_X, SPI_I2S_IT_RXNE, DISABLE);
                /* 禁止I2S2 TXE中断(发送缓冲区空)，需要时再打开 */
                SPI_I2S_ITConfig(BSP_WM8978_IIS_X, SPI_I2S_IT_TXE, DISABLE);

                BspKey_NewSign   =  1;
                BspKey_Value     =  BSP_KEY_E_KEY8;
            }
#else   /* 选择一个声道进行记录 */
            if (SPI_I2S_GetFlagStatus(BSP_WM8978_IIS_X, I2S_FLAG_CHSIDE) != SET)    /* 判断是不是左声道 */
            {
                g_tRec.pAudio[g_tRec.uiCursor] = usData;    /* 保存音频数据 */
                if (++g_tRec.uiCursor >= EXT_SRAM_SIZE / 2)
                {
                    g_tRec.uiCursor = EXT_SRAM_SIZE / 2;
                    /* 禁止I2S2 RXNE中断(接收不空)，需要时再打开 */
                    SPI_I2S_ITConfig(BSP_WM8978_IIS_EXT_X, SPI_I2S_IT_RXNE, DISABLE);
                    /* 禁止I2S2 TXE中断(发送缓冲区空)，需要时再打开 */
                    SPI_I2S_ITConfig(BSP_WM8978_IIS_X, SPI_I2S_IT_TXE, DISABLE);

                    BspKey_NewSign   =  1;
                    BspKey_Value     =  BSP_KEY_E_KEY8;
                }
            }
#endif
        }

        /* 即使在录音状态，也需要向I2S设备发送1个字节，以触发下次进入本中断 */
        if (SPI_I2S_GetITStatus(BSP_WM8978_IIS_X, SPI_I2S_IT_TXE) == SET)
        {
            SPI_I2S_SendData(BSP_WM8978_IIS_X, 0);  /* 随便发送1个数据 */
        }
    }
    else if (g_tRec.ucStatus == STA_PLAYING)    /* 放音状态，右声道的值等于左声道 */
    {
        if (SPI_I2S_GetITStatus(BSP_WM8978_IIS_X, SPI_I2S_IT_TXE) == SET)
        {
#if 1
            //TF放音
            usData = g_tRec.pAudio[g_tRec.uiCursor];

            g_tRec.uiCursor+=1;
            if(g_tRec.uiCursor==10)
            {
                //读数据到备份缓存
                BspWm897_ItCmd=1;
            }
            else if (g_tRec.uiCursor >= (5*1024L/2))
            {
                //备份缓存到正式缓存
                if(g_tRec.pAudio==pBspWm8978_pbuf1)
                {
                    g_tRec.pAudio=pBspWm8978_pbuf2;
                }
                else if(g_tRec.pAudio==pBspWm8978_pbuf2)
                {
                    g_tRec.pAudio=pBspWm8978_pbuf1;
                }
                g_tRec.uiCursor=0;
            }
            else if (g_tRec.uiCursor >= EXT_SRAM_SIZE / 2)
            {
                g_tRec.uiCursor = EXT_SRAM_SIZE / 2;
                // 禁止I2S2 RXNE中断(接收不空)，需要时再打开
                SPI_I2S_ITConfig(BSP_WM8978_IIS_EXT_X, SPI_I2S_IT_RXNE, DISABLE);
                // 禁止I2S2 TXE中断(发送缓冲区空)，需要时再打开
                SPI_I2S_ITConfig(BSP_WM8978_IIS_X, SPI_I2S_IT_TXE, DISABLE);
            }
            SPI_I2S_SendData(BSP_WM8978_IIS_X, usData);
            /*
            //双通道放音
            usData = g_tRec.pAudio[g_tRec.uiCursor];
            if (++g_tRec.uiCursor >= EXT_SRAM_SIZE / 2)
            {
                g_tRec.uiCursor = EXT_SRAM_SIZE / 2;
                // 禁止I2S2 RXNE中断(接收不空)，需要时再打开
                SPI_I2S_ITConfig(BSP_WM8978_IIS_EXT_X, SPI_I2S_IT_RXNE, DISABLE);
                // 禁止I2S2 TXE中断(发送缓冲区空)，需要时再打开
                SPI_I2S_ITConfig(BSP_WM8978_IIS_X, SPI_I2S_IT_TXE, DISABLE);

                BspKey_NewSign   =  1;
                BspKey_Value     =  BSP_KEY_E_KEY8;
            }
            SPI_I2S_SendData(BSP_WM8978_IIS_X, usData);
            */
#else   /* 可以将其中1个通道的声音复制到另外1个通道(用于左声道录音或右声道录音) */
            usData = g_tRec.pAudio[g_tRec.uiCursor];        /* 读取音频数据 */
            if (SPI_I2S_GetFlagStatus(BSP_WM8978_IIS_X, I2S_FLAG_CHSIDE) != SET)    /* 判断是不是左声道 */
            {
                if (++g_tRec.uiCursor >= EXT_SRAM_SIZE / 2)
                {
                    g_tRec.uiCursor = EXT_SRAM_SIZE / 2;
                    /* 禁止I2S2 RXNE中断(接收不空)，需要时再打开 */
                    SPI_I2S_ITConfig(BSP_WM8978_IIS_EXT_X, SPI_I2S_IT_RXNE, DISABLE);
                    /* 禁止I2S2 TXE中断(发送缓冲区空)，需要时再打开 */
                    SPI_I2S_ITConfig(BSP_WM8978_IIS_X, SPI_I2S_IT_TXE, DISABLE);

                    BspKey_NewSign   =  1;
                    BspKey_Value     =  BSP_KEY_E_KEY8;
                }
            }
            SPI_I2S_SendData(BSP_WM8978_IIS_X, usData);
#endif
        }
    }
    else    /* 临时状态 STA_STOP_I2S，用于停止中断 */
    {
        /* 禁止I2S2 RXNE中断(接收不空)，需要时再打开 */
        SPI_I2S_ITConfig(BSP_WM8978_IIS_EXT_X, SPI_I2S_IT_RXNE, DISABLE);
        /* 禁止I2S2 TXE中断(发送缓冲区空)，需要时再打开 */
        SPI_I2S_ITConfig(BSP_WM8978_IIS_X, SPI_I2S_IT_TXE, DISABLE);
    }
}
/*
**************************************************************************
*   函 数 名: SPI2_IRQHandler
*   功能说明: I2S数据传输中断服务程序
*   形    参：无
*   返 回 值: 无
**************************************************************************
*/
/*
void SPI2_IRQHandler(void)
{
#if   (defined(OS_UCOSIII))
    CPU_SR_ALLOC();
    CPU_CRITICAL_ENTER();
    OSIntNestingCtr++;
    CPU_CRITICAL_EXIT();
    //-----用户代码-----
    I2S_CODEC_DataTransfer();
    //------------------
    OSIntExit();
#else
    I2S_CODEC_DataTransfer();
#endif
}
*/
//---------------------------------------------------------------------------------------------------- 测试函数
//音频格式数组
#define FMT_COUNT   6
uint32_t const g_FmtList[FMT_COUNT][3] =
{
    {I2S_Standard_Phillips, I2S_DataFormat_16b, I2S_AudioFreq_8k},
    {I2S_Standard_Phillips, I2S_DataFormat_16b, I2S_AudioFreq_16k},
    {I2S_Standard_Phillips, I2S_DataFormat_16b, I2S_AudioFreq_22k},
    {I2S_Standard_Phillips, I2S_DataFormat_16b, I2S_AudioFreq_44k},
    {I2S_Standard_Phillips, I2S_DataFormat_16b, I2S_AudioFreq_96k},
    {I2S_Standard_Phillips, I2S_DataFormat_16b, I2S_AudioFreq_192k},
};
static void DispStatus(void)
{
    char buf[50];

    /*显示当前音频格式*/
    sprintf(buf, "音频格式: Philips,16Bit,%d.%dkHz  ",
            g_FmtList[g_tRec.ucFmtIdx][2]/1000,
            (g_FmtList[g_tRec.ucFmtIdx][2]%1000)/100
           );
    DebugOutStr((int8_t*)buf);

    sprintf(buf, "录音时长: %d.%03d秒  ",
            (EXT_SRAM_SIZE / 2) / g_FmtList[g_tRec.ucFmtIdx][2],
            (((EXT_SRAM_SIZE / 2) * 1000) / g_FmtList[g_tRec.ucFmtIdx][2]) % 1000
           );
    DebugOutStr((int8_t*)buf);

    if (g_tRec.ucStatus == STA_IDLE)
    {
        DebugOutStr("状态=空闲");
    }
    else if (g_tRec.ucStatus == STA_RECORDING)
    {
        DebugOutStr("状态=正在录音");
    }
    else if (g_tRec.ucStatus == STA_PLAYING)
    {
        DebugOutStr("状态=正在回放");
    }

    sprintf(buf, "  麦克风增益=%d", g_tRec.ucMicGain);
    DebugOutStr((int8_t*)buf);

    sprintf(buf, "  耳机音量=%d\r\n", g_tRec.ucVolume);
    DebugOutStr((int8_t*)buf);
}
//录放音测试
void BspWM8978_Record_DebugTestOnOff(uint8_t OnOff)
{
    static uint8_t s1=0;
    if(OnOff==ON)
    {
        switch(s1)
        {
            case 0:  //首次进入
                s1=1;
                //检测器件
                if(OK!=BspWm8978_Init())
                {
                    DebugOutStr("Don't Find WM8978\r\n");
                }
                else
                {
                    DebugOutStr("Find WM8978 OK!\r\n");
                }
                //显示帮助
                DebugOutStr("K1-->Select audio formats\r\n");
                DebugOutStr("K2-->Record\r\n");
                DebugOutStr("K3-->Playback\r\n");
                DebugOutStr("UD-->Speaker volume\r\n");
                DebugOutStr("LR-->Mic gain\r\n");
                DebugOutStr("OK-->Stop\r\n");
                //设定初始状态
                g_tRec.ucVolume =  39;
                g_tRec.ucMicGain=  34;
                g_tRec.ucFmtIdx =  1;
                g_tRec.pAudio   =  (int16_t *)EXT_SRAM_ADDR;
                g_tRec.ucStatus =  STA_IDLE;
                //清录音区(SRAM-2MB)
                {
                    int i;

                    for (i = 0 ; i < 1 * 1024 * 1024; i++)
                    {
                        g_tRec.pAudio[i] = 0;
                    }
                }
                break;
            case 1:  //处理按键
                if(BspKey_NewSign==1)
                {
                    BspKey_NewSign  =  0;
                    switch(BspKey_Value)
                    {
                        case BSP_KEY_E_KEY1:
                            if (++g_tRec.ucFmtIdx >= FMT_COUNT)
                            {
                                g_tRec.ucFmtIdx = 0;
                            }
                            s1=2;
                            break;
                        case BSP_KEY_E_KEY2:
                            //停止
                            g_tRec.ucStatus = STA_STOP_I2S;
                            MODULE_OS_DELAY_MS(10);
                            BspWm8978_Init();
                            BspWm8978_Stop();
                            //录音
                            g_tRec.ucStatus = STA_RECORDING;
                            g_tRec.uiCursor = 0;
                            BspWm8978_CfgAudioPath(MIC_LEFT_ON | MIC_RIGHT_ON | ADC_ON, EAR_LEFT_ON | EAR_RIGHT_ON);
                            BspWm8978_SetEarVolume(g_tRec.ucVolume);
                            BspWm8978_SetMicGain(g_tRec.ucMicGain);
                            BspWm8978_CfgAudioIF(I2S_Standard_Phillips, 16);
                            BspWm8978_StartRecord(g_FmtList[g_tRec.ucFmtIdx][0], g_FmtList[g_tRec.ucFmtIdx][1], g_FmtList[g_tRec.ucFmtIdx][2]);
                            s1=2;
                            break;
                        case BSP_KEY_E_KEY3:
                            //
                            //停止
                            g_tRec.ucStatus = STA_STOP_I2S;
                            MODULE_OS_DELAY_MS(10);
                            BspWm8978_Init();
                            BspWm8978_Stop();
                            //放音
                            g_tRec.ucStatus = STA_PLAYING;
                            g_tRec.uiCursor = 0;
                            BspWm8978_CfgAudioPath(DAC_ON, EAR_LEFT_ON | EAR_RIGHT_ON | SPK_ON);
                            BspWm8978_SetEarVolume(g_tRec.ucVolume);
                            BspWm8978_SetSpkVolume(g_tRec.ucVolume);
                            BspWm8978_CfgAudioIF(I2S_Standard_Phillips, 16);
                            BspWm8978_StartPlay(g_FmtList[g_tRec.ucFmtIdx][0], g_FmtList[g_tRec.ucFmtIdx][1], g_FmtList[g_tRec.ucFmtIdx][2]);
                            s1=2;
                            break;
                        case BSP_KEY_E_KEY4: // up
                            if (g_tRec.ucVolume <= VOLUME_MAX - VOLUME_STEP)
                            {
                                g_tRec.ucVolume += VOLUME_STEP;
                                BspWm8978_SetEarVolume(g_tRec.ucVolume);
                                BspWm8978_SetSpkVolume(g_tRec.ucVolume);
                            }
                            s1=2;
                            break;
                        case BSP_KEY_E_KEY5: // domn
                            if (g_tRec.ucVolume >= VOLUME_STEP)
                            {
                                g_tRec.ucVolume -= VOLUME_STEP;
                                BspWm8978_SetEarVolume(g_tRec.ucVolume);
                                BspWm8978_SetSpkVolume(g_tRec.ucVolume);
                            }
                            s1=2;
                            break;
                        case BSP_KEY_E_KEY6: // left
                            if (g_tRec.ucMicGain >= GAIN_STEP)
                            {
                                g_tRec.ucMicGain -= GAIN_STEP;
                                BspWm8978_SetMicGain(g_tRec.ucMicGain);
                            }
                            s1=2;
                            break;
                        case BSP_KEY_E_KEY7: // right
                            if (g_tRec.ucMicGain <= GAIN_MAX - GAIN_STEP)
                            {
                                g_tRec.ucMicGain += GAIN_STEP;
                                BspWm8978_SetMicGain(g_tRec.ucMicGain);
                            }
                            s1=2;
                            break;
                        case BSP_KEY_E_KEY8: // ok
                            g_tRec.ucStatus = STA_IDLE;
                            BspWm8978_Stop();
                            BspWm8978_Init();
                            s1=2;
                            break;
                        default:
                            break;
                    }
                }
                break;
            case 2:
                s1=1;
                DispStatus();
            default:
                break;
        }
        //
    }
    else
    {
        s1=0;
    }
}
#include "Module_SdFat.h"
FIL      BspWm8978_fsrc;
FATFS    BspWm8978_Fs;
void BspWM8978_TfPlayer_DebugTestOnOff(uint8_t OnOff)
{
    FRESULT fat_res;
    uint32_t i32=5*1024L;
    uint8_t *pstr;
    OnOff=OnOff;
	  fat_res=fat_res;
    i32=5*1024L;
    //申请DMA缓存
    pBspWm8978_pbuf1 =  MemManager_Get(E_MEM_MANAGER_TYPE_5KB_BASIC);
    pBspWm8978_pbuf2 =  MemManager_Get(E_MEM_MANAGER_TYPE_5KB_BASIC);
    pstr    =   MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    //挂载
    fat_res = f_mount(&BspWm8978_Fs,MODULE_SDFAT_SD_PATH,0);
    //打开文件
    fat_res = f_open(&BspWm8978_fsrc, "/wav1.wav", FA_OPEN_EXISTING | FA_READ | FA_OPEN_ALWAYS);
    //
    //停止
    g_tRec.ucStatus = STA_STOP_I2S;
    MODULE_OS_DELAY_MS(10);
    BspWm8978_Init();
    BspWm8978_Stop();
    //放音
    g_tRec.ucStatus  =  STA_PLAYING;
    g_tRec.uiCursor  =  0;
    g_tRec.ucFmtIdx  =  3;
    g_tRec.pAudio    =  (int16_t*)pBspWm8978_pbuf1;
    g_tRec.ucVolume  =  39;
    BspWm8978_CfgAudioPath(DAC_ON, EAR_LEFT_ON | EAR_RIGHT_ON | SPK_ON);
    BspWm8978_SetEarVolume(g_tRec.ucVolume);
    BspWm8978_SetSpkVolume(1);
    BspWm8978_CfgAudioIF(I2S_Standard_Phillips, 16);
    //读取文件数据
    fat_res = f_read(&BspWm8978_fsrc, pBspWm8978_pbuf1, 5*1024L, &i32);
    BspWm8978_StartPlay(g_FmtList[g_tRec.ucFmtIdx][0], g_FmtList[g_tRec.ucFmtIdx][1], g_FmtList[g_tRec.ucFmtIdx][2]);
    while(1)
    {
        if(BspWm897_ItCmd==1)
        {
            BspWm897_ItCmd=0;
            if(g_tRec.pAudio==pBspWm8978_pbuf1)
            {
                fat_res = f_read(&BspWm8978_fsrc, pBspWm8978_pbuf2, 5*1024L, &i32);
            }
            else
            {
                fat_res = f_read(&BspWm8978_fsrc, pBspWm8978_pbuf1, 5*1024L, &i32);
            }
            if(i32!=5*1024L)
            {
                MODULE_OS_DELAY_MS(1000);
                break;
            }
        }
        MODULE_OS_DELAY_MS(10);
        //按键处理
        if(BspKey_NewSign==1)
        {
            BspKey_NewSign  =  0;
            switch(BspKey_Value)
            {
                case BSP_KEY_E_KEY1:
                    if (g_tRec.ucVolume <= VOLUME_MAX - VOLUME_STEP)
                    {
                        g_tRec.ucVolume += VOLUME_STEP;
                        BspWm8978_SetEarVolume(g_tRec.ucVolume);
                        //BspWm8978_SetSpkVolume(g_tRec.ucVolume);
                    }
                    break;
                case BSP_KEY_E_KEY2: // domn
                    if (g_tRec.ucVolume >= VOLUME_STEP)
                    {
                        g_tRec.ucVolume -= VOLUME_STEP;
                        BspWm8978_SetEarVolume(g_tRec.ucVolume);
                        //BspWm8978_SetSpkVolume(g_tRec.ucVolume);
                    }
                    break;
                default:
                    break;
            }
        }
    }
    g_tRec.ucStatus = STA_IDLE;
    BspWm8978_Stop();
    BspWm8978_Init();
    //关闭文件
    fat_res = f_close(&BspWm8978_fsrc);
    //卸载
    fat_res = f_mount(NULL,MODULE_SDFAT_SD_PATH,0);
    //释放DMA缓存
    MemManager_Free(E_MEM_MANAGER_TYPE_5KB_BASIC,pBspWm8978_pbuf1);
    MemManager_Free(E_MEM_MANAGER_TYPE_5KB_BASIC,pBspWm8978_pbuf2);
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pstr);
}
//---------------------------------------------------------------

