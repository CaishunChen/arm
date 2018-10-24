/*
***********************************************************************************
*                    作    者: 徐松亮
*                    更新时间: 2015-06-03
***********************************************************************************
*/
#ifndef __SDFAT_H
#define __SDFAT_H
//-------------------加载库函数------------------------------
#include "ff.h"
#include "diskio.h"
//-------------------接口宏定义(硬件相关)--------------------
#if		(defined(NRF51)||defined(NRF52))
#include "nrf.h"
#include "bsp.h"
#include "ff.h"
#include "diskio_blkdev.h"
#include "nrf_block_dev_sdc.h"

//#define NRF_LOG_MODULE_NAME "APP"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#define FILE_NAME   "NORDIC.TXT"
#define TEST_STRING "SD card example.\r\n"

#define SDC_SCK_PIN     13 // ARDUINO_13_PIN  ///< SDC serial clock (SCK) pin.
#define SDC_MOSI_PIN    11 // ARDUINO_11_PIN  ///< SDC serial data in (DI) pin.
#define SDC_MISO_PIN    15 // ARDUINO_12_PIN  ///< SDC serial data out (DO) pin.
#define SDC_CS_PIN      18 // ARDUINO_10_PIN  ///< SDC chip select (CS) pin.
#endif
//-------------------接口宏定义(硬件无关)--------------------
#define	 MODULE_SDFAT_SD_PATH	"0:"
typedef enum MODULE_SDFAT_E_APP_CMD
{    
   //正常读写操作
   MODULE_SDFAT_E_APP_CMD_FILE_R = 0,
   MODULE_SDFAT_E_APP_CMD_FILE_W,
   MODULE_SDFAT_E_APP_CMD_FILE_ADD,
   MODULE_SDFAT_E_APP_CMD_FILE_GETSIZE,
   //特殊指令
   MODULE_SDFAT_E_APP_CMD_FORMAT,
}MODULE_SDFAT_E_APP_CMD;
//-------------------接口变量--------------------------------
extern FIL     ModuleSdFat_fsrc;
extern FRESULT ModuleSdFat_Res;
extern FATFS   ModuleSdFat_Fs;
//-------------------接口函数--------------------------------
extern void SdFat_1msPro(void);
extern uint8_t SdFat_App(MODULE_SDFAT_E_APP_CMD cmd,char *pPathFileName,void *pbuf,uint32_t *pPos,uint16_t *plen,uint32_t *pbw);
extern void SdFat_Test(uint8_t OnOff);
//-----------------------------------------------------------
#endif

