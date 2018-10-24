/*
***********************************************************************************
*                    作    者: 徐松亮
*                    更新时间: 2015-06-03
***********************************************************************************
*/
#ifndef __IAP_H
#define __IAP_H
//-------------------加载库函数------------------------------
#include "includes.h"
//-------------------宏定义----------------------------------
// 选择Flash
#if (defined(PROJECT_XKAP_V3)\
   ||defined(XKAP_ICARE_B_D_M) \
   || defined(PROJECT_ARMFLY_V5_XSL))
#define IAP_FLASH_SPI
#elif (defined(PROJECT_BASE_STM32F1)\
   || defined(PROJECT_BASE_STM32F4)\
   ||defined(PROJECT_TCI_V30)\
   ||defined(PROJECT_SPI_SLAVE))
#define IAP_FLASH_CPU
#else
#error Please Set Project to Bsp_ExtFlash.h
#endif
//
// 配置
// 最大支持1M
#define IAP_MAX_BYTE          (1024*1024L)
#define IAP_PACKET_SIZE       (1024L)
//-------------------数据类型--------------------------------
enum IAP_APP_CMD
{
    IAP_APP_CMD_DATA=0,
    IAP_APP_CMD_INIT,
};
//-------------------接口变量--------------------------------
extern uint16_t Iap_AllPackage,Iap_CurrentPackage;
//-------------------接口函数--------------------------------
extern void Iap_StmFlashPara_Write_Save(uint16_t offset_addr,uint8_t *buf,uint16_t len);
extern uint8_t IAP_APP(uint8_t cmd,uint8_t *buf,uint16_t len,uint16_t MaxPacket,uint16_t CurrentPacket,uint32_t Sum,uint8_t Ver,uint16_t* NextPacket);
//-----------------------------------------------------------
#endif

