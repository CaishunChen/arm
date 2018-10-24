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
#ifndef __SPI_MASTER_H
#define __SPI_MASTER_H
//-------------------------------------------------------------------------------宏定义
//系统选择
//#define OS_LINUX
//#define OS_UCOSII
//#define OS_UCOSIII
//调试输出
#define SPI_MASTER_DEBUG_ENABLE
//-------------------------------------------------------------------------------库
#ifdef SPI_MASTER_OS_LINUX
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <string.h>
#else
#include "includes.h"
#include "uctsk_Debug.h"
#include "Bsp_Uart.h"
#endif
//-------------------------------------------------------------------------------数据类型
//
#ifndef U_IAP_PARA
typedef struct S_IAP_UART_SET
{
    uint8_t Bsp;       // 1-300 2-600 3-1200   4-2400   5-4800   6-9600   7-19200  8-38400  9-57600  10-115200
    uint8_t DataBit;   // 5/6/7/8
    uint8_t StopBit;   // 0/1
    uint8_t Parity;    // 0/1/2
    uint8_t AutoTxNum; // 重发次数
    uint8_t res;       // 备用
} S_IAP_UART_SET;
typedef union U_IAP_PARA
{
    uint8_t setcs[128];      //2044
    struct
    {
        //下面区域固定32字节
        uint8_t  ParaVer[8];  //参数版本:"PARA-Vxx"
        uint8_t  Updata[6];   //升级标志:"UPDATA"
        uint16_t RtcAlarm1;   //闹钟
        uint8_t  RtcFormat;   //时钟表达方式(0->24制式,1->12制式)
        uint8_t  LcdBackLight;//液晶背光
        uint16_t Rfms_SubThreshold; //差分阈值
        uint8_t  res1[32-8-6-2-1-1-2];
        //下面区域暂定32个字节
        uint32_t UserParaState; // 0x12345678---参数区有效 其他无效
        uint8_t  Addr;          // 设备地址
        S_IAP_UART_SET  UartSetBuf[5];   
        //INT8U  res2[32-4-1-8];
        //-----
    } data;
} U_IAP_PARA;
#endif
//
#ifndef ERR
#define ERR       1
#define OK        0
#endif
//
#ifdef SPI_MASTER_OS_LINUX
#define SPI_SLAVE_MAX_UART 5
typedef struct SPI_SLAVE_S_STATE
{
    uint8_t UartRxNum[SPI_SLAVE_MAX_UART];
    uint8_t UartRxSize[SPI_SLAVE_MAX_UART];
} SPI_SLAVE_S_STATE;
#endif
//-------------------------------------------------------------------------------接口宏定义
#ifdef SPI_MASTER_OS_LINUX
#define SPI_MASTER_CS_LOW()
#define SPI_MASTER_CS_HIGH()
#else
#define SPI_MASTER_SPI_CH                     2
#endif

#if (SPI_MASTER_SPI_CH==2)
#define SPI_MASTER_SPIx                       SPI2
#define SPI_MASTER_CS_LOW()                   //GPIO_ResetBits(GPIOB, GPIO_Pin_12)
#define SPI_MASTER_CS_HIGH()                  //GPIO_SetBits(GPIOB, GPIO_Pin_12)
#elif (SPI_MASTER_SPI_CH==3)
#define SPI_MASTER_SPIx                       SPI3
#define SPI_MASTER_CS_LOW()                   //GPIO_ResetBits(GPIOA, GPIO_Pin_15)
#define SPI_MASTER_CS_HIGH()                  //GPIO_SetBits(GPIOA, GPIO_Pin_15)
#endif
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------输出函数
//底层函数
extern void Spi_MasterIntExtIRQHandler(void);
extern void SpiMaster_DebugTestOnOff(uint8_t OnOff);
//应用层函数
extern uint8_t Spi_MasterApp(uint8_t SlaveAddr,uint8_t cmd,uint16_t regAddr,void *pBuf,uint16_t *pregByteLen);
//
#endif
