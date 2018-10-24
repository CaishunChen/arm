/**
  ******************************************************************************
  * @file    PWMWAV.c 
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
/*******************************************************************************
说明一: 文件应用 
  1,通过系统定时器定时WAV文件的采样频率,目前为16KHz,在os_cpu_c.c中更改.
  2,通过TIM3_PWM_WAV_INIT()中设置PWM的频率.在此设为72000000/256= 281250Hz,这样正好
    识别8位采样的WAV文件
说明二: WAV文件制作
  1,先用千千静听把mp3文件改为WAV文件.
  2,再用"录音器"更改WAV文件的采样率和位数.
  3,此文件应该更改WAV文件为:16Khz/8位/单通道采样.
说明三: 由于此文件应用TIM3,所以相对于TCI-V1电路板来说与PWMIN.c冲突.
说明四:硬件更改说明:
  1,RCRC滤波电路对音质很重要,原始使用10K104电容,低音超重.
	                                   10k103电容,有所改善,但有颤音.
*******************************************************************************/
#include "includes.h"
//------------------------------------------------------------------------------Private define
//------------------------------------------------------------------------------Private variables
//------------------------------------------------------------------------------Private function
//------------------------------------------------------------------------------
//CD4051
void CD4051(uchar ch)
  {
	  GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
		//GPIO初始化									//SPK 			 //MIC
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
		//选择
		switch(ch)
		  {
			  case 0://MCU-PWM-WAV		PWM-MIC-OFF
				    GPIO_SetBits(GPIOC , GPIO_Pin_4);
				    GPIO_ResetBits(GPIOC , GPIO_Pin_5);
				    break;
				case 1://M10-PHONE			PWM-MIC-OFF
				    GPIO_ResetBits(GPIOC , GPIO_Pin_5);
				    GPIO_ResetBits(GPIOC , GPIO_Pin_4);
				    break;
				default:
				    GPIO_SetBits(GPIOC , GPIO_Pin_4);
				    GPIO_ResetBits(GPIOC , GPIO_Pin_5);
				    break;
			}
	}
//PWMWAV
void TIM3_PWM_WAV_INIT(void)
  {
	  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;

		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		//GPIO初始化(PB0,PB1)
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
		//PWMOUT初始化(PWM-P:TIM3_CH3(PB0)  PWM-N-P:TIM3_CH4(PB1))
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
		//CD4051初始化
		CD4051(0);
		/* -----------------------------------------------------------------------
    TIM3 Configuration: generate 4 PWM signals with 4 different duty cycles:
    TIM3 CLK = 72 MHz, Prescaler = 0x0, TIM4 counter clock = 72 MHz
    TIM3 ARR Register = 999 => TIM3 Frequency = TIM3 counter clock/(ARR + 1)
    TIM3 Frequency = 72 KHz.
    TIM3 Channel1 duty cycle = (TIM3_CCR1/ TIM4_ARR)* 100 = 50%
    TIM3 Channel2 duty cycle = (TIM3_CCR2/ TIM4_ARR)* 100 = 37.5%
    TIM3 Channel3 duty cycle = (TIM3_CCR3/ TIM4_ARR)* 100 = 25%
    TIM3 Channel4 duty cycle = (TIM3_CCR4/ TIM4_ARR)* 100 = 12.5%
    ----------------------------------------------------------------------- */
		// Time base configuration
		TIM_TimeBaseStructure.TIM_Period        = 256;//280KHz
    TIM_TimeBaseStructure.TIM_Prescaler     = 0;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    // PWM3 Mode configuration: Channel3
    TIM_OCInitStructure.TIM_OCMode          = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState     = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse           = 128;
    TIM_OCInitStructure.TIM_OCPolarity      = TIM_OCPolarity_High;
    TIM_OC3Init(TIM3, &TIM_OCInitStructure);
    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
		// PWM4 Mode configuration: Channel4
    TIM_OCInitStructure.TIM_OutputState     = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse           = 128;
    TIM_OC4Init(TIM3, &TIM_OCInitStructure);
    TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
	  //---
	  TIM_ARRPreloadConfig(TIM3, ENABLE);
    // TIM3 enable counter
    TIM_Cmd(TIM3, ENABLE);
		// TIM1 Main Output Enable
    TIM_CtrlPWMOutputs(TIM3, ENABLE);
	}
//---------------------------------------------------------
void PWM_Pulse_f(uint16_t f)
  { 
		TIM_OCInitTypeDef  TIM_OCInitStructure;
		f = f;
		TIM_OCInitStructure.TIM_OCMode          = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState     = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse           = f;
    TIM_OCInitStructure.TIM_OCPolarity      = TIM_OCPolarity_High;
    TIM_OC3Init(TIM3, &TIM_OCInitStructure);
    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
		
		TIM_OC4Init(TIM3, &TIM_OCInitStructure);
    TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
	}
//---------------------------------------------------------
