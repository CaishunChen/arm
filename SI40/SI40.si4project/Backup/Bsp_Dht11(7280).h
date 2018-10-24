/**
  ******************************************************************************
  * @file    Bsp_TempHumiSensor.h
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  * @brief   bottom-driven -->   DHT系列的温湿度传感器驱动.
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
#ifndef __BSP_DHT11_H
#define __BSP_DHT11_H
/* Includes ------------------------------------------------------------------*/
#include "includes.h"
/* Exported types ------------------------------------------------------------*/
typedef enum
{
    BSP_DHTXX_E_DHT11=0,
    BSP_DHTXX_E_DHT22,
} BSP_DHTXX_E_CHIP;
typedef struct
{
    int16_t     Temp;       // Temperature  温度 *10      摄氏度
    uint16_t    Hum;        // Humidity     湿度 *10      百分比
    uint8_t     Buf[5];
    uint8_t     PeriodS;    // 采集周期 秒 (0无效)
} BSP_DHTXX_S_INFO;
/* Exported constants --------------------------------------------------------*/
/* Transplant define ---------------------------------------------------------*/
#if     (defined(PROJECT_ARMFLY_V5_XSL))
#define BSP_DHTXX_RCC_ENABLE    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB , ENABLE);
#define BSP_DHTXX_PORT          GPIOB
#define BSP_DHTXX_PIN           GPIO_Pin_1
#define BSP_DHTXX_CHIP          BSP_DHTXX_E_DHT11
#elif   (defined(TEST_NRF52_V1))
#define BSP_DHTXX_PIN           8
#define BSP_DHTXX_CHIP          BSP_DHTXX_E_DHT11
#else
#error Please Set Project to Bsp_Dht11.h
#endif
/* Application define --------------------------------------------------------*/
#if     (defined(STM32F1)||defined(STM32F4))
#define BSP_DHTXX_DQ_H          GPIO_SetBits(BSP_DHTXX_PORT, BSP_DHTXX_PIN)
#define BSP_DHTXX_DQ_L          GPIO_ResetBits(BSP_DHTXX_PORT, BSP_DHTXX_PIN)
#define BSP_DHTXX_DQ_R          (BSP_DHTXX_PORT->IDR & BSP_DHTXX_PIN)
#elif   (defined(NRF51)||defined(NRF52))
#define BSP_DHTXX_DQ_H          nrf_gpio_pin_write(BSP_DHTXX_PIN,1)
#define BSP_DHTXX_DQ_L          nrf_gpio_pin_write(BSP_DHTXX_PIN,0)
#define BSP_DHTXX_DQ_R          nrf_gpio_pin_read(BSP_DHTXX_PIN)
#endif
/* Exported macro ------------------------------------------------------------*/
extern BSP_DHTXX_S_INFO BspDhtXX_s_Info;
/* Exported functions --------------------------------------------------------*/
extern void BspDhtXX_Init(void);
extern void BspDhtXX_100ms(void);
extern void BspDhtXX_DebugTestOnOff(uint8_t OnOff);
//-------------------------------------------------------------------------------
#endif


