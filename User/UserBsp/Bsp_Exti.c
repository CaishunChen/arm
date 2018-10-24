/**
  ******************************************************************************
  * @file    Bsp_Exit.c 
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
//-------------------------------------------------------------------------------
#include <includes.h>
//-------------------------------------------------------------------------------
void Bsp_Exti_Init(void)
{
#if 0
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    //GPIO初始化
    //
    /*
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    GPIO_InitStructure.GPIO_Pin     =  GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    */
    //
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PA0
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//输入模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//下拉
    GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化
    //EXIT初始化
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource10);
    //
    //EXTI_ClearITPendingBit(EXTI_Line0);
    //EXTI_ClearITPendingBit(EXTI_Line1);
    //EXTI_ClearITPendingBit(EXTI_Line2);
    //EXTI_ClearITPendingBit(EXTI_Line3);
    //EXTI_ClearITPendingBit(EXTI_Line4);
    //EXTI_ClearITPendingBit(EXTI_Line5);
    //EXTI_ClearITPendingBit(EXTI_Line9);
    EXTI_ClearITPendingBit(EXTI_Line10);
    //EXTI_ClearITPendingBit(EXTI_Line11);
    //EXTI_ClearITPendingBit(EXTI_Line12);
    //EXTI_ClearITPendingBit(EXTI_Line13);
    //EXTI_ClearITPendingBit(EXTI_Line14);
    //EXTI_ClearITPendingBit(EXTI_Line15);
    //
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    /*
    EXTI_InitStructure.EXTI_Line    = EXTI_Line0 | EXTI_Line1  | EXTI_Line4 |EXTI_Line5
                                       | EXTI_Line10| EXTI_Line11| EXTI_Line12| EXTI_Line13;
                                       */
    EXTI_InitStructure.EXTI_Line    = EXTI_Line10;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    //NVIC初始化
    /*
    //
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    */
    //
    /*
    NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    */
    //
    /*
    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    */
    //
    /*
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    */
    //
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
#else
#if (defined(PROJECT_XKAP_V3)||defined(XKAP_ICARE_B_D_M))

    EXTI_InitTypeDef    EXTI_InitStructure;
    NVIC_InitTypeDef    NVIC_InitStructure;
    GPIO_InitTypeDef    GPIO_InitStructure;
    //使能SYSCFG时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    //
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType   =  GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd    =  GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Pin     =  GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin     =  GPIO_Pin_10;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
#if (HARDWARE_VER == 8)
    //连接EXTI Line10 到PD10
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource0);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource1);
    //配置
    EXTI_InitStructure.EXTI_Line    = EXTI_Line0|EXTI_Line1;
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
#else
    //连接EXTI Line10 到PD10
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource10);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource0);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource1);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource2);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource3);
    //SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource1);
    //配置
    EXTI_InitStructure.EXTI_Line    = EXTI_Line0|EXTI_Line1|EXTI_Line2|EXTI_Line3|EXTI_Line10;
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    //EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
#endif
#endif
#endif
}
/*******************************************************************************
* Function Name  : EXTI9_5_IRQHandler
* Description    : This function handles External lines 5-9 interrupt request.
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void Bsp_Exti_Fun(void)
{
    MCU_SOFT_RESET;
}
/********************************************************************************
      END FILE
********************************************************************************/

