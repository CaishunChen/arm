/**
  ******************************************************************************
  * @file    bsp_FsmcSram.h 
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  * @brief   bottom-driven -->   基于Fsmc的SRAM驱动.
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
         标配的SRAM为 IS61WV102416BLL-10TL  容量2M字节，16Bit，10ns速度
  @endverbatim      
  ******************************************************************************
  * @attention
  *
  * GNU General Public License (GPL) 
  *
  * <h2><center>&copy; COPYRIGHT 2017 XSLXHN</center></h2>
  ******************************************************************************
  */  


#ifndef _BSP_FSMC_SRAM_H
#define _BSP_FSMC_SRAM_H

#include "includes.h"

#define EXT_SRAM_ADDR  	((uint32_t)0x68000000)
#define EXT_SRAM_SIZE	(2L * 1024 * 1024)

void BspFsmcSram_Init(void);
uint8_t BspFsmcSram_Test(void);
void BspFsmcSram_DebugTestOnOff(uint8_t OnOff);
#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
