/*
***********************************************************************************
*                    作    者: 徐松亮
*                    更新时间: 2014-05-29
***********************************************************************************
*/
/*
通讯流程:   每操作一个功能码,都要执行一次下面的流程.
   主机     :  CS=0     数据操作 CS=1
   从机     :  初始化   数据整合 释放数据
*/
//-------------------------------------------------------------------------------
#ifndef __SPI_SLAVE_H
#define __SPI_SLAVE_H
//-------------------------------------------------------------------------------接口宏定义
#define SPI_SLAVE_SPIx                       SPI3
#define SPI_SLAVE_CS                         (GPIOA->IDR & GPIO_Pin_15)
#define SPI_SLAVE_INT_LOW                    GPIO_ResetBits(GPIOE, GPIO_Pin_0)
#define SPI_SLAVE_INT_HIGH                   GPIO_SetBits(GPIOE, GPIO_Pin_0)
//-------------------------------------------------------------------------------宏定义
//主端口
#define SPI_SLAVE_EXTEND_M_SPI_EN
#define SPI_SLAVE_EXTEND_M_UART1_EN
//#define SPI_SLAVE_EXTEND_M_UART2_EN
//#define SPI_SLAVE_EXTEND_M_UART3_EN
//#define SPI_SLAVE_EXTEND_M_UART4_EN
//#define SPI_SLAVE_EXTEND_M_UART5_EN
//控制端口
//#define SPI_SLAVE_EXTEND_S_UART1_EN
#define SPI_SLAVE_EXTEND_S_UART2_EN
//#define SPI_SLAVE_EXTEND_S_UART3_EN  /*暂时串口3与SPI从机有冲突,不可启动*/
#define SPI_SLAVE_EXTEND_S_UART4_EN
#define SPI_SLAVE_EXTEND_S_UART5_EN
#define SPI_SLAVE_EXTEND_S_RTC_EN
#define SPI_SLAVE_EXTEND_S_AD_EN
#define SPI_SLAVE_EXTEND_S_KIN_EN
#define SPI_SLAVE_EXTEND_S_KOUT_EN
//
#if (defined(SPI_SLAVE_EXTEND_S_UART1_EN)||defined(SPI_SLAVE_EXTEND_S_UART2_EN)||defined(SPI_SLAVE_EXTEND_S_UART3_EN)||defined(SPI_SLAVE_EXTEND_S_UART4_EN)||defined(SPI_SLAVE_EXTEND_S_UART5_EN))
#define SPI_SLAVE_EXTEND_S_UARTX_EN
#endif
//通讯协议最大长度
#define SPI_SLAVE_MAX_DATA 256
#define SPI_SLAVE_MAX_UART 5
//消息队列长度
#define SPI_SLAVE_QBUF_MAX  10
//-------------------------------------------------------------------------------数据类型
//从机待发送的数据类型准备标志
typedef struct SPI_SLAVE_S_STATE
{
    uint8_t UartRxNum[SPI_SLAVE_MAX_UART];
    uint8_t UartRxSize[SPI_SLAVE_MAX_UART];
} SPI_SLAVE_S_STATE;
typedef struct SPI_SLAVE_S_CONTROL
{
    uint16_t SysReset;
} SPI_SLAVE_S_CONTROL;
//-------------------------------------------------------------------------------输出函数
extern void App_SpiTaskCreate (void);
extern void Spi_Slave_1ms(void);
void SpiSlave_UartIrq_Rx1(uint8_t *pbuf,uint16_t len);
void SpiSlave_UartIrq_Rx2(uint8_t *pbuf,uint16_t len);
void SpiSlave_UartIrq_Rx3(uint8_t *pbuf,uint16_t len);
void SpiSlave_UartIrq_Rx4(uint8_t *pbuf,uint16_t len);
void SpiSlave_UartIrq_Rx5(uint8_t *pbuf,uint16_t len);
//
#endif
