/**
  ******************************************************************************
  * @file    Bsp_StepperMotor.h 
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  * @brief   bottom-driven -->   步进电机.
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STEPPER_MOTOR_H
#define __STEPPER_MOTOR_H
/* Includes ------------------------------------------------------------------*/
#include "includes.h"
/* Exported types ------------------------------------------------------------*/
typedef struct BSP_STEPPER_MOTOR_S
{
   // 操作
   uint16_t Speed;            // 速度     	0x0000-最快	0xFFFF-不转
   uint16_t StepNum;          // 步进步数 		0x0000-不转 0xFFFF-常转
   uint8_t  Flag_PN  :1;      // 正反转   0-正  1-反
   uint8_t  res      :7;      // 备用
   // 初始化后不可变更
   uint8_t  PhaseNumber;      // 相数
   uint8_t  ReductionRatio;   // 减速比
}BSP_STEPPER_MOTOR_S;
/* Exported constants --------------------------------------------------------*/
/* Transplant define ---------------------------------------------------------*/
#if   (defined(PROJECT_TCI_V30))
//---->
//数量
#define BSP_STEPPER_MOTOR_NUM                (4)
#define BSP_STEPPER_MOTOR_1_PHASE_NUM        (4)
#define BSP_STEPPER_MOTOR_1_REDUCTION_RATIO  (64)
#define BSP_STEPPER_MOTOR_2_PHASE_NUM        (4)
#define BSP_STEPPER_MOTOR_2_REDUCTION_RATIO  (64)
#define BSP_STEPPER_MOTOR_3_PHASE_NUM        (4)
#define BSP_STEPPER_MOTOR_3_REDUCTION_RATIO  (64)
#define BSP_STEPPER_MOTOR_4_PHASE_NUM        (4)
#define BSP_STEPPER_MOTOR_4_REDUCTION_RATIO  (64)
//时钟
#if   (defined(STM32F1))
#define BSP_STEPPER_MOTOR_RCC_ENABLE      RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOF|RCC_APB2Periph_GPIOG , ENABLE);
#elif (defined(STM32F4))
#define BSP_STEPPER_MOTOR_RCC_ENABLE      RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOF|RCC_AHB1Periph_GPIOG , ENABLE);
#endif
//端口
#define BSP_STEPPER_MOTOR_1_A_PORT        GPIOF
#define BSP_STEPPER_MOTOR_1_A_PIN         GPIO_Pin_4  /*MG_OUT9*/
#define BSP_STEPPER_MOTOR_1_B_PORT        GPIOF
#define BSP_STEPPER_MOTOR_1_B_PIN         GPIO_Pin_8  /*MG_OUT10*/
#define BSP_STEPPER_MOTOR_1_C_PORT        GPIOF
#define BSP_STEPPER_MOTOR_1_C_PIN         GPIO_Pin_2  /*MG_OUT11*/
#define BSP_STEPPER_MOTOR_1_D_PORT        GPIOF
#define BSP_STEPPER_MOTOR_1_D_PIN         GPIO_Pin_10 /*MG_OUT12*/
#define BSP_STEPPER_MOTOR_2_A_PORT        GPIOG
#define BSP_STEPPER_MOTOR_2_A_PIN         GPIO_Pin_11 /*MG_OUT13*/
#define BSP_STEPPER_MOTOR_2_B_PORT        GPIOF
#define BSP_STEPPER_MOTOR_2_B_PIN         GPIO_Pin_15 /*MG_OUT14*/
#define BSP_STEPPER_MOTOR_2_C_PORT        GPIOE
#define BSP_STEPPER_MOTOR_2_C_PIN         GPIO_Pin_5  /*MG_OUT15*/
#define BSP_STEPPER_MOTOR_2_D_PORT        GPIOE
#define BSP_STEPPER_MOTOR_2_D_PIN         GPIO_Pin_3  /*MG_OUT16*/
#define BSP_STEPPER_MOTOR_3_A_PORT        GPIOE
#define BSP_STEPPER_MOTOR_3_A_PIN         GPIO_Pin_4  /*MG_OUT1*/
#define BSP_STEPPER_MOTOR_3_B_PORT        GPIOE
#define BSP_STEPPER_MOTOR_3_B_PIN         GPIO_Pin_6  /*MG_OUT2*/
#define BSP_STEPPER_MOTOR_3_C_PORT        GPIOF
#define BSP_STEPPER_MOTOR_3_C_PIN         GPIO_Pin_12 /*MG_OUT3*/
#define BSP_STEPPER_MOTOR_3_D_PORT        GPIOG
#define BSP_STEPPER_MOTOR_3_D_PIN         GPIO_Pin_10 /*MG_OUT4*/
#define BSP_STEPPER_MOTOR_4_A_PORT        GPIOF
#define BSP_STEPPER_MOTOR_4_A_PIN         GPIO_Pin_9  /*MG_OUT5*/
#define BSP_STEPPER_MOTOR_4_B_PORT        GPIOF
#define BSP_STEPPER_MOTOR_4_B_PIN         GPIO_Pin_3  /*MG_OUT6*/
#define BSP_STEPPER_MOTOR_4_C_PORT        GPIOA
#define BSP_STEPPER_MOTOR_4_C_PIN         GPIO_Pin_1  /*MG_OUT7*/
#define BSP_STEPPER_MOTOR_4_D_PORT        GPIOF
#define BSP_STEPPER_MOTOR_4_D_PIN         GPIO_Pin_5  /*MG_OUT8*/

//反向
#define BSP_STEPPER_MOTOR_REVERSE         TRUE
//<----
#elif	(defined(TEST_NRF52_V1))
//---->
//数量
#define BSP_STEPPER_MOTOR_NUM                (1)
#define BSP_STEPPER_MOTOR_1_PHASE_NUM        (4)
#define BSP_STEPPER_MOTOR_1_REDUCTION_RATIO  (64)
//端口
#define BSP_STEPPER_MOTOR_1_A_PIN         10  
#define BSP_STEPPER_MOTOR_1_B_PIN         25  
#define BSP_STEPPER_MOTOR_1_C_PIN         27  
#define BSP_STEPPER_MOTOR_1_D_PIN         29 
//反向
#define BSP_STEPPER_MOTOR_REVERSE         TRUE
//<----

#else 
#error Please Set Project to Bsp_StepperMotor.h
#endif
/* Application define --------------------------------------------------------*/
#if   (defined(STM32F1)||defined(STM32F4))
#if   (BSP_STEPPER_MOTOR_REVERSE!=TRUE)
#define BSP_STEPPER_MOTOR_SET             GPIO_SetBits
#define BSP_STEPPER_MOTOR_CLR             GPIO_ResetBits
#else
#define BSP_STEPPER_MOTOR_SET             GPIO_ResetBits
#define BSP_STEPPER_MOTOR_CLR             GPIO_SetBits
#endif

#ifdef BSP_STEPPER_MOTOR_1_A_PIN
#define BSP_STEPPER_MOTOR_1_A_SET         BSP_STEPPER_MOTOR_SET(BSP_STEPPER_MOTOR_1_A_PORT , BSP_STEPPER_MOTOR_1_A_PIN)
#define BSP_STEPPER_MOTOR_1_A_CLR         BSP_STEPPER_MOTOR_CLR(BSP_STEPPER_MOTOR_1_A_PORT , BSP_STEPPER_MOTOR_1_A_PIN)
#endif
#ifdef BSP_STEPPER_MOTOR_1_B_PIN
#define BSP_STEPPER_MOTOR_1_B_SET         BSP_STEPPER_MOTOR_SET(BSP_STEPPER_MOTOR_1_B_PORT , BSP_STEPPER_MOTOR_1_B_PIN)
#define BSP_STEPPER_MOTOR_1_B_CLR         BSP_STEPPER_MOTOR_CLR(BSP_STEPPER_MOTOR_1_B_PORT , BSP_STEPPER_MOTOR_1_B_PIN)
#endif
#ifdef BSP_STEPPER_MOTOR_1_C_PIN
#define BSP_STEPPER_MOTOR_1_C_SET         BSP_STEPPER_MOTOR_SET(BSP_STEPPER_MOTOR_1_C_PORT , BSP_STEPPER_MOTOR_1_C_PIN)
#define BSP_STEPPER_MOTOR_1_C_CLR         BSP_STEPPER_MOTOR_CLR(BSP_STEPPER_MOTOR_1_C_PORT , BSP_STEPPER_MOTOR_1_C_PIN)
#endif
#ifdef BSP_STEPPER_MOTOR_1_D_PIN
#define BSP_STEPPER_MOTOR_1_D_SET         BSP_STEPPER_MOTOR_SET(BSP_STEPPER_MOTOR_1_D_PORT , BSP_STEPPER_MOTOR_1_D_PIN)
#define BSP_STEPPER_MOTOR_1_D_CLR         BSP_STEPPER_MOTOR_CLR(BSP_STEPPER_MOTOR_1_D_PORT , BSP_STEPPER_MOTOR_1_D_PIN)
#endif
//
#ifdef BSP_STEPPER_MOTOR_2_A_PIN
#define BSP_STEPPER_MOTOR_2_A_SET         BSP_STEPPER_MOTOR_SET(BSP_STEPPER_MOTOR_2_A_PORT , BSP_STEPPER_MOTOR_2_A_PIN)
#define BSP_STEPPER_MOTOR_2_A_CLR         BSP_STEPPER_MOTOR_CLR(BSP_STEPPER_MOTOR_2_A_PORT , BSP_STEPPER_MOTOR_2_A_PIN)
#endif
#ifdef BSP_STEPPER_MOTOR_2_B_PIN
#define BSP_STEPPER_MOTOR_2_B_SET         BSP_STEPPER_MOTOR_SET(BSP_STEPPER_MOTOR_2_B_PORT , BSP_STEPPER_MOTOR_2_B_PIN)
#define BSP_STEPPER_MOTOR_2_B_CLR         BSP_STEPPER_MOTOR_CLR(BSP_STEPPER_MOTOR_2_B_PORT , BSP_STEPPER_MOTOR_2_B_PIN)
#endif
#ifdef BSP_STEPPER_MOTOR_2_C_PIN
#define BSP_STEPPER_MOTOR_2_C_SET         BSP_STEPPER_MOTOR_SET(BSP_STEPPER_MOTOR_2_C_PORT , BSP_STEPPER_MOTOR_2_C_PIN)
#define BSP_STEPPER_MOTOR_2_C_CLR         BSP_STEPPER_MOTOR_CLR(BSP_STEPPER_MOTOR_2_C_PORT , BSP_STEPPER_MOTOR_2_C_PIN)
#endif
#ifdef BSP_STEPPER_MOTOR_2_D_PIN
#define BSP_STEPPER_MOTOR_2_D_SET         BSP_STEPPER_MOTOR_SET(BSP_STEPPER_MOTOR_2_D_PORT , BSP_STEPPER_MOTOR_2_D_PIN)
#define BSP_STEPPER_MOTOR_2_D_CLR         BSP_STEPPER_MOTOR_CLR(BSP_STEPPER_MOTOR_2_D_PORT , BSP_STEPPER_MOTOR_2_D_PIN)
#endif
//
#ifdef BSP_STEPPER_MOTOR_3_A_PIN
#define BSP_STEPPER_MOTOR_3_A_SET         BSP_STEPPER_MOTOR_SET(BSP_STEPPER_MOTOR_3_A_PORT , BSP_STEPPER_MOTOR_3_A_PIN)
#define BSP_STEPPER_MOTOR_3_A_CLR         BSP_STEPPER_MOTOR_CLR(BSP_STEPPER_MOTOR_3_A_PORT , BSP_STEPPER_MOTOR_3_A_PIN)
#endif
#ifdef BSP_STEPPER_MOTOR_3_B_PIN
#define BSP_STEPPER_MOTOR_3_B_SET         BSP_STEPPER_MOTOR_SET(BSP_STEPPER_MOTOR_3_B_PORT , BSP_STEPPER_MOTOR_3_B_PIN)
#define BSP_STEPPER_MOTOR_3_B_CLR         BSP_STEPPER_MOTOR_CLR(BSP_STEPPER_MOTOR_3_B_PORT , BSP_STEPPER_MOTOR_3_B_PIN)
#endif
#ifdef BSP_STEPPER_MOTOR_3_C_PIN
#define BSP_STEPPER_MOTOR_3_C_SET         BSP_STEPPER_MOTOR_SET(BSP_STEPPER_MOTOR_3_C_PORT , BSP_STEPPER_MOTOR_3_C_PIN)
#define BSP_STEPPER_MOTOR_3_C_CLR         BSP_STEPPER_MOTOR_CLR(BSP_STEPPER_MOTOR_3_C_PORT , BSP_STEPPER_MOTOR_3_C_PIN)
#endif
#ifdef BSP_STEPPER_MOTOR_3_D_PIN
#define BSP_STEPPER_MOTOR_3_D_SET         BSP_STEPPER_MOTOR_SET(BSP_STEPPER_MOTOR_3_D_PORT , BSP_STEPPER_MOTOR_3_D_PIN)
#define BSP_STEPPER_MOTOR_3_D_CLR         BSP_STEPPER_MOTOR_CLR(BSP_STEPPER_MOTOR_3_D_PORT , BSP_STEPPER_MOTOR_3_D_PIN)
#endif
//
#ifdef BSP_STEPPER_MOTOR_4_A_PIN
#define BSP_STEPPER_MOTOR_4_A_SET         BSP_STEPPER_MOTOR_SET(BSP_STEPPER_MOTOR_4_A_PORT , BSP_STEPPER_MOTOR_4_A_PIN)
#define BSP_STEPPER_MOTOR_4_A_CLR         BSP_STEPPER_MOTOR_CLR(BSP_STEPPER_MOTOR_4_A_PORT , BSP_STEPPER_MOTOR_4_A_PIN)
#endif
#ifdef BSP_STEPPER_MOTOR_4_B_PIN
#define BSP_STEPPER_MOTOR_4_B_SET         BSP_STEPPER_MOTOR_SET(BSP_STEPPER_MOTOR_4_B_PORT , BSP_STEPPER_MOTOR_4_B_PIN)
#define BSP_STEPPER_MOTOR_4_B_CLR         BSP_STEPPER_MOTOR_CLR(BSP_STEPPER_MOTOR_4_B_PORT , BSP_STEPPER_MOTOR_4_B_PIN)
#endif
#ifdef BSP_STEPPER_MOTOR_4_C_PIN
#define BSP_STEPPER_MOTOR_4_C_SET         BSP_STEPPER_MOTOR_SET(BSP_STEPPER_MOTOR_4_C_PORT , BSP_STEPPER_MOTOR_4_C_PIN)
#define BSP_STEPPER_MOTOR_4_C_CLR         BSP_STEPPER_MOTOR_CLR(BSP_STEPPER_MOTOR_4_C_PORT , BSP_STEPPER_MOTOR_4_C_PIN)
#endif
#ifdef BSP_STEPPER_MOTOR_4_D_PIN
#define BSP_STEPPER_MOTOR_4_D_SET         BSP_STEPPER_MOTOR_SET(BSP_STEPPER_MOTOR_4_D_PORT , BSP_STEPPER_MOTOR_4_D_PIN)
#define BSP_STEPPER_MOTOR_4_D_CLR         BSP_STEPPER_MOTOR_CLR(BSP_STEPPER_MOTOR_4_D_PORT , BSP_STEPPER_MOTOR_4_D_PIN)
#endif
//
#elif (defined(NRF52)||defined(NRF51))
#if   (BSP_STEPPER_MOTOR_REVERSE!=TRUE)
#define BSP_STEPPER_MOTOR_SET(a,b)        nrf_gpio_pin_write(a,1)
#define BSP_STEPPER_MOTOR_CLR(a,b)        nrf_gpio_pin_write(a,0)
#else
#define BSP_STEPPER_MOTOR_SET(a,b)        nrf_gpio_pin_write(a,0)
#define BSP_STEPPER_MOTOR_CLR(a,b)        nrf_gpio_pin_write(a,1)
#endif
//
#ifdef BSP_STEPPER_MOTOR_1_A_PIN
#define BSP_STEPPER_MOTOR_1_A_SET         BSP_STEPPER_MOTOR_SET(BSP_STEPPER_MOTOR_1_A_PIN,1)
#define BSP_STEPPER_MOTOR_1_A_CLR         BSP_STEPPER_MOTOR_CLR(BSP_STEPPER_MOTOR_1_A_PIN,0)
#endif
#ifdef BSP_STEPPER_MOTOR_1_B_PIN
#define BSP_STEPPER_MOTOR_1_B_SET         BSP_STEPPER_MOTOR_SET(BSP_STEPPER_MOTOR_1_B_PIN,1)
#define BSP_STEPPER_MOTOR_1_B_CLR         BSP_STEPPER_MOTOR_CLR(BSP_STEPPER_MOTOR_1_B_PIN,0)
#endif
#ifdef BSP_STEPPER_MOTOR_1_C_PIN
#define BSP_STEPPER_MOTOR_1_C_SET         BSP_STEPPER_MOTOR_SET(BSP_STEPPER_MOTOR_1_C_PIN,1)
#define BSP_STEPPER_MOTOR_1_C_CLR         BSP_STEPPER_MOTOR_CLR(BSP_STEPPER_MOTOR_1_C_PIN,0)
#endif
#ifdef BSP_STEPPER_MOTOR_1_D_PIN
#define BSP_STEPPER_MOTOR_1_D_SET         BSP_STEPPER_MOTOR_SET(BSP_STEPPER_MOTOR_1_D_PIN,1)
#define BSP_STEPPER_MOTOR_1_D_CLR         BSP_STEPPER_MOTOR_CLR(BSP_STEPPER_MOTOR_1_D_PIN,0)
#endif
//
#ifdef BSP_STEPPER_MOTOR_2_A_PIN
#define BSP_STEPPER_MOTOR_2_A_SET         BSP_STEPPER_MOTOR_SET(BSP_STEPPER_MOTOR_2_A_PIN,1)
#define BSP_STEPPER_MOTOR_2_A_CLR         BSP_STEPPER_MOTOR_CLR(BSP_STEPPER_MOTOR_2_A_PIN,0)
#endif
#ifdef BSP_STEPPER_MOTOR_2_B_PIN
#define BSP_STEPPER_MOTOR_2_B_SET         BSP_STEPPER_MOTOR_SET(BSP_STEPPER_MOTOR_2_B_PIN,1)
#define BSP_STEPPER_MOTOR_2_B_CLR         BSP_STEPPER_MOTOR_CLR(BSP_STEPPER_MOTOR_2_B_PIN,0)
#endif
#ifdef BSP_STEPPER_MOTOR_2_C_PIN
#define BSP_STEPPER_MOTOR_2_C_SET         BSP_STEPPER_MOTOR_SET(BSP_STEPPER_MOTOR_2_C_PIN,1)
#define BSP_STEPPER_MOTOR_2_C_CLR         BSP_STEPPER_MOTOR_CLR(BSP_STEPPER_MOTOR_2_C_PIN,0)
#endif
#ifdef BSP_STEPPER_MOTOR_2_D_PIN
#define BSP_STEPPER_MOTOR_2_D_SET         BSP_STEPPER_MOTOR_SET(BSP_STEPPER_MOTOR_2_D_PIN,1)
#define BSP_STEPPER_MOTOR_2_D_CLR         BSP_STEPPER_MOTOR_CLR(BSP_STEPPER_MOTOR_2_D_PIN,0)
#endif
//
#ifdef BSP_STEPPER_MOTOR_3_A_PIN
#define BSP_STEPPER_MOTOR_3_A_SET         BSP_STEPPER_MOTOR_SET(BSP_STEPPER_MOTOR_3_A_PIN,1)
#define BSP_STEPPER_MOTOR_3_A_CLR         BSP_STEPPER_MOTOR_CLR(BSP_STEPPER_MOTOR_3_A_PIN,0)
#endif
#ifdef BSP_STEPPER_MOTOR_3_B_PIN
#define BSP_STEPPER_MOTOR_3_B_SET         BSP_STEPPER_MOTOR_SET(BSP_STEPPER_MOTOR_3_B_PIN,1)
#define BSP_STEPPER_MOTOR_3_B_CLR         BSP_STEPPER_MOTOR_CLR(BSP_STEPPER_MOTOR_3_B_PIN,0)
#endif
#ifdef BSP_STEPPER_MOTOR_3_C_PIN
#define BSP_STEPPER_MOTOR_3_C_SET         BSP_STEPPER_MOTOR_SET(BSP_STEPPER_MOTOR_3_C_PIN,1)
#define BSP_STEPPER_MOTOR_3_C_CLR         BSP_STEPPER_MOTOR_CLR(BSP_STEPPER_MOTOR_3_C_PIN,0)
#endif
#ifdef BSP_STEPPER_MOTOR_3_D_PIN
#define BSP_STEPPER_MOTOR_3_D_SET         BSP_STEPPER_MOTOR_SET(BSP_STEPPER_MOTOR_3_D_PIN,1)
#define BSP_STEPPER_MOTOR_3_D_CLR         BSP_STEPPER_MOTOR_CLR(BSP_STEPPER_MOTOR_3_D_PIN,0)
#endif
//
#ifdef BSP_STEPPER_MOTOR_4_A_PIN
#define BSP_STEPPER_MOTOR_4_A_SET         BSP_STEPPER_MOTOR_SET(BSP_STEPPER_MOTOR_4_A_PIN,1)
#define BSP_STEPPER_MOTOR_4_A_CLR         BSP_STEPPER_MOTOR_CLR(BSP_STEPPER_MOTOR_4_A_PIN,0)
#endif
#ifdef BSP_STEPPER_MOTOR_4_B_PIN
#define BSP_STEPPER_MOTOR_4_B_SET         BSP_STEPPER_MOTOR_SET(BSP_STEPPER_MOTOR_4_B_PIN,1)
#define BSP_STEPPER_MOTOR_4_B_CLR         BSP_STEPPER_MOTOR_CLR(BSP_STEPPER_MOTOR_4_B_PIN,0)
#endif
#ifdef BSP_STEPPER_MOTOR_4_C_PIN
#define BSP_STEPPER_MOTOR_4_C_SET         BSP_STEPPER_MOTOR_SET(BSP_STEPPER_MOTOR_4_C_PIN,1)
#define BSP_STEPPER_MOTOR_4_C_CLR         BSP_STEPPER_MOTOR_CLR(BSP_STEPPER_MOTOR_4_C_PIN,0)
#endif
#ifdef BSP_STEPPER_MOTOR_4_D_PIN
#define BSP_STEPPER_MOTOR_4_D_SET         BSP_STEPPER_MOTOR_SET(BSP_STEPPER_MOTOR_4_D_PIN,1)
#define BSP_STEPPER_MOTOR_4_D_CLR         BSP_STEPPER_MOTOR_CLR(BSP_STEPPER_MOTOR_4_D_PIN,0)
#endif
//
#endif
//-------------------接口变量--------------------------------
extern BSP_STEPPER_MOTOR_S BspStepperMotor_s[BSP_STEPPER_MOTOR_NUM];
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
extern void BspStepperMotor_Init(void);
extern void BspStepperMotor_1ms(void);
extern void BspStepperMotor_DebugTestOnOff(uint8_t OnOff);
//-----------------------------------------------------------
#endif

