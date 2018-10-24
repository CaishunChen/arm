/**
  ******************************************************************************
  * @file    Bsp_Led.h 
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  * @brief   bottom-driven -->   LED.
  * @note    
  * @verbatim
    
 ===============================================================================
                     ##### How to use this driver #####
 ===============================================================================
   1,    适用芯片
         STM      :  STM32F1  STM32F4
         Nordic   :  Nrf51    Nrf52
   2,    移植步骤
         2.1   在Bsp_Led.h中设定时钟与引脚信息。
         2.2   在主函数调用            BspLed_Init()
         2.3   在100mS循环函数中调用   BspLed_100ms()
   3,    验证方法
         3.1   在Debug环境加入调试指令，执行后所有LED循环点亮，退出后所有LED关闭
   4,    使用方法
         4.1   用户可以调用Bsp_Led.h中的宏定义直接控制LED
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
#ifndef __BSP_LED_H
#define __BSP_LED_H
/* Includes ------------------------------------------------------------------*/
#include "includes.h"
/* Exported types ------------------------------------------------------------*/
typedef enum
{
    BSP_LED_E_MODE_NULL=0,
    BSP_LED_E_MODE_ONCE,
    BSP_LED_E_MODE_FLICK,
    BSP_LED_E_MODE_PWM,
    BSP_LED_E_MODE_ON,
    BSP_LED_E_MODE_OFF,
} BSP_LED_E_MODE;


/* Exported constants --------------------------------------------------------*/
/* Transplant define ---------------------------------------------------------*/
/* ---  PROJECT_TCI_V30 ---*/ 
#if   (defined(PROJECT_TCI_V30))
//---->
//时钟
#if   (defined(STM32F1))
#define BSP_LED_RCC_ENABLE    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOF , ENABLE);
#elif (defined(STM32F4))
#define BSP_LED_RCC_ENABLE    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOF , ENABLE);
#endif
//端口
#define BSP_LED_LED1_PORT     GPIOF
#define BSP_LED_LED1_PIN      GPIO_Pin_11
#define BSP_LED_LED2_PORT     GPIOF
#define BSP_LED_LED2_PIN      GPIO_Pin_0
#define BSP_LED_LED3_PORT     GPIOF
#define BSP_LED_LED3_PIN      GPIO_Pin_1
//
#define BSP_LED_MAX_NUM			3
//<----
/* ---  PROJECT_XKAP_V3 || XKAP_ICARE_B_D_M ---*/ 
#elif (defined(PROJECT_XKAP_V3)||defined(XKAP_ICARE_B_D_M))
//---->
//时钟
#if   (defined(STM32F1))
#define BSP_LED_RCC_ENABLE    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD , ENABLE);
#elif (defined(STM32F4))
#define BSP_LED_RCC_ENABLE    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOD , ENABLE);
#endif
//端口
#define BSP_LED_LED1_PORT     GPIOD
#define BSP_LED_LED1_PIN      GPIO_Pin_4
#define BSP_LED_LED2_PORT     GPIOD
#define BSP_LED_LED2_PIN      GPIO_Pin_5
#define BSP_LED_LED3_PORT     GPIOD
#define BSP_LED_LED3_PIN      GPIO_Pin_6
#define BSP_LED_MAX_NUM			3
//<----
/* ---  PROJECT_ARMFLY_V5_XSL ---*/ 
#elif (defined(PROJECT_ARMFLY_V5_XSL))
//---->
//时钟
#if   (defined(STM32F1))
#define BSP_LED_RCC_ENABLE    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC , ENABLE);
#elif (defined(STM32F4))
#define BSP_LED_RCC_ENABLE    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOC , ENABLE);
#endif
//端口
#define BSP_LED_LED1_PORT     GPIOC
#define BSP_LED_LED1_PIN      GPIO_Pin_2
//
#define BSP_LED_MAX_NUM			1
//<----
/* ---  PROJECT_SPI_SLAVE ---*/ 
#elif (defined(PROJECT_SPI_SLAVE))
//---->
//时钟
#if   (defined(STM32F1))
#define BSP_LED_RCC_ENABLE    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD , ENABLE);
#elif (defined(STM32F4))
#define BSP_LED_RCC_ENABLE    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOD , ENABLE);
#endif
//端口
#define BSP_LED_LED1_PORT     GPIOD
#define BSP_LED_LED1_PIN      GPIO_Pin_13
//
#define BSP_LED_MAX_NUM			1
//<----
/* ---  PROJECT_BIB_TEST1 ---*/ 
#elif (defined(PROJECT_BIB_TEST1))
//---->
//时钟
#define BSP_LED_RCC_ENABLE    {RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOB , ENABLE);GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);}
//端口
#define BSP_LED_LED1_PORT     GPIOB
#define BSP_LED_LED1_PIN      GPIO_Pin_4
//
#define BSP_LED_MAX_NUM			1
//<----
/* ---  BASE_NRF52 ---*/ 
#elif (defined(BASE_NRF52))
//---->
#define BSP_LED_LED1_PIN      17
#define BSP_LED_LED2_PIN      18
#define BSP_LED_LED3_PIN      19
#define BSP_LED_LED4_PIN      20
//
#define BSP_LED_MAX_NUM			4
//<----
/* ---  XKAP_ICARE_A_M || XKAP_ICARE_A_S ---*/ 
#elif (defined(XKAP_ICARE_A_M)||defined(XKAP_ICARE_A_S))
//---->
#define BSP_LED_LED1_PIN      17
#define BSP_LED_LED2_PIN      18
#define BSP_LED_LED3_PIN      19
#define BSP_LED_LED4_PIN      20
//
#define BSP_LED_MAX_NUM			4
//<----
/* ---  XKAP_ICARE_B_M ---*/ 
#elif (defined(XKAP_ICARE_B_M))
//---->
#if   (HARDWARE_SUB_VER==1)
#define BSP_LED_LED1_PIN      13
#define BSP_LED_LED2_PIN      24
#define BSP_LED_LED3_PIN      25
#define BSP_LED_MAX_NUM			3
#elif (HARDWARE_SUB_VER==2)
#define BSP_LED_LED1_PIN      22 // Light
#define BSP_LED_LED2_PIN      17 // PCB-led1
#define BSP_LED_LED3_PIN      18 // PCB-led2
#define BSP_LED_LED4_PIN      6  // SOS-led
#define BSP_LED_PWM_ENABLE
#define BSP_LED_MAX_NUM			4
#endif
//<----
/* ---  BASE_NRF51 || XKAP_ICARE_B_C || PROJECT_NRF5X_BLE ---*/ 
#elif (defined(BASE_NRF51)||defined(XKAP_ICARE_B_C)||defined(PROJECT_NRF5X_BLE))
//---->
#define BSP_LED_LED1_PIN      17
#define BSP_LED_LED2_PIN      19
#define BSP_LED_LED3_PIN      18
//
#define BSP_LED_MAX_NUM			3
//<----
/* ---  XKAP_ICARE_B_D ---*/ 
#elif (defined(XKAP_ICARE_B_D))
//---->
#define BSP_LED_LED1_PIN      9
#define BSP_LED_LED2_PIN      11
//
#define BSP_LED_MAX_NUM			2
//<----
#else 
#error Please Set Project to Bsp_Led.h
#endif
//----------
/* Application define --------------------------------------------------------*/
#if   (defined(STM32F1)||defined(STM32F4))  
//控制LED亮灭
#ifdef BSP_LED_LED1_PIN
#define BSP_LED_LED1_ON       GPIO_ResetBits(BSP_LED_LED1_PORT , BSP_LED_LED1_PIN)
#define BSP_LED_LED1_OFF      GPIO_SetBits(BSP_LED_LED1_PORT , BSP_LED_LED1_PIN)
#define BSP_LED_LED1_TOGGLE   BSP_LED_LED1_PORT->ODR ^= BSP_LED_LED1_PIN
#define BSP_LED_LED1_READ     BSP_LED_LED1_PORT->ODR & BSP_LED_LED1_PIN
#endif
#ifdef BSP_LED_LED2_PIN
#define BSP_LED_LED2_ON       GPIO_ResetBits(BSP_LED_LED2_PORT , BSP_LED_LED2_PIN)
#define BSP_LED_LED2_OFF      GPIO_SetBits(BSP_LED_LED2_PORT , BSP_LED_LED2_PIN)
#define BSP_LED_LED2_TOGGLE   BSP_LED_LED2_PORT->ODR ^= BSP_LED_LED2_PIN
#define BSP_LED_LED2_READ     BSP_LED_LED2_PORT->ODR & BSP_LED_LED2_PIN
#endif
#ifdef BSP_LED_LED3_PIN
#define BSP_LED_LED3_ON       GPIO_ResetBits(BSP_LED_LED3_PORT , BSP_LED_LED3_PIN)
#define BSP_LED_LED3_OFF      GPIO_SetBits(BSP_LED_LED3_PORT , BSP_LED_LED3_PIN)
#define BSP_LED_LED3_TOGGLE   BSP_LED_LED3_PORT->ODR ^= BSP_LED_LED3_PIN
#define BSP_LED_LED3_READ     BSP_LED_LED3_PORT->ODR & BSP_LED_LED3_PIN
#endif
#ifdef BSP_LED_LED4_PIN
#define BSP_LED_LED4_ON       GPIO_ResetBits(BSP_LED_LED4_PORT , BSP_LED_LED4_PIN)
#define BSP_LED_LED4_OFF      GPIO_SetBits(BSP_LED_LED4_PORT , BSP_LED_LED4_PIN)
#define BSP_LED_LED4_TOGGLE   BSP_LED_LED4_PORT->ODR ^= BSP_LED_LED4_PIN
#define BSP_LED_LED4_READ     BSP_LED_LED4_PORT->ODR & BSP_LED_LED4_PIN
#endif
#elif(defined(NRF52)||defined(NRF51))
#ifdef BSP_LED_LED1_PIN
#define BSP_LED_LED1_ON       nrf_gpio_pin_write(BSP_LED_LED1_PIN,0)
#define BSP_LED_LED1_OFF      nrf_gpio_pin_write(BSP_LED_LED1_PIN,1)
#define BSP_LED_LED1_TOGGLE   nrf_gpio_pin_toggle(BSP_LED_LED1_PIN)
#define BSP_LED_LED1_READ     nrf_gpio_pin_out_read(BSP_LED_LED1_PIN)
#endif
#ifdef BSP_LED_LED2_PIN
#define BSP_LED_LED2_ON       nrf_gpio_pin_write(BSP_LED_LED2_PIN,0)
#define BSP_LED_LED2_OFF      nrf_gpio_pin_write(BSP_LED_LED2_PIN,1)
#define BSP_LED_LED2_TOGGLE   nrf_gpio_pin_toggle(BSP_LED_LED2_PIN)
#define BSP_LED_LED2_READ     nrf_gpio_pin_out_read(BSP_LED_LED2_PIN)
#endif
#ifdef BSP_LED_LED3_PIN
#define BSP_LED_LED3_ON       nrf_gpio_pin_write(BSP_LED_LED3_PIN,0)
#define BSP_LED_LED3_OFF      nrf_gpio_pin_write(BSP_LED_LED3_PIN,1)
#define BSP_LED_LED3_TOGGLE   nrf_gpio_pin_toggle(BSP_LED_LED3_PIN)
#define BSP_LED_LED3_READ     nrf_gpio_pin_out_read(BSP_LED_LED3_PIN)
#endif
#ifdef BSP_LED_LED4_PIN
#define BSP_LED_LED4_ON       nrf_gpio_pin_write(BSP_LED_LED4_PIN,0)
#define BSP_LED_LED4_OFF      nrf_gpio_pin_write(BSP_LED_LED4_PIN,1)
#define BSP_LED_LED4_TOGGLE   nrf_gpio_pin_toggle(BSP_LED_LED4_PIN)
#define BSP_LED_LED4_READ     nrf_gpio_pin_out_read(BSP_LED_LED4_PIN)
#endif
#endif
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
extern void BspLed_Init(void);
extern void BspLed_1ms(void);
extern void BspLed_100ms(void);
extern void BspLed_PwmLed(uint8_t num,uint8_t pwmValue);
extern void BspLed_Mode(uint8_t num,BSP_LED_E_MODE mode);
extern void BspLed_DebugTestOnOff(uint8_t OnOff);

#endif
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
