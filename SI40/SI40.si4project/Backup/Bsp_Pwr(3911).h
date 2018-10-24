/**
  ******************************************************************************
  * @file    Bsp_Pwr.h 
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  * @brief   bottom-driven -->   电源管理驱动.
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
#ifndef __BSP_PWR_H
#define __BSP_PWR_H
//-------------------数据结构--------------------------------
typedef enum BSP_PWR_E_RESET_FLAG
{
   // 掉电复位
   BSP_PWR_E_RESET_FLAG_SFTRST=0,
   // POR复位
   BSP_PWR_E_RESET_FLAG_PORRST,
   // 引脚复位
   BSP_PWR_E_RESET_FLAG_PINRST,
   // 独立看门狗复位
   BSP_PWR_E_RESET_FLAG_IWDGRST,
   // 窗口看门狗复位
   BSP_PWR_E_RESET_FLAG_WWDGRST,
   // 低压复位
   BSP_PWR_E_RESET_FLAG_LPWRRST,
   // BOR复位
   BSP_PWR_E_RESET_FLAG_BORRST,
}BSP_PWR_E_RESET_FLAG;
typedef enum BSP_PWR_E_POWER
{
   BSP_PWR_E_POWER_NULL=0,
   BSP_PWR_E_POWER_AC,
   BSP_PWR_E_POWER_BAT_FULL,
   BSP_PWR_E_POWER_BAT_MID,
   BSP_PWR_E_POWER_LOW,
}BSP_PWR_E_POWER;
//-----------------------------------------------------------
// LED
#define  BSP_PWR_CMD_LED               0
#define  BSP_PWR_CMD_LED_NUM_1         0
#define  BSP_PWR_CMD_LED_NUM_2         1
#define  BSP_PWR_CMD_LED_NUM_3         2
#define  BSP_PWR_CMD_LED_PARA_ON       0
#define  BSP_PWR_CMD_LED_PARA_OFF      1
#define  BSP_PWR_CMD_LED_PARA_TOGGLE   2
#define  BSP_PWR_CMD_LED_PARA_ONOFF    3
// KEY
#define  BSP_PWR_CMD_KEY               1
#define  BSP_PWR_CMD_KEY_NUM_1         0
#define  BSP_PWR_CMD_KEY_NUM_2         1
#define  BSP_PWR_CMD_KEY_NUM_3         2
#define  BSP_PWR_CMD_KEY_NUM_4         3
// BMP180
#define  BSP_PWR_CMD_BMP180            2
#define  BSP_PWR_CMD_BMP180_GET        0
// VCC
#define  BSP_PWR_CMD_VCC               3
// 周期性执行
#define  BSP_PWR_CMD_TIMER			   4
//
typedef struct BSP_PWR_S_CMD
{
   uint8_t cmd;
   uint8_t num;
   uint16_t para1;
   void *pPara;
}BSP_PWR_S_CMD;
//-------------------加载库函数------------------------------
#if   (defined(STM32F1)||defined(STM32F4))
#include "Bsp_BkpRam.h"
#include "Bsp_Rtc.h"
#if   (defined(PROJECT_XKAP_V3)||defined(XKAP_ICARE_B_D_M))
#include "uctsk_GprsNet.h"
#include "uctsk_RFMS.h"
#include "Bsp_TempHumidSensor.h"
#include "bsp_tft_lcd.h"
#include "Bsp_Exti.h"
#include "LCD_ILI9341.h"
#include "uctsk_AD.h"
#include "Bsp_Dac.h"
extern struct tm CurrentDate;
#endif
#elif (defined(NRF51)||defined(NRF52))
#if   	(defined(PROJECT_NRF5X_BLE))
#include "Bsp_Led.h"
#include "uctsk_HCI.h"
#include "uctsk_AD.h"
#define  BSP_PWR_CMD_ENABLE
#elif	(defined(XKAP_ICARE_A_C))
#include "Bsp_Led.h"
#include "Bsp_Key.h"
#include "Bsp_NrfRfEsb.h"
#define  BSP_PWR_CMD_ENABLE
#endif
#endif
//-------------------接口宏定义(硬件相关)--------------------
//-------------------接口宏定义(硬件无关)--------------------
#define BSP_PWR_AC_MV         4000
#define BSP_PWR_BAT_FULL_MV   4000
#define BSP_PWR_BAT_MID_MV    3500
#define BSP_PWR_BAT_LOW_MV    3000
//
#define BSP_PWR_LIGHT_LEVEL   50
//-------------------接口变量--------------------------------
extern BSP_PWR_E_RESET_FLAG Bsp_Pwr_ResetFlag;
//-------------------接口函数--------------------------------
// 初始化
void Bsp_Pwr_Init(void);
// 停止模式
void Bsp_Pwr_EnterStop(void);
// 待机模式
void Bsp_Pwr_EnterStandby(void);
// 探测
BSP_PWR_E_POWER Bsp_Pwr_Monitor(void);
// 重启动
void Bsp_Pwr_Reset_DebugTestOnOff(uint8_t OnOff);

void BspPwr_Cmd_parse(void);
#ifdef   BSP_PWR_CMD_ENABLE
void BspPwr_Cmd_timer_1s(void);
#endif
//-------------------------------------------------------------------------------输出函数
#endif
