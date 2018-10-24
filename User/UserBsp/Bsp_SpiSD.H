/**
  ******************************************************************************
  * @file    Bsp_Led.h
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  * @brief   Bsp-SpiSD -->   SD/TF.
  * @note    对内部Flash与外部Flash的统一存储管理
  * @verbatim

 ===============================================================================
                     ##### How to use this driver #####
 ===============================================================================
   1,    适用芯片
         STM      :  STM32F1  STM32F4
         Nordic   :  Nrf51    Nrf52
   2,    移植步骤
   3,    验证方法
   4,    使用方法
   5,    其他说明
         无
  @endverbatim
  ******************************************************************************
  * @attention
  *
  * GNU General Public License (GPL)
  *
  * <h2><center>&copy; COPYRIGHT 2017 XSLXHN</center></h2>
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef _MMC_SD_H_
#define _MMC_SD_H_		 
#include "nrf.h"	 
#include "nrf_spi.h" 
#include "nrf_gpio.h" 
 						    	 
// SD卡类型定义  
#define SD_TYPE_ERR     0X00
#define SD_TYPE_MMC     0X01
#define SD_TYPE_V1      0X02
#define SD_TYPE_V2      0X04
#define SD_TYPE_V2HC    0X06	   
// SD卡指令表  	   
#define CMD0    0       //卡复位
#define CMD1    1
#define CMD8    8       //命令8 ，SEND_IF_COND
#define CMD9    9       //命令9 ，读CSD数据
#define CMD10   10      //命令10，读CID数据
#define CMD12   12      //命令12，停止数据传输
#define CMD16   16      //命令16，设置SectorSize 应返回0x00
#define CMD17   17      //命令17，读sector
#define CMD18   18      //命令18，读Multi sector
#define CMD23   23      //命令23，设置多sector写入前预先擦除N个block
#define CMD24   24      //命令24，写sector
#define CMD25   25      //命令25，写Multi sector
#define CMD41   41      //命令41，应返回0x00
#define CMD55   55      //命令55，应返回0x01
#define CMD58   58      //命令58，读OCR信息
#define CMD59   59      //命令59，使能/禁止CRC，应返回0x00
//数据写入回应字意义
#define MSD_DATA_OK                0x05
#define MSD_DATA_CRC_ERROR         0x0B
#define MSD_DATA_WRITE_ERROR       0x0D
#define MSD_DATA_OTHER_ERROR       0xFF
//SD卡回应标记字
#define MSD_RESPONSE_NO_ERROR      0x00
#define MSD_IN_IDLE_STATE          0x01
#define MSD_ERASE_RESET            0x02
#define MSD_ILLEGAL_COMMAND        0x04
#define MSD_COM_CRC_ERROR          0x08
#define MSD_ERASE_SEQUENCE_ERROR   0x10
#define MSD_ADDRESS_ERROR          0x20
#define MSD_PARAMETER_ERROR        0x40
#define MSD_RESPONSE_FAILURE       0xFF
 							   						 	 
//这部分应根据具体的连线来修改!

#if (defined(TEST_NRF52_V1)) 	    	  
#define SD_MOSI						11
#define SD_MISO						15
#define SD_SCLK						13
#define SD_CS						14	
#else
#error Please Set Project to Bsp_SpiSD.h
#endif


#define SD_CS_1 nrf_gpio_pin_set(SD_CS)
#define SD_CS_0 nrf_gpio_pin_clear(SD_CS)

extern uint8_t  SD_Type;//SD卡的类型
//函数申明区 
uint8_t SD_WaitReady(void);							//等待SD卡准备
uint8_t SD_GetResponse(uint8_t Response);					//获得相应
uint8_t SD_Initialize(void);							//初始化
uint8_t SD_ReadDisk(uint8_t*buf,uint32_t sector,uint8_t cnt);		//读块
uint8_t SD_WriteDisk(uint8_t*buf,uint32_t sector,uint8_t cnt);		//写块
uint32_t SD_GetSectorCount(void);   					//读扇区数
uint32_t SD_GetCapacity(void);								////返回容量，单位为MB
uint8_t SD_GetCID(uint8_t *cid_data);                     //读SD卡CID
uint8_t SD_GetCSD(uint8_t *csd_data);                     //读SD卡CSD
 
 
void SD_SPI_SpeedHigh(void); 
void SD_SPI_SpeedLow(void);
uint8_t SD_SPI_ReadWriteByte(uint8_t data);
#endif




