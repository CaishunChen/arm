/**
  ******************************************************************************
  * @file    Bsp_Relay.h 
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  * @brief   bottom-driven -->   继电器驱动(包含延时单稳态与多权限).
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
//-------------------------------------------------------------------------------
#ifndef __BSP_RELAY_H
#define __BSP_RELAY_H
//-------------------加载库函数------------------------------
#include "includes.h"
//-------------------接口宏定义(硬件相关)--------------------
#if   (defined(PROJECT_TCI_V30)||defined(PROJECT_SPI_SLAVE))
//---->
//时钟
#if   (defined(STM32F1))
#define BSP_RELAY_RCC_ENABLE  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOG , ENABLE);
#elif (defined(STM32F4))
#define BSP_RELAY_RCC_ENABLE  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOE|RCC_APB2Periph_GPIOG , ENABLE);
#endif
//端口
#define BSP_RELAY_RELAY1_PORT GPIOE
#define BSP_RELAY_RELAY1_PIN  GPIO_Pin_2
#define BSP_RELAY_RELAY2_PORT GPIOE
#define BSP_RELAY_RELAY2_PIN  GPIO_Pin_1
#define BSP_RELAY_RELAY3_PORT GPIOG
#define BSP_RELAY_RELAY3_PIN  GPIO_Pin_15
#define BSP_RELAY_RELAY4_PORT GPIOG
#define BSP_RELAY_RELAY4_PIN  GPIO_Pin_14
//<----
#else
#error Please Set Project to Bsp_Relay.h
#endif
//-------------------接口宏定义(硬件无关)--------------------
#define BSP_RELAY_MAX_NUM 8
//
#define BSP_RELAY_ON  1
#define BSP_RELAY_OFF 0
//继电器跳变是否产生记录
//#define RELAY_RECORD_ENABLE
//
enum RELAY_LEVEL_ENUM
{
    BSP_RELAY_LEVEL_LOW=0,
    BSP_RELAY_LEVEL_1,  
    BSP_RELAY_LEVEL_2,  
    BSP_RELAY_LEVEL_3,  
    BSP_RELAY_LEVEL_4,  
    BSP_RELAY_LEVEL_5,  
    // -----门控器专用-----Begin
    BSP_RELAY_LEVEL_LOGIC,      //刷卡开门
    BSP_RELAY_LEVEL_FIRSTCARD,  //首卡开门
    BSP_RELAY_LEVEL_PC,         //平台强行开门
    // -----门控器专用-----End
    BSP_RELAY_LEVEL_DEGUG,
    BSP_RELAY_LEVEL_MENU,
    BSP_RELAY_LEVEL_HIGH=255,
};
//
enum RELAY_WIRTE_ERR_ENUM
{
    BSP_RELAY_WIRTE_ERR_NO=0,
    BSP_RELAY_WIRTE_ERR_PARA,
    BSP_RELAY_WIRTE_ERR_LEVEL,
};
//
#ifndef  BSP_RELAY1_SAFE_MAX_TIME_100MS
#define  BSP_RELAY1_SAFE_MAX_TIME_100MS   0xFFFFFFFF
#endif
#ifndef  BSP_RELAY2_SAFE_MAX_TIME_100MS
#define  BSP_RELAY2_SAFE_MAX_TIME_100MS   0xFFFFFFFF
#endif
#ifndef  BSP_RELAY3_SAFE_MAX_TIME_100MS
#define  BSP_RELAY3_SAFE_MAX_TIME_100MS   0xFFFFFFFF
#endif
#ifndef  BSP_RELAY4_SAFE_MAX_TIME_100MS
#define  BSP_RELAY4_SAFE_MAX_TIME_100MS   0xFFFFFFFF
#endif
#ifndef  BSP_RELAY5_SAFE_MAX_TIME_100MS
#define  BSP_RELAY5_SAFE_MAX_TIME_100MS   0xFFFFFFFF
#endif
#ifndef  BSP_RELAY6_SAFE_MAX_TIME_100MS
#define  BSP_RELAY6_SAFE_MAX_TIME_100MS   0xFFFFFFFF
#endif
#ifndef  BSP_RELAY7_SAFE_MAX_TIME_100MS
#define  BSP_RELAY7_SAFE_MAX_TIME_100MS   0xFFFFFFFF
#endif
#ifndef  BSP_RELAY8_SAFE_MAX_TIME_100MS
#define  BSP_RELAY8_SAFE_MAX_TIME_100MS   0xFFFFFFFF
#endif

#ifdef   BSP_RELAY_RELAY1_PIN
#define  BSP_RELAY_RELAY1_ON   GPIO_SetBits(BSP_RELAY_RELAY1_PORT   , BSP_RELAY_RELAY1_PIN)
#define  BSP_RELAY_RELAY1_OFF  GPIO_ResetBits(BSP_RELAY_RELAY1_PORT , BSP_RELAY_RELAY1_PIN)
#endif
#ifdef   BSP_RELAY_RELAY2_PIN
#define  BSP_RELAY_RELAY2_ON   GPIO_SetBits(BSP_RELAY_RELAY2_PORT   , BSP_RELAY_RELAY2_PIN)
#define  BSP_RELAY_RELAY2_OFF  GPIO_ResetBits(BSP_RELAY_RELAY2_PORT , BSP_RELAY_RELAY2_PIN)
#endif
#ifdef   BSP_RELAY_RELAY3_PIN
#define  BSP_RELAY_RELAY3_ON   GPIO_SetBits(BSP_RELAY_RELAY3_PORT   , BSP_RELAY_RELAY3_PIN)
#define  BSP_RELAY_RELAY3_OFF  GPIO_ResetBits(BSP_RELAY_RELAY3_PORT , BSP_RELAY_RELAY3_PIN)
#endif
#ifdef   BSP_RELAY_RELAY4_PIN
#define  BSP_RELAY_RELAY4_ON   GPIO_SetBits(BSP_RELAY_RELAY4_PORT   , BSP_RELAY_RELAY4_PIN)
#define  BSP_RELAY_RELAY4_OFF  GPIO_ResetBits(BSP_RELAY_RELAY4_PORT , BSP_RELAY_RELAY4_PIN)
#endif
#ifdef   BSP_RELAY_RELAY5_PIN
#define  BSP_RELAY_RELAY5_ON   GPIO_SetBits(BSP_RELAY_RELAY5_PORT   , BSP_RELAY_RELAY5_PIN)
#define  BSP_RELAY_RELAY5_OFF  GPIO_ResetBits(BSP_RELAY_RELAY5_PORT , BSP_RELAY_RELAY5_PIN)
#endif
#ifdef   BSP_RELAY_RELAY6_PIN
#define  BSP_RELAY_RELAY6_ON   GPIO_SetBits(BSP_RELAY_RELAY6_PORT   , BSP_RELAY_RELAY6_PIN)
#define  BSP_RELAY_RELAY6_OFF  GPIO_ResetBits(BSP_RELAY_RELAY6_PORT , BSP_RELAY_RELAY6_PIN)
#endif
#ifdef   BSP_RELAY_RELAY7_PIN
#define  BSP_RELAY_RELAY7_ON   GPIO_SetBits(BSP_RELAY_RELAY7_PORT   , BSP_RELAY_RELAY7_PIN)
#define  BSP_RELAY_RELAY7_OFF  GPIO_ResetBits(BSP_RELAY_RELAY7_PORT , BSP_RELAY_RELAY7_PIN)
#endif
#ifdef   BSP_RELAY_RELAY8_PIN
#define  BSP_RELAY_RELAY8_ON   GPIO_SetBits(BSP_RELAY_RELAY8_PORT   , BSP_RELAY_RELAY8_PIN)
#define  BSP_RELAY_RELAY8_OFF  GPIO_ResetBits(BSP_RELAY_RELAY8_PORT , BSP_RELAY_RELAY8_PIN)
#endif
//-------------------------------------------------------------------------------输出变量
extern uint8_t Relay_PcSign[BSP_RELAY_MAX_NUM];       //0-无效,1-强开,2-强闭,3-临时开,4-临时闭
extern uint8_t Relay_LinkageSign[BSP_RELAY_MAX_NUM];  //继电器需要报联动信息的标志(0-不需要上报,1-需要上报)
//-------------------------------------------------------------------------------输出函数
extern void BspRelay_100ms(void);
extern uint8_t BspRelay_Write(uint8_t level,uint8_t ch,uint32_t time,uint16_t delaytime);
extern uint8_t BspRelay_Read(void);
extern uint8_t BspRelay_Test(void);
extern void BspRelay_DebugTestOnOff(uint8_t OnOff);

#ifdef RELAY_RECORD_ENABLE
extern void Relay_Record(S_MEMORY_ALARM_RECORD* p_record);
#endif
//
#endif
