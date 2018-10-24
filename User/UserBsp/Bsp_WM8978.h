/**
  ******************************************************************************
  * @file    Bsp_WM8978.h 
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  * @brief   bottom-driven -->   基于WM8978的音频解码与输出.
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
#ifndef __BSP_WM8978_H
#define __BSP_WM8978_H
//-------------------加载库函数------------------------------
#include "includes.h"
//-------------------数据结构--------------------------------
// WM8978 音频输入通道控制选项, 可以选择多路，比如 MIC_LEFT_ON | LINE_ON
typedef enum
{
	IN_PATH_OFF		= 0x00,	// 无输入
	MIC_LEFT_ON 	= 0x01,	// LIN,LIP脚，MIC左声道（接板载咪头）
	MIC_RIGHT_ON	= 0x02,	// RIN,RIP脚，MIC右声道（接GPRS模块音频输出）
	LINE_ON			= 0x04,  // L2,R2 立体声输入
	AUX_ON			= 0x08,	// AUXL,AUXR 立体声输入
	DAC_ON			= 0x10,	// I2S数据DAC (CPU产生音频信号)
	ADC_ON			= 0x20	// 输入的音频馈入WM8978内部ADC （I2S录音)
}IN_PATH_E;

// WM8978 音频输出通道控制选项, 可以选择多路
typedef enum
{
	OUT_PATH_OFF	= 0x00,	// 无输出
	EAR_LEFT_ON 	= 0x01,	// LOUT1 耳机左声道
	EAR_RIGHT_ON	= 0x02,	// ROUT1 耳机右声道
	SPK_ON			= 0x04,	// LOUT2和ROUT2反相输出单声道,接扬声器
	OUT3_4_ON		= 0x08,	// OUT3 和 OUT4 输出单声道音频， 接GSM模块的音频输入
}OUT_PATH_E;
//-------------------接口宏定义(硬件相关)--------------------
#define WM8978_SLAVE_ADDRESS    0x34	
#if   (defined(PROJECT_ARMFLY_V5_XSL))
//---->
//时钟
#if   (defined(STM32F1))
#define BSP_WM8978_IIS_RCC_ENABLE      RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE)
#define BSP_WM8978_IIS_RCC_DISABLE     RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, DISABLE)
#define BSP_WM8978_IIS_GPIO_RCC_ENABLE RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOI, ENABLE)
#elif (defined(STM32F4))
#define BSP_WM8978_IIS_RCC_ENABLE      RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE)
#define BSP_WM8978_IIS_RCC_DISABLE     RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, DISABLE)
#define BSP_WM8978_IIS_GPIO_RCC_ENABLE RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOI, ENABLE)
#endif
//端口
#define BSP_WM8978_IIS_X               SPI2
#define BSP_WM8978_IIS_EXT_X           I2S2ext
#define BSP_WM8978_IIS_LRC_PORT        GPIOB
#define BSP_WM8978_IIS_LRC_PIN         GPIO_Pin_12
#define BSP_WM8978_IIS_BLCK_PORT       GPIOB
#define BSP_WM8978_IIS_BLCK_PIN        GPIO_Pin_13
#define BSP_WM8978_IIS_MLCK_PORT       GPIOC
#define BSP_WM8978_IIS_MLCK_PIN        GPIO_Pin_6
#define BSP_WM8978_IIS_ADCDAT_PORT     GPIOI
#define BSP_WM8978_IIS_ADCDAT_PIN      GPIO_Pin_2
#define BSP_WM8978_IIS_DACDAT_PORT     GPIOC
#define BSP_WM8978_IIS_DACDAT_PIN      GPIO_Pin_3
//<----
#else 
#error Please Set Project to Bsp_WM8978.h
#endif
//-------------------接口宏定义(硬件无关)--------------------
//定义最大音量
#define VOLUME_MAX   63    // 最大音量
#define VOLUME_STEP	1		// 音量调节步长
//定义最大MIC增益
#define GAIN_MAX     63    // 最大增益
#define GAIN_STEP		1     // 增益步长
//-------------------接口函数--------------------------------
uint8_t BspWm8978_Init(void);
uint8_t BspWm8978_ReadEarVolume(void);
void BspWm8978_SetEarVolume(uint8_t _ucVolume);
uint8_t BspWm8978_ReadSpkVolume(void);
void BspWm8978_SetSpkVolume(uint8_t _ucVolume);
void BspWm8978_OutMute(uint8_t _ucMute);
void BspWm8978_SetMicGain(uint8_t _ucGain);
void BspWm8978_SetLineGain(uint8_t _ucGain);
void BspWm8978_PowerDown(void);
void BspWm8978_CfgAudioIF(uint16_t _usStandard, uint8_t _ucWordLen);
void BspWm8978_CfgAudioPath(uint16_t _InPath, uint16_t _OutPath);
void BspWm8978_NotchFilter(uint16_t _NFA0, uint16_t _NFA1);
void BspWm8978_CtrlGPIO1(uint8_t _ucValue);

void BspWm8978_StartPlay(uint16_t _usStandard, uint16_t _usWordLen,  uint32_t _uiAudioFreq);
void BspWm8978_StartRecord(uint16_t _usStandard, uint16_t _usWordLen,  uint32_t _uiAudioFreq);
void BspWm8978_Stop(void);

void BspWM8978_Record_DebugTestOnOff(uint8_t OnOff);
void BspWM8978_TfPlayer_DebugTestOnOff(uint8_t OnOff);

void I2S_CODEC_DataTransfer(void);
//-------------------------------------------------------------------------------
#endif

