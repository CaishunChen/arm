/*
***********************************************************************************
*                    ��    ��: ������
*                    ����ʱ��: 2014-05-29
***********************************************************************************
*/
//------------------------------- Includes -----------------------------------
#define SPI_SLAVE_GLOBAL
#include "includes.h"
#include "IAP.h"
#include "Bsp_CpuFlash.h"
#include "uctsk_SpiSlave.h"
#include "Module_Memory.h"
#include "Bsp_Uart.h"
#include "Bsp_Rtc.h"
#include "uctsk_AD.h"
#include "Bsp_Key.h"
#include "Bsp_Relay.h"
//----------------------------------------------------------------------------��������
enum SPI_E_SLAVE_STEP
{
    //���ڽ���
    SPI_E_SLAVE_CMD_UART1_R,
    SPI_E_SLAVE_CMD_UART2_R,
    SPI_E_SLAVE_CMD_UART3_R,
    SPI_E_SLAVE_CMD_UART4_R,
    SPI_E_SLAVE_CMD_UART5_R,
    //��������
    SPI_E_SLAVE_CMD_M_PARSE,
} ;
typedef struct SPI_SLAVE_S_Q
{
    uint16_t   cmd;
    uint16_t   len;
    uint8_t    *pBuf;
    uint8_t    lock;
} SPI_SLAVE_S_Q;
static const uint32_t UCTSK_SPISLAVE_UART_BSP[8]= {1200,2400,4800,9600,19200,38400,57600,115200};
//----------------------------------------------------------------------------ȫ�ֱ���
//����
MODULE_OS_TASK_TAB(AppTaskSpiTCB);
MODULE_OS_TASK_STK(App_TaskSpiStk,APP_TASK_SPI_STK_SIZE);
//��Ϣ����
MODULE_OS_Q(SpiSlave_pQ);
//��Ϣ���е���Ϣʵ��
SPI_SLAVE_S_Q Spi_Slave_S_QBuf[SPI_SLAVE_QBUF_MAX];
//״̬�ṹ��
static SPI_SLAVE_S_STATE SPI_Slave_S_State;
//
#define SPI_SLAVE_TXBUF_MAX   256
#define SPI_SLAVE_RXBUF_MAX   256
static uint8_t Spi_SlaveTxBuf[SPI_SLAVE_TXBUF_MAX];
static uint8_t Spi_SlaveRxBuf[SPI_SLAVE_RXBUF_MAX];
static uint16_t Spi_SlaveRxLen=0;
//SPI����
static uint8_t Spi_SlaveErrCounter=0;
//DMA���ռ�ʱ/����
static uint8_t Spi_SlaveRxDMATimer=0;
//�Զ�ѭ��Modbus���ڻ���
static uint16_t Spi_SlaveAutoRxUartBuf[5][1024]= {0};
static uint16_t Spi_SlaveAutoRxUartNumBuf[5]= {0};
//����ģʽ: 0-��������ģʽ 1-�����Զ�����ģʽ 2-��������͸��ģʽ
static uint8_t  Spi_SlaveUartModeBuf[5]= {0,0,0,0,0};
//����ָ��: 0-��ָ��,      1-���������Զ�����ģʽ,2-������������͸��ģʽ
static uint8_t  Spi_SlaveUartCmdBuf[5]= {0};
//���滷
#ifdef SPI_SLAVE_EXTEND_S_UART1_EN
#define UCTSK_SPISLAVE_UART_TX1_BUFFERLOOP_BUF_MAX   256
#define UCTSK_SPISLAVE_UART_TX1_BUFFERLOOP_NODE_MAX  10
static uint8_t UctskSpiSlave_UartTx1_BufferLoop_Buf[UCTSK_SPISLAVE_UART_TX1_BUFFERLOOP_BUF_MAX];
static COUNT_BUFFERLOOP_S_NODE UctskSpiSlave_UartTx1_BufferLoop_Node[UCTSK_SPISLAVE_UART_TX1_BUFFERLOOP_NODE_MAX];
static COUNT_BUFFERLOOP_S_LIMIT UctskSpiSlave_UartTx1_BufferLoop_Limit;
static COUNT_BUFFERLOOP_S UctskSpiSlave_UartTx1_BufferLoop;
#define UCTSK_SPISLAVE_UART_RX1_BUFFERLOOP_BUF_MAX   256
#define UCTSK_SPISLAVE_UART_RX1_BUFFERLOOP_NODE_MAX  10
static uint8_t UctskSpiSlave_UartRx1_BufferLoop_Buf[UCTSK_SPISLAVE_UART_RX1_BUFFERLOOP_BUF_MAX];
static COUNT_BUFFERLOOP_S_NODE UctskSpiSlave_UartRx1_BufferLoop_Node[UCTSK_SPISLAVE_UART_RX1_BUFFERLOOP_NODE_MAX];
static COUNT_BUFFERLOOP_S_LIMIT UctskSpiSlave_UartRx1_BufferLoop_Limit;
static COUNT_BUFFERLOOP_S UctskSpiSlave_UartRx1_BufferLoop;
#endif
#ifdef SPI_SLAVE_EXTEND_S_UART2_EN
#define UCTSK_SPISLAVE_UART_TX2_BUFFERLOOP_BUF_MAX   256
#define UCTSK_SPISLAVE_UART_TX2_BUFFERLOOP_NODE_MAX  10
static uint8_t UctskSpiSlave_UartTx2_BufferLoop_Buf[UCTSK_SPISLAVE_UART_TX2_BUFFERLOOP_BUF_MAX];
static COUNT_BUFFERLOOP_S_NODE UctskSpiSlave_UartTx2_BufferLoop_Node[UCTSK_SPISLAVE_UART_TX2_BUFFERLOOP_NODE_MAX];
static COUNT_BUFFERLOOP_S_LIMIT UctskSpiSlave_UartTx2_BufferLoop_Limit;
static COUNT_BUFFERLOOP_S UctskSpiSlave_UartTx2_BufferLoop;
#define UCTSK_SPISLAVE_UART_RX2_BUFFERLOOP_BUF_MAX   256
#define UCTSK_SPISLAVE_UART_RX2_BUFFERLOOP_NODE_MAX  10
static uint8_t UctskSpiSlave_UartRx2_BufferLoop_Buf[UCTSK_SPISLAVE_UART_RX2_BUFFERLOOP_BUF_MAX];
static COUNT_BUFFERLOOP_S_NODE UctskSpiSlave_UartRx2_BufferLoop_Node[UCTSK_SPISLAVE_UART_RX2_BUFFERLOOP_NODE_MAX];
static COUNT_BUFFERLOOP_S_LIMIT UctskSpiSlave_UartRx2_BufferLoop_Limit;
static COUNT_BUFFERLOOP_S UctskSpiSlave_UartRx2_BufferLoop;
#endif
#ifdef SPI_SLAVE_EXTEND_S_UART3_EN
#define UCTSK_SPISLAVE_UART_TX3_BUFFERLOOP_BUF_MAX   256
#define UCTSK_SPISLAVE_UART_TX3_BUFFERLOOP_NODE_MAX  10
static uint8_t UctskSpiSlave_UartTx3_BufferLoop_Buf[UCTSK_SPISLAVE_UART_TX3_BUFFERLOOP_BUF_MAX];
static COUNT_BUFFERLOOP_S_NODE UctskSpiSlave_UartTx3_BufferLoop_Node[UCTSK_SPISLAVE_UART_TX3_BUFFERLOOP_NODE_MAX];
static COUNT_BUFFERLOOP_S_LIMIT UctskSpiSlave_UartTx3_BufferLoop_Limit;
static COUNT_BUFFERLOOP_S UctskSpiSlave_UartTx3_BufferLoop;
#define UCTSK_SPISLAVE_UART_RX3_BUFFERLOOP_BUF_MAX   256
#define UCTSK_SPISLAVE_UART_RX3_BUFFERLOOP_NODE_MAX  10
static uint8_t UctskSpiSlave_UartRx3_BufferLoop_Buf[UCTSK_SPISLAVE_UART_RX3_BUFFERLOOP_BUF_MAX];
static COUNT_BUFFERLOOP_S_NODE UctskSpiSlave_UartRx3_BufferLoop_Node[UCTSK_SPISLAVE_UART_RX3_BUFFERLOOP_NODE_MAX];
static COUNT_BUFFERLOOP_S_LIMIT UctskSpiSlave_UartRx3_BufferLoop_Limit;
static COUNT_BUFFERLOOP_S UctskSpiSlave_UartRx3_BufferLoop;
#endif
#ifdef SPI_SLAVE_EXTEND_S_UART4_EN
#define UCTSK_SPISLAVE_UART_TX4_BUFFERLOOP_BUF_MAX   256
#define UCTSK_SPISLAVE_UART_TX4_BUFFERLOOP_NODE_MAX  10
static uint8_t UctskSpiSlave_UartTx4_BufferLoop_Buf[UCTSK_SPISLAVE_UART_TX4_BUFFERLOOP_BUF_MAX];
static COUNT_BUFFERLOOP_S_NODE UctskSpiSlave_UartTx4_BufferLoop_Node[UCTSK_SPISLAVE_UART_TX4_BUFFERLOOP_NODE_MAX];
static COUNT_BUFFERLOOP_S_LIMIT UctskSpiSlave_UartTx4_BufferLoop_Limit;
static COUNT_BUFFERLOOP_S UctskSpiSlave_UartTx4_BufferLoop;
#define UCTSK_SPISLAVE_UART_RX4_BUFFERLOOP_BUF_MAX   256
#define UCTSK_SPISLAVE_UART_RX4_BUFFERLOOP_NODE_MAX  10
static uint8_t UctskSpiSlave_UartRx4_BufferLoop_Buf[UCTSK_SPISLAVE_UART_RX4_BUFFERLOOP_BUF_MAX];
static COUNT_BUFFERLOOP_S_NODE UctskSpiSlave_UartRx4_BufferLoop_Node[UCTSK_SPISLAVE_UART_RX4_BUFFERLOOP_NODE_MAX];
static COUNT_BUFFERLOOP_S_LIMIT UctskSpiSlave_UartRx4_BufferLoop_Limit;
static COUNT_BUFFERLOOP_S UctskSpiSlave_UartRx4_BufferLoop;
#endif
#ifdef SPI_SLAVE_EXTEND_S_UART5_EN
#define UCTSK_SPISLAVE_UART_TX5_BUFFERLOOP_BUF_MAX   256
#define UCTSK_SPISLAVE_UART_TX5_BUFFERLOOP_NODE_MAX  10
static uint8_t UctskSpiSlave_UartTx5_BufferLoop_Buf[UCTSK_SPISLAVE_UART_TX5_BUFFERLOOP_BUF_MAX];
static COUNT_BUFFERLOOP_S_NODE UctskSpiSlave_UartTx5_BufferLoop_Node[UCTSK_SPISLAVE_UART_TX5_BUFFERLOOP_NODE_MAX];
static COUNT_BUFFERLOOP_S_LIMIT UctskSpiSlave_UartTx5_BufferLoop_Limit;
static COUNT_BUFFERLOOP_S UctskSpiSlave_UartTx5_BufferLoop;
#define UCTSK_SPISLAVE_UART_RX5_BUFFERLOOP_BUF_MAX   256
#define UCTSK_SPISLAVE_UART_RX5_BUFFERLOOP_NODE_MAX  10
static uint8_t UctskSpiSlave_UartRx5_BufferLoop_Buf[UCTSK_SPISLAVE_UART_RX5_BUFFERLOOP_BUF_MAX];
static COUNT_BUFFERLOOP_S_NODE UctskSpiSlave_UartRx5_BufferLoop_Node[UCTSK_SPISLAVE_UART_RX5_BUFFERLOOP_NODE_MAX];
static COUNT_BUFFERLOOP_S_LIMIT UctskSpiSlave_UartRx5_BufferLoop_Limit;
static COUNT_BUFFERLOOP_S UctskSpiSlave_UartRx5_BufferLoop;
#endif
//----------------------------------------------------------------------------���غ���
static void Spi_SlaveInit(void);
static void SpiSlave_M_Irq(uint8_t *pbuf,uint16_t len);
static void Spi_SlaveBufferloopPush(uint16_t cmd,uint8_t *pbuf,uint16_t len);
static void uctsk_Spi (void);
/*******************************************************************************
��������: ��Ϣ����ʵ��
��    ��:
�� �� ֵ:
*******************************************************************************/
//��ʼ����Ϣʵ��
static void Spi_SlaveInitQ(void)
{
    memset((char*)&Spi_Slave_S_QBuf[0],0,sizeof(Spi_Slave_S_QBuf));
}
//��ȡ������Ϣͷ��ַ
static SPI_SLAVE_S_Q *Spi_SlaveGetQ()
{
    uint8_t i;
    SPI_SLAVE_S_Q *pQ;
    for(i=0; i<SPI_SLAVE_QBUF_MAX; i++)
    {
        if(Spi_Slave_S_QBuf[i].lock==0)
        {
            Spi_Slave_S_QBuf[i].lock=1;
            pQ=&Spi_Slave_S_QBuf[i];
            break;
        }
    }
    if(i==SPI_SLAVE_QBUF_MAX)
    {
        pQ=NULL;
    }
    return pQ;
}
//�ͷ���Ϣʵ��
static void Spi_SlaveFreeQ(SPI_SLAVE_S_Q *pQ)
{
    memset((char*)pQ,0,sizeof(SPI_SLAVE_S_Q));
}
/*******************************************************************************
* ��������: ���ݴ�����Ϣ��,��������Ϣ������
* ��    ��:
* Return  :
*******************************************************************************/
static void Spi_SlaveBufferloopPush(uint16_t cmd,uint8_t *pbuf,uint16_t len)
{
    SPI_SLAVE_S_Q *pQ;
    switch(cmd)
    {
#ifdef SPI_SLAVE_EXTEND_S_UART1_EN
        case SPI_E_SLAVE_CMD_UART1_R:
            Count_BufferLoopPush(&UctskSpiSlave_UartRx1_BufferLoop,pbuf,len);
        case COUNT_MODBUS_REGISTERS_ADDR_UART1:
            Count_BufferLoopPush(&UctskSpiSlave_UartTx1_BufferLoop,pbuf,len);
#endif
#ifdef SPI_SLAVE_EXTEND_S_UART2_EN
        case SPI_E_SLAVE_CMD_UART2_R:
            Count_BufferLoopPush(&UctskSpiSlave_UartRx2_BufferLoop,pbuf,len);
        case COUNT_MODBUS_REGISTERS_ADDR_UART2:
            Count_BufferLoopPush(&UctskSpiSlave_UartTx2_BufferLoop,pbuf,len);
#endif
#ifdef SPI_SLAVE_EXTEND_S_UART3_EN
        case SPI_E_SLAVE_CMD_UART3_R:
            Count_BufferLoopPush(&UctskSpiSlave_UartRx3_BufferLoop,pbuf,len);
        case COUNT_MODBUS_REGISTERS_ADDR_UART3:
            Count_BufferLoopPush(&UctskSpiSlave_UartTx3_BufferLoop,pbuf,len);
#endif
#ifdef SPI_SLAVE_EXTEND_S_UART4_EN
        case SPI_E_SLAVE_CMD_UART4_R:
            Count_BufferLoopPush(&UctskSpiSlave_UartRx4_BufferLoop,pbuf,len);
        case COUNT_MODBUS_REGISTERS_ADDR_UART4:
            Count_BufferLoopPush(&UctskSpiSlave_UartTx4_BufferLoop,pbuf,len);
#endif
#ifdef SPI_SLAVE_EXTEND_S_UART5_EN
        case SPI_E_SLAVE_CMD_UART5_R:
            Count_BufferLoopPush(&UctskSpiSlave_UartRx5_BufferLoop,pbuf,len);
        case COUNT_MODBUS_REGISTERS_ADDR_UART5:
            Count_BufferLoopPush(&UctskSpiSlave_UartTx5_BufferLoop,pbuf,len);
#endif
#ifdef SPI_SLAVE_EXTEND_S_UARTX_EN
            pQ=Spi_SlaveGetQ();
            if(pQ!=NULL)
            {
                pQ->cmd=cmd;
                MODULE_OS_Q_POST(SpiSlave_pQ,*pQ,sizeof(SPI_SLAVE_S_Q));
            }
            break;
#endif
        default:
            break;
    }
}
static void SpiSlave_M_Irq(uint8_t *pbuf,uint16_t len)
{
    SPI_SLAVE_S_Q *pQ;
    pbuf=pbuf;
    if(len>0x00FF)
    {
        Spi_SlaveRxLen=0;
        return;
    }
    //���ݸ���
    memcpy(Spi_SlaveRxBuf,pbuf,len);
    Spi_SlaveRxLen=len;
    //���ͽ�����Ϣ
    pQ=Spi_SlaveGetQ();
    if(pQ!=NULL)
    {
        pQ->cmd=SPI_E_SLAVE_CMD_M_PARSE;
        MODULE_OS_Q_POST(SpiSlave_pQ,*pQ,sizeof(SPI_SLAVE_S_Q));
    }
}
void SpiSlave_UartIrq_Rx1(uint8_t *pbuf,uint16_t len)
{
#ifdef SPI_SLAVE_EXTEND_M_UART1_EN
    SpiSlave_M_Irq(pbuf,len);
#endif
#ifdef SPI_SLAVE_EXTEND_S_UART1_EN
    Spi_SlaveBufferloopPush(SPI_E_SLAVE_CMD_UART1_R,pbuf,len);
#endif
}
void SpiSlave_UartIrq_Rx2(uint8_t *pbuf,uint16_t len)
{
#ifdef SPI_SLAVE_EXTEND_M_UART2_EN
    SpiSlave_M_Irq(pbuf,len);
#endif
#ifdef SPI_SLAVE_EXTEND_S_UART2_EN
    Spi_SlaveBufferloopPush(SPI_E_SLAVE_CMD_UART2_R,pbuf,len);
#endif
}
void SpiSlave_UartIrq_Rx3(uint8_t *pbuf,uint16_t len)
{
#ifdef SPI_SLAVE_EXTEND_M_UART3_EN
    SpiSlave_M_Irq(pbuf,len);
#endif
#ifdef SPI_SLAVE_EXTEND_S_UART3_EN
    Spi_SlaveBufferloopPush(SPI_E_SLAVE_CMD_UART3_R,pbuf,len);
#endif
}
void SpiSlave_UartIrq_Rx4(uint8_t *pbuf,uint16_t len)
{
#ifdef SPI_SLAVE_EXTEND_M_UART4_EN
    SpiSlave_M_Irq(pbuf,len);
#endif
#ifdef SPI_SLAVE_EXTEND_S_UART4_EN
    Spi_SlaveBufferloopPush(SPI_E_SLAVE_CMD_UART4_R,pbuf,len);
#endif
}
void SpiSlave_UartIrq_Rx5(uint8_t *pbuf,uint16_t len)
{
#ifdef SPI_SLAVE_EXTEND_M_UART5_EN
    SpiSlave_M_Irq(pbuf,len);
#endif
#ifdef SPI_SLAVE_EXTEND_S_UART5_EN
    Spi_SlaveBufferloopPush(SPI_E_SLAVE_CMD_UART5_R,pbuf,len);
#endif
}
/*******************************************************************************
��������: SPI�ӻ���ʼ��
��    ��:
�� �� ֵ:
*******************************************************************************/
static void Spi_SlaveInit(void)
{
    //GPIO
    SPI_InitTypeDef  SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    DMA_InitTypeDef  DMA_InitStructure;
    //
    memset((char*)&SPI_InitStructure,0,sizeof(SPI_InitTypeDef));
    memset((char*)&GPIO_InitStructure,0,sizeof(GPIO_InitTypeDef));
    memset((char*)&NVIC_InitStructure,0,sizeof(NVIC_InitTypeDef));
    memset((char*)&DMA_InitStructure,0,sizeof(DMA_InitTypeDef));
    //ʱ������
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    //-------------���˿ڳ�ʼ������
#ifdef SPI_SLAVE_EXTEND_M_SPI_EN
    //
    SPI_Cmd(SPI_SLAVE_SPIx, DISABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
    //����JTAG��������(ע��: ������������������ʹ,��֪ԭ��)
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
    //CLK MOSI
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    //MISO
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    //CS(�ж�)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //INT(���������,STM32F1ϵ����������,����Ϊ���缫��·)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    //DMA
    RCC_AHBPeriphClockCmd( RCC_AHBPeriph_DMA2,ENABLE);
    //����ͨ��
    {
        //�ж�����
        NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel2_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
        //
        DMA_Cmd(DMA2_Channel2, DISABLE);
        //
        DMA_DeInit(DMA2_Channel2);
        DMA_InitStructure.DMA_PeripheralBaseAddr =(u32)&SPI_SLAVE_SPIx->DR;
        DMA_InitStructure.DMA_MemoryBaseAddr = (u32)Spi_SlaveTxBuf;
        DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
        DMA_InitStructure.DMA_BufferSize = SPI_SLAVE_TXBUF_MAX;
        DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
        DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
        DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
        DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
        DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
        DMA_InitStructure.DMA_Priority = DMA_Priority_High;
        DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
        DMA_Init(DMA2_Channel2, &DMA_InitStructure);
        DMA_Cmd(DMA2_Channel2,ENABLE);
        SPI_I2S_DMACmd(SPI_SLAVE_SPIx, SPI_I2S_DMAReq_Tx, ENABLE);
    }
    //����ͨ��
    {
        //�ж�����
        NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel1_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
        //
        DMA_Cmd(DMA2_Channel1, DISABLE);
        //
        DMA_DeInit(DMA2_Channel1);
        DMA_InitStructure.DMA_PeripheralBaseAddr =(u32)&SPI_SLAVE_SPIx->DR;
        DMA_InitStructure.DMA_MemoryBaseAddr = (u32)Spi_SlaveRxBuf;
        DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
        DMA_InitStructure.DMA_BufferSize = SPI_SLAVE_RXBUF_MAX;
        DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
        DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
        DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
        DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
        DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
        DMA_InitStructure.DMA_Priority = DMA_Priority_High;
        DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
        DMA_Init(DMA2_Channel1, &DMA_InitStructure);
        DMA_Cmd(DMA2_Channel1,ENABLE);
        SPI_I2S_DMACmd(SPI_SLAVE_SPIx, SPI_I2S_DMAReq_Rx, ENABLE);
    }
    //-----XSL-----����---ʹ���ж�
    SPI_I2S_ClearITPendingBit(SPI_SLAVE_SPIx, SPI_I2S_IT_RXNE);
    //-------------
    SPI_I2S_ITConfig(SPI_SLAVE_SPIx, SPI_I2S_IT_RXNE, ENABLE);
    //SPI2 Config
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    //ʱ�Ӽ���(SPI_CPOL_Low�͵�ƽ����  SPI_CPOL_High�ߵ�ƽ����)
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    //ʱ�Ӳ���(SPI_CPHA_1Edge��һ�������ز���,SPI_CPHA_2Edge�ڶ��������ز���)
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_CalculateCRC( SPI_SLAVE_SPIx,  DISABLE);
    SPI_Init(SPI_SLAVE_SPIx, &SPI_InitStructure);
    SPI_Cmd(SPI_SLAVE_SPIx, ENABLE);
#endif
#ifdef SPI_SLAVE_EXTEND_M_UART1_EN
    UART_INIT(1,115200);
#endif
    //-------------�Ӷ˿ڳ�ʼ������
    if(1)
    {
        MODULE_MEMORY_S_PARA *pPara;
        uint8_t i,res;
        //���뻺��
        pPara=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        //��ȡ����
        Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_R,(uint8_t*)pPara,NULL);
        //װ�ز���
        Count_ModbusSlaveAddr=pPara->Addr;
        Count_ModbusSlaveAddr=1;
        //
        res=OK;
        for(i=0; i<8; i++)
        {
            if(pPara->UartBps[i]>8)
            {
                pPara->UartBps[i]=8;
                res=ERR;
            }
        }
        if(res==ERR)
        {
            Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_W,(uint8_t*)pPara,NULL);
        }
        //�ͷŻ���
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pPara);
    }
    //
#ifdef SPI_SLAVE_EXTEND_S_UART1_EN
    UART_INIT(1,UCTSK_SPISLAVE_UART_BSP[ModuleMemory_psPara->UartBps[0]]);
#endif
#ifdef SPI_SLAVE_EXTEND_S_UART2_EN
    //UART_INIT(2,UCTSK_SPISLAVE_UART_BSP[ModuleMemory_psPara->UartBps[1]]);
    UART_INIT(2,115200);
#endif
#ifdef SPI_SLAVE_EXTEND_S_UART3_EN
    UART_INIT(3,UCTSK_SPISLAVE_UART_BSP[ModuleMemory_psPara->UartBps[2]]);
#endif

#ifdef SPI_SLAVE_EXTEND_S_UART4_EN
    UART_INIT(4,UCTSK_SPISLAVE_UART_BSP[ModuleMemory_psPara->UartBps[3]]);
#endif
#ifdef SPI_SLAVE_EXTEND_S_UART5_EN
    UART_INIT(5,UCTSK_SPISLAVE_UART_BSP[ModuleMemory_psPara->UartBps[4]]);
#endif
}
/*******************************************************************************
��������: ���ݽ���
��    ��:
�� �� ֵ:
*******************************************************************************/
static void Spi_SlaveRxParsePro(uint8_t *pBuf,uint16_t len)
{
    uint8_t i,num;
    uint8_t *pbuf1;
    //uint8_t res;
    //
    uint8_t cmd;
    uint16_t len16=0,i16;
    uint16_t reg_Addr;
    uint16_t reg_WordLen;
    uint8_t reg_ByteLen;
    uint8_t *preg;
    uint16_t *pi16;
    if(len==0)
    {
        return;
    }
    cmd=pBuf[1];
    switch(cmd)
    {
        case COUNT_MODBUS_FC_READ_COILS:
            break;
        case  COUNT_MODBUS_FC_READ_DISCRETE_INPUTS:
            break;
        case  COUNT_MODBUS_FC_READ_HOLDING_REGISTERS:
            //�Ĵ�����ַ
            reg_Addr=pBuf[3];
            reg_Addr<<=8;
            reg_Addr+=pBuf[2];
            //�Ĵ�������
            reg_WordLen=pBuf[5];
            reg_WordLen<<=8;
            reg_WordLen+=pBuf[4];
            switch(reg_Addr)
            {
                case COUNT_MODBUS_REGISTERS_ADDR_UART_PARA:
                    reg_ByteLen=reg_WordLen*2;
                    Count_Modbus_Array(Spi_SlaveTxBuf,&len16,Count_ModbusSlaveAddr,cmd,NULL,NULL,&reg_ByteLen,(uint8_t *)&(ModuleMemory_psPara->UartBps[0]),reg_ByteLen);
                    Spi_SlaveErrCounter=0;
                    break;
                case COUNT_MODBUS_REGISTERS_ADDR_STATE:
                    //���뻺��
                    pbuf1=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                    //׼������
                    memcpy((char*)pbuf1,(char*)&SPI_Slave_S_State,sizeof(SPI_Slave_S_State));
                    reg_ByteLen=COUNT_MODBUS_REGISTERS_LEN_STATE;
                    Count_Modbus_Array(Spi_SlaveTxBuf,&len16,Count_ModbusSlaveAddr,cmd,NULL,NULL,&reg_ByteLen,pbuf1,reg_ByteLen);
                    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf1);
                    //
                    Spi_SlaveErrCounter=0;
                    break;
                case COUNT_MODBUS_REGISTERS_ADDR_PARA:
                    //���뻺��
                    pbuf1=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                    //׼������
                    Bsp_CpuFlash_Read(BSP_CPU_FLASH_PARA_ADDR,pbuf1,sizeof(MODULE_MEMORY_S_PARA));
                    reg_ByteLen=COUNT_MODBUS_REGISTERS_LEN_PARA;
                    Count_Modbus_Array(Spi_SlaveTxBuf,&len16,Count_ModbusSlaveAddr,cmd,NULL,NULL,&reg_ByteLen,pbuf1,reg_ByteLen);
                    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf1);
                    //
                    Spi_SlaveErrCounter=0;
                    break;
#ifdef SPI_SLAVE_EXTEND_S_UART1_EN
                case COUNT_MODBUS_REGISTERS_ADDR_UART1_AUTO_R:
                    //׼������
                    i16=0x1234;
                    reg_ByteLen=2;
                    Count_Modbus_Array(Spi_SlaveTxBuf,&len16,Count_ModbusSlaveAddr,cmd,NULL,NULL,&reg_ByteLen,(uint8_t*)&i16,reg_ByteLen);
                    break;
                case COUNT_MODBUS_REGISTERS_ADDR_UART1:
                    num=(reg_Addr-COUNT_MODBUS_REGISTERS_ADDR_UART1)/COUNT_MODBUS_REGISTERS_SIZE_UART;
                    //���뻺��
                    pbuf1=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                    pi16=(uint16_t*)pbuf1;
                    len=0;
                    if(0!=SPI_Slave_S_State.UartRxNum[num])
                    {
                        Count_BufferLoopPop(&UctskSpiSlave_UartRx1_BufferLoop,&pbuf1[2],&len,COUNT_BUFFERLOOP_E_POPMODE_DELETE);
                        SPI_Slave_S_State.UartRxNum[num]--;
                        Count_BufferLoopPop(&UctskSpiSlave_UartRx1_BufferLoop,NULL,&i16,COUNT_BUFFERLOOP_E_POPMODE_KEEP);
                        SPI_Slave_S_State.UartRxSize[num]=i16;
                        //�Ĵ����ֽ���
                        *pi16=len;
                        //���������ֽ���
                        len+=2;
                        reg_ByteLen=len;
                    }
                    Count_Modbus_Array(Spi_SlaveTxBuf,&len16,Count_ModbusSlaveAddr,cmd,NULL,NULL,&reg_ByteLen,pbuf1,reg_ByteLen);
                    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf1);
                    break;
#endif
#ifdef SPI_SLAVE_EXTEND_S_UART2_EN
                case COUNT_MODBUS_REGISTERS_ADDR_UART2_AUTO_R:
                    //׼������
                    i16=0x1234;
                    reg_ByteLen=2;
                    Count_Modbus_Array(Spi_SlaveTxBuf,&len16,Count_ModbusSlaveAddr,cmd,NULL,NULL,&reg_ByteLen,(uint8_t*)&i16,reg_ByteLen);
                    break;
                case COUNT_MODBUS_REGISTERS_ADDR_UART2:
                    num=(reg_Addr-COUNT_MODBUS_REGISTERS_ADDR_UART1)/COUNT_MODBUS_REGISTERS_SIZE_UART;
                    //���뻺��
                    pbuf1=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                    pi16=(uint16_t*)pbuf1;
                    len=0;
                    if(0!=SPI_Slave_S_State.UartRxNum[num])
                    {
                        Count_BufferLoopPop(&UctskSpiSlave_UartRx2_BufferLoop,&pbuf1[2],&len,COUNT_BUFFERLOOP_E_POPMODE_DELETE);
                        SPI_Slave_S_State.UartRxNum[num]--;
                        Count_BufferLoopPop(&UctskSpiSlave_UartRx2_BufferLoop,NULL,&i16,COUNT_BUFFERLOOP_E_POPMODE_KEEP);
                        SPI_Slave_S_State.UartRxSize[num]=i16;
                        //�Ĵ����ֽ���
                        *pi16=len;
                        //���������ֽ���
                        len+=2;
                        reg_ByteLen=len;
                    }
                    Count_Modbus_Array(Spi_SlaveTxBuf,&len16,Count_ModbusSlaveAddr,cmd,NULL,NULL,&reg_ByteLen,pbuf1,reg_ByteLen);
                    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf1);
                    break;
#endif
#ifdef SPI_SLAVE_EXTEND_S_UART3_EN
                case COUNT_MODBUS_REGISTERS_ADDR_UART3_AUTO_R:
                    //׼������
                    i16=0x1234;
                    reg_ByteLen=2;
                    Count_Modbus_Array(Spi_SlaveTxBuf,&len16,Count_ModbusSlaveAddr,cmd,NULL,NULL,&reg_ByteLen,(uint8_t*)&i16,reg_ByteLen);
                    break;
                case COUNT_MODBUS_REGISTERS_ADDR_UART3:
                    num=(reg_Addr-COUNT_MODBUS_REGISTERS_ADDR_UART1)/COUNT_MODBUS_REGISTERS_SIZE_UART;
                    //���뻺��
                    pbuf1=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                    pi16=(uint16_t*)pbuf1;
                    len=0;
                    if(0!=SPI_Slave_S_State.UartRxNum[num])
                    {
                        Count_BufferLoopPop(&UctskSpiSlave_UartRx3_BufferLoop,&pbuf1[2],&len,COUNT_BUFFERLOOP_E_POPMODE_DELETE);
                        SPI_Slave_S_State.UartRxNum[num]--;
                        Count_BufferLoopPop(&UctskSpiSlave_UartRx3_BufferLoop,NULL,&i16,COUNT_BUFFERLOOP_E_POPMODE_KEEP);
                        SPI_Slave_S_State.UartRxSize[num]=i16;
                        //�Ĵ����ֽ���
                        *pi16=len;
                        //���������ֽ���
                        len+=2;
                        reg_ByteLen=len;
                    }
                    Count_Modbus_Array(Spi_SlaveTxBuf,&len16,Count_ModbusSlaveAddr,cmd,NULL,NULL,&reg_ByteLen,pbuf1,reg_ByteLen);
                    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf1);
                    break;
#endif
#ifdef SPI_SLAVE_EXTEND_S_UART4_EN
                case COUNT_MODBUS_REGISTERS_ADDR_UART4_AUTO_R:
                    //׼������
                    i16=0x1234;
                    reg_ByteLen=2;
                    Count_Modbus_Array(Spi_SlaveTxBuf,&len16,Count_ModbusSlaveAddr,cmd,NULL,NULL,&reg_ByteLen,(uint8_t*)&i16,reg_ByteLen);
                    break;
                case COUNT_MODBUS_REGISTERS_ADDR_UART4:
                    num=(reg_Addr-COUNT_MODBUS_REGISTERS_ADDR_UART1)/COUNT_MODBUS_REGISTERS_SIZE_UART;
                    //���뻺��
                    pbuf1=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                    pi16=(uint16_t*)pbuf1;
                    len=0;
                    if(0!=SPI_Slave_S_State.UartRxNum[num])
                    {
                        Count_BufferLoopPop(&UctskSpiSlave_UartRx4_BufferLoop,&pbuf1[2],&len,COUNT_BUFFERLOOP_E_POPMODE_DELETE);
                        SPI_Slave_S_State.UartRxNum[num]--;
                        Count_BufferLoopPop(&UctskSpiSlave_UartRx4_BufferLoop,NULL,&i16,COUNT_BUFFERLOOP_E_POPMODE_KEEP);
                        SPI_Slave_S_State.UartRxSize[num]=i16;
                        //�Ĵ����ֽ���
                        *pi16=len;
                        //���������ֽ���
                        len+=2;
                        reg_ByteLen=len;
                    }
                    Count_Modbus_Array(Spi_SlaveTxBuf,&len16,Count_ModbusSlaveAddr,cmd,NULL,NULL,&reg_ByteLen,pbuf1,reg_ByteLen);
                    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf1);
                    break;
#endif
#ifdef SPI_SLAVE_EXTEND_S_UART5_EN
                case COUNT_MODBUS_REGISTERS_ADDR_UART5_AUTO_R:
                    //׼������
                    i16=0x1234;
                    reg_ByteLen=2;
                    Count_Modbus_Array(Spi_SlaveTxBuf,&len16,Count_ModbusSlaveAddr,cmd,NULL,NULL,&reg_ByteLen,(uint8_t*)&i16,reg_ByteLen);
                    break;
                case COUNT_MODBUS_REGISTERS_ADDR_UART5:
                    num=(reg_Addr-COUNT_MODBUS_REGISTERS_ADDR_UART1)/COUNT_MODBUS_REGISTERS_SIZE_UART;
                    //���뻺��
                    pbuf1=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                    pi16=(uint16_t*)pbuf1;
                    len=0;
                    if(0!=SPI_Slave_S_State.UartRxNum[num])
                    {
                        Count_BufferLoopPop(&UctskSpiSlave_UartRx5_BufferLoop,&pbuf1[2],&len,COUNT_BUFFERLOOP_E_POPMODE_DELETE);
                        SPI_Slave_S_State.UartRxNum[num]--;
                        Count_BufferLoopPop(&UctskSpiSlave_UartRx5_BufferLoop,NULL,&i16,COUNT_BUFFERLOOP_E_POPMODE_KEEP);
                        SPI_Slave_S_State.UartRxSize[num]=i16;
                        //�Ĵ����ֽ���
                        *pi16=len;
                        //���������ֽ���
                        len+=2;
                        reg_ByteLen=len;
                    }
                    Count_Modbus_Array(Spi_SlaveTxBuf,&len16,Count_ModbusSlaveAddr,cmd,NULL,NULL,&reg_ByteLen,pbuf1,reg_ByteLen);
                    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf1);
                    break;
#endif
#ifdef SPI_SLAVE_EXTEND_S_RTC_EN
                case COUNT_MODBUS_REGISTERS_ADDR_RTC:
                    //���뻺��
                    pbuf1=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                    //׼������
                    BspRtc_ReadRealTime(NULL,(struct tm *)pbuf1,NULL,NULL);
                    reg_ByteLen=COUNT_MODBUS_REGISTERS_LEN_RTC;
                    Count_Modbus_Array(Spi_SlaveTxBuf,&len16,Count_ModbusSlaveAddr,cmd,NULL,NULL,&reg_ByteLen,pbuf1,reg_ByteLen);
                    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf1);
                    break;
#endif
#ifdef SPI_SLAVE_EXTEND_S_AD_EN
                case COUNT_MODBUS_REGISTERS_ADDR_AD:
                    //���뻺��
                    pbuf1=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                    //׼������
                    for(i=0; i<(ADC_CHANNEL_NUM-1); i++)
                    {
                        pbuf1[2*i]=ADC_mV[i]>>8;
                        pbuf1[1+2*i]=ADC_mV[i];
                    }
                    reg_ByteLen=COUNT_MODBUS_REGISTERS_LEN_AD;
                    Count_Modbus_Array(Spi_SlaveTxBuf,&len16,Count_ModbusSlaveAddr,cmd,NULL,NULL,&reg_ByteLen,pbuf1,reg_ByteLen);
                    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf1);
                    break;
#endif
#ifdef SPI_SLAVE_EXTEND_S_KIN_EN
                case COUNT_MODBUS_REGISTERS_ADDR_KIN:
                    //���뻺��
                    pbuf1=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                    //׼������
                    pbuf1[0]=0;
                    if(BspKey_RealValueBuf[0])
                        pbuf1[0]|=0x01;
                    if(BspKey_RealValueBuf[1])
                        pbuf1[0]|=0x02;
                    pbuf1[1]=0;
                    reg_ByteLen=COUNT_MODBUS_REGISTERS_LEN_KIN;
                    Count_Modbus_Array(Spi_SlaveTxBuf,&len16,Count_ModbusSlaveAddr,cmd,NULL,NULL,&reg_ByteLen,pbuf1,reg_ByteLen);
                    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf1);
                    break;
#endif
#ifdef SPI_SLAVE_EXTEND_S_KOUT_EN
                case COUNT_MODBUS_REGISTERS_ADDR_KOUT:
                    //׼������
                    i16=BspRelay_Read();
                    reg_ByteLen=COUNT_MODBUS_REGISTERS_LEN_KOUT;
                    Count_Modbus_Array(Spi_SlaveTxBuf,&len16,Count_ModbusSlaveAddr,cmd,NULL,NULL,&reg_ByteLen,(uint8_t*)&i16,reg_ByteLen);
                    break;
#endif
                default:
                    //������
                    if((reg_Addr>=COUNT_MODBUS_REGISTERS_ADDR_UART1_AUTO_R)&&(reg_Addr<(COUNT_MODBUS_REGISTERS_ADDR_UART1_AUTO_R+COUNT_MODBUS_REGISTERS_SIZE_UARTX_AUTO_R)))
                    {
                        //�����ֽڳ���
                        reg_ByteLen=reg_WordLen*2;
                        Count_Modbus_Array(Spi_SlaveTxBuf,&len16,Count_ModbusSlaveAddr,cmd,NULL,NULL,&reg_ByteLen,(uint8_t*)&Spi_SlaveAutoRxUartBuf[0][reg_Addr-COUNT_MODBUS_REGISTERS_ADDR_UART2_AUTO_R],reg_ByteLen);
                    }
                    else if((reg_Addr>=COUNT_MODBUS_REGISTERS_ADDR_UART2_AUTO_R)&&(reg_Addr<(COUNT_MODBUS_REGISTERS_ADDR_UART2_AUTO_R+COUNT_MODBUS_REGISTERS_SIZE_UARTX_AUTO_R)))
                    {
                        //�����ֽڳ���
                        reg_ByteLen=reg_WordLen*2;
                        Count_Modbus_Array(Spi_SlaveTxBuf,&len16,Count_ModbusSlaveAddr,cmd,NULL,NULL,&reg_ByteLen,(uint8_t*)&Spi_SlaveAutoRxUartBuf[1][reg_Addr-COUNT_MODBUS_REGISTERS_ADDR_UART2_AUTO_R],reg_ByteLen);
                    }
                    else if((reg_Addr>=COUNT_MODBUS_REGISTERS_ADDR_UART3_AUTO_R)&&(reg_Addr<(COUNT_MODBUS_REGISTERS_ADDR_UART3_AUTO_R+COUNT_MODBUS_REGISTERS_SIZE_UARTX_AUTO_R)))
                    {
                        //�����ֽڳ���
                        reg_ByteLen=reg_WordLen*2;
                        Count_Modbus_Array(Spi_SlaveTxBuf,&len16,Count_ModbusSlaveAddr,cmd,NULL,NULL,&reg_ByteLen,(uint8_t*)&Spi_SlaveAutoRxUartBuf[2][reg_Addr-COUNT_MODBUS_REGISTERS_ADDR_UART3_AUTO_R],reg_ByteLen);
                    }
                    else if((reg_Addr>=COUNT_MODBUS_REGISTERS_ADDR_UART4_AUTO_R)&&(reg_Addr<(COUNT_MODBUS_REGISTERS_ADDR_UART4_AUTO_R+COUNT_MODBUS_REGISTERS_SIZE_UARTX_AUTO_R)))
                    {
                        //�����ֽڳ���
                        reg_ByteLen=reg_WordLen*2;
                        Count_Modbus_Array(Spi_SlaveTxBuf,&len16,Count_ModbusSlaveAddr,cmd,NULL,NULL,&reg_ByteLen,(uint8_t*)&Spi_SlaveAutoRxUartBuf[3][reg_Addr-COUNT_MODBUS_REGISTERS_ADDR_UART4_AUTO_R],reg_ByteLen);
                    }
                    else if((reg_Addr>=COUNT_MODBUS_REGISTERS_ADDR_UART5_AUTO_R)&&(reg_Addr<(COUNT_MODBUS_REGISTERS_ADDR_UART5_AUTO_R+COUNT_MODBUS_REGISTERS_SIZE_UARTX_AUTO_R)))
                    {
                        //�����ֽڳ���
                        reg_ByteLen=reg_WordLen*2;
                        Count_Modbus_Array(Spi_SlaveTxBuf,&len16,Count_ModbusSlaveAddr,cmd,NULL,NULL,&reg_ByteLen,(uint8_t*)&Spi_SlaveAutoRxUartBuf[4][reg_Addr-COUNT_MODBUS_REGISTERS_ADDR_UART5_AUTO_R],reg_ByteLen);
                    }
                    //������
                    else if((reg_Addr>=COUNT_MODBUS_REGISTERS_ADDR_UART1_AUTO_W)&&(reg_Addr<(COUNT_MODBUS_REGISTERS_ADDR_UART1_AUTO_W+COUNT_MODBUS_REGISTERS_SIZE_UARTX_AUTO_W)))
                    {
                        //���뻺��
                        pbuf1=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                        //ִ��д����
                        reg_ByteLen=reg_WordLen*2;
                        for(i16=0; i16<reg_ByteLen; i16+=sizeof(S_COUNT_MODBUS_UART_AUTO_W))
                        {
                            //Bsp_CpuFlash_Read(IAP_ADDR_UART1_AUTO_TX+2*(reg_Addr-COUNT_MODBUS_REGISTERS_ADDR_UART1_AUTO_W)+i16,&pbuf1[i16],sizeof(S_COUNT_MODBUS_UART_AUTO_W));
                        }
                        Count_Modbus_Array(Spi_SlaveTxBuf,&len16,Count_ModbusSlaveAddr,cmd,NULL,NULL,&reg_ByteLen,pbuf1,reg_ByteLen);
                        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf1);
                    }
                    else if((reg_Addr>=COUNT_MODBUS_REGISTERS_ADDR_UART2_AUTO_W)&&(reg_Addr<(COUNT_MODBUS_REGISTERS_ADDR_UART2_AUTO_W+COUNT_MODBUS_REGISTERS_SIZE_UARTX_AUTO_W)))
                    {
                        //���뻺��
                        pbuf1=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                        //ִ��д����
                        reg_ByteLen=reg_WordLen*2;
                        for(i16=0; i16<reg_ByteLen; i16+=sizeof(S_COUNT_MODBUS_UART_AUTO_W))
                        {
                            //Bsp_CpuFlash_Read(IAP_ADDR_UART2_AUTO_TX+2*(reg_Addr-COUNT_MODBUS_REGISTERS_ADDR_UART2_AUTO_W)+i16,&pbuf1[i16],sizeof(S_COUNT_MODBUS_UART_AUTO_W));
                        }
                        Count_Modbus_Array(Spi_SlaveTxBuf,&len16,Count_ModbusSlaveAddr,cmd,NULL,NULL,&reg_ByteLen,pbuf1,reg_ByteLen);
                        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf1);
                    }
                    else if((reg_Addr>=COUNT_MODBUS_REGISTERS_ADDR_UART3_AUTO_W)&&(reg_Addr<(COUNT_MODBUS_REGISTERS_ADDR_UART3_AUTO_W+COUNT_MODBUS_REGISTERS_SIZE_UARTX_AUTO_W)))
                    {
                        //���뻺��
                        pbuf1=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                        //ִ��д����
                        reg_ByteLen=reg_WordLen*2;
                        for(i16=0; i16<reg_ByteLen; i16+=sizeof(S_COUNT_MODBUS_UART_AUTO_W))
                        {
                            //Bsp_CpuFlash_Read(IAP_ADDR_UART3_AUTO_TX+2*(reg_Addr-COUNT_MODBUS_REGISTERS_ADDR_UART3_AUTO_W)+i16,&pbuf1[i16],sizeof(S_COUNT_MODBUS_UART_AUTO_W));
                        }
                        Count_Modbus_Array(Spi_SlaveTxBuf,&len16,Count_ModbusSlaveAddr,cmd,NULL,NULL,&reg_ByteLen,pbuf1,reg_ByteLen);
                        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf1);
                    }
                    else if((reg_Addr>=COUNT_MODBUS_REGISTERS_ADDR_UART4_AUTO_W)&&(reg_Addr<(COUNT_MODBUS_REGISTERS_ADDR_UART4_AUTO_W+COUNT_MODBUS_REGISTERS_SIZE_UARTX_AUTO_W)))
                    {
                        //���뻺��
                        pbuf1=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                        //ִ��д����
                        reg_ByteLen=reg_WordLen*2;
                        for(i16=0; i16<reg_ByteLen; i16+=sizeof(S_COUNT_MODBUS_UART_AUTO_W))
                        {
                            //Bsp_CpuFlash_Read(IAP_ADDR_UART4_AUTO_TX+2*(reg_Addr-COUNT_MODBUS_REGISTERS_ADDR_UART4_AUTO_W)+i16,&pbuf1[i16],sizeof(S_COUNT_MODBUS_UART_AUTO_W));
                        }
                        Count_Modbus_Array(Spi_SlaveTxBuf,&len16,Count_ModbusSlaveAddr,cmd,NULL,NULL,&reg_ByteLen,pbuf1,reg_ByteLen);
                        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf1);
                    }
                    else if((reg_Addr>=COUNT_MODBUS_REGISTERS_ADDR_UART5_AUTO_W)&&(reg_Addr<(COUNT_MODBUS_REGISTERS_ADDR_UART5_AUTO_W+COUNT_MODBUS_REGISTERS_SIZE_UARTX_AUTO_W)))
                    {
                        //���뻺��
                        pbuf1=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                        //ִ��д����
                        reg_ByteLen=reg_WordLen*2;
                        for(i16=0; i16<reg_ByteLen; i16+=sizeof(S_COUNT_MODBUS_UART_AUTO_W))
                        {
                            //Bsp_CpuFlash_Read(IAP_ADDR_UART5_AUTO_TX+2*(reg_Addr-COUNT_MODBUS_REGISTERS_ADDR_UART5_AUTO_W)+i16,&pbuf1[i16],sizeof(S_COUNT_MODBUS_UART_AUTO_W));
                        }
                        Count_Modbus_Array(Spi_SlaveTxBuf,&len16,Count_ModbusSlaveAddr,cmd,NULL,NULL,&reg_ByteLen,pbuf1,reg_ByteLen);
                        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf1);
                    }
                    break;
            }
            break;
        case  COUNT_MODBUS_FC_READ_INPUT_REGISTERS:
            break;
        case  COUNT_MODBUS_FC_WRITE_SINGLE_COIL:
            break;
        case COUNT_MODBUS_FC_WRITE_SINGLE_REGISTER:
            break;
        case  COUNT_MODBUS_FC_READ_EXCEPTION_STATUS:
            break;
        case  COUNT_MODBUS_FC_WRITE_MULTIPLE_COILS:
            break;
        case  COUNT_MODBUS_FC_WRITE_MULTIPLE_REGISTERS:
            //�Ĵ�����ַ
            reg_Addr=pBuf[3];
            reg_Addr<<=8;
            reg_Addr+=pBuf[2];
            //�Ĵ����ֳ���
            reg_WordLen=pBuf[5];
            reg_WordLen<<=8;
            reg_WordLen+=pBuf[4];
            //�Ĵ����ֽڳ���
            reg_ByteLen = pBuf[6];
            //����ָ��
            preg = &pBuf[7];
            switch(reg_Addr)
            {
                case COUNT_MODBUS_REGISTERS_ADDR_UART_PARA:
                    //Ӧ��
                    Count_Modbus_Array(Spi_SlaveTxBuf,&len16,Count_ModbusSlaveAddr,cmd,&reg_Addr,&reg_WordLen,NULL,NULL,NULL);
                    //����
                    {
                        MODULE_MEMORY_S_PARA *p_uIapPara;
                        p_uIapPara=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                        //�޸�
                        MemManager_Free(E_MEM_MANAGER_TYPE_256B,p_uIapPara);
                    }
                    break;
                case COUNT_MODBUS_REGISTERS_ADDR_PARA:
                    //Ӧ��
                    Count_Modbus_Array(Spi_SlaveTxBuf,&len16,Count_ModbusSlaveAddr,cmd,&reg_Addr,&reg_WordLen,NULL,NULL,NULL);
                    //����
                    //��λ
                    MCU_SOFT_RESET;
                    break;
                case COUNT_MODBUS_REGISTERS_ADDR_CONTROL:
                    //Ӧ��
                    Count_Modbus_Array(Spi_SlaveTxBuf,&len16,Count_ModbusSlaveAddr,cmd,&reg_Addr,&reg_WordLen,NULL,NULL,NULL);
                    //����
                    memcpy((uint8_t*)&i16,preg,COUNT_MODBUS_REGISTERS_LEN_CONTROL);
                    if(i16!=0)
                    {
                        MCU_SOFT_RESET;
                    }
                    break;
#ifdef SPI_SLAVE_EXTEND_S_RTC_EN
                case COUNT_MODBUS_REGISTERS_ADDR_RTC:
                    //Ӧ��
                    Count_Modbus_Array(Spi_SlaveTxBuf,&len16,Count_ModbusSlaveAddr,cmd,&reg_Addr,&reg_WordLen,NULL,NULL,NULL);
                    //����
                    pbuf1=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                    memcpy(pbuf1,preg,COUNT_MODBUS_REGISTERS_LEN_RTC);
                    BspRtc_SetRealTime(NULL,(struct tm *)pbuf1,NULL,NULL);
                    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf1);
                    break;
#endif
#ifdef SPI_SLAVE_EXTEND_S_KOUT_EN
                case COUNT_MODBUS_REGISTERS_ADDR_KOUT:
                    //Ӧ��
                    Count_Modbus_Array(Spi_SlaveTxBuf,&len16,Count_ModbusSlaveAddr,cmd,&reg_Addr,&reg_WordLen,NULL,NULL,NULL);
                    //����
                    i16=preg[1];
                    i16<<=8;
                    i16+=preg[0];
                    for(i=0; i<sizeof(i16); i++)
                    {
                        if(i16&(1<<i))
                        {
                            BspRelay_Write(BSP_RELAY_LEVEL_LOW,i,0xFFFFFFFF,0);
                        }
                        else
                        {
                            BspRelay_Write(BSP_RELAY_LEVEL_LOW,i,0,0);
                        }
                    }
                    break;
#endif
#ifdef SPI_SLAVE_EXTEND_S_UART1_EN
                case COUNT_MODBUS_REGISTERS_ADDR_UART1:
                    //Ӧ��
                    Count_Modbus_Array(Spi_SlaveTxBuf,&len16,Count_ModbusSlaveAddr,cmd,&reg_Addr,&reg_WordLen,NULL,NULL,NULL);
                    //����
                    i16=*((uint16_t*)preg);
                    Spi_SlaveBufferloopPush(reg_Addr,&preg[2],i16);
                    break;
#endif
#ifdef SPI_SLAVE_EXTEND_S_UART2_EN
                case COUNT_MODBUS_REGISTERS_ADDR_UART2:
                    //Ӧ��
                    Count_Modbus_Array(Spi_SlaveTxBuf,&len16,Count_ModbusSlaveAddr,cmd,&reg_Addr,&reg_WordLen,NULL,NULL,NULL);
                    //����
                    i16=*((uint16_t*)preg);
                    Spi_SlaveBufferloopPush(reg_Addr,&preg[2],i16);
                    break;
#endif
#ifdef SPI_SLAVE_EXTEND_S_UART3_EN
                case COUNT_MODBUS_REGISTERS_ADDR_UART3:
                    //Ӧ��
                    Count_Modbus_Array(Spi_SlaveTxBuf,&len16,Count_ModbusSlaveAddr,cmd,&reg_Addr,&reg_WordLen,NULL,NULL,NULL);
                    //����
                    i16=*((uint16_t*)preg);
                    Spi_SlaveBufferloopPush(reg_Addr,&preg[2],i16);
                    break;
#endif
#ifdef SPI_SLAVE_EXTEND_S_UART4_EN
                case COUNT_MODBUS_REGISTERS_ADDR_UART4:
                    //Ӧ��
                    Count_Modbus_Array(Spi_SlaveTxBuf,&len16,Count_ModbusSlaveAddr,cmd,&reg_Addr,&reg_WordLen,NULL,NULL,NULL);
                    //����
                    i16=*((uint16_t*)preg);
                    Spi_SlaveBufferloopPush(reg_Addr,&preg[2],i16);
                    break;
#endif
#ifdef SPI_SLAVE_EXTEND_S_UART5_EN
                case COUNT_MODBUS_REGISTERS_ADDR_UART5:
                    //Ӧ��
                    Count_Modbus_Array(Spi_SlaveTxBuf,&len16,Count_ModbusSlaveAddr,cmd,&reg_Addr,&reg_WordLen,NULL,NULL,NULL);
                    //����
                    i16=*((uint16_t*)preg);
                    Spi_SlaveBufferloopPush(reg_Addr,&preg[2],i16);
                    break;
#endif
                default:
#ifdef SPI_SLAVE_EXTEND_S_UART1_EN
                    if((reg_Addr>=COUNT_MODBUS_REGISTERS_ADDR_UART1_AUTO_W)&&(reg_Addr<(COUNT_MODBUS_REGISTERS_ADDR_UART1_AUTO_W+COUNT_MODBUS_REGISTERS_SIZE_UARTX_AUTO_W)))
                    {
                        //Ӧ��
                        Count_Modbus_Array(Spi_SlaveTxBuf,&len16,Count_ModbusSlaveAddr,cmd,&reg_Addr,&reg_WordLen,NULL,NULL,NULL);
                        //����
                        //����յ����ǵ�һ����ַ�Ĳ���,���������Ƭ��
                        if(reg_Addr==COUNT_MODBUS_REGISTERS_ADDR_UART1_AUTO_W)
                        {
                            //Bsp_CpuFlash_Erase(IAP_ADDR_UART1_AUTO_TX,IAP_ADDR_UART1_AUTO_TX);
                        }
                        //ִ��д����
                        for(i16=0; i16<reg_ByteLen; i16+=sizeof(S_COUNT_MODBUS_UART_AUTO_W))
                        {
                            //Bsp_CpuFlash_Write(IAP_ADDR_UART1_AUTO_TX+2*(reg_Addr-COUNT_MODBUS_REGISTERS_ADDR_UART1_AUTO_W)+i16,&preg[i16],sizeof(S_COUNT_MODBUS_UART_AUTO_W));
                        }
                    }
#endif
#ifdef SPI_SLAVE_EXTEND_S_UART2_EN
                    if((reg_Addr>=COUNT_MODBUS_REGISTERS_ADDR_UART2_AUTO_W)&&(reg_Addr<(COUNT_MODBUS_REGISTERS_ADDR_UART2_AUTO_W+COUNT_MODBUS_REGISTERS_SIZE_UARTX_AUTO_W)))
                    {
                        //Ӧ��
                        Count_Modbus_Array(Spi_SlaveTxBuf,&len16,Count_ModbusSlaveAddr,cmd,&reg_Addr,&reg_WordLen,NULL,NULL,NULL);
                        //����
                        //����յ����ǵ�һ����ַ�Ĳ���,���������Ƭ��
                        if(reg_Addr==COUNT_MODBUS_REGISTERS_ADDR_UART2_AUTO_W)
                        {
                            //Bsp_CpuFlash_Erase(IAP_ADDR_UART2_AUTO_TX,IAP_ADDR_UART2_AUTO_TX);
                        }
                        //ִ��д����
                        for(i16=0; i16<reg_ByteLen; i16+=sizeof(S_COUNT_MODBUS_UART_AUTO_W))
                        {
                            //Bsp_CpuFlash_Write(IAP_ADDR_UART2_AUTO_TX+2*(reg_Addr-COUNT_MODBUS_REGISTERS_ADDR_UART2_AUTO_W)+i16,&preg[i16],sizeof(S_COUNT_MODBUS_UART_AUTO_W));
                        }
                    }
#endif
#ifdef SPI_SLAVE_EXTEND_S_UART3_EN
                    else if((reg_Addr>=COUNT_MODBUS_REGISTERS_ADDR_UART3_AUTO_W)&&(reg_Addr<(COUNT_MODBUS_REGISTERS_ADDR_UART3_AUTO_W+COUNT_MODBUS_REGISTERS_SIZE_UARTX_AUTO_W)))
                    {
                        //Ӧ��
                        Count_Modbus_Array(Spi_SlaveTxBuf,&len16,Count_ModbusSlaveAddr,cmd,&reg_Addr,&reg_WordLen,NULL,NULL,NULL);
                        //����
                        //����յ����ǵ�һ����ַ�Ĳ���,���������Ƭ��
                        if(reg_Addr==COUNT_MODBUS_REGISTERS_ADDR_UART3_AUTO_W)
                        {
                            //Bsp_CpuFlash_Erase(IAP_ADDR_UART3_AUTO_TX,IAP_ADDR_UART3_AUTO_TX);
                        }
                        //ִ��д����
                        for(i16=0; i16<reg_ByteLen; i16+=sizeof(S_COUNT_MODBUS_UART_AUTO_W))
                        {
                            //Bsp_CpuFlash_Write(IAP_ADDR_UART3_AUTO_TX+2*(reg_Addr-COUNT_MODBUS_REGISTERS_ADDR_UART3_AUTO_W)+i16,&preg[i16],sizeof(S_COUNT_MODBUS_UART_AUTO_W));
                        }
                    }
#endif
#ifdef SPI_SLAVE_EXTEND_S_UART4_EN
                    else if((reg_Addr>=COUNT_MODBUS_REGISTERS_ADDR_UART4_AUTO_W)&&(reg_Addr<(COUNT_MODBUS_REGISTERS_ADDR_UART4_AUTO_W+COUNT_MODBUS_REGISTERS_SIZE_UARTX_AUTO_W)))
                    {
                        //Ӧ��
                        Count_Modbus_Array(Spi_SlaveTxBuf,&len16,Count_ModbusSlaveAddr,cmd,&reg_Addr,&reg_WordLen,NULL,NULL,NULL);
                        //����
                        //����յ����ǵ�һ����ַ�Ĳ���,���������Ƭ��
                        if(reg_Addr==COUNT_MODBUS_REGISTERS_ADDR_UART4_AUTO_W)
                        {
                            //Bsp_CpuFlash_Erase(IAP_ADDR_UART4_AUTO_TX,IAP_ADDR_UART4_AUTO_TX);
                        }
                        //ִ��д����
                        for(i16=0; i16<reg_ByteLen; i16+=sizeof(S_COUNT_MODBUS_UART_AUTO_W))
                        {
                            //Bsp_CpuFlash_Write(IAP_ADDR_UART4_AUTO_TX+2*(reg_Addr-COUNT_MODBUS_REGISTERS_ADDR_UART4_AUTO_W)+i16,&preg[i16],sizeof(S_COUNT_MODBUS_UART_AUTO_W));
                        }
                    }
#endif
#ifdef SPI_SLAVE_EXTEND_S_UART5_EN
                    else if((reg_Addr>=COUNT_MODBUS_REGISTERS_ADDR_UART5_AUTO_W)&&(reg_Addr<(COUNT_MODBUS_REGISTERS_ADDR_UART5_AUTO_W+COUNT_MODBUS_REGISTERS_SIZE_UARTX_AUTO_W)))
                    {
                        //Ӧ��
                        Count_Modbus_Array(Spi_SlaveTxBuf,&len16,Count_ModbusSlaveAddr,cmd,&reg_Addr,&reg_WordLen,NULL,NULL,NULL);
                        //����
                        if(reg_Addr==COUNT_MODBUS_REGISTERS_ADDR_UART5_AUTO_W)
                        {
                            //Bsp_CpuFlash_Erase(IAP_ADDR_UART5_AUTO_TX,IAP_ADDR_UART5_AUTO_TX);
                        }
                        //ִ��д����
                        for(i16=0; i16<reg_ByteLen; i16+=sizeof(S_COUNT_MODBUS_UART_AUTO_W))
                        {
                            //Bsp_CpuFlash_Write(IAP_ADDR_UART5_AUTO_TX+2*(reg_Addr-COUNT_MODBUS_REGISTERS_ADDR_UART5_AUTO_W)+i16,&preg[i16],sizeof(S_COUNT_MODBUS_UART_AUTO_W));
                        }
                    }
#endif
                    break;
            }
            break;
        case  COUNT_MODBUS_FC_REPORT_SLAVE_ID:
            break;
        case  COUNT_MODBUS_FC_MASK_WRITE_REGISTER :
            break;
        case  COUNT_MODBUS_FC_WRITE_AND_READ_REGISTERS:
            break;
        case  COUNT_MODBUS_FC_USER_UART_WRITE_BLOCK_READ:
            //Ӧ��
            Count_Modbus_Array(Spi_SlaveTxBuf,&len16,Count_ModbusSlaveAddr,cmd,&reg_Addr,&reg_WordLen,NULL,NULL,NULL);
            len16=0;
            //����
            {
                //��ȡ���ݳ���(�Ӵ��ں�����)
                pBuf[2]=pBuf[2];
                //��ȡ���ں�
                pBuf[3]=pBuf[3];
                //���������ֽ�
                pBuf[4]=pBuf[4];
                //��ȡ��������ָ��
                pBuf[5]=pBuf[5];
                //��ַӳ��
                if(pBuf[3]==1)
                {
                    reg_Addr=COUNT_MODBUS_REGISTERS_ADDR_UART2;
                }
                if(pBuf[3]==2)
                {
                    reg_Addr=COUNT_MODBUS_REGISTERS_ADDR_UART3;
                }
                if(pBuf[3]==3)
                {
                    reg_Addr=COUNT_MODBUS_REGISTERS_ADDR_UART4;
                }
                if(pBuf[3]==4)
                {
                    reg_Addr=COUNT_MODBUS_REGISTERS_ADDR_UART5;
                }
                else
                {
                    break;
                }
                //���봮��ģʽΪ����͸��ģʽ
                Spi_SlaveUartCmdBuf[pBuf[3]-1]=2;
                //
                Spi_SlaveBufferloopPush(reg_Addr,&pBuf[5],pBuf[2]-2);
            }
            break;
        default:
            Spi_SlaveErrCounter++;
            break;
    }
//�����ط�����
    if(len16!=0)
    {
#ifdef SPI_SLAVE_EXTEND_M_UART1_EN
        UART_DMA_Tx(1,Spi_SlaveTxBuf,len16);
#endif
    }
}

/*******************************************************************************
* ��������: 1ms��ʱ�жϵ���
* ��    ��:
* Return  :
*******************************************************************************/
void Spi_Slave_1ms(void)
{
    uint8_t i;
    static uint8_t int_first=0;
    static uint16_t Spi_SlaveIntTimer_ms=0;//0-�ر�,����-����
    //�ж�����
    if(Spi_SlaveIntTimer_ms==0)
    {
        for(i=0; i<SPI_SLAVE_MAX_UART; i++)
        {
            if(SPI_Slave_S_State.UartRxNum[i]!=0)
            {
                if(int_first==1)
                {
                    Spi_SlaveIntTimer_ms=1000;
                }
                else
                {
                    Spi_SlaveIntTimer_ms=1;
                }
                break;
            }
        }
    }
    else
    {
        int_first=1;
    }
    //��������(5ms����)
    if(Spi_SlaveIntTimer_ms!=0)
    {
        Spi_SlaveIntTimer_ms++;
        if(Spi_SlaveIntTimer_ms==1000+1)
        {
            SPI_SLAVE_INT_LOW;
        }
        else if(Spi_SlaveIntTimer_ms>=1000+5)
        {
            Spi_SlaveIntTimer_ms=0;
            SPI_SLAVE_INT_HIGH;
        }
    }
    //DMA���շֶδ���(����1ms�����ݷֶ�)
#ifdef SPI_SLAVE_EXTEND_M_SPI_EN
    {
        uint16_t i16;

        static uint16_t Spi_SlaveRxDMAData=0;
        static uint16_t stimer=0;
        if(Spi_SlaveRxDMATimer==0xFF)
        {
            //��ʱ����
            stimer++;
            if(stimer>=1000*5)
            {
                //SPI����DMA��ʼ��
                DMA_Cmd(DMA2_Channel2,DISABLE);
                DMA_SetCurrDataCounter(DMA2_Channel2,SPI_SLAVE_TXBUF_MAX);
                DMA_Cmd(DMA2_Channel2, ENABLE);
                //SPI����DMA��ʼ��
                DMA_Cmd(DMA2_Channel1,DISABLE);
                DMA_SetCurrDataCounter(DMA2_Channel1,SPI_SLAVE_RXBUF_MAX);
                DMA_Cmd(DMA2_Channel1,ENABLE);
                //��ռ�ʱ
                Spi_SlaveRxDMATimer=0;
            }
        }
        else if(Spi_SlaveRxDMATimer==0)
        {
            Spi_SlaveRxDMAData=DMA_GetCurrDataCounter(DMA2_Channel1);
            if(SPI_SLAVE_RXBUF_MAX!=Spi_SlaveRxDMAData && 0!=Spi_SlaveRxDMAData)
            {
                Spi_SlaveRxDMATimer=1;
            }
        }
        else if(Spi_SlaveRxDMATimer<2)
        {
            i16=DMA_GetCurrDataCounter(DMA2_Channel1);
            if(Spi_SlaveRxDMAData==i16)
            {
                Spi_SlaveRxLen = SPI_SLAVE_RXBUF_MAX - DMA_GetCurrDataCounter(DMA2_Channel1);
                Spi_SlaveRxDMATimer++;
            }
            else
            {
                Spi_SlaveRxDMAData=i16;
                Spi_SlaveRxDMATimer=1;
            }
        }
        else
        {
            SPI_SLAVE_S_Q *pQ;
            pQ=Spi_SlaveGetQ();
            if(pQ!=NULL)
            {
                pQ->cmd=SPI_E_SLAVE_CMD_M_PARSE;
                MODULE_OS_Q_POST(SpiSlave_pQ,*pQ,sizeof(SPI_SLAVE_S_Q));
            }
            Spi_SlaveRxDMATimer=0xFF;
            stimer=0;
        }
    }
#endif
}
//-------------------------------------------------------------Modbus���񴴽�
void  App_SpiTaskCreate (void)
{
    MODULE_OS_TASK_CREATE("App Task SPI",\
                          uctsk_Spi,\
                          APP_TASK_SPI_PRIO,\
                          App_TaskSpiStk,\
                          APP_TASK_SPI_STK_SIZE,\
                          AppTaskSpiTCB,\
                          NULL);
}
void SpiMaster_DebugTestOnOff(uint8_t OnOff);
static void uctsk_Spi (void)
{
    SPI_SLAVE_S_Q *p_cq;
    uint8_t *pbuf,*pbuf1;
    uint16_t len;
    uint8_t res;
    uint32_t i32;
    MODULE_OS_ERR err;
    //Ӳ����ʼ��
    SpiMaster_DebugTestOnOff(ON);
    Spi_SlaveInit();
    //��ʼ�����滷
#ifdef SPI_SLAVE_EXTEND_S_UART1_EN
    Count_BufferLoopInit(&UctskSpiSlave_UartTx1_BufferLoop,\
                         UctskSpiSlave_UartTx1_BufferLoop_Buf,\
                         UCTSK_SPISLAVE_UART_TX1_BUFFERLOOP_BUF_MAX,\
                         UctskSpiSlave_UartTx1_BufferLoop_Node,\
                         UCTSK_SPISLAVE_UART_TX1_BUFFERLOOP_NODE_MAX,\
                         &UctskSpiSlave_UartTx1_BufferLoop_Limit);
    Count_BufferLoopInit(&UctskSpiSlave_UartRx1_BufferLoop,\
                         UctskSpiSlave_UartRx1_BufferLoop_Buf,\
                         UCTSK_SPISLAVE_UART_RX1_BUFFERLOOP_BUF_MAX,\
                         UctskSpiSlave_UartRx1_BufferLoop_Node,\
                         UCTSK_SPISLAVE_UART_RX1_BUFFERLOOP_NODE_MAX,\
                         &UctskSpiSlave_UartRx1_BufferLoop_Limit);
#endif
#ifdef SPI_SLAVE_EXTEND_S_UART2_EN
    Count_BufferLoopInit(&UctskSpiSlave_UartTx2_BufferLoop,\
                         UctskSpiSlave_UartTx2_BufferLoop_Buf,\
                         UCTSK_SPISLAVE_UART_TX2_BUFFERLOOP_BUF_MAX,\
                         UctskSpiSlave_UartTx2_BufferLoop_Node,\
                         UCTSK_SPISLAVE_UART_TX2_BUFFERLOOP_NODE_MAX,\
                         &UctskSpiSlave_UartTx2_BufferLoop_Limit);
    Count_BufferLoopInit(&UctskSpiSlave_UartRx2_BufferLoop,\
                         UctskSpiSlave_UartRx2_BufferLoop_Buf,\
                         UCTSK_SPISLAVE_UART_RX2_BUFFERLOOP_BUF_MAX,\
                         UctskSpiSlave_UartRx2_BufferLoop_Node,\
                         UCTSK_SPISLAVE_UART_RX2_BUFFERLOOP_NODE_MAX,\
                         &UctskSpiSlave_UartRx2_BufferLoop_Limit);
#endif
#ifdef SPI_SLAVE_EXTEND_S_UART3_EN
    Count_BufferLoopInit(&UctskSpiSlave_UartTx3_BufferLoop,\
                         UctskSpiSlave_UartTx3_BufferLoop_Buf,\
                         UCTSK_SPISLAVE_UART_TX3_BUFFERLOOP_BUF_MAX,\
                         UctskSpiSlave_UartTx3_BufferLoop_Node,\
                         UCTSK_SPISLAVE_UART_TX3_BUFFERLOOP_NODE_MAX,\
                         &UctskSpiSlave_UartTx3_BufferLoop_Limit);
    Count_BufferLoopInit(&UctskSpiSlave_UartRx3_BufferLoop,\
                         UctskSpiSlave_UartRx3_BufferLoop_Buf,\
                         UCTSK_SPISLAVE_UART_RX3_BUFFERLOOP_BUF_MAX,\
                         UctskSpiSlave_UartRx3_BufferLoop_Node,\
                         UCTSK_SPISLAVE_UART_RX3_BUFFERLOOP_NODE_MAX,\
                         &UctskSpiSlave_UartRx3_BufferLoop_Limit);
#endif
#ifdef SPI_SLAVE_EXTEND_S_UART4_EN
    Count_BufferLoopInit(&UctskSpiSlave_UartTx4_BufferLoop,\
                         UctskSpiSlave_UartTx4_BufferLoop_Buf,\
                         UCTSK_SPISLAVE_UART_TX4_BUFFERLOOP_BUF_MAX,\
                         UctskSpiSlave_UartTx4_BufferLoop_Node,\
                         UCTSK_SPISLAVE_UART_TX4_BUFFERLOOP_NODE_MAX,\
                         &UctskSpiSlave_UartTx4_BufferLoop_Limit);
    Count_BufferLoopInit(&UctskSpiSlave_UartRx4_BufferLoop,\
                         UctskSpiSlave_UartRx4_BufferLoop_Buf,\
                         UCTSK_SPISLAVE_UART_RX4_BUFFERLOOP_BUF_MAX,\
                         UctskSpiSlave_UartRx4_BufferLoop_Node,\
                         UCTSK_SPISLAVE_UART_RX4_BUFFERLOOP_NODE_MAX,\
                         &UctskSpiSlave_UartRx4_BufferLoop_Limit);
#endif
#ifdef SPI_SLAVE_EXTEND_S_UART5_EN
    Count_BufferLoopInit(&UctskSpiSlave_UartTx5_BufferLoop,\
                         UctskSpiSlave_UartTx5_BufferLoop_Buf,\
                         UCTSK_SPISLAVE_UART_TX5_BUFFERLOOP_BUF_MAX,\
                         UctskSpiSlave_UartTx5_BufferLoop_Node,\
                         UCTSK_SPISLAVE_UART_TX5_BUFFERLOOP_NODE_MAX,\
                         &UctskSpiSlave_UartTx5_BufferLoop_Limit);
    Count_BufferLoopInit(&UctskSpiSlave_UartRx5_BufferLoop,\
                         UctskSpiSlave_UartRx5_BufferLoop_Buf,\
                         UCTSK_SPISLAVE_UART_RX5_BUFFERLOOP_BUF_MAX,\
                         UctskSpiSlave_UartRx5_BufferLoop_Node,\
                         UCTSK_SPISLAVE_UART_RX5_BUFFERLOOP_NODE_MAX,\
                         &UctskSpiSlave_UartRx5_BufferLoop_Limit);
#endif
    //������Ϣ����
    MODULE_OS_Q_CREATE(SpiSlave_pQ,"SpiSlave_pQ",SPI_SLAVE_QBUF_MAX);
    //��ʼ�����л���
    Spi_SlaveInitQ();
    //������ʼ��
    memset((uint8_t*)&SPI_Slave_S_State,0,sizeof(SPI_Slave_S_State));
    Spi_SlaveErrCounter=0;
    for(;;)
    {
        MODULE_OS_Q_PEND(p_cq,SpiSlave_pQ,3000,TRUE,err);
        if(err == MODULE_OS_ERR_NONE)
        {
            switch(p_cq->cmd)
            {
                // ������������������
                case SPI_E_SLAVE_CMD_M_PARSE:
#ifdef SPI_SLAVE_EXTEND_M_SPI_EN
                    Spi_SlaveRxLen = 256 - DMA_GetCurrDataCounter(DMA2_Channel1);
                    //SPI����DMA��ʼ��
                    DMA_Cmd(DMA2_Channel2,DISABLE);
                    DMA_SetCurrDataCounter(DMA2_Channel2,SPI_SLAVE_TXBUF_MAX);
                    DMA_Cmd(DMA2_Channel2, ENABLE);
                    //SPI����DMA��ʼ��
                    DMA_Cmd(DMA2_Channel1,DISABLE);
                    DMA_SetCurrDataCounter(DMA2_Channel1,SPI_SLAVE_RXBUF_MAX);
                    DMA_Cmd(DMA2_Channel1,ENABLE);
                    Spi_SlaveRxDMATimer=0;
#endif
                    //У��CRC
                    if(OK==Count_Modbus_Check(Spi_SlaveRxBuf,Spi_SlaveRxLen,Spi_SlaveRxBuf[0]))
                    {
                        //����
                        Spi_SlaveRxParsePro(Spi_SlaveRxBuf,Spi_SlaveRxLen);
                    }
                    Spi_SlaveRxLen=0;
                    break;
                    // �������Դ��ڵ�����
#ifdef SPI_SLAVE_EXTEND_S_UART1_EN
                case SPI_E_SLAVE_CMD_UART1_R:
                    //��������ģʽ
                    if(Spi_SlaveUartModeBuf[0]==0)
                    {
                        if(0==SPI_Slave_S_State.UartRxNum[0])
                        {
                            Count_BufferLoopPop(&UctskSpiSlave_UartRx2_BufferLoop,NULL,&len,COUNT_BUFFERLOOP_E_POPMODE_KEEP);
                            SPI_Slave_S_State.UartRxSize[0]=len;
                        }
                        if(0xFF!=SPI_Slave_S_State.UartRxNum[0])
                        {
                            SPI_Slave_S_State.UartRxNum[0]++;
                        }
                    }
                    //�Զ�����ģʽ
                    else if(Spi_SlaveUartModeBuf[0]==1)
                    {
                        //���뻺��
                        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                        //��ȡ����
                        res=Count_BufferLoopPop(&UctskSpiSlave_UartRx2_BufferLoop,pbuf,&len,COUNT_BUFFERLOOP_E_POPMODE_DELETE);
                        //����Э��,���ݵ��ڴ�
                        //CRCУ��
                        //����У��
                        if((pbuf[2]+5)==len)
                        {
                            //���ݸ�ֵ
                            memcpy(&Spi_SlaveAutoRxUartBuf[0][Spi_SlaveAutoRxUartNumBuf[0]+2],&pbuf[3],len-5);
                            //��־��ֵ
                            i32=Spi_SlaveAutoRxUartBuf[0][0];
                            i32<<=16;
                            i32+=Spi_SlaveAutoRxUartBuf[0][1];
                            i32|=(1<<(pbuf[0]-1));
                            Spi_SlaveAutoRxUartBuf[0][0]=i32>>16;
                            Spi_SlaveAutoRxUartBuf[0][1]=i32;
                        }
                        //�ͷŻ���
                        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
                    }
                    //����͸������ģʽ
                    else if(Spi_SlaveUartModeBuf[0]==2)
                    {
                        uint8_t reg_ByteLen;
                        uint16_t len16;
                        //���뻺��
                        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                        pbuf1=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                        //��ȡ����
                        res=Count_BufferLoopPop(&UctskSpiSlave_UartRx2_BufferLoop,&pbuf[2],&len,COUNT_BUFFERLOOP_E_POPMODE_DELETE);
                        //���ں�
                        pbuf[0]=1;
                        //����
                        pbuf[1]=0;
                        //����
                        reg_ByteLen=len+2;
                        Count_Modbus_Array(pbuf1,&len16,Count_ModbusSlaveAddr,COUNT_MODBUS_FC_USER_UART_WRITE_BLOCK_READ,NULL,NULL,&reg_ByteLen,(uint8_t*)pbuf,reg_ByteLen);
                        UART_DMA_Tx(1,pbuf1,len16);
                        Spi_SlaveUartCmdBuf[0]=1;
                        //�ͷŻ���
                        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf1);
                        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
                    }
                    break;
#endif
#ifdef SPI_SLAVE_EXTEND_S_UART2_EN
                case SPI_E_SLAVE_CMD_UART2_R:
                    //��������ģʽ
                    if(Spi_SlaveUartModeBuf[1]==0)
                    {
                        if(0==SPI_Slave_S_State.UartRxNum[1])
                        {
                            Count_BufferLoopPop(&UctskSpiSlave_UartRx2_BufferLoop,NULL,&len,COUNT_BUFFERLOOP_E_POPMODE_KEEP);
                            SPI_Slave_S_State.UartRxSize[1]=len;
                        }
                        if(0xFF!=SPI_Slave_S_State.UartRxNum[1])
                        {
                            SPI_Slave_S_State.UartRxNum[1]++;
                        }
                    }
                    //�Զ�����ģʽ
                    else if(Spi_SlaveUartModeBuf[1]==1)
                    {
                        //���뻺��
                        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                        //��ȡ����
                        res=Count_BufferLoopPop(&UctskSpiSlave_UartRx2_BufferLoop,pbuf,&len,COUNT_BUFFERLOOP_E_POPMODE_DELETE);
                        //����Э��,���ݵ��ڴ�
                        //CRCУ��
                        //����У��
                        if((pbuf[2]+5)==len)
                        {
                            //���ݸ�ֵ
                            memcpy(&Spi_SlaveAutoRxUartBuf[1][Spi_SlaveAutoRxUartNumBuf[1]+2],&pbuf[3],len-5);
                            //��־��ֵ
                            i32=Spi_SlaveAutoRxUartBuf[1][0];
                            i32<<=16;
                            i32+=Spi_SlaveAutoRxUartBuf[1][1];
                            i32|=(1<<(pbuf[0]-1));
                            Spi_SlaveAutoRxUartBuf[1][0]=i32>>16;
                            Spi_SlaveAutoRxUartBuf[1][1]=i32;
                        }
                        //�ͷŻ���
                        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
                    }
                    //����͸������ģʽ
                    else if(Spi_SlaveUartModeBuf[1]==2)
                    {
                        uint8_t reg_ByteLen;
                        uint16_t len16;
                        //���뻺��
                        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                        pbuf1=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                        //��ȡ����
                        res=Count_BufferLoopPop(&UctskSpiSlave_UartRx2_BufferLoop,&pbuf[2],&len,COUNT_BUFFERLOOP_E_POPMODE_DELETE);
                        //���ں�
                        pbuf[0]=1;
                        //����
                        pbuf[1]=0;
                        //����
                        reg_ByteLen=len+2;
                        Count_Modbus_Array(pbuf1,&len16,Count_ModbusSlaveAddr,COUNT_MODBUS_FC_USER_UART_WRITE_BLOCK_READ,NULL,NULL,&reg_ByteLen,(uint8_t*)pbuf,reg_ByteLen);
                        UART_DMA_Tx(1,pbuf1,len16);
                        Spi_SlaveUartCmdBuf[1]=1;
                        //�ͷŻ���
                        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf1);
                        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
                    }
                    break;
#endif
#ifdef SPI_SLAVE_EXTEND_S_UART3_EN
                case SPI_E_SLAVE_CMD_UART3_R:
                    //��������ģʽ
                    if(Spi_SlaveUartModeBuf[2]==0)
                    {
                        if(0==SPI_Slave_S_State.UartRxNum[2])
                        {
                            Count_BufferLoopPop(&UctskSpiSlave_UartRx3_BufferLoop,NULL,&len,COUNT_BUFFERLOOP_E_POPMODE_KEEP);
                            SPI_Slave_S_State.UartRxSize[2]=len;
                        }
                        if(0xFF!=SPI_Slave_S_State.UartRxNum[2])
                        {
                            SPI_Slave_S_State.UartRxNum[2]++;
                        }
                    }
                    //�Զ�����ģʽ
                    else if(Spi_SlaveUartModeBuf[2]==1)
                    {
                        //���뻺��
                        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                        //��ȡ����
                        res=Count_BufferLoopPop(&UctskSpiSlave_UartRx3_BufferLoop,pbuf,&len,COUNT_BUFFERLOOP_E_POPMODE_DELETE);
                        //����Э��,���ݵ��ڴ�
                        //CRCУ��
                        //����У��
                        if((pbuf[2]+5)==len)
                        {
                            //���ݸ�ֵ
                            memcpy(&Spi_SlaveAutoRxUartBuf[2][Spi_SlaveAutoRxUartNumBuf[2]+2],&pbuf[3],len-5);
                            //��־��ֵ
                            i32=Spi_SlaveAutoRxUartBuf[2][0];
                            i32<<=16;
                            i32+=Spi_SlaveAutoRxUartBuf[2][1];
                            i32|=(1<<(pbuf[0]-1));
                            Spi_SlaveAutoRxUartBuf[2][0]=i32>>16;
                            Spi_SlaveAutoRxUartBuf[2][1]=i32;
                        }
                        //�ͷŻ���
                        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
                    }
                    //����͸������ģʽ
                    else if(Spi_SlaveUartModeBuf[2]==2)
                    {
                        uint8_t reg_ByteLen;
                        uint16_t len16;
                        //���뻺��
                        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                        pbuf1=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                        //��ȡ����
                        res=Count_BufferLoopPop(&UctskSpiSlave_UartRx3_BufferLoop,&pbuf[2],&len,COUNT_BUFFERLOOP_E_POPMODE_DELETE);
                        //���ں�
                        pbuf[0]=2;
                        //����
                        pbuf[1]=0;
                        //����
                        reg_ByteLen=len+2;
                        Count_Modbus_Array(pbuf1,&len16,Count_ModbusSlaveAddr,COUNT_MODBUS_FC_USER_UART_WRITE_BLOCK_READ,NULL,NULL,&reg_ByteLen,(uint8_t*)pbuf,reg_ByteLen);
                        UART_DMA_Tx(1,pbuf1,len16);
                        Spi_SlaveUartCmdBuf[2]=1;
                        //�ͷŻ���
                        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf1);
                        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
                    }
                    break;
#endif
#ifdef SPI_SLAVE_EXTEND_S_UART4_EN
                case SPI_E_SLAVE_CMD_UART4_R:
                    //��������ģʽ
                    if(Spi_SlaveUartModeBuf[3]==0)
                    {
                        if(0==SPI_Slave_S_State.UartRxNum[3])
                        {
                            Count_BufferLoopPop(&UctskSpiSlave_UartRx4_BufferLoop,NULL,&len,COUNT_BUFFERLOOP_E_POPMODE_KEEP);
                            SPI_Slave_S_State.UartRxSize[3]=len;
                        }
                        if(0xFF!=SPI_Slave_S_State.UartRxNum[3])
                        {
                            SPI_Slave_S_State.UartRxNum[3]++;
                        }
                    }
                    //�Զ�����ģʽ
                    else if(Spi_SlaveUartModeBuf[3]==1)
                    {
                        //���뻺��
                        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                        //��ȡ����
                        res=Count_BufferLoopPop(&UctskSpiSlave_UartRx4_BufferLoop,pbuf,&len,COUNT_BUFFERLOOP_E_POPMODE_DELETE);
                        //����Э��,���ݵ��ڴ�
                        //CRCУ��
                        //����У��
                        if((pbuf[2]+5)==len)
                        {
                            //���ݸ�ֵ
                            memcpy(&Spi_SlaveAutoRxUartBuf[3][Spi_SlaveAutoRxUartNumBuf[3]+2],&pbuf[3],len-5);
                            //��־��ֵ
                            i32=Spi_SlaveAutoRxUartBuf[3][0];
                            i32<<=16;
                            i32+=Spi_SlaveAutoRxUartBuf[3][1];
                            i32|=(1<<(pbuf[0]-1));
                            Spi_SlaveAutoRxUartBuf[3][0]=i32>>16;
                            Spi_SlaveAutoRxUartBuf[3][1]=i32;
                        }
                        //�ͷŻ���
                        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
                    }
                    //����͸������ģʽ
                    else if(Spi_SlaveUartModeBuf[3]==2)
                    {
                        uint8_t reg_ByteLen;
                        uint16_t len16;
                        //���뻺��
                        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                        pbuf1=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                        //��ȡ����
                        res=Count_BufferLoopPop(&UctskSpiSlave_UartRx4_BufferLoop,&pbuf[2],&len,COUNT_BUFFERLOOP_E_POPMODE_DELETE);
                        //���ں�
                        pbuf[0]=3;
                        //����
                        pbuf[1]=0;
                        //����
                        reg_ByteLen=len+2;
                        Count_Modbus_Array(pbuf1,&len16,Count_ModbusSlaveAddr,COUNT_MODBUS_FC_USER_UART_WRITE_BLOCK_READ,NULL,NULL,&reg_ByteLen,(uint8_t*)pbuf,reg_ByteLen);
                        UART_DMA_Tx(1,pbuf1,len16);
                        Spi_SlaveUartCmdBuf[3]=1;
                        //�ͷŻ���
                        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf1);
                        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
                    }
                    break;
#endif
#ifdef SPI_SLAVE_EXTEND_S_UART5_EN
                case SPI_E_SLAVE_CMD_UART5_R:
                    //��������ģʽ
                    if(Spi_SlaveUartModeBuf[4]==0)
                    {
                        if(0==SPI_Slave_S_State.UartRxNum[4])
                        {
                            Count_BufferLoopPop(&UctskSpiSlave_UartRx5_BufferLoop,NULL,&len,COUNT_BUFFERLOOP_E_POPMODE_KEEP);
                            SPI_Slave_S_State.UartRxSize[4]=len;
                        }
                        if(0xFF!=SPI_Slave_S_State.UartRxNum[4])
                        {
                            SPI_Slave_S_State.UartRxNum[4]++;
                        }
                    }
                    //�Զ�����ģʽ
                    else if(Spi_SlaveUartModeBuf[4]==1)
                    {
                        //���뻺��
                        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                        //��ȡ����
                        res=Count_BufferLoopPop(&UctskSpiSlave_UartRx5_BufferLoop,pbuf,&len,COUNT_BUFFERLOOP_E_POPMODE_DELETE);
                        //����Э��,���ݵ��ڴ�
                        //CRCУ��
                        //����У��
                        if((pbuf[2]+5)==len)
                        {
                            //���ݸ�ֵ
                            memcpy(&Spi_SlaveAutoRxUartBuf[4][Spi_SlaveAutoRxUartNumBuf[4]+2],&pbuf[3],len-5);
                            //��־��ֵ
                            i32=Spi_SlaveAutoRxUartBuf[4][0];
                            i32<<=16;
                            i32+=Spi_SlaveAutoRxUartBuf[4][1];
                            i32|=(1<<(pbuf[0]-1));
                            Spi_SlaveAutoRxUartBuf[4][0]=i32>>16;
                            Spi_SlaveAutoRxUartBuf[4][1]=i32;
                        }
                        //�ͷŻ���
                        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
                    }
                    //����͸������ģʽ
                    else if(Spi_SlaveUartModeBuf[4]==2)
                    {
                        uint8_t reg_ByteLen;
                        uint16_t len16;
                        //���뻺��
                        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                        pbuf1=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                        //��ȡ����
                        res=Count_BufferLoopPop(&UctskSpiSlave_UartRx5_BufferLoop,&pbuf[2],&len,COUNT_BUFFERLOOP_E_POPMODE_DELETE);
                        //���ں�
                        pbuf[0]=4;
                        //����
                        pbuf[1]=0;
                        //����
                        reg_ByteLen=len+2;
                        Count_Modbus_Array(pbuf1,&len16,Count_ModbusSlaveAddr,COUNT_MODBUS_FC_USER_UART_WRITE_BLOCK_READ,NULL,NULL,&reg_ByteLen,(uint8_t*)pbuf,reg_ByteLen);
                        UART_DMA_Tx(1,pbuf1,len16);
                        Spi_SlaveUartCmdBuf[4]=1;
                        //�ͷŻ���
                        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf1);
                        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
                    }
                    break;
#endif
                case COUNT_MODBUS_REGISTERS_ADDR_UART1:
                case COUNT_MODBUS_REGISTERS_ADDR_UART2:
                case COUNT_MODBUS_REGISTERS_ADDR_UART3:
                case COUNT_MODBUS_REGISTERS_ADDR_UART4:
                case COUNT_MODBUS_REGISTERS_ADDR_UART5:
                    i32=((p_cq->cmd)-COUNT_MODBUS_REGISTERS_ADDR_UART1)/COUNT_MODBUS_REGISTERS_SIZE_UART;
                    //��ʱǿ��ת��,δ��һ��Ҫ����(�˴�Ϊbug)
                    //�жϴ���״̬
                    if(Spi_SlaveUartCmdBuf[i32]==1)
                    {
                        Spi_SlaveUartCmdBuf[i32]=0;
                        Spi_SlaveUartModeBuf[i32]=1;
                    }
                    else if(Spi_SlaveUartCmdBuf[i32]==2)
                    {
                        Spi_SlaveUartCmdBuf[i32]=0;
                        Spi_SlaveUartModeBuf[i32]=2;
                    }
                    //���뻺��
                    pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                    //��ȡ����
#ifdef SPI_SLAVE_EXTEND_S_UART1_EN
                    if(p_cq->cmd==COUNT_MODBUS_REGISTERS_ADDR_UART1)
                    {
                        res=Count_BufferLoopPop(&UctskSpiSlave_UartTx1_BufferLoop,pbuf,&len,COUNT_BUFFERLOOP_E_POPMODE_DELETE);
                    }
#endif
#ifdef SPI_SLAVE_EXTEND_S_UART2_EN
                    if(p_cq->cmd==COUNT_MODBUS_REGISTERS_ADDR_UART2)
                    {
                        res=Count_BufferLoopPop(&UctskSpiSlave_UartTx2_BufferLoop,pbuf,&len,COUNT_BUFFERLOOP_E_POPMODE_DELETE);
                    }
#endif
#ifdef SPI_SLAVE_EXTEND_S_UART3_EN
                    if(p_cq->cmd==COUNT_MODBUS_REGISTERS_ADDR_UART3)
                    {
                        res=Count_BufferLoopPop(&UctskSpiSlave_UartTx3_BufferLoop,pbuf,&len,COUNT_BUFFERLOOP_E_POPMODE_DELETE);
                    }
#endif
#ifdef SPI_SLAVE_EXTEND_S_UART4_EN
                    if(p_cq->cmd==COUNT_MODBUS_REGISTERS_ADDR_UART4)
                    {
                        res=Count_BufferLoopPop(&UctskSpiSlave_UartTx4_BufferLoop,pbuf,&len,COUNT_BUFFERLOOP_E_POPMODE_DELETE);
                    }
#endif
#ifdef SPI_SLAVE_EXTEND_S_UART5_EN
                    if(p_cq->cmd==COUNT_MODBUS_REGISTERS_ADDR_UART5)
                    {
                        res=Count_BufferLoopPop(&UctskSpiSlave_UartTx5_BufferLoop,pbuf,&len,COUNT_BUFFERLOOP_E_POPMODE_DELETE);
                    }
#endif
                    //��������
                    if(res==OK)
                    {
                        UART_DMA_Tx(1+i32,pbuf,len);
                    }
                    //�ͷŻ���
                    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
                    break;
                default:
                    break;
            }
            //�ͷ���Ϣ
            Spi_SlaveFreeQ(p_cq);
        }
        //��ʱ
        else if(err == MODULE_OS_ERR_TIMEOUT)
        {
            //�����ʼ��
            if(Spi_SlaveErrCounter>10)
            {
                Spi_SlaveErrCounter=0;
                Spi_SlaveInit();
                memset((uint8_t*)&SPI_Slave_S_State,0,sizeof(SPI_Slave_S_State));
                Spi_SlaveErrCounter=0;
            }
            //�����Զ�����
#if 0
            {
                uint8_t i;
                static uint8_t IntervalBuf_100ms[4]= {0};
                static uint8_t NumBuf[4]= {0};
                static uint16_t LastReadSizeBuf[4]= {0};
                const uint32_t IAP_ADDR_UART_AUTO_TX_BUF[4]= {IAP_ADDR_UART2_AUTO_TX,IAP_ADDR_UART3_AUTO_TX,IAP_ADDR_UART4_AUTO_TX,IAP_ADDR_UART5_AUTO_TX};
                S_COUNT_MODBUS_UART_AUTO_W *p_SCountMoubusUartAutoW;
                //���뻺��
                p_SCountMoubusUartAutoW=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                for(i=0; i<4; i++)
                {
                    //�жϴ���״̬
                    if(Spi_SlaveUartCmdBuf[i]==1)
                    {
                        Spi_SlaveUartCmdBuf[i]=0;
                        Spi_SlaveUartModeBuf[i]=1;
                    }
                    else if(Spi_SlaveUartCmdBuf[i]==2)
                    {
                        Spi_SlaveUartCmdBuf[i]=0;
                        Spi_SlaveUartModeBuf[i]=2;
                        continue;
                    }
                    //����ѭ������
                    if(Spi_SlaveUartModeBuf[i]==1)
                    {
                        //��ȡ����
                        Bsp_CpuFlash_Read(BSP_CPU_FLASH_PARA_ADDR,IAP_ADDR_UART_AUTO_TX_BUF[i]+sizeof(S_COUNT_MODBUS_UART_AUTO_W)*NumBuf[i],(uint8_t*)p_SCountMoubusUartAutoW,sizeof(S_COUNT_MODBUS_UART_AUTO_W));
                        if(p_SCountMoubusUartAutoW->Interval_ms==0xFF && NumBuf[i]!=0)
                        {
                            NumBuf[i]=0;
                            Bsp_CpuFlash_Read(BSP_CPU_FLASH_PARA_ADDR,IAP_ADDR_UART_AUTO_TX_BUF[i]+sizeof(S_COUNT_MODBUS_UART_AUTO_W)*NumBuf[i],(uint8_t*)p_SCountMoubusUartAutoW,sizeof(S_COUNT_MODBUS_UART_AUTO_W));
                            Spi_SlaveAutoRxUartNumBuf[i]=0;
                            LastReadSizeBuf[i]=0;
                        }
                        NumBuf[i]++;
                        //��������
                        if(p_SCountMoubusUartAutoW->Interval_ms!=0xFF)
                        {
                            //���»����ַ
                            Spi_SlaveAutoRxUartNumBuf[i]+=LastReadSizeBuf[i];
                            //��¼���ε�ַ
                            LastReadSizeBuf[i]=p_SCountMoubusUartAutoW->Txbuf[4];
                            LastReadSizeBuf[i]<<=8;
                            LastReadSizeBuf[i]+=p_SCountMoubusUartAutoW->Txbuf[5];
                            //����
                            UART_DMA_Tx(2+i,p_SCountMoubusUartAutoW->Txbuf,8);
                        }
                    }
                }
                //�ͷŻ���
                MemManager_Free(E_MEM_MANAGER_TYPE_256B,p_SCountMoubusUartAutoW);
            }
#endif
        }
        else
        {
            //�����Ϣ����
            MODULE_OS_Q_FLUSH(SpiSlave_pQ);
        }
    }
}
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
