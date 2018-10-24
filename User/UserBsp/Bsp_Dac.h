/**
  ******************************************************************************
  * @file    bsp_Dac.h 
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  * @brief   bottom-driven -->   芯片内部DAC驱动.
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
   			 DAC特性:
				 1、两路
				 2、12bit时数据可被配置成左对齐或右对齐
				 3、具有同步更新能力
				 4、噪声产生
				 5、三角波产生
				 6、两个通道独立转换或同步转换
				 7、每个通道都有DMA能力
				 8、DMA出错检测
				 9、可外部触发
				 可以设置成定时器触发，TIM6和TIM7 是专公DAC转换用的定时器。初始化也很方便。
  @endverbatim      
  ******************************************************************************
  * @attention
  *
  * GNU General Public License (GPL) 
  *
  * <h2><center>&copy; COPYRIGHT 2017 XSLXHN</center></h2>
  ******************************************************************************
  */ 
#ifndef __BSP_DAC_H
#define __BSP_DAC_H
//-------------------加载库函数------------------------------
#include "includes.h"
//-------------------接口宏定义(硬件相关)--------------------
//基准源设定
#define BSP_DAC_REF_MV           3300
//DAC1
#if   (defined(STM32F1))
#define BSP_DAC1_GPIO_RCC_ENABLE RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA , ENABLE);
#define BSP_DAC1_RCC_ENABLE      RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC,ENABLE);
#elif (defined(STM32F4))
#define BSP_DAC1_GPIO_RCC_ENABLE RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA , ENABLE);
#define BSP_DAC1_RCC_ENABLE      RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC,ENABLE);
#define BSP_DAC1_RCC_DISABLE     RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC,DISABLE);
#endif
#define BSP_DAC1_PORT            GPIOA
#define BSP_DAC1_PIN             GPIO_Pin_4
#define BSP_DAC1_CH              DAC_Channel_1
#define BSP_DAC1_SET_FUN         DAC_SetChannel1Data
//DAC2
#if   (defined(STM32F1))
#define BSP_DAC2_GPIO_RCC_ENABLE RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA , ENABLE);
#define BSP_DAC2_RCC_ENABLE      RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC,ENABLE);
#elif (defined(STM32F4))
#define BSP_DAC2_GPIO_RCC_ENABLE RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA , ENABLE);
#define BSP_DAC2_RCC_ENABLE      RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC,ENABLE);
#define BSP_DAC2_RCC_DISABLE     RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC,DISABLE);
#endif
#define BSP_DAC2_PORT            GPIOA
#define BSP_DAC2_PIN             GPIO_Pin_5
#define BSP_DAC2_CH              DAC_Channel_2
#define BSP_DAC2_SET_FUN         DAC_SetChannel2Data
//-------------------接口函数--------------------------------
extern void BspDac_Init(uint8_t ch);
extern void BspDac_DeInit(uint8_t ch);
extern void BspDac_Write(uint8_t ch,uint16_t mv);
//-------------------------------------------------------------------------------输出函数
#endif
