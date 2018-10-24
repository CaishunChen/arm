/**
  ******************************************************************************
  * @file    Bsp_Dac.c 
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  ******************************************************************************
  * @attention
  *
  * GNU General Public License (GPL) 
  *
  * <h2><center>&copy; COPYRIGHT 2017 XSLXHN</center></h2>
  ******************************************************************************
  */ 
//------------------------------- Includes -----------------------------------
#include "Bsp_Dac.h"
//----------------------------------------------------------------------------
void BspDac_Init(uint8_t ch)
{
#if   (defined(STM32F1))
	  ch=ch;
#elif (defined(STM32F4))	
    GPIO_InitTypeDef GPIO_InitStructure;
    DAC_InitTypeDef DAC_InitStructure;
    //----------初始化DAC1
    switch(ch)
    {
        case 1:
            //GPIO初始化
            BSP_DAC1_GPIO_RCC_ENABLE;
#if   (defined(STM32F1))
            GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP;
#elif (defined(STM32F4))
            GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AN;
            GPIO_InitStructure.GPIO_OType=  GPIO_OType_PP;
            GPIO_InitStructure.GPIO_PuPd =  GPIO_PuPd_NOPULL;
#endif
            GPIO_InitStructure.GPIO_Pin  =  BSP_DAC1_PIN;
            GPIO_InitStructure.GPIO_Speed=  GPIO_Speed_2MHz;
            GPIO_Init(BSP_DAC1_PORT,&GPIO_InitStructure);
            //DAC1初始化
            BSP_DAC1_RCC_ENABLE;
            DAC_InitStructure.DAC_WaveGeneration  = DAC_WaveGeneration_None;
            DAC_InitStructure.DAC_Trigger         = DAC_Trigger_Software;
            DAC_InitStructure.DAC_OutputBuffer    = DAC_OutputBuffer_Enable;
            DAC_Init(BSP_DAC1_CH,&DAC_InitStructure);
            DAC_Cmd(BSP_DAC1_CH,ENABLE);
            BSP_DAC1_SET_FUN(DAC_Align_12b_R,0x0000);
            DAC_SoftwareTriggerCmd(BSP_DAC1_CH,ENABLE);
            break;
        //----------初始化DAC2
        case 2:
            //GPIO初始化
            BSP_DAC2_GPIO_RCC_ENABLE;
#if   (defined(STM32F1))
            GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP;
#elif (defined(STM32F4))
            GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AN;
            GPIO_InitStructure.GPIO_OType=  GPIO_OType_PP;
            GPIO_InitStructure.GPIO_PuPd =  GPIO_PuPd_NOPULL;
#endif
            GPIO_InitStructure.GPIO_Pin  =  BSP_DAC2_PIN;
            GPIO_InitStructure.GPIO_Speed=  GPIO_Speed_2MHz;
            GPIO_Init(BSP_DAC2_PORT,&GPIO_InitStructure);
            //DAC2初始化
            BSP_DAC2_RCC_ENABLE;
            DAC_InitStructure.DAC_WaveGeneration  = DAC_WaveGeneration_None;
            DAC_InitStructure.DAC_Trigger         = DAC_Trigger_Software;
            DAC_InitStructure.DAC_OutputBuffer    = DAC_OutputBuffer_Enable;
            DAC_Init(BSP_DAC2_CH,&DAC_InitStructure);
            DAC_Cmd(BSP_DAC2_CH,ENABLE);
            BSP_DAC2_SET_FUN(DAC_Align_12b_R,0x0000);
            DAC_SoftwareTriggerCmd(BSP_DAC2_CH,ENABLE);
            break;
        default:
            break;
    }
#endif		
}
void BspDac_DeInit(uint8_t ch)
{
#if   (defined(STM32F1))
	  ch=ch;
#elif (defined(STM32F4))	
    GPIO_InitTypeDef GPIO_InitStructure;
    //----------初始化DAC1
    switch(ch)
    {
        case 1:
            //GPIO初始化
            //BSP_DAC1_GPIO_RCC_ENABLE;
#if   (defined(STM32F1))
            GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IN_FLOATING;
#elif (defined(STM32F4))
            GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IN;
            GPIO_InitStructure.GPIO_OType=  GPIO_OType_OD;
            GPIO_InitStructure.GPIO_PuPd =  GPIO_PuPd_NOPULL;
#endif
            GPIO_InitStructure.GPIO_Pin  =  BSP_DAC1_PIN;
            GPIO_InitStructure.GPIO_Speed=  GPIO_Speed_2MHz;
            GPIO_Init(BSP_DAC1_PORT,&GPIO_InitStructure);
            //DAC1初始化
            BSP_DAC1_RCC_DISABLE;
            DAC_Cmd(BSP_DAC1_CH,DISABLE);
            DAC_SoftwareTriggerCmd(BSP_DAC1_CH,DISABLE);
            break;
        //----------初始化DAC2
        case 2:
            //GPIO初始化
            BSP_DAC2_GPIO_RCC_ENABLE;
#if   (defined(STM32F1))
            GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IN_FLOATING;
#elif (defined(STM32F4))
            GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IN;
            GPIO_InitStructure.GPIO_OType=  GPIO_OType_OD;
            GPIO_InitStructure.GPIO_PuPd =  GPIO_PuPd_NOPULL;
#endif
            GPIO_InitStructure.GPIO_Pin  =  BSP_DAC2_PIN;
            GPIO_InitStructure.GPIO_Speed=  GPIO_Speed_2MHz;
            GPIO_Init(BSP_DAC2_PORT,&GPIO_InitStructure);
            //DAC2初始化
            BSP_DAC2_RCC_DISABLE;
            DAC_Cmd(BSP_DAC2_CH,DISABLE);
            DAC_SoftwareTriggerCmd(BSP_DAC2_CH,DISABLE);
            break;
        default:
            break;
    }
#endif		
}
/*******************************************************************************
函数功能: 设定DA
*******************************************************************************/
void BspDac_Write(uint8_t ch,uint16_t mv)
{
#if   (defined(STM32F1))	
	  ch=ch;
	  mv=mv;
#elif (defined(STM32F4))		
    uint16_t i16;
    mv=mv;
    i16=(4096*mv)/BSP_DAC_REF_MV;
    switch(ch)
    {
        case 1:
            BSP_DAC1_SET_FUN(DAC_Align_12b_R,i16);
            DAC_SoftwareTriggerCmd(BSP_DAC1_CH,ENABLE);
            break;
        case 2:
            BSP_DAC2_SET_FUN(DAC_Align_12b_R,i16);
            DAC_SoftwareTriggerCmd(BSP_DAC2_CH,ENABLE);
            break;
        default:
            break;
    }
#endif			
}
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
