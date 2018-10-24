/**
  ******************************************************************************
  * @file    Bsp_It.c 
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
#include "Bsp_It.h"
#include "Bsp_Tim.h"
#include "Bsp_CpuFlash.h"
#include "Bsp_Exti.h"
#include "Module_OS.h"
#if   (defined(STM32F1))
//---->
#include "Module_Memory.h"
#include "Bsp_SdioSd_F1.h"
//<----
#elif (defined(STM32F4))
//---->
#include "Module_Memory.h"
#include "Bsp_SdioSd_F4.h"
#ifndef BOOTLOADER
#include "usb_core.h"
#include "usbd_core.h"
#include "usbd_cdc_core.h"
extern USB_OTG_CORE_HANDLE           USB_OTG_dev;
extern uint32_t USBD_OTG_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);
#endif
//<----
#endif

#ifdef PROJECT_ARMFLY_V5_XSL
#include "Bsp_WM8978.h"
#endif

/*
*********************************************************************************************************
*   函 数 名: NMI_Handler
*   功能说明: 不可屏蔽中断服务程序。
*   形    参: 无
*   返 回 值: 无
*********************************************************************************************************
*/
void NMI_Handler(void)
{
    MODULE_OS_DISABLE_INT;
    //-----用户代码-----
    //------------------
    MODULE_OS_ENABLE_INT;
}

/*
*********************************************************************************************************
*   函 数 名: HardFault_Handler
*   功能说明: 硬件故障中断服务程序。其他异常处理被关闭，而又发生了异常，则触发。
*               执行异常处理时，发生了异常，则触发。复位时默认使能。
*   形    参: 无
*   返 回 值: 无
*********************************************************************************************************
*/
void HardFault_Handler(void)
{
#ifndef BOOTLOADER
    Module_Memory_App(MODULE_MEMORY_APP_CMD_HARDFAULT,NULL,NULL);
#endif
    while (1)
    {
    }
}

/*
*********************************************************************************************************
*   函 数 名: MemManage_Handler
*   功能说明: 内存管理异常中断服务程序。违反MPU设定的存储器访问规则时触发。 复位时默认未使能
*   形    参: 无
*   返 回 值: 无
*********************************************************************************************************
*/
void MemManage_Handler(void)
{
    /* 当内存管理异常发生时进入死循环 */
    while (1)
    {
    }
}

/*
*********************************************************************************************************
*   函 数 名: BusFault_Handler
*   功能说明: 总线访问异常中断服务程序。取指令、数据读写、堆栈操作出现异常。 复位时默认未使能
*   形    参: 无
*   返 回 值: 无
*********************************************************************************************************
*/
void BusFault_Handler(void)
{
    /* 当总线异常时进入死循环 */
    while (1)
    {
    }
}

/*
*********************************************************************************************************
*   函 数 名: UsageFault_Handler
*   功能说明: 用法错误中断服务程序。执行未定义指令、非对齐操作、除零时触发。 复位时默认未使能
*   形    参: 无
*   返 回 值: 无
*********************************************************************************************************
*/
void UsageFault_Handler(void)
{
    /* 当用法异常时进入死循环 */
    while (1)
    {
    }
}

/*
*********************************************************************************************************
*   函 数 名: SVC_Handler
*   功能说明: 通过SWI指令的系统服务调用中断服务程序。
*   形    参: 无
*   返 回 值: 无
*********************************************************************************************************
*/
#ifndef OS_FREERTOS
void SVC_Handler(void)
{
    MODULE_OS_DISABLE_INT;
    //-----用户代码-----
    //------------------
    MODULE_OS_ENABLE_INT;
}
#endif
/*
*********************************************************************************************************
*   函 数 名: DebugMon_Handler
*   功能说明: 调试监视器中断服务程序。
*   形    参: 无
*   返 回 值: 无
*********************************************************************************************************
*/
void DebugMon_Handler(void)
{
    MODULE_OS_DISABLE_INT;
    //-----用户代码-----
    //------------------
    MODULE_OS_ENABLE_INT;
}

/*
*********************************************************************************************************
*   函 数 名: PendSV_Handler
*   功能说明: 可挂起的系统服务调用中断服务程序。
*   形    参: 无
*   返 回 值: 无
*********************************************************************************************************
*/
#ifndef OS_FREERTOS
void PendSV_Handler(void)
{
    MODULE_OS_DISABLE_INT;
    //-----用户代码-----
    //------------------
    MODULE_OS_ENABLE_INT;
}
#endif
/*
*********************************************************************************************************
*   功能说明: USB中断
*   形    参: 无
*   返 回 值: 无
*********************************************************************************************************
*/
#if   (defined(STM32F1))
extern void USB_Istr(void);
void USB_LP_CAN1_RX0_IRQHandler(void)
{
    MODULE_OS_DISABLE_INT;
    //-----用户代码-----
    USB_Istr();
    //------------------
    MODULE_OS_ENABLE_INT;
}
#endif
//----------
#ifdef USE_USB_OTG_FS
void OTG_FS_WKUP_IRQHandler(void)
{
    MODULE_OS_DISABLE_INT;
    //-----用户代码-----
    if(USB_OTG_dev.cfg.low_power)
    {
        *(uint32_t *)(0xE000ED10) &= 0xFFFFFFF9 ;
        SystemInit();
        USB_OTG_UngateClock(&USB_OTG_dev);
    }
    EXTI_ClearITPendingBit(EXTI_Line18);
    //------------------
    MODULE_OS_ENABLE_INT;
}
void OTG_FS_IRQHandler(void)
{
    MODULE_OS_DISABLE_INT;
    //-----用户代码-----
    USBD_OTG_ISR_Handler (&USB_OTG_dev);
    //------------------
    MODULE_OS_ENABLE_INT;
}
#endif

/*******************************************************************************
* Function Name  : TIM2_IRQHandler
* Description    : This function handles USART1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM2_IRQHandler(void)
{
    MODULE_OS_DISABLE_INT;
    //-----用户代码-----
#ifndef BOOTLOADER
    TIM2_ISRHandler();
#endif
    //------------------
    MODULE_OS_ENABLE_INT;
}
/*
*********************************************************************************************************
*   函 数 名: USARTx_IRQHandler
*   功能说明: 串口中断服务程序。
*   形    参: 无
*   返 回 值: 无
*********************************************************************************************************
*/
void USARTx_IRQHandler(void)
{
    MODULE_OS_DISABLE_INT;
    //-----用户代码-----
    //------------------
    MODULE_OS_ENABLE_INT;
}

/*
*********************************************************************************************************
*   函 数 名: USARTx_DMA_TX_IRQHandler
*   功能说明: 串口1在DMA方式下的发送中断
*   形    参: 无
*   返 回 值: 无
*********************************************************************************************************
*/
void USARTx_DMA_TX_IRQHandler(void)
{
    MODULE_OS_DISABLE_INT;
    //-----用户代码-----
    //------------------
    MODULE_OS_ENABLE_INT;
}

/*
*********************************************************************************************************
*   函 数 名: USARTx_DMA_RX_IRQHandler
*   功能说明: 串口1在DMA方式下的接收中断
*   形    参: 无
*   返 回 值: 无
*********************************************************************************************************
*/
void USARTx_DMA_RX_IRQHandler(void)
{
    MODULE_OS_DISABLE_INT;
    //-----用户代码-----
    //------------------
    MODULE_OS_ENABLE_INT;
}
/*
*********************************************************************************************************
*   函 数 名: 外中断
*   功能说明:
*********************************************************************************************************
*/
void EXTI0_IRQHandler(void)
{
    MODULE_OS_DISABLE_INT;
    if(EXTI_GetITStatus(EXTI_Line0) != RESET)
    {
        //-----用户代码
#ifndef BOOTLOADER
        Bsp_Exti_Fun();
#endif
        //-----
        EXTI->IMR&=~(1<<0);
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
    MODULE_OS_ENABLE_INT;
}
//----------
void EXTI1_IRQHandler(void)
{
    MODULE_OS_DISABLE_INT;
    if(EXTI_GetITStatus(EXTI_Line1) != RESET)
    {
        //-----用户代码
#ifndef BOOTLOADER
        Bsp_Exti_Fun();
#endif
        //-----
        EXTI->IMR&=~(1<<1);
        EXTI_ClearITPendingBit(EXTI_Line1);
    }
    MODULE_OS_ENABLE_INT;
}
//----------
void EXTI2_IRQHandler(void)
{
    MODULE_OS_DISABLE_INT;
    if(EXTI_GetITStatus(EXTI_Line2) != RESET)
    {
        //-----用户代码
#ifndef BOOTLOADER
        Bsp_Exti_Fun();
#endif
        //-----
        EXTI->IMR&=~(1<<2);
        EXTI_ClearITPendingBit(EXTI_Line2);
    }
    MODULE_OS_ENABLE_INT;
}
//----------
void EXTI3_IRQHandler(void)
{
    MODULE_OS_DISABLE_INT;
    if(EXTI_GetITStatus(EXTI_Line3) != RESET)
    {
        //-----用户代码
#ifndef BOOTLOADER
        Bsp_Exti_Fun();
#endif
        //-----
        EXTI->IMR&=~(1<<3);
        EXTI_ClearITPendingBit(EXTI_Line3);
    }
    MODULE_OS_ENABLE_INT;
}
void EXTI4_IRQHandler(void)
{
    MODULE_OS_DISABLE_INT;
    if(EXTI_GetITStatus(EXTI_Line4) != RESET)
    {
        //-----用户代码
        //-----
        EXTI->IMR&=~(1<<4);
        EXTI_ClearITPendingBit(EXTI_Line4);
    }
    MODULE_OS_ENABLE_INT;
}
void EXTI9_5_IRQHandler(void)
{
    MODULE_OS_DISABLE_INT;
    if(EXTI_GetITStatus(EXTI_Line5) != RESET)
    {
        //-----用户代码
        //-----
        EXTI->IMR&=~(1<<5);
        EXTI_ClearITPendingBit(EXTI_Line5);
    }
    if(EXTI_GetITStatus(EXTI_Line6) != RESET)
    {
        //-----用户代码
        //-----
        EXTI->IMR&=~(1<<6);
        EXTI_ClearITPendingBit(EXTI_Line6);
    }
    if(EXTI_GetITStatus(EXTI_Line7) != RESET)
    {
        //-----用户代码
        //-----
        EXTI->IMR&=~(1<<7);
        EXTI_ClearITPendingBit(EXTI_Line7);
    }
    if(EXTI_GetITStatus(EXTI_Line8) != RESET)
    {
        //-----用户代码
        //-----
        EXTI->IMR&=~(1<<8);
        EXTI_ClearITPendingBit(EXTI_Line8);
    }
    if(EXTI_GetITStatus(EXTI_Line9) != RESET)
    {
        //-----用户代码
        //-----
        EXTI->IMR&=~(1<<9);
        EXTI_ClearITPendingBit(EXTI_Line9);
    }
    MODULE_OS_ENABLE_INT;
}
void EXTI15_10_IRQHandler(void)
{
    MODULE_OS_DISABLE_INT;
    if(EXTI_GetITStatus(EXTI_Line10) != RESET)
    {
        //-----用户代码
#ifndef BOOTLOADER
        Bsp_Exti_Fun();
#endif
        //-----
        EXTI->IMR&=~(1<<10);
        EXTI_ClearITPendingBit(EXTI_Line10);
    }
    if(EXTI_GetITStatus(EXTI_Line11) != RESET)
    {
        //-----用户代码
        //-----
        EXTI->IMR&=~(1<<11);
        EXTI_ClearITPendingBit(EXTI_Line11);
    }
    if(EXTI_GetITStatus(EXTI_Line12) != RESET)
    {
        //-----用户代码
        //-----
        EXTI->IMR&=~(1<<12);
        EXTI_ClearITPendingBit(EXTI_Line12);
    }
    if(EXTI_GetITStatus(EXTI_Line13) != RESET)
    {
        //-----用户代码
        //-----
        EXTI->IMR&=~(1<<13);
        EXTI_ClearITPendingBit(EXTI_Line13);
    }
    if(EXTI_GetITStatus(EXTI_Line14) != RESET)
    {
        //-----用户代码
        //-----
        EXTI->IMR&=~(1<<14);
        EXTI_ClearITPendingBit(EXTI_Line14);
    }
    if(EXTI_GetITStatus(EXTI_Line15) != RESET)
    {
        //-----用户代码
        //-----
        EXTI->IMR&=~(1<<15);
        EXTI_ClearITPendingBit(EXTI_Line15);
    }
    MODULE_OS_ENABLE_INT;
}

#if   (defined(STM32F4))
/*
*********************************************************************************************************
*   函 数 名: SDIO_IRQHandler
*   功能说明: This function handles WWDG interrupt request.
*   形    参：无
*   返 回 值: 无
*********************************************************************************************************
*/

void SDIO_IRQHandler(void)
{
    MODULE_OS_DISABLE_INT;
    //-----用户代码-----
    SD_ProcessIRQSrc();
    //------------------
    MODULE_OS_ENABLE_INT;
}

/*
*********************************************************************************************************
*   函 数 名: SD_SDIO_DMA_IRQHANDLER
*   功能说明: This function handles WWDG interrupt request.
*   形    参：无
*   返 回 值: 无
*********************************************************************************************************
*/
void SD_SDIO_DMA_IRQHANDLER(void)
{
    MODULE_OS_DISABLE_INT;
    //-----用户代码-----
    SD_ProcessDMAIRQ();
    //------------------
    MODULE_OS_ENABLE_INT;
}

/*
*********************************************************************************************************
*   功能说明: 闹钟中断
*   形    参：无
*   返 回 值: 无
*********************************************************************************************************
*/
void RTC_Alarm_IRQHandler(void)
{
    MODULE_OS_DISABLE_INT;
    if(RTC_GetITStatus(RTC_IT_ALRA) != RESET)
    {
        //-----用户代码
#ifndef BOOTLOADER
        Bsp_Exti_Fun();
#endif
        //-----
        PWR_BackupAccessCmd(ENABLE);
        RTC_ClearITPendingBit(RTC_IT_ALRA);
        PWR_BackupAccessCmd(DISABLE);
        EXTI_ClearITPendingBit(EXTI_Line17);
    }
    MODULE_OS_ENABLE_INT;
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
    MODULE_OS_DISABLE_INT;
    //-----XSL-----
    if (SPI_I2S_GetITStatus(SPI2, SPI_I2S_IT_RXNE) != RESET)
    {
        uint8_t data;
        data=data;
        data=SPI2->DR;
    }
#ifdef PROJECT_ARMFLY_V5_XSL
    I2S_CODEC_DataTransfer();
#endif
    //-------------
    MODULE_OS_ENABLE_INT;
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
    MODULE_OS_DISABLE_INT;
    //-----XSL-----
    if (SPI_I2S_GetITStatus(SPI3, SPI_I2S_IT_RXNE) != RESET)
    {
        uint8_t data;
        data=data;
        data=SPI3->DR;
    }
    //-------------
    MODULE_OS_ENABLE_INT;
}
#endif
/**************************************************************/
