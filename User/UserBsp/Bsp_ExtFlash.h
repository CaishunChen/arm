/**
  ******************************************************************************
  * @file    bsp_ExtFlash.h 
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  * @brief   bottom-driven -->   基于SPI的FLASH.
  * @note    
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
  @endverbatim      
  ******************************************************************************
  * @attention
  *
  * GNU General Public License (GPL) 
  *
  * <h2><center>&copy; COPYRIGHT 2017 XSLXHN</center></h2>
  ******************************************************************************
  */ 
#ifndef __BSP_EXT_FLASH_H 
#define __BSP_EXT_FLASH_H
//-------------------加载库函数------------------------------
#include "includes.h"
//-------------------接口宏定义(硬件相关)--------------------
//CPU
#if   (defined(PROJECT_TCI_V30))
//---->
#define BSP_EXTFLASH_SPIX              SPI2
#define BSP_EXTFLASH_SPI_MOSI_PORT     GPIOB
#define BSP_EXTFLASH_SPI_MOSI_PIN      GPIO_Pin_15
#define BSP_EXTFLASH_SPI_MOSI_SOURCE   GPIO_PinSource15
#define BSP_EXTFLASH_SPI_MISO_PORT     GPIOB
#define BSP_EXTFLASH_SPI_MISO_PIN      GPIO_Pin_14
#define BSP_EXTFLASH_SPI_MISO_SOURCE   GPIO_PinSource14
#define BSP_EXTFLASH_SPI_SCLK_PORT     GPIOB
#define BSP_EXTFLASH_SPI_SCLK_PIN      GPIO_Pin_13
#define BSP_EXTFLASH_SPI_SCLK_SOURCE   GPIO_PinSource13
#define BSP_EXTFLASH_SPI_CS_PORT       GPIOB
#define BSP_EXTFLASH_SPI_CS_PIN        GPIO_Pin_12
#define BSP_EXTFLASH_SPI_CS_SOURCE     GPIO_PinSource12
#define BSP_EXTFLASH_SPI_WP_PORT       GPIOE
#define BSP_EXTFLASH_SPI_WP_PIN        GPIO_Pin_1
#define BSP_EXTFLASH_SPI_WP_SOURCE     GPIO_PinSource1
#if   (defined(STM32F1))
#define BSP_EXTFLASH_GPIO_RCC_ENABLE   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOE, ENABLE);
#define BSP_EXTFLASH_SPI_RCC_ENABLE    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
#define BSP_EXTFLASH_SPI_MOSI_REMAP
#define BSP_EXTFLASH_SPI_MISO_REMAP
#define BSP_EXTFLASH_SPI_SCLK_REMAP
#define BSP_EXTFLASH_SPI_CS_REMAP
#elif (defined(STM32F4))
#define BSP_EXTFLASH_GPIO_RCC_ENABLE   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOE, ENABLE);
#define BSP_EXTFLASH_SPI_RCC_ENABLE    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
#define BSP_EXTFLASH_SPI_MOSI_REMAP    GPIO_PinAFConfig(BSP_EXTFLASH_SPI_MOSI_PORT, BSP_EXTFLASH_SPI_MOSI_SOURCE, GPIO_AF_SPI2);
#define BSP_EXTFLASH_SPI_MISO_REMAP    GPIO_PinAFConfig(BSP_EXTFLASH_SPI_MISO_PORT, BSP_EXTFLASH_SPI_MISO_SOURCE, GPIO_AF_SPI2);
#define BSP_EXTFLASH_SPI_SCLK_REMAP    GPIO_PinAFConfig(BSP_EXTFLASH_SPI_SCLK_PORT, BSP_EXTFLASH_SPI_SCLK_SOURCE, GPIO_AF_SPI2);
#define BSP_EXTFLASH_SPI_CS_REMAP      
#endif
//<----
#elif (defined(PROJECT_XKAP_V3) \
||defined(XKAP_ICARE_B_D_M)\
|| (defined(BOOTLOADER)&&defined(STM32F1))\
|| (defined(BOOTLOADER)&&defined(STM32F4)))
//---->
#define BSP_EXTFLASH_SPIX              SPI1
#define BSP_EXTFLASH_SPI_MOSI_PORT     GPIOB
#define BSP_EXTFLASH_SPI_MOSI_PIN      GPIO_Pin_5
#define BSP_EXTFLASH_SPI_MOSI_SOURCE   GPIO_PinSource5
#define BSP_EXTFLASH_SPI_MISO_PORT     GPIOB
#define BSP_EXTFLASH_SPI_MISO_PIN      GPIO_Pin_4
#define BSP_EXTFLASH_SPI_MISO_SOURCE   GPIO_PinSource4
#define BSP_EXTFLASH_SPI_SCLK_PORT     GPIOB
#define BSP_EXTFLASH_SPI_SCLK_PIN      GPIO_Pin_3
#define BSP_EXTFLASH_SPI_SCLK_SOURCE   GPIO_PinSource3
#define BSP_EXTFLASH_SPI_CS_PORT       GPIOD
#define BSP_EXTFLASH_SPI_CS_PIN        GPIO_Pin_7
#define BSP_EXTFLASH_SPI_CS_SOURCE     GPIO_PinSource7
#define BSP_EXTFLASH_SPI_WP_PORT       GPIOE
#define BSP_EXTFLASH_SPI_WP_PIN        GPIO_Pin_1
#define BSP_EXTFLASH_SPI_WP_SOURCE     GPIO_PinSource1
#if   (defined(STM32F1))
#define BSP_EXTFLASH_GPIO_RCC_ENABLE   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE, ENABLE);
#define BSP_EXTFLASH_SPI_RCC_ENABLE    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
#define BSP_EXTFLASH_SPI_MOSI_REMAP
#define BSP_EXTFLASH_SPI_MISO_REMAP
#define BSP_EXTFLASH_SPI_SCLK_REMAP
#define BSP_EXTFLASH_SPI_CS_REMAP
#elif (defined(STM32F4))
#define BSP_EXTFLASH_GPIO_RCC_ENABLE   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE, ENABLE);
#define BSP_EXTFLASH_SPI_RCC_ENABLE    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
#define BSP_EXTFLASH_SPI_MOSI_REMAP    GPIO_PinAFConfig(BSP_EXTFLASH_SPI_MOSI_PORT, BSP_EXTFLASH_SPI_MOSI_SOURCE, GPIO_AF_SPI1);
#define BSP_EXTFLASH_SPI_MISO_REMAP    GPIO_PinAFConfig(BSP_EXTFLASH_SPI_MISO_PORT, BSP_EXTFLASH_SPI_MISO_SOURCE, GPIO_AF_SPI1);
#define BSP_EXTFLASH_SPI_SCLK_REMAP    GPIO_PinAFConfig(BSP_EXTFLASH_SPI_SCLK_PORT, BSP_EXTFLASH_SPI_SCLK_SOURCE, GPIO_AF_SPI1);
#define BSP_EXTFLASH_SPI_CS_REMAP 
#endif
//<----
#elif (defined(PROJECT_ARMFLY_V5_XSL))
//---->
#define BSP_EXTFLASH_SPIX              SPI1
#define BSP_EXTFLASH_SPI_MOSI_PORT     GPIOB
#define BSP_EXTFLASH_SPI_MOSI_PIN      GPIO_Pin_5
#define BSP_EXTFLASH_SPI_MOSI_SOURCE   GPIO_PinSource5
#define BSP_EXTFLASH_SPI_MISO_PORT     GPIOB
#define BSP_EXTFLASH_SPI_MISO_PIN      GPIO_Pin_4
#define BSP_EXTFLASH_SPI_MISO_SOURCE   GPIO_PinSource4
#define BSP_EXTFLASH_SPI_SCLK_PORT     GPIOB
#define BSP_EXTFLASH_SPI_SCLK_PIN      GPIO_Pin_3
#define BSP_EXTFLASH_SPI_SCLK_SOURCE   GPIO_PinSource3
#define BSP_EXTFLASH_SPI_CS_PORT       GPIOF
#define BSP_EXTFLASH_SPI_CS_PIN        GPIO_Pin_8
#define BSP_EXTFLASH_SPI_CS_SOURCE     GPIO_PinSource8
//#define BSP_EXTFLASH_SPI_WP_PORT       GPIOE
//#define BSP_EXTFLASH_SPI_WP_PIN        GPIO_Pin_1
//#define BSP_EXTFLASH_SPI_WP_SOURCE     GPIO_PinSource1
//
#define BSP_EXTFLASH_GPIO_RCC_ENABLE   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOF, ENABLE);
#define BSP_EXTFLASH_SPI_RCC_ENABLE    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
#define BSP_EXTFLASH_SPI_MOSI_REMAP    GPIO_PinAFConfig(BSP_EXTFLASH_SPI_MOSI_PORT, BSP_EXTFLASH_SPI_MOSI_SOURCE, GPIO_AF_SPI1);
#define BSP_EXTFLASH_SPI_MISO_REMAP    GPIO_PinAFConfig(BSP_EXTFLASH_SPI_MISO_PORT, BSP_EXTFLASH_SPI_MISO_SOURCE, GPIO_AF_SPI1);
#define BSP_EXTFLASH_SPI_SCLK_REMAP    GPIO_PinAFConfig(BSP_EXTFLASH_SPI_SCLK_PORT, BSP_EXTFLASH_SPI_SCLK_SOURCE, GPIO_AF_SPI1);
#define BSP_EXTFLASH_SPI_CS_REMAP 
//<----
#elif (defined(XKAP_ICARE_A_M))
//---->
#include "nrf_drv_spi.h"
#define BSP_EXTFLASH_SPI_MOSI_PIN      14
#define BSP_EXTFLASH_SPI_MISO_PIN      16
#define BSP_EXTFLASH_SPI_SCLK_PIN      13
#define BSP_EXTFLASH_SPI_CS_PIN        15
//<----
#elif (defined(XKAP_ICARE_B_M))
//---->
#include "nrf_drv_spi.h"
#if   (HARDWARE_SUB_VER==1)
#define BSP_EXTFLASH_SPI_MOSI_PIN      1
#define BSP_EXTFLASH_SPI_MISO_PIN      3
#define BSP_EXTFLASH_SPI_SCLK_PIN      4
#define BSP_EXTFLASH_SPI_CS_PIN        2
#elif (HARDWARE_SUB_VER==2)
#define BSP_EXTFLASH_SPI_MOSI_PIN      12
#define BSP_EXTFLASH_SPI_MISO_PIN      15
#define BSP_EXTFLASH_SPI_SCLK_PIN      13
#define BSP_EXTFLASH_SPI_CS_PIN        16
#elif (HARDWARE_SUB_VER==3)
#define BSP_EXTFLASH_SPI_MOSI_PIN      6
#define BSP_EXTFLASH_SPI_MISO_PIN      9
#define BSP_EXTFLASH_SPI_SCLK_PIN      7
#define BSP_EXTFLASH_SPI_CS_PIN        10
#endif
//<----
#elif (defined(BOOTLOADER)&&defined(NRF51))
//---->
#include "nrf_drv_spi.h"
#define BSP_EXTFLASH_SPI_MOSI_PIN      1
#define BSP_EXTFLASH_SPI_MISO_PIN      3
#define BSP_EXTFLASH_SPI_SCLK_PIN      4
#define BSP_EXTFLASH_SPI_CS_PIN        2
//<----
#elif (defined(BOOTLOADER)&&defined(NRF52))
//---->
#include "nrf_drv_spi.h"
#define BSP_EXTFLASH_SPI_MOSI_PIN      1
#define BSP_EXTFLASH_SPI_MISO_PIN      3
#define BSP_EXTFLASH_SPI_SCLK_PIN      4
#define BSP_EXTFLASH_SPI_CS_PIN        2
//<----
#elif (defined(TEST_NRF52_V1))
//---->
#include "nrf_drv_spi.h"
#define BSP_EXTFLASH_SPI_MOSI_PIN      11
#define BSP_EXTFLASH_SPI_MISO_PIN      15
#define BSP_EXTFLASH_SPI_SCLK_PIN      13
#define BSP_EXTFLASH_SPI_CS_PIN        14
//<----
#else
#error Please Set Project to Bsp_ExtFlash.h
#endif
//-------------------接口宏定义(硬件无关)--------------------
#if   (defined(STM32F1)||defined(STM32F4))
#define BSP_EXTFLASH_SPI_CS_LOW        GPIO_ResetBits(BSP_EXTFLASH_SPI_CS_PORT, BSP_EXTFLASH_SPI_CS_PIN)
#define BSP_EXTFLASH_SPI_CS_HIGH       GPIO_SetBits(BSP_EXTFLASH_SPI_CS_PORT, BSP_EXTFLASH_SPI_CS_PIN)
#ifdef BSP_EXTFLASH_SPI_WP_PORT
#define BSP_EXTFLASH_SPI_WP_LOW        GPIO_ResetBits(BSP_EXTFLASH_SPI_WP_PORT, BSP_EXTFLASH_SPI_WP_PIN)
#define BSP_EXTFLASH_SPI_WP_HIGH       GPIO_SetBits(BSP_EXTFLASH_SPI_WP_PORT, BSP_EXTFLASH_SPI_WP_PIN)
#else
#define BSP_EXTFLASH_SPI_WP_LOW
#define BSP_EXTFLASH_SPI_WP_HIGH
#endif
#elif (defined(NRF51)||defined(NRF52))
#define BSP_EXTFLASH_SPI_CS_LOW        nrf_gpio_pin_clear(BSP_EXTFLASH_SPI_CS_PIN); 
#define BSP_EXTFLASH_SPI_CS_HIGH       nrf_gpio_pin_set(BSP_EXTFLASH_SPI_CS_PIN);  
#define BSP_EXTFLASH_SPI_WP_LOW
#define BSP_EXTFLASH_SPI_WP_HIGH
#endif
enum
{
	BSP_EXTFLASH_SST25VF016B_ID = 0xBF2541,
   BSP_EXTFLASH_SST25VF064C_ID = 0xBF254B, 
	BSP_EXTFLASH_MX25L1606E_ID  = 0xC22015,
	BSP_EXTFLASH_W25Q80DV_ID    = 0xEF4014,
	BSP_EXTFLASH_W25Q16BV_ID    = 0xEF4015,
	BSP_EXTFLASH_W25Q32BV_ID    = 0xEF4016,
	BSP_EXTFLASH_W25Q64BV_ID    = 0xEF4017
};
typedef struct
{
	uint32_t ChipID;     // 芯片ID
	char ChipName[16];   // 芯片型号字符串
	uint32_t TotalSize;	//	总容量
	uint16_t PageSize;	// 页面大小
}BSP_EXTFLASH_S_FLASH;
//-------------------接口变量--------------------------------
extern BSP_EXTFLASH_S_FLASH BspExtFlash_s_Flash;
//-------------------接口函数--------------------------------
extern void    Bsp_ExtFlash_Init(void);
extern uint8_t Bsp_ExtFlash_Erase(uint32_t addr1,uint32_t addr2);
extern uint8_t Bsp_ExtFlash_Write_nByte(uint32_t Dst, uint8_t* SndbufPt,uint32_t NByte);
extern uint8_t Bsp_ExtFlash_Read_nByte(uint32_t Dst, uint8_t* RcvBufPt,uint32_t NByte);
#ifndef BOOTLOADER  
extern void    Bsp_ExtFlash_DebugTestOnOff(uint8_t OnOff);
#endif
//
#endif
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

