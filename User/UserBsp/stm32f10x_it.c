/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "includes.h"
#include "Bsp_Uart.h"
#include "Bsp_Tim.h"
#include "uctsk_Debug.h"
//------------------------------- 用户变量 ----------------------------------

//extern  void TIM3_ISRHandler(void);
extern  void PWM_Pulse_f(uint8_t f);
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
#if   (defined(OS_UCOSIII))
    CPU_SR_ALLOC();
    CPU_CRITICAL_ENTER();
    OSIntNestingCtr++;
    CPU_CRITICAL_EXIT();
    //-----用户代码-----
    //------------------
    OSIntExit();
#else
#endif
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
    while (1)
    {
    }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
    /* Go to infinite loop when Memory Manage exception occurs */
    while (1)
    {
    }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
    /* Go to infinite loop when Bus Fault exception occurs */
    while (1)
    {
    }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
    /* Go to infinite loop when Usage Fault exception occurs */
    while (1)
    {
    }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}
/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
/*
void PendSV_Handler(void)
{
}
*/


/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */

void SysTick_Handler(void)
{
#if (defined(OS_UCOSII))
    CPU_SR         cpu_sr;
    OS_ENTER_CRITICAL();
    OSIntNesting++;
    OS_EXIT_CRITICAL();
#endif
    //-----
    OSTimeTick();
    //-----
#if (defined(OS_UCOSII))
    OSIntExit();
#endif
}


/*******************************************************************************
* Function Name  : TIM2_IRQHandler
* Description    : This function handles USART1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM2_IRQHandler(void)
{
#if (defined(OS_UCOSII))
    CPU_SR         cpu_sr;
    OS_ENTER_CRITICAL();
    OSIntNesting++;
    OS_EXIT_CRITICAL();
#endif
    //-----
    TIM2_ISRHandler();
    //-----
#if (defined(OS_UCOSII))
    OSIntExit();
#endif
}
/*******************************************************************************
* Function Name  : TIM3_IRQHandler
* Description    : This function handles USART1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM3_IRQHandler(void)
{
#if (defined(OS_UCOSII))
    CPU_SR         cpu_sr;
    OS_ENTER_CRITICAL();
    OSIntNesting++;
    OS_EXIT_CRITICAL();
#endif
    //-----
    //TIM3_ISRHandler();
    //-----
#if (defined(OS_UCOSII))
    OSIntExit();
#endif
}
/*******************************************************************************
* Function Name  : EXTI0_IRQHandler
* Description    : This function handles External lines 0 interrupt request.
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void EXTI0_IRQHandler(void)
{
#if (defined(OS_UCOSII))
    CPU_SR         cpu_sr;
    OS_ENTER_CRITICAL();
    OSIntNesting++;
    OS_EXIT_CRITICAL();
#endif
    //-----
    if ( EXTI_GetITStatus(EXTI_Line0) != RESET )
    {
        EXTI_ClearITPendingBit(EXTI_Line0);
        //-----应用层-----BEGIN
        //-----应用层-----END
    }
    //-----
#if (defined(OS_UCOSII))
    OSIntExit();
#endif
}
/*******************************************************************************
* Function Name  : EXTI1_IRQHandler
* Description    : This function handles External lines 1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void EXTI1_IRQHandler(void)
{
#if (defined(OS_UCOSII))
    CPU_SR         cpu_sr;
    OS_ENTER_CRITICAL();
    OSIntNesting++;
    OS_EXIT_CRITICAL();
#endif
    //-----
    if ( EXTI_GetITStatus(EXTI_Line1) != RESET )
    {
        EXTI_ClearITPendingBit(EXTI_Line1);
        //-----应用层-----BEGIN
        //-----应用层-----END
    }
    //-----
#if (defined(OS_UCOSII))
    OSIntExit();
#endif
}
/*******************************************************************************
* Function Name  : EXTI2_IRQHandler
* Description    : This function handles External lines 2 interrupt request.
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void EXTI2_IRQHandler(void)
{
#if (defined(OS_UCOSII))
    CPU_SR         cpu_sr;
    OS_ENTER_CRITICAL();
    OSIntNesting++;
    OS_EXIT_CRITICAL();
#endif
    //-----
    if ( EXTI_GetITStatus(EXTI_Line2) != RESET )
    {
        EXTI_ClearITPendingBit(EXTI_Line2);
        //-----应用层-----BEGIN
        //EXTI_ISRHandler();
        //-----应用层-----END
    }
    //-----
#if (defined(OS_UCOSII))
    OSIntExit();
#endif
}
/*******************************************************************************
* Function Name  : EXTI3_IRQHandler
* Description    : This function handles External lines 3 interrupt request.
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void EXTI3_IRQHandler(void)
{
#if (defined(OS_UCOSII))
    CPU_SR         cpu_sr;
    OS_ENTER_CRITICAL();
    OSIntNesting++;
    OS_EXIT_CRITICAL();
#endif
    //-----
    if ( EXTI_GetITStatus(EXTI_Line3) != RESET )
    {
        EXTI_ClearITPendingBit(EXTI_Line3);
        //-----应用层-----BEGIN
        //EXTI_ISRHandler();
        //-----应用层-----END
    }
    //-----
#if (defined(OS_UCOSII))
    OSIntExit();
#endif
}
/*******************************************************************************
* Function Name  : EXTI4_IRQHandler
* Description    : This function handles External lines 4 interrupt request.
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void EXTI4_IRQHandler(void)
{
#if (defined(OS_UCOSII))
    CPU_SR         cpu_sr;
    OS_ENTER_CRITICAL();
    OSIntNesting++;
    OS_EXIT_CRITICAL();
#endif
    //-----
    if ( EXTI_GetITStatus(EXTI_Line4) != RESET )
    {
        EXTI_ClearITPendingBit(EXTI_Line4);
        //-----应用层-----BEGIN
        //EXTI_ISRHandler();
        //-----应用层-----END
    }
    //-----
#if (defined(OS_UCOSII))
    OSIntExit();
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
void EXTI9_5_IRQHandler(void)
{
#if (defined(OS_UCOSII))
    CPU_SR         cpu_sr;
    OS_ENTER_CRITICAL();
    OSIntNesting++;
    OS_EXIT_CRITICAL();
#endif
    //-----
    if ( EXTI_GetITStatus(EXTI_Line5) != RESET )
    {
        EXTI_ClearITPendingBit(EXTI_Line5);
        //-----应用层-----BEGIN
        //EXTI_ISRHandler();
        //-----应用层-----END
    }
    if ( EXTI_GetITStatus(EXTI_Line6) != RESET )
    {
        EXTI_ClearITPendingBit(EXTI_Line6);
        //-----应用层-----BEGIN
        //EXTI_ISRHandler();
        //-----应用层-----END
    }
    if ( EXTI_GetITStatus(EXTI_Line7) != RESET )
    {
        EXTI_ClearITPendingBit(EXTI_Line7);
        //-----应用层-----BEGIN
        //EXTI_ISRHandler();
#ifdef __W5500_H
        W5500_IRQHandler();
#endif
        //-----应用层-----END
    }
    if ( EXTI_GetITStatus(EXTI_Line8) != RESET )
    {
        EXTI_ClearITPendingBit(EXTI_Line8);
        //-----应用层-----BEGIN
        //EXTI_ISRHandler();
        //-----应用层-----END
    }
    if ( EXTI_GetITStatus(EXTI_Line9) != RESET )
    {
        EXTI_ClearITPendingBit(EXTI_Line9);
        //-----应用层-----BEGIN
        //EXTI_ISRHandler();
        //-----应用层-----END
    }
    //-----
#if (defined(OS_UCOSII))
    OSIntExit();
#endif
}
/*******************************************************************************
* Function Name  : EXTI15_10_IRQHandler
* Description    : This function handles External lines 10-15 interrupt request.
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void EXTI15_10_IRQHandler(void)
{
#if (defined(OS_UCOSII))
    CPU_SR         cpu_sr;
    OS_ENTER_CRITICAL();
    OSIntNesting++;
    OS_EXIT_CRITICAL();
#endif
    //-----
    if ( EXTI_GetITStatus(EXTI_Line10) != RESET )
    {
        EXTI_ClearITPendingBit(EXTI_Line10);
        //-----应用层-----BEGIN
        //EXTI_ISRHandler();
        //-----应用层-----END
    }
    if ( EXTI_GetITStatus(EXTI_Line11) != RESET )
    {
        EXTI_ClearITPendingBit(EXTI_Line11);
        //-----应用层-----BEGIN
        //EXTI_ISRHandler();
        //-----应用层-----END
    }
    if ( EXTI_GetITStatus(EXTI_Line12) != RESET )
    {
        EXTI_ClearITPendingBit(EXTI_Line12);
        //-----应用层-----BEGIN
        //-----应用层-----END
    }
    if ( EXTI_GetITStatus(EXTI_Line13) != RESET )
    {
        EXTI_ClearITPendingBit(EXTI_Line13);
        //-----应用层-----BEGIN
        //EXTI_ISRHandler();
        //-----应用层-----END
    }
    if ( EXTI_GetITStatus(EXTI_Line14) != RESET )
    {
        EXTI_ClearITPendingBit(EXTI_Line14);
        //-----应用层-----BEGIN
        //EXTI_ISRHandler();
        //-----应用层-----END
    }
    if ( EXTI_GetITStatus(EXTI_Line15) != RESET )
    {
        EXTI_ClearITPendingBit(EXTI_Line15);
        //-----应用层-----BEGIN
        //-----应用层-----END
    }
    //-----
#if (defined(OS_UCOSII))
    OSIntExit();
#endif
}

/*******************************************************************************
* Function Name  : CAN1_RX0_IRQHandler
* Description    : Can接收中断.
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
extern void USB_Istr(void);
void USB_LP_CAN1_RX0_IRQHandler(void)
{
#if (defined(OS_UCOSII))
    CPU_SR         cpu_sr;
    OS_ENTER_CRITICAL();
    OSIntNesting++;
    OS_EXIT_CRITICAL();
#endif
    //-----
    //CAN_RX_IRQHandler();
    USB_Istr();
    //-----
#if (defined(OS_UCOSII))
    OSIntExit();
#endif
}

/*******************************************************************************
* Function Name  : SPI2_IRQHandler
* Description    : This function handles SPI2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI2_IRQHandler(void)
{
#if (defined(OS_UCOSII))
    CPU_SR         cpu_sr;
    OS_ENTER_CRITICAL();
    OSIntNesting++;
    OS_EXIT_CRITICAL();
#endif
    //-----XSL-----用户代码
    if (SPI_I2S_GetITStatus(SPI2, SPI_I2S_IT_RXNE) != RESET)
    {
        uint8_t data;
        data=data;
        data=SPI2->DR;
    }
    //-------------
#if (defined(OS_UCOSII))
    OSIntExit();
#endif
}
/*******************************************************************************
* Function Name  : SPI3_IRQHandler
* Description    : This function handles SPI2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI3_IRQHandler(void)
{
#if (defined(OS_UCOSII))
    CPU_SR         cpu_sr;
    OS_ENTER_CRITICAL();
    OSIntNesting++;
    OS_EXIT_CRITICAL();
#endif
    //-----XSL-----用户代码
    if (SPI_I2S_GetITStatus(SPI3, SPI_I2S_IT_RXNE) != RESET)
    {
        uint8_t data;
        data=data;
        data=SPI3->DR;
    }
    //-------------
#if (defined(OS_UCOSII))
    OSIntExit();
#endif
}
/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */


/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
