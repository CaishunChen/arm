/*
***********************************************************************************
*                    作    者: 徐松亮
*                    更新时间: 2015-06-03
***********************************************************************************
*/
//-------------------------------------------------------------------------------
#ifndef __AD_H
#define __AD_H
//-------------------加载库函数------------------------------
#include "includes.h"
//-------------------配置宏定义------------------------------
#if   (defined(PROJECT_ARMFLY_V5_XSL)||defined(PROJECT_SPI_SLAVE))
#define AD_DMA_ENABLE
#else
//#define AD_DMA_ENABLE
#endif
//-------------------接口宏定义(硬件相关)--------------------
#define AD_ADCX               ADC1
#if   (defined(STM32F1))
#define AD_GPIO_RCC_ENABLE    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC,ENABLE)
#define AD_RCC_ENABLE         RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE)
#define AD_RCC_DISABLE        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, DISABLE)
#define AD_DMA_RX_RCC_ENABLE  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE)
#define AD_DMA_RX_RCC_DISABLE RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, DISABLE)
#define AD_DMA_RX_CHANNEL     DMA1_Channel1
#define AD_DMA_RX_STREAM      AD_DMA_RX_CHANNEL
#elif (defined(STM32F4))
#define AD_GPIO_RCC_ENABLE    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC , ENABLE);
#define AD_RCC_ENABLE         RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE)
#define AD_RCC_DISABLE        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, DISABLE)
#define AD_DMA_RX_RCC_ENABLE  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE)
#define AD_DMA_RX_RCC_DISABLE RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, DISABLE)
#define AD_DMA_RX_CHANNEL     DMA_Channel_0
#define AD_DMA_RX_STREAM      DMA2_Stream0
#elif (defined(NRF51)||defined(NRF52))
#endif
//-------------------根据项目开启通道-----------------------
//#define  ADC_ADC1_CHANNEL_0_ENABLE
//#define  ADC_ADC1_CHANNEL_1_ENABLE
//#define  ADC_ADC1_CHANNEL_2_ENABLE
//#define  ADC_ADC1_CHANNEL_3_ENABLE
//#define  ADC_ADC1_CHANNEL_4_ENABLE
//#define  ADC_ADC1_CHANNEL_5_ENABLE
//#define  ADC_ADC1_CHANNEL_6_ENABLE
//#define  ADC_ADC1_CHANNEL_7_ENABLE
//#define  ADC_ADC1_CHANNEL_8_ENABLE
//#define  ADC_ADC1_CHANNEL_9_ENABLE
//#define  ADC_ADC1_CHANNEL_10_ENABLE
//#define  ADC_ADC1_CHANNEL_11_ENABLE
//#define  ADC_ADC1_CHANNEL_12_ENABLE
//#define  ADC_ADC1_CHANNEL_13_ENABLE
//#define  ADC_ADC1_CHANNEL_14_ENABLE
//#define  ADC_ADC1_CHANNEL_15_ENABLE
//-----
#if (defined(PROJECT_BASE_STM32F1) || defined(PROJECT_BASE_STM32F4) \
      || defined(PROJECT_ARMFLY_V5_XSL)||defined(PROJECT_TCI_V30))
#define  ADC_USER_CHANNEL_NUM (0)
#elif (defined(PROJECT_SPI_SLAVE))
#define  ADC_USER_CHANNEL_NUM (12)
#define  ADC_ADC1_CHANNEL_0_ENABLE
#define  ADC_ADC1_CHANNEL_1_ENABLE
#define  ADC_ADC1_CHANNEL_2_ENABLE
#define  ADC_ADC1_CHANNEL_3_ENABLE
#define  ADC_ADC1_CHANNEL_4_ENABLE
#define  ADC_ADC1_CHANNEL_5_ENABLE
#define  ADC_ADC1_CHANNEL_6_ENABLE
#define  ADC_ADC1_CHANNEL_7_ENABLE
#define  ADC_ADC1_CHANNEL_8_ENABLE
#define  ADC_ADC1_CHANNEL_9_ENABLE
#define  ADC_ADC1_CHANNEL_10_ENABLE
#define  ADC_ADC1_CHANNEL_11_ENABLE
#define  ADC_ADC1_CHANNEL_12_ENABLE
#elif (defined(PROJECT_XKAP_V3)||defined(XKAP_ICARE_B_D_M))
#define  ADC_USER_CHANNEL_NUM (2)
#define  ADC_ADC1_CHANNEL_0_ENABLE
#define  ADC_ADC1_CHANNEL_1_ENABLE
enum AD_E_CHANNEL
{
   AD_E_CHANNEL_BAT=0,
   AD_E_CHANNEL_AC,   
};
#elif (defined(XKAP_ICARE_A_S))
#include "Bsp_NrfRfEsb.h"
#define  ADC_USER_CHANNEL_NUM (1)
#define  ADC_ADC1_CHANNEL_1_ENABLE
#define  ADC_ADC1_1MS_FUN(x)  Bsp_NrfRfEsb_AD(x)
#elif (defined(PROJECT_NRF5X_BLE))
#define  ADC_USER_CHANNEL_NUM (0)
#elif (defined(XKAP_ICARE_B_M ))
#define  ADC_USER_CHANNEL_NUM (0)
#else
#error Please Set Project to uctsk_AD.h
#endif
//-------------------接口宏定义(硬件无关)--------------------
//基准电压
#if   (defined(STM32F1))
#define  AD_REFERENCE_VOLTAGE_MV 3300
#define  AD_RESOLUTION           (1<<12)
#elif (defined(STM32F4))
#define  AD_REFERENCE_VOLTAGE_MV ADC_Vref_mV
#define  AD_RESOLUTION           (1<<12)
#elif (defined(NRF51))
//#define  AD_REFERENCE_VOLTAGE_MV 3300
//#define  AD_RESOLUTION           (1<<10)
#endif
//ADC缓存采集单路次数

//AD通道数量(12路采集+1路温度+1路参考电压+1路电池电压)
#if   (defined(STM32F1))
#define ADC_NEX                  (10)
#define  ADC_CHANNEL_NUM (ADC_USER_CHANNEL_NUM+1)
#elif (defined(STM32F4))
#define ADC_NEX                  (10)
#define  ADC_CHANNEL_NUM (ADC_USER_CHANNEL_NUM+3)
#elif (defined(NRF51)||defined(NRF52))
#define ADC_NEX                  (1)
#define  ADC_CHANNEL_NUM (ADC_USER_CHANNEL_NUM)
#endif
//-------------------接口变量--------------------------------
//特殊情况下使用
#if   (ADC_CHANNEL_NUM!=0)
extern uint16_t  ADC_ConvertedValue[ADC_NEX][ADC_CHANNEL_NUM];
extern uint16_t ADC_mV[ADC_CHANNEL_NUM];
#endif
//
extern uint16_t ADC_Temperature;
extern uint16_t ADC_Vref_mV;
extern uint16_t ADC_Vbat_mV;
//-------------------接口函数--------------------------------
extern void ADC_Configuration(void);
extern void  App_ADTaskCreate (void);
//extern void AD_DebugTest_100ms(void);
extern uint16_t AD_VBAT_Read_mv(void);
extern void AD_DebugTestOnOff(uint8_t OnOff);
//extern void AD_WaveOut_DebugTestOnOff(uint8_t OnOff);
extern uint8_t AD_ReadValue(uint8_t ch,uint16_t *pValue);
//-----------------------------------------------------------
#endif

