/**
  ******************************************************************************
  * @file    Bsp_CpuFlash.h 
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  * @brief   bottom-driven -->   操作内部Flash.
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
         注意: STM32F1的内部Flash在写入之前必须要擦除,无论数据是不是FF
  @endverbatim      
  ******************************************************************************
  * @attention
  *
  * GNU General Public License (GPL) 
  *
  * <h2><center>&copy; COPYRIGHT 2017 XSLXHN</center></h2>
  ******************************************************************************
  */ 
#ifndef _BSP_CPU_FLASH_H
#define _BSP_CPU_FLASH_H
//-------------------加载库函数------------------------------
//-------------------接口宏定义(硬件相关)--------------------
#if			(PROJECT_BIB_TEST1)
//---->
#define BSP_CPU_FLASH_SIZE                   (128*1024)
#define  BSP_CPU_FLASH_BASE_ADDR             0x08000000
#define  BSP_CPU_FLASH_PARA_ADDR             (0x08000000+124*1024)
#define  BSP_CPU_FLASH_PARA_ADDR_END         (0x08000000+126*1024-1)
#define  BSP_CPU_FLASH_PARA_BACKUP_ADDR      (0x08000000+126*1024)
#define  BSP_CPU_FLASH_PARA_BACKUP_ADDR_END  (0x08000000+128*1024-1)
#define  BSP_CPU_FLASH_APP_ADDR              0x08000000
#define  BSP_CPU_FLASH_APP_BACKUP_ADDR       BSP_CPU_FLASH_APP_ADDR+((BSP_CPU_FLASH_SIZE-(BSP_CPU_FLASH_APP_ADDR-BSP_CPU_FLASH_BASE_ADDR))/2)
//中断向量
#define  BSP_CPU_FLASH_SET_VECTOR_TABLE      NVIC_SetVectorTable(NVIC_VectTab_FLASH, BSP_CPU_FLASH_APP_ADDR-BSP_CPU_FLASH_BASE_ADDR);
//<----
#elif   (defined(STM32F1))
//---->
#define  BSP_CPU_FLASH_SIZE                   (1*512*1024)
#define  BSP_CPU_FLASH_BASE_ADDR             0x08000000
#define  BSP_CPU_FLASH_PARA_ADDR             0x08004000           // 16k-18k 参数存储
#define  BSP_CPU_FLASH_PARA_ADDR_END         (0x08004800-1)
#define  BSP_CPU_FLASH_PARA_BACKUP_ADDR      0x08004800           // 18k-20k 参数修改
#define  BSP_CPU_FLASH_PARA_BACKUP_ADDR_END  (0x08005000-1)
#define  BSP_CPU_FLASH_APP_ADDR              0x08005000           // 20k
#define  BSP_CPU_FLASH_APP_BACKUP_ADDR       BSP_CPU_FLASH_APP_ADDR+((BSP_CPU_FLASH_SIZE-(BSP_CPU_FLASH_APP_ADDR-BSP_CPU_FLASH_BASE_ADDR))/2)
#define  BSP_CPU_FLASH_END_ADDR              (BSP_CPU_FLASH_BASE_ADDR+BSP_CPU_FLASH_SIZE-1)
//存储地址
#define  BSP_CPU_FLASH_SAVE_ADDR             (BSP_CPU_FLASH_END_ADDR+1-3*2048)
#define  BSP_CPU_FLASH_SAVE_BACKUP_ADDR      (BSP_CPU_FLASH_END_ADDR+1-1*2048)
//中断向量
#define  BSP_CPU_FLASH_SET_VECTOR_TABLE      NVIC_SetVectorTable(NVIC_VectTab_FLASH, BSP_CPU_FLASH_APP_ADDR-BSP_CPU_FLASH_BASE_ADDR);
//<----
#elif (defined(STM32F4))
//---->
#define BSP_CPU_FLASH_SIZE                   (1*512*1024)
#define  BSP_CPU_FLASH_BASE_ADDR             0x08000000
#define  BSP_CPU_FLASH_PARA_ADDR             0x08004000           // 16k-32k 参数存储
#define  BSP_CPU_FLASH_PARA_ADDR_END         (0x08008000-1)
#define  BSP_CPU_FLASH_PARA_BACKUP_ADDR      0x08008000           // 32k-48k 参数修改
#define  BSP_CPU_FLASH_PARA_BACKUP_ADDR_END  (0x0800C000-1)
#define  BSP_CPU_FLASH_APP_ADDR              0x0800C000           // 48k-256k(应用程序不可超过212k)
#define  BSP_CPU_FLASH_APP_BACKUP_ADDR       0x08040000           // 256k-512k
#define  BSP_CPU_FLASH_END_ADDR              (BSP_CPU_FLASH_BASE_ADDR+BSP_CPU_FLASH_SIZE-1)
//地址分配(扇区地址)
#define  BSP_CPU_FLASH_ADDR_SECTOR_0         ((uint32_t)0x08000000)   // Base @ of Sector 0, 16 Kbytes
#define  BSP_CPU_FLASH_ADDR_SECTOR_1         ((uint32_t)0x08004000)   // Base @ of Sector 1, 16 Kbytes 
#define  BSP_CPU_FLASH_ADDR_SECTOR_2         ((uint32_t)0x08008000)   // Base @ of Sector 2, 16 Kbytes 
#define  BSP_CPU_FLASH_ADDR_SECTOR_3         ((uint32_t)0x0800C000)   // Base @ of Sector 3, 16 Kbytes 
#define  BSP_CPU_FLASH_ADDR_SECTOR_4         ((uint32_t)0x08010000)   // Base @ of Sector 4, 64 Kbytes 
#define  BSP_CPU_FLASH_ADDR_SECTOR_5         ((uint32_t)0x08020000)   // Base @ of Sector 5, 128 Kbytes 
#define  BSP_CPU_FLASH_ADDR_SECTOR_6         ((uint32_t)0x08040000)   // Base @ of Sector 6, 128 Kbytes 
#define  BSP_CPU_FLASH_ADDR_SECTOR_7         ((uint32_t)0x08060000)   // Base @ of Sector 7, 128 Kbytes 
#define  BSP_CPU_FLASH_ADDR_SECTOR_8         ((uint32_t)0x08080000)   // Base @ of Sector 8, 128 Kbytes 
#define  BSP_CPU_FLASH_ADDR_SECTOR_9         ((uint32_t)0x080A0000)   // Base @ of Sector 9, 128 Kbytes 
#define  BSP_CPU_FLASH_ADDR_SECTOR_10        ((uint32_t)0x080C0000)   // Base @ of Sector 10, 128 Kbytes
#define  BSP_CPU_FLASH_ADDR_SECTOR_11        ((uint32_t)0x080E0000)   // Base @ of Sector 11, 128 Kbytes
//中断向量
#define  BSP_CPU_FLASH_SET_VECTOR_TABLE      NVIC_SetVectorTable(NVIC_VectTab_FLASH, BSP_CPU_FLASH_APP_ADDR-BSP_CPU_FLASH_BASE_ADDR);
//<----
#elif (defined(NRF51)||defined(NRF52))
//---->
#define  BSP_CPU_FLASH_SIZE                  ((NRF_FICR->CODESIZE)*(NRF_FICR->CODEPAGESIZE))
#define  BSP_CPU_FLASH_BASE_ADDR             0
#define  BSP_CPU_FLASH_PARA_ADDR             ((NRF_FICR->CODESIZE-2)*(NRF_FICR->CODEPAGESIZE))
#define  BSP_CPU_FLASH_PARA_ADDR_END         ((NRF_FICR->CODESIZE-1)*(NRF_FICR->CODEPAGESIZE)-1)
#define  BSP_CPU_FLASH_PARA_BACKUP_ADDR      ((NRF_FICR->CODESIZE-1)*(NRF_FICR->CODEPAGESIZE))
#define  BSP_CPU_FLASH_PARA_BACKUP_ADDR_END  ((NRF_FICR->CODESIZE-0)*(NRF_FICR->CODEPAGESIZE)-1)
#define  BSP_CPU_FLASH_APP_ADDR              0
#define  BSP_CPU_FLASH_APP_BACKUP_ADDR       ((NRF_FICR->CODESIZE-3)*(NRF_FICR->CODEPAGESIZE))
#define  BSP_CPU_FLASH_END_ADDR              (BSP_CPU_FLASH_BASE_ADDR+BSP_CPU_FLASH_SIZE-1)
//中断向量
#define  BSP_CPU_FLASH_SET_VECTOR_TABLE
//<----
#endif
//-------------------接口宏定义(硬件无关)--------------------
#define  BSP_CPU_FLASH_IS_EQU                 0                 // Flash内容和待写入的数据相等，不需要擦除和写操作
#define  BSP_CPU_FLASH_REQ_WRITE              1                 // Flash不需要擦除，直接写
#define  BSP_CPU_FLASH_REQ_ERASE              2                 // Flash需要先擦除,再写
#define  BSP_CPU_FLASH_PARAM_ERR              3                 // 函数参数错误
//-------------------数据类型--------------------------------
//-------------------接口变量--------------------------------
//-------------------接口函数--------------------------------
//Flash 底层
extern void Bsp_CpuFlash_Init(void);
extern void Bsp_CpuFlash_Erase(uint32_t FlashBeginAddr,uint32_t FlashEndAddr);
extern uint8_t  Bsp_CpuFlash_Read(uint32_t _ulFlashAddr, uint8_t *_ucpDst, uint32_t _ulSize);
extern uint8_t  Bsp_CpuFlash_Write(uint32_t _ulFlashAddr, uint8_t *_ucpSrc, uint32_t _ulSize);
extern uint8_t  Bsp_CpuFlash_Cmp(uint32_t _ulFlashAddr, uint8_t *_ucpBuf, uint32_t _ulSize);
extern void Bsp_CpuFlash_DebugTestOnOff(uint8_t OnOff);
//参数
#ifndef BOOTLOADER
extern void Bsp_CpuFlash_HardFault(void);
#endif
//-----------------------------------------------------------
#endif

