/**
  ******************************************************************************
  * @file    Bsp_Uart.c 
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
//------------------------------------------------------------------------------
#include "Bsp_Uart.h"
#include "uctsk_Debug.h"

#if (defined(NRF51)||defined(NRF52))
#include "app_uart.h"
#include "nrf_delay.h"
#endif

#ifdef BSP_UART0_USB_ENABLE
//#include "HW_Config.h"
extern void USB_Config(void);
extern uint32_t USB_TxWrite(uint8_t *buffter, uint32_t writeLen);
#endif
//------------------------------------------------------------------------------Private define
static uint8_t BspUart_DebugTest_Enable=0;
//------------------------------------------------------------------------------Private variables
#ifdef BSP_UART1_ENABLE
static uint8_t Uart_Tx1SendEnable=2;  //0->不允许发送,1->发送即将完成(用于485切换),2->发送已完成
#if   (defined(STM32F1)||defined(STM32F4))
static uint8_t Uart_Tx1Buf[UART1_TX_BUF_SIZE];
#endif
static uint8_t Uart_Rx1Buf[UART1_RX_BUF_SIZE];
#endif
//
#ifdef BSP_UART2_ENABLE
static uint8_t Uart_Tx2SendEnable=2;  //0->不允许发送,1->发送即将完成(用于485切换),2->发送已完成
#if   (defined(STM32F1)||defined(STM32F4))
static uint8_t Uart_Tx2Buf[UART2_TX_BUF_SIZE];
static uint8_t Uart_Rx2Buf[UART2_RX_BUF_SIZE];
#endif
#endif
//
#ifdef BSP_UART3_ENABLE
static uint8_t Uart_Tx3SendEnable=2;  //0->不允许发送,1->发送即将完成(用于485切换),2->发送已完成
#if   (defined(STM32F1)||defined(STM32F4))
static uint8_t Uart_Tx3Buf[UART3_TX_BUF_SIZE];
static uint8_t Uart_Rx3Buf[UART3_RX_BUF_SIZE];
#endif
#endif
//
#ifdef BSP_UART4_ENABLE
static uint8_t Uart_Tx4SendEnable=2;  //0->不允许发送,1->发送即将完成(用于485切换),2->发送已完成
#if   (defined(STM32F1)||defined(STM32F4))
static uint8_t Uart_Tx4Buf[UART4_TX_BUF_SIZE];
static uint8_t Uart_Rx4Buf[UART4_RX_BUF_SIZE];
#endif
#endif
//
#ifdef BSP_UART5_ENABLE
static uint8_t Uart_Tx5SendEnable=2;  //0->不允许发送,1->发送即将完成(用于485切换),2->发送已完成
#if   (defined(STM32F1)||defined(STM32F4))
static uint8_t Uart_Tx5Buf[UART5_TX_BUF_SIZE];
static uint8_t Uart_Rx5Buf[UART5_RX_BUF_SIZE];
#endif
static uint8_t Uart_Rx5Step=0;        //0->未开始接收,1->正在接受中,2->接收完毕
static uint8_t Uart_Rx5Timer_ms=0;
void UART_TX5_IRQ(uint16_t len);
void UART_RX5_IRQ(uint8_t d);
#endif
//
#ifdef BSP_UART6_ENABLE
#endif
//------------------------------------------------------------------------------
#if   (defined(BSP_UART0_USB_ENABLE)&&defined(STM32F1))
void BspUart_UsbVpcRx(void)
{
    uint32_t len = 0;
    uint8_t *pbuf;
    //申请缓存
    pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    len = USB_RxRead(pbuf,256);
    if (len > 0)
    {
        Debug_InterruptRx(pbuf, len);
    }
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
}
#endif
//------------------------------------------------------------------------------
#ifdef BSP_UART1_ENABLE
/*******************************************************************************
* 函数功能: 中断函数-->串口1DMA方式发送
*******************************************************************************/
void UART1_DMA_TX_IRQHandler(void)
{
#if   (defined(OS_UCOSII))
    CPU_SR         cpu_sr;
    OS_ENTER_CRITICAL();
    OSIntNesting++;
    OS_EXIT_CRITICAL();
#elif (defined(OS_UCOSIII))
    CPU_SR_ALLOC();
    CPU_CRITICAL_ENTER();
    OSIntNestingCtr++;
    CPU_CRITICAL_EXIT();
#endif
    //
    //清除标志位
#if   (defined(STM32F1))
    DMA_ClearFlag(UART1_TX_DMA_FLAG_TCIF);
    //关闭DMA
    DMA_Cmd(UART1_TX_DMA_STREAM,DISABLE);
#elif (defined(STM32F4))
    DMA_ClearFlag(UART1_TX_DMA_STREAM,UART1_TX_DMA_FLAG_TCIF);
    //关闭DMA
    DMA_Cmd(UART1_TX_DMA_STREAM,DISABLE);
#endif
    //数据即将发送完毕
    Uart_Tx1SendEnable = 1;
#if   (defined(OS_UCOSII))
    OSIntExit();
#elif (defined(OS_UCOSIII))
    OSIntExit();
#endif
}
/*******************************************************************************
* 函数功能: 串口1接收函数
*******************************************************************************/
void UART1_RxTx_IRQHandler(void)
{
    uint16_t tep;
    uint8_t val;
#if   (defined(OS_UCOSII))
    CPU_SR         cpu_sr;
    OS_ENTER_CRITICAL();
    OSIntNesting++;
    OS_EXIT_CRITICAL();
#elif (defined(OS_UCOSIII))
    CPU_SR_ALLOC();
    CPU_CRITICAL_ENTER();
    OSIntNestingCtr++;
    CPU_CRITICAL_EXIT();
#endif
#if   (defined(STM32F1)||defined(STM32F4))
    //出错处理
    if(  USART_GetITStatus(USART1, USART_IT_ORE) == SET
         ||USART_GetITStatus(USART1, USART_IT_NE)  == SET
         ||USART_GetITStatus(USART1, USART_IT_FE)  == SET
         ||USART_GetITStatus(USART1, USART_IT_PE)  == SET)
    {
        tep = USART1->SR;
        tep = USART1->DR;
    }

    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        val = USART1->DR;
        //-----用户程序-----Begin
        val=val;
        //-----用户程序-----End
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
    if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
    {
        //USART_ClearFlag(USART1,USART_IT_IDLE);
        tep = USART1->SR;
        tep = USART1->DR; //清USART_IT_IDLE标志
        DMA_Cmd(UART1_RX_DMA_STREAM,DISABLE);
        tep = UART1_RX_BUF_SIZE - DMA_GetCurrDataCounter(UART1_RX_DMA_STREAM);
        //-----用户程序-----Begin
#ifdef  UART1_DMA_RX_PARSE_PRO
        UART1_DMA_RX_PARSE_PRO(Uart_Rx1Buf,tep);
#else
        tep=tep;
#endif
        //-----用户程序-----End
        //设置传输数据长度
        DMA_SetCurrDataCounter(UART1_RX_DMA_STREAM,UART1_RX_BUF_SIZE);
        //打开DMA
        DMA_Cmd(UART1_RX_DMA_STREAM,ENABLE);
    }
#elif (defined(NRF51)||defined(NRF52)) 
    val=val;
    tep=tep;
#endif    
    __nop();
#if   (defined(OS_UCOSII))
    OSIntExit();
#elif (defined(OS_UCOSIII))
    OSIntExit();
#endif
}
#endif
#ifdef BSP_UART2_ENABLE
/*******************************************************************************
* 函数功能: 中断函数-->串口2DMA方式发送
*******************************************************************************/
void UART2_DMA_TX_IRQHandler(void)
{
#if   (defined(OS_UCOSII))
    CPU_SR         cpu_sr;
    OS_ENTER_CRITICAL();
    OSIntNesting++;
    OS_EXIT_CRITICAL();
#elif (defined(OS_UCOSIII))
    CPU_SR_ALLOC();
    CPU_CRITICAL_ENTER();
    OSIntNestingCtr++;
    CPU_CRITICAL_EXIT();
#endif
    //清除标志位
#if   (defined(STM32F1))
    DMA_ClearFlag(UART2_TX_DMA_FLAG_TCIF);
    //关闭DMA
    DMA_Cmd(UART2_TX_DMA_STREAM,DISABLE);
#elif (defined(STM32F4))
    DMA_ClearFlag(UART2_TX_DMA_STREAM,UART2_TX_DMA_FLAG_TCIF);
    //关闭DMA
    DMA_Cmd(UART2_TX_DMA_STREAM,DISABLE);
#endif
    //数据即将发送完毕
    Uart_Tx2SendEnable = 1;
#if   (defined(OS_UCOSII))
    OSIntExit();
#elif (defined(OS_UCOSIII))
    OSIntExit();
#endif
}
/*******************************************************************************
* 函数功能: 串口2接收函数
*******************************************************************************/
void UART2_RxTx_IRQHandler(void)
{
    uint8_t val;
    uint16_t tep;
#if   (defined(OS_UCOSII))
    CPU_SR         cpu_sr;
    OS_ENTER_CRITICAL();
    OSIntNesting++;
    OS_EXIT_CRITICAL();
#elif (defined(OS_UCOSIII))
    CPU_SR_ALLOC();
    CPU_CRITICAL_ENTER();
    OSIntNestingCtr++;
    CPU_CRITICAL_EXIT();
#endif
    //出错处理
    if(  USART_GetITStatus(USART2, USART_IT_ORE) == SET
         ||USART_GetITStatus(USART2, USART_IT_NE)  == SET
         ||USART_GetITStatus(USART2, USART_IT_FE)  == SET
         ||USART_GetITStatus(USART2, USART_IT_PE)  == SET)
    {
        tep = USART2->SR;
        tep = USART2->DR;
    }

    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        val = USART2->DR;
        //-----用户程序-----Begin
        val=val;
        //Debug_InterruptRx(val);
        //-----用户程序-----End
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
    }
    if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
    {
        //USART_ClearFlag(USART1,USART_IT_IDLE);
        tep = USART2->SR;
        tep = USART2->DR; //清USART_IT_IDLE标志
        DMA_Cmd(UART2_RX_DMA_STREAM,DISABLE);

        tep = UART2_RX_BUF_SIZE - DMA_GetCurrDataCounter(UART2_RX_DMA_STREAM);
        //-----用户程序-----Begin
#ifdef  UART2_DMA_RX_PARSE_PRO
        UART2_DMA_RX_PARSE_PRO(Uart_Rx2Buf,tep);
#else
        tep=tep;
#endif
        //-----用户程序-----End
        //设置传输数据长度
        DMA_SetCurrDataCounter(UART2_RX_DMA_STREAM,UART2_RX_BUF_SIZE);
        //打开DMA
        DMA_Cmd(UART2_RX_DMA_STREAM,ENABLE);
    }

    __nop();
#if   (defined(OS_UCOSII))
    OSIntExit();
#elif (defined(OS_UCOSIII))
    OSIntExit();
#endif
}
#endif
#ifdef BSP_UART3_ENABLE
/*******************************************************************************
* 函数功能: 中断函数-->串口3DMA方式发送
*******************************************************************************/
void UART3_DMA_TX_IRQHandler(void)
{
#if   (defined(OS_UCOSII))
    CPU_SR         cpu_sr;
    OS_ENTER_CRITICAL();
    OSIntNesting++;
    OS_EXIT_CRITICAL();
#elif (defined(OS_UCOSIII))
    CPU_SR_ALLOC();
    CPU_CRITICAL_ENTER();
    OSIntNestingCtr++;
    CPU_CRITICAL_EXIT();
#endif
    //清除标志位
#if   (defined(STM32F1))
    DMA_ClearFlag(UART3_TX_DMA_FLAG_TCIF);
    //关闭DMA
    DMA_Cmd(UART3_TX_DMA_STREAM,DISABLE);
#elif (defined(STM32F4))
    DMA_ClearFlag(UART3_TX_DMA_STREAM,UART3_TX_DMA_FLAG_TCIF);
    //关闭DMA
    DMA_Cmd(UART3_TX_DMA_STREAM,DISABLE);
#endif
    //数据即将发送完毕
    Uart_Tx3SendEnable = 1;
#if   (defined(OS_UCOSII))
    OSIntExit();
#elif (defined(OS_UCOSIII))
    OSIntExit();
#endif
}
/*******************************************************************************
* 函数功能: 串口3接收函数
*******************************************************************************/
void UART3_RxTx_IRQHandler(void)
{
    uint8_t val;
    uint16_t tep;
#if   (defined(OS_UCOSII))
    CPU_SR         cpu_sr;
    OS_ENTER_CRITICAL();
    OSIntNesting++;
    OS_EXIT_CRITICAL();
#elif (defined(OS_UCOSIII))
    CPU_SR_ALLOC();
    CPU_CRITICAL_ENTER();
    OSIntNestingCtr++;
    CPU_CRITICAL_EXIT();
#endif
    //出错处理
    if(  USART_GetITStatus(USART3, USART_IT_ORE) == SET
         ||USART_GetITStatus(USART3, USART_IT_NE)  == SET
         ||USART_GetITStatus(USART3, USART_IT_FE)  == SET
         ||USART_GetITStatus(USART3, USART_IT_PE)  == SET)
    {
        tep = USART3->SR;
        tep = USART3->DR;
    }

    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
        val = USART3->DR;
        //-----用户程序-----Begin
        val=val;
        //-----用户程序-----End
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
    }
    if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)
    {
        //USART_ClearFlag(USART1,USART_IT_IDLE);
        tep = USART3->SR;
        tep = USART3->DR; //清USART_IT_IDLE标志
        DMA_Cmd(UART3_RX_DMA_STREAM,DISABLE);

        tep = UART3_RX_BUF_SIZE - DMA_GetCurrDataCounter(UART3_RX_DMA_STREAM);
        //-----用户程序-----Begin
        if(BspUart_DebugTest_Enable==1)
        {
            DebugOut((int8_t*)Uart_Rx3Buf,tep);
        }
#ifdef  UART3_DMA_RX_PARSE_PRO
        UART3_DMA_RX_PARSE_PRO(Uart_Rx3Buf,tep);
#else
        tep=tep;
#endif
        //-----用户程序-----End
        //设置传输数据长度
        DMA_SetCurrDataCounter(UART3_RX_DMA_STREAM,UART3_RX_BUF_SIZE);
        //打开DMA
        DMA_Cmd(UART3_RX_DMA_STREAM,ENABLE);
    }

    __nop();
#if   (defined(OS_UCOSII))
    OSIntExit();
#elif (defined(OS_UCOSIII))
    OSIntExit();
#endif
}
#endif
#ifdef BSP_UART4_ENABLE
/*******************************************************************************
* 函数功能: 中断函数-->串口4DMA方式发送
*******************************************************************************/
void UART4_DMA_TX_IRQHandler(void)
{
#if   (defined(OS_UCOSII))
    CPU_SR         cpu_sr;
    OS_ENTER_CRITICAL();
    OSIntNesting++;
    OS_EXIT_CRITICAL();
#elif (defined(OS_UCOSIII))
    CPU_SR_ALLOC();
    CPU_CRITICAL_ENTER();
    OSIntNestingCtr++;
    CPU_CRITICAL_EXIT();
#endif
    //清除标志位
#if   (defined(STM32F1))
    DMA_ClearFlag(UART4_TX_DMA_FLAG_TCIF);
    //关闭DMA
    DMA_Cmd(UART4_TX_DMA_STREAM,DISABLE);
#elif (defined(STM32F4))
    DMA_ClearFlag(UART4_TX_DMA_STREAM,UART4_TX_DMA_FLAG_TCIF);
    //关闭DMA
    DMA_Cmd(UART4_TX_DMA_STREAM,DISABLE);
#endif
    //数据即将发送完毕
    Uart_Tx4SendEnable = 1;
#if   (defined(OS_UCOSII))
    OSIntExit();
#elif (defined(OS_UCOSIII))
    OSIntExit();
#endif
}
/*******************************************************************************
* 函数功能: 串口4接收函数
*******************************************************************************/
void UART4_RxTx_IRQHandler(void)
{
    uint8_t val;
    uint16_t tep;
#if   (defined(OS_UCOSII))
    CPU_SR         cpu_sr;
    OS_ENTER_CRITICAL();
    OSIntNesting++;
    OS_EXIT_CRITICAL();
#elif (defined(OS_UCOSIII))
    CPU_SR_ALLOC();
    CPU_CRITICAL_ENTER();
    OSIntNestingCtr++;
    CPU_CRITICAL_EXIT();
#endif
    //出错处理
    if(  USART_GetITStatus(UART4, USART_IT_ORE) == SET
         ||USART_GetITStatus(UART4, USART_IT_NE)  == SET
         ||USART_GetITStatus(UART4, USART_IT_FE)  == SET
         ||USART_GetITStatus(UART4, USART_IT_PE)  == SET)
    {
        tep = UART4->SR;
        tep = UART4->DR;
    }

    if (USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
    {
        val = UART4->DR;
        //-----用户程序-----Begin
        val=val;
        //-----用户程序-----End
        USART_ClearITPendingBit(UART4, USART_IT_RXNE);
    }
    if(USART_GetITStatus(UART4, USART_IT_IDLE) != RESET)
    {
        //USART_ClearFlag(USART1,USART_IT_IDLE);
        tep = UART4->SR;
        tep = UART4->DR; //清USART_IT_IDLE标志
        DMA_Cmd(UART4_RX_DMA_STREAM,DISABLE);

        tep = UART4_RX_BUF_SIZE - DMA_GetCurrDataCounter(UART4_RX_DMA_STREAM);
        //-----用户程序-----Begin
#ifdef  UART4_DMA_RX_PARSE_PRO
        UART4_DMA_RX_PARSE_PRO(Uart_Rx4Buf,tep);
#else
        tep=tep;
#endif
        //-----用户程序-----End
        //设置传输数据长度
        DMA_SetCurrDataCounter(UART4_RX_DMA_STREAM,UART4_RX_BUF_SIZE);
        //打开DMA
        DMA_Cmd(UART4_RX_DMA_STREAM,ENABLE);
    }

    __nop();
#if   (defined(OS_UCOSII))
    OSIntExit();
#elif (defined(OS_UCOSIII))
    OSIntExit();
#endif
}
#endif
#ifdef BSP_UART5_ENABLE
/*******************************************************************************
* 函数功能: 串口5DMA方式发送中断函数
*******************************************************************************/
void UART_TX5_IRQ(uint16_t len)
{
    static uint16_t s_num=0;
    static uint16_t s_size=0;
    if(len!=0)
    {
        if(s_size==0)
        {
            s_size=len;
            s_num=0;
            //启动发送
            USART_ITConfig(UART5, USART_IT_TXE, ENABLE);
        }
        return;
    }
    if(s_size!=0)
    {
        UART5->DR = Uart_Tx5Buf[s_num++];
        if(s_num>=s_size)
        {
            s_num=s_size=0;
            Uart_Tx5SendEnable=1;
            //关闭发送
            USART_ITConfig(UART5, USART_IT_TXE, DISABLE);
        }
    }
    else
    {
        //USART_ITConfig(UART5, USART_IT_TXE, DISABLE);
    }
}
/*******************************************************************************
* 函数功能: 串口5接收函数(调用于中断函数与定时中断)
*******************************************************************************/
void UART_RX5_IRQ(uint8_t d)
{
    static uint16_t s_num=0;
    Uart_Rx5Timer_ms=0;
    if(Uart_Rx5Step==0)
    {
        Uart_Rx5Step=1;
        s_num=0;
        Uart_Rx5Buf[s_num++]=d;
    }
    else if(Uart_Rx5Step==1)
    {
        Uart_Rx5Buf[s_num++]=d;
    }
    else if(Uart_Rx5Step==2)
    {
        Uart_Rx5Step=0;
        //-----用户程序
        Uart_Rx5Buf[0]=Uart_Rx5Buf[0];
#ifdef UART5_DMA_RX_PARSE_PRO
        UART5_DMA_RX_PARSE_PRO(Uart_Rx5Buf,s_num);
#else
        Uart_Rx5Buf[0]=Uart_Rx5Buf[0];
        s_num=s_num;
#endif
        //-----
    }
    else
    {
        Uart_Rx5Step=0;
    }
    //
}
void UART5_RxTx_IRQHandler(void)
{
    uint8_t val;
    vu16  tep;
#if   (defined(OS_UCOSII))
    CPU_SR         cpu_sr;
    OS_ENTER_CRITICAL();
    OSIntNesting++;
    OS_EXIT_CRITICAL();
#elif (defined(OS_UCOSIII))
    CPU_SR_ALLOC();
    CPU_CRITICAL_ENTER();
    OSIntNestingCtr++;
    CPU_CRITICAL_EXIT();
#endif
    //出错处理
    if(  USART_GetITStatus(UART5, USART_IT_ORE) == SET
         ||USART_GetITStatus(UART5, USART_IT_NE)  == SET
         ||USART_GetITStatus(UART5, USART_IT_FE)  == SET
         ||USART_GetITStatus(UART5, USART_IT_PE)  == SET)
    {
        tep = UART5->SR;
        tep = UART5->DR;
    }
    //接收中断
    if(USART_GetITStatus(UART5, USART_IT_RXNE) == SET)
    {
        val = UART5->DR;
        UART_RX5_IRQ(val);
        USART_ClearITPendingBit(UART5, USART_IT_RXNE);          // Clear the USART Receive interrupt
    }
    //发送中断
    if(USART_GetITStatus(UART5, USART_IT_TXE) == SET)
    {
        UART_TX5_IRQ(0);
    }
#if   (defined(OS_UCOSII))
    OSIntExit();
#elif (defined(OS_UCOSIII))
    OSIntExit();
#endif
}
#endif
#ifdef BSP_UART6_ENABLE
/*******************************************************************************
* 函数功能: 串口6DMA方式发送中断函数
*******************************************************************************/
/*******************************************************************************
* 函数功能: 串口6接收函数
*******************************************************************************/
#endif
#if (defined(NRF51)||defined(NRF52))
void uart_error_handle(app_uart_evt_t * p_event)
{
    if (p_event->evt_type == APP_UART_COMMUNICATION_ERROR)
    {
        APP_ERROR_HANDLER(p_event->data.error_communication);
    }
    else if (p_event->evt_type == APP_UART_FIFO_ERROR)
    {
        APP_ERROR_HANDLER(p_event->data.error_code);
    }
}
#endif
//-----------------------------------------------------------------------
// 1mS定时处理函数,调用于1mS定时中断
void UART_1ms_IRQ(void)
{
    static uint16_t s_timer=0;
    //Rx1的接收超时
#ifdef  UART1_DMA_RX_PARSE_PRO
    if(UART1_RX_OVERTIME_MS!=0)
    {
#if   (defined(STM32F1)||defined(STM32F4))
        static uint16_t s_Uart1RxMs=0;
        static uint16_t s_Uart1RxOldAddr=0;
        static uint16_t s_Uart1RxNewAddr=0;
        s_Uart1RxNewAddr=UART1_RX_BUF_SIZE-DMA_GetCurrDataCounter(UART1_RX_DMA_STREAM);
        if(s_Uart1RxNewAddr == s_Uart1RxOldAddr && s_Uart1RxNewAddr!=0)
        {
            s_Uart1RxMs++;
        }
        else
        {
            s_Uart1RxOldAddr = s_Uart1RxNewAddr;
            s_Uart1RxMs=0;
        }
        if(s_Uart1RxMs>UART1_RX_OVERTIME_MS)
        {
            //处理数据
            DMA_Cmd(UART1_RX_DMA_STREAM,DISABLE);
            //-----用户程序-----Begin
            UART1_DMA_RX_PARSE_PRO(Uart_Rx1Buf,UART1_RX_BUF_SIZE - DMA_GetCurrDataCounter(UART1_RX_DMA_STREAM));
            //-----用户程序-----End
            //设置传输数据长度
            DMA_SetCurrDataCounter(UART1_RX_DMA_STREAM,UART1_RX_BUF_SIZE);
            //打开DMA
            DMA_Cmd(UART1_RX_DMA_STREAM,ENABLE);
        }
#elif (defined(NRF51)||defined(NRF52))
        static uint16_t s_Uart1RxMs=0;
        static uint16_t s_Uart1RxNum=0;
        static uint8_t step=0;
        uint8_t i;
        switch(step)
        {
            case 0:
                s_Uart1RxMs   =  0;
                s_Uart1RxNum  =  0;
                step++;
                break;
            case 1:
                if(app_uart_get(&i)==NRF_SUCCESS)
                {
                    s_Uart1RxMs=0;
                    if(s_Uart1RxNum<UART1_RX_BUF_SIZE)
                    {
                        Uart_Rx1Buf[s_Uart1RxNum++]=i;
                    }
                }
                if(s_Uart1RxNum!=0)
                {
                    s_Uart1RxMs++;
                }
                if(s_Uart1RxMs>=UART1_RX_OVERTIME_MS)
                {
                    step++;
                }
                break;
            case 2:
                UART1_DMA_RX_PARSE_PRO(Uart_Rx1Buf,s_Uart1RxNum);
						    step=0;
                break;
            default:
                break;
        }
#endif
    }
#endif
    //Rx2的接收超时
#ifdef  UART2_DMA_RX_PARSE_PRO
    if(UART2_RX_OVERTIME_MS!=0)
    {
        static uint16_t s_Uart2RxMs=0;
        static uint16_t s_Uart2RxOldAddr=0;
        static uint16_t s_Uart2RxNewAddr=0;
        s_Uart2RxNewAddr=UART2_RX_BUF_SIZE-DMA_GetCurrDataCounter(UART2_RX_DMA_STREAM);
        if(s_Uart2RxNewAddr == s_Uart2RxOldAddr && s_Uart2RxNewAddr!=0)
        {
            s_Uart2RxMs++;
        }
        else
        {
            s_Uart2RxOldAddr = s_Uart2RxNewAddr;
            s_Uart2RxMs=0;
        }
        if(s_Uart2RxMs>UART2_RX_OVERTIME_MS)
        {
            //处理数据
            DMA_Cmd(UART2_RX_DMA_STREAM,DISABLE);
            //-----用户程序-----Begin
            UART2_DMA_RX_PARSE_PRO(Uart_Rx2Buf,UART2_RX_BUF_SIZE - DMA_GetCurrDataCounter(UART2_RX_DMA_STREAM));
            //-----用户程序-----End
            //设置传输数据长度
            DMA_SetCurrDataCounter(UART2_RX_DMA_STREAM,UART2_RX_BUF_SIZE);
            //打开DMA
            DMA_Cmd(UART2_RX_DMA_STREAM,ENABLE);
        }
    }
#endif
    //Rx3的接收超时
#ifdef  UART3_DMA_RX_PARSE_PRO
    if(UART3_RX_OVERTIME_MS!=0)
    {
        static uint16_t s_Uart3RxMs=0;
        static uint16_t s_Uart3RxOldAddr=0;
        static uint16_t s_Uart3RxNewAddr=0;
        s_Uart3RxNewAddr=UART3_RX_BUF_SIZE-DMA_GetCurrDataCounter(UART3_RX_DMA_STREAM);
        if(s_Uart3RxNewAddr == s_Uart3RxOldAddr && s_Uart3RxNewAddr!=0)
        {
            s_Uart3RxMs++;
        }
        else
        {
            s_Uart3RxOldAddr = s_Uart3RxNewAddr;
            s_Uart3RxMs=0;
        }
        if(s_Uart3RxMs>UART3_RX_OVERTIME_MS)
        {
            //处理数据
            DMA_Cmd(UART3_RX_DMA_STREAM,DISABLE);
            //-----用户程序-----Begin
            UART3_DMA_RX_PARSE_PRO(Uart_Rx3Buf,UART3_RX_BUF_SIZE - DMA_GetCurrDataCounter(UART3_RX_DMA_STREAM));
            //-----用户程序-----End
            //设置传输数据长度
            DMA_SetCurrDataCounter(UART3_RX_DMA_STREAM,UART3_RX_BUF_SIZE);
            //打开DMA
            DMA_Cmd(UART3_RX_DMA_STREAM,ENABLE);
        }
    }
#endif
    //Rx4的接收超时
#ifdef  UART4_DMA_RX_PARSE_PRO
    if(UART4_RX_OVERTIME_MS!=0)
    {
        static uint16_t s_Uart4RxMs=0;
        static uint16_t s_Uart4RxOldAddr=0;
        static uint16_t s_Uart4RxNewAddr=0;
        s_Uart4RxNewAddr=UART4_RX_BUF_SIZE-DMA_GetCurrDataCounter(UART4_RX_DMA_STREAM);
        if(s_Uart4RxNewAddr == s_Uart4RxOldAddr && s_Uart4RxNewAddr!=0)
        {
            s_Uart4RxMs++;
        }
        else
        {
            s_Uart4RxOldAddr = s_Uart4RxNewAddr;
            s_Uart4RxMs=0;
        }
        if(s_Uart4RxMs>UART4_RX_OVERTIME_MS)
        {
            //处理数据
            DMA_Cmd(UART4_RX_DMA_STREAM,DISABLE);
            //-----用户程序-----Begin
            UART4_DMA_RX_PARSE_PRO(Uart_Rx4Buf,UART4_RX_BUF_SIZE - DMA_GetCurrDataCounter(UART4_RX_DMA_STREAM));
            //-----用户程序-----End
            //设置传输数据长度
            DMA_SetCurrDataCounter(UART4_RX_DMA_STREAM,UART4_RX_BUF_SIZE);
            //打开DMA
            DMA_Cmd(UART4_RX_DMA_STREAM,ENABLE);
        }
    }
#endif
#ifdef  BSP_UART1_ENABLE
    //TX1的485切换计时
    if(Uart_Tx1SendEnable==1)
    {
        static uint8_t tx1_ms=0;
        if(tx1_ms<UART1_TX_OVERTIME_MS)
        {
            tx1_ms++;
        }
        else
        {
            tx1_ms=0;
            Uart_Tx1SendEnable=2;
            UART1_MAX485_R;
        }
    }
#endif
#ifdef  BSP_UART2_ENABLE
    //TX2的485切换计时
    if(Uart_Tx2SendEnable==1)
    {
        static uint8_t tx2_ms=0;
        if(tx2_ms<UART2_TX_OVERTIME_MS)
        {
            tx2_ms++;
        }
        else
        {
            tx2_ms=0;
            Uart_Tx2SendEnable=2;
            UART2_MAX485_R;
        }
    }
#endif
#ifdef  BSP_UART3_ENABLE
    //TX3的485切换计时
    if(Uart_Tx3SendEnable==1)
    {
        static uint8_t tx3_ms=0;
        if(tx3_ms<UART3_TX_OVERTIME_MS)
        {
            tx3_ms++;
        }
        else
        {
            tx3_ms=0;
            Uart_Tx3SendEnable=2;
            UART3_MAX485_R;
        }
    }
#endif
#ifdef  BSP_UART4_ENABLE
    //TX4的485切换计时
    if(Uart_Tx4SendEnable==1)
    {
        static uint8_t tx4_ms=0;
        if(tx4_ms<UART4_TX_OVERTIME_MS)
        {
            tx4_ms++;
        }
        else
        {
            tx4_ms=0;
            Uart_Tx4SendEnable=2;
            UART4_MAX485_R;
        }
    }
#endif
#ifdef  BSP_UART5_ENABLE
    //TX5的485切换计时
    if(Uart_Tx5SendEnable==1)
    {
        static uint8_t tx5_ms=0;
        if(tx5_ms<3)
        {
            tx5_ms++;
        }
        else
        {
            tx5_ms=0;
            Uart_Tx5SendEnable=2;
            UART5_MAX485_R;
        }
    }
    //RX的接收计时
    if(Uart_Rx5Step==1)
    {
        if(Uart_Rx5Timer_ms<UART5_TX_OVERTIME_MS)
        {
            Uart_Rx5Timer_ms++;
        }
        else
        {
            Uart_Rx5Timer_ms=0;
            Uart_Rx5Step=2;
            UART_RX5_IRQ(0);
        }
    }
#endif
    //测试
    if(BspUart_DebugTest_Enable==1)
    {
        if(s_timer==0)
        {
            UART_INIT(3,115200);
        }
        s_timer++;
        if(s_timer>=1000)
        {
            s_timer=1;
            UART_DMA_Tx(3,"1234567890\r\n",12);
        }
    }
}
/*******************************************************************************
* 函数功能: 入口函数-初始化
*******************************************************************************/
void UART_INIT(uint8_t UartX,uint32_t baudrate)
{
    //
    switch(UartX)
    {
#ifdef BSP_UART0_USB_ENABLE
        case 0:
#if   (defined(STM32F1)||defined(STM32F4))
            USB_Config();
#endif
            break;
#endif
#ifdef BSP_UART1_ENABLE
        case 1:
            //------------------------------UART1(DEBUG)------------------------------Begin
#if   (defined(STM32F1)||defined(STM32F4))
            //关闭串口
            USART_Cmd(USART1, DISABLE);
            //-----启动相应时钟
            UART1_RCC_ENABLE;
            //-----DMA发送
            {
                //启动时钟
                UART1_DMA_TX_RCC_ENABLE;
                //中断设置
                {
                    NVIC_InitTypeDef    NVIC_InitStructure;
                    //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
                    NVIC_InitStructure.NVIC_IRQChannel = UART1_DMA_TX_IRQn;
                    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
                    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
                    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
                    NVIC_Init(&NVIC_InitStructure);
                }
                //配置
                {
                    DMA_InitTypeDef     DMA_InitStructure;
                    DMA_DeInit(UART1_TX_DMA_STREAM);
#if   (defined(STM32F1))
                    DMA_InitStructure.DMA_PeripheralBaseAddr =  (u32)(&USART1->DR);
                    DMA_InitStructure.DMA_MemoryBaseAddr     =  (u32)Uart_Tx1Buf;
                    DMA_InitStructure.DMA_DIR                =  DMA_DIR_PeripheralDST;        //方向：内存-->外设
                    DMA_InitStructure.DMA_BufferSize         =  0;
                    DMA_InitStructure.DMA_PeripheralInc      =  DMA_PeripheralInc_Disable;
                    DMA_InitStructure.DMA_MemoryInc          =  DMA_MemoryInc_Enable;
                    DMA_InitStructure.DMA_PeripheralDataSize =  DMA_PeripheralDataSize_Byte;
                    DMA_InitStructure.DMA_MemoryDataSize     =  DMA_MemoryDataSize_Byte;
                    DMA_InitStructure.DMA_Mode               =  DMA_Mode_Normal;              //DMA模式：一次传输，非循环
                    DMA_InitStructure.DMA_Priority           =  DMA_Priority_High;
                    DMA_InitStructure.DMA_M2M                =  DMA_M2M_Disable;
#elif (defined(STM32F4))
                    DMA_InitStructure.DMA_BufferSize         =  0;                            //配置DMA大小
                    DMA_InitStructure.DMA_FIFOMode           =  DMA_FIFOMode_Disable;         //在这个程序里面使能或者禁止都可以的
                    DMA_InitStructure.DMA_FIFOThreshold      =  DMA_FIFOThreshold_Full;       //设置阀值
                    DMA_InitStructure.DMA_MemoryBurst        =  DMA_MemoryBurst_Single ;      //设置内存为单字节突发模式
                    DMA_InitStructure.DMA_MemoryDataSize     =  DMA_MemoryDataSize_Byte;      //设置内存数据的位宽是字节
                    DMA_InitStructure.DMA_MemoryInc          =  DMA_MemoryInc_Enable;         //使能地址自增
                    DMA_InitStructure.DMA_Mode               =  DMA_Mode_Normal;              //设置DMA是正常模式
                    DMA_InitStructure.DMA_PeripheralBaseAddr =  (uint32_t) (&(USART1->DR)) ;  //设置外设地址
                    DMA_InitStructure.DMA_PeripheralBurst    =  DMA_PeripheralBurst_Single;   //设置外设为单字节突发模式
                    DMA_InitStructure.DMA_PeripheralDataSize =  DMA_PeripheralDataSize_Byte;; //设置外设数据的位宽是字节
                    DMA_InitStructure.DMA_PeripheralInc      =  DMA_PeripheralInc_Disable;    //禁止外设地址自增
                    DMA_InitStructure.DMA_Priority           =  DMA_Priority_High;            //设置优先级
                    // 配置 TX DMA
                    DMA_InitStructure.DMA_Channel            =  UART1_TX_DMA_CHANNEL ;        //配置发送通道
                    DMA_InitStructure.DMA_DIR                =  DMA_DIR_MemoryToPeripheral ;  //设置从内存到外设
                    DMA_InitStructure.DMA_Memory0BaseAddr    =  (uint32_t)Uart_Tx1Buf;        //设置内存地址
#endif
                    DMA_Init(UART1_TX_DMA_STREAM, &DMA_InitStructure);
                    //开启中断
                    DMA_ITConfig(UART1_TX_DMA_STREAM,DMA_IT_TC,ENABLE);
                }
            }
            //-----DMA接收
            {
                DMA_InitTypeDef     DMA_InitStructure;
                //启动时钟
                UART1_DMA_RX_RCC_ENABLE;
                //DMA1通道5配置
                DMA_DeInit(UART1_RX_DMA_STREAM);
#if   (defined(STM32F1))
                //外设地址
                DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART1->DR);
                //内存地址
                DMA_InitStructure.DMA_MemoryBaseAddr     = (u32)Uart_Rx1Buf;
                //dma传输方向单向
                DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralSRC;
                //设置DMA在传输时缓冲区的长度
                DMA_InitStructure.DMA_BufferSize         = UART1_RX_BUF_SIZE;
                //设置DMA的外设递增模式，一个外设
                DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
                //设置DMA的内存递增模式
                DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
                //外设数据字长
                DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
                //内存数据字长
                DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
                //设置DMA的传输模式
                //DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
                DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;
                //设置DMA的优先级别
                DMA_InitStructure.DMA_Priority           = DMA_Priority_Medium;
                //设置DMA的2个memory中的变量互相访问
                DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
#elif (defined(STM32F4))
                DMA_InitStructure.DMA_BufferSize         =  UART1_RX_BUF_SIZE;                          // 配置DMA大小
                DMA_InitStructure.DMA_FIFOMode           =  DMA_FIFOMode_Disable;         // 在这个程序里面使能或者禁止都可以的
                DMA_InitStructure.DMA_FIFOThreshold      =  DMA_FIFOThreshold_Full;       // 设置阀值
                DMA_InitStructure.DMA_MemoryBurst        =  DMA_MemoryBurst_Single ;      // 设置内存为单字节突发模式
                DMA_InitStructure.DMA_MemoryDataSize     =  DMA_MemoryDataSize_Byte;      // 设置内存数据的位宽是字节
                DMA_InitStructure.DMA_MemoryInc          =  DMA_MemoryInc_Enable;         // 使能地址自增
                DMA_InitStructure.DMA_Mode               =  DMA_Mode_Circular;            // 设置DMA是正常模式
                DMA_InitStructure.DMA_PeripheralBaseAddr =  (uint32_t) (&(USART1->DR)) ;  // 设置外设地址
                DMA_InitStructure.DMA_PeripheralBurst    =  DMA_PeripheralBurst_Single;   // 设置外设为单字节突发模式
                DMA_InitStructure.DMA_PeripheralDataSize =  DMA_PeripheralDataSize_Byte;  // 设置外设数据的位宽是字节
                DMA_InitStructure.DMA_PeripheralInc      =  DMA_PeripheralInc_Disable;    // 禁止外设地址自增
                DMA_InitStructure.DMA_Priority           =  DMA_Priority_Medium;          // 设置优先级
                // 配置 RX DMA
                DMA_InitStructure.DMA_Channel            =  UART1_RX_DMA_CHANNEL ;        // 配置接收通道
                DMA_InitStructure.DMA_DIR                =  DMA_DIR_PeripheralToMemory ;  // 设置从外设到内存
                DMA_InitStructure.DMA_Memory0BaseAddr    =  (uint32_t)Uart_Rx1Buf ;       // 设置内存地址
#endif
                DMA_Init(UART1_RX_DMA_STREAM,&DMA_InitStructure);
                //使能通道5
                DMA_Cmd(UART1_RX_DMA_STREAM,ENABLE);
            }
            //-----串口初始化
            {
                USART_InitTypeDef   USART_InitStructure;
                //配置UART
                USART_InitStructure.USART_BaudRate            = baudrate;            // 波特率
                USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
                USART_InitStructure.USART_StopBits            = USART_StopBits_1;
                USART_InitStructure.USART_Parity              = USART_Parity_No;
                USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
                USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
                USART_Init(USART1, &USART_InitStructure);
                //使能中断
                USART_ITConfig(USART1,USART_IT_TC   ,DISABLE);
                USART_ITConfig(USART1,USART_IT_RXNE ,DISABLE);
#if   (UART1_RX_OVERTIME_MS==0)
                USART_ITConfig(USART1,USART_IT_IDLE ,ENABLE);
#else
                USART_ITConfig(USART1,USART_IT_IDLE ,DISABLE);
#endif
                {
                    NVIC_InitTypeDef    NVIC_InitStructure;
                    //配置中断
                    NVIC_InitStructure.NVIC_IRQChannel       = USART1_IRQn;
                    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
                    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
                    NVIC_InitStructure.NVIC_IRQChannelCmd    = ENABLE;
                    NVIC_Init(&NVIC_InitStructure);
                }
                //采用DMA方式发送+接收
                USART_DMACmd(USART1, USART_DMAReq_Rx | USART_DMAReq_Tx, ENABLE);
                //启动串口
                USART_Cmd(USART1, ENABLE);
            }
            //-----配置GPIO
            {
                GPIO_InitTypeDef    GPIO_InitStructure;
                UART1_GPIO_RCC_ENABLE;
                UART1_GPIO_TX_REMAP;
                UART1_GPIO_RX_REMAP;
#if   (defined(STM32F1))
                //USART1-Rx-floating
                GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_IN_FLOATING;
                GPIO_InitStructure.GPIO_Pin     =  UART1_GPIO_RX_PIN;
                GPIO_Init(UART1_GPIO_RX_PORT, &GPIO_InitStructure);
                //USART1-Tx
                GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_50MHz;
                GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_AF_PP;
                //GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_OD;
                GPIO_InitStructure.GPIO_Pin     =  UART1_GPIO_TX_PIN;
                GPIO_Init(UART1_GPIO_TX_PORT, &GPIO_InitStructure);
#elif (defined(STM32F4))
                GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_AF;
                GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_2MHz;
                GPIO_InitStructure.GPIO_OType   =  GPIO_OType_PP;
                GPIO_InitStructure.GPIO_PuPd    =  GPIO_PuPd_UP;
                GPIO_InitStructure.GPIO_Pin     =  UART1_GPIO_TX_PIN;
                GPIO_Init(UART1_GPIO_TX_PORT, &GPIO_InitStructure);
                GPIO_InitStructure.GPIO_Pin     =  UART1_GPIO_RX_PIN;
                GPIO_Init(UART1_GPIO_RX_PORT, &GPIO_InitStructure);
#endif
            }
#elif (defined(NRF51)||defined(NRF52))

            {
                uint32_t err_code;
                const app_uart_comm_params_t comm_params =
                {
                    UART1_RX_PIN,
                    UART1_TX_PIN,
                    5, /*RTS_PIN_NUMBER,*/
                    7, /*CTS_PIN_NUMBER,*/
                    APP_UART_FLOW_CONTROL_DISABLED,
                    false,
                    UART_BAUDRATE_BAUDRATE_Baud115200
                };
                APP_UART_FIFO_INIT(&comm_params,
                                   UART1_RX_BUF_SIZE,
                                   UART1_TX_BUF_SIZE,
                                   uart_error_handle,
                                   APP_IRQ_PRIORITY_LOWEST,
                                   err_code);
                APP_ERROR_CHECK(err_code);
            }
#endif
            //------------------------------UART1(DEBUG)------------------------------End
            break;
#endif
#ifdef BSP_UART2_ENABLE
        case 2:
            //------------------------------UART2(RS485)------------------------------Begin
#if   (defined(STM32F1)||defined(STM32F4))
            //关闭串口
            USART_Cmd(USART2, DISABLE);
            //-----启动相应时钟
            UART2_RCC_ENABLE;
            //-----DMA发送
            {
                //启动时钟
                UART2_DMA_TX_RCC_ENABLE;
                //中断设置
                {
                    NVIC_InitTypeDef    NVIC_InitStructure;
                    NVIC_InitStructure.NVIC_IRQChannel = UART2_DMA_TX_IRQn;
                    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
                    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
                    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
                    NVIC_Init(&NVIC_InitStructure);
                }
                //配置
                {
                    DMA_InitTypeDef     DMA_InitStructure;
                    DMA_DeInit(UART2_TX_DMA_STREAM);
#if   (defined(STM32F1))
                    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART2->DR);
                    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)Uart_Tx2Buf;
                    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                          //方向：内存-->外设
                    DMA_InitStructure.DMA_BufferSize = 0;
                    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
                    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
                    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
                    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
                    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                               //DMA模式：一次传输，非循环
                    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
                    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
#elif (defined(STM32F4))
                    DMA_InitStructure.DMA_BufferSize         =  0;                            //配置DMA大小
                    DMA_InitStructure.DMA_FIFOMode           =  DMA_FIFOMode_Disable;         //在这个程序里面使能或者禁止都可以的
                    DMA_InitStructure.DMA_FIFOThreshold      =  DMA_FIFOThreshold_Full;       //设置阀值
                    DMA_InitStructure.DMA_MemoryBurst        =  DMA_MemoryBurst_Single ;      //设置内存为单字节突发模式
                    DMA_InitStructure.DMA_MemoryDataSize     =  DMA_MemoryDataSize_Byte;      //设置内存数据的位宽是字节
                    DMA_InitStructure.DMA_MemoryInc          =  DMA_MemoryInc_Enable;         //使能地址自增
                    DMA_InitStructure.DMA_Mode               =  DMA_Mode_Normal;              //设置DMA是正常模式
                    DMA_InitStructure.DMA_PeripheralBaseAddr =  (uint32_t) (&(USART2->DR)) ;  //设置外设地址
                    DMA_InitStructure.DMA_PeripheralBurst    =  DMA_PeripheralBurst_Single;   //设置外设为单字节突发模式
                    DMA_InitStructure.DMA_PeripheralDataSize =  DMA_PeripheralDataSize_Byte;; //设置外设数据的位宽是字节
                    DMA_InitStructure.DMA_PeripheralInc      =  DMA_PeripheralInc_Disable;    //禁止外设地址自增
                    DMA_InitStructure.DMA_Priority           =  DMA_Priority_High;            //设置优先级
                    // 配置 TX DMA
                    DMA_InitStructure.DMA_Channel            =  UART2_TX_DMA_CHANNEL ;       //配置发送通道
                    DMA_InitStructure.DMA_DIR                =  DMA_DIR_MemoryToPeripheral ;  //设置从内存到外设
                    DMA_InitStructure.DMA_Memory0BaseAddr    =  (uint32_t)Uart_Tx2Buf;        //设置内存地址
#endif
                    DMA_Init(UART2_TX_DMA_STREAM, &DMA_InitStructure);
                    //开启中断
                    DMA_ITConfig(UART2_TX_DMA_STREAM,DMA_IT_TC,ENABLE);
                }
            }
            //-----DMA接收
            {
                DMA_InitTypeDef     DMA_InitStructure;
                //启动时钟
                UART2_DMA_RX_RCC_ENABLE;
                //DMA1通道5配置
                DMA_DeInit(UART2_RX_DMA_STREAM);
#if   (defined(STM32F1))
                //外设地址
                DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART2->DR);
                //内存地址
                DMA_InitStructure.DMA_MemoryBaseAddr = (u32)Uart_Rx2Buf;
                //dma传输方向单向
                DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
                //设置DMA在传输时缓冲区的长度
                DMA_InitStructure.DMA_BufferSize = UART2_RX_BUF_SIZE;
                //设置DMA的外设递增模式，一个外设
                DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
                //设置DMA的内存递增模式
                DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
                //外设数据字长
                DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
                //内存数据字长
                DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
                //设置DMA的传输模式
                //DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
                DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
                //设置DMA的优先级别
                DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
                //设置DMA的2个memory中的变量互相访问
                DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
#elif (defined(STM32F4))
                DMA_InitStructure.DMA_BufferSize         =  UART2_RX_BUF_SIZE;                          // 配置DMA大小
                DMA_InitStructure.DMA_FIFOMode           =  DMA_FIFOMode_Disable;         // 在这个程序里面使能或者禁止都可以的
                DMA_InitStructure.DMA_FIFOThreshold      =  DMA_FIFOThreshold_Full;       // 设置阀值
                DMA_InitStructure.DMA_MemoryBurst        =  DMA_MemoryBurst_Single ;      // 设置内存为单字节突发模式
                DMA_InitStructure.DMA_MemoryDataSize     =  DMA_MemoryDataSize_Byte;      // 设置内存数据的位宽是字节
                DMA_InitStructure.DMA_MemoryInc          =  DMA_MemoryInc_Enable;         // 使能地址自增
                DMA_InitStructure.DMA_Mode               =  DMA_Mode_Circular;            // 设置DMA是正常模式
                DMA_InitStructure.DMA_PeripheralBaseAddr =  (uint32_t) (&(USART2->DR)) ;  // 设置外设地址
                DMA_InitStructure.DMA_PeripheralBurst    =  DMA_PeripheralBurst_Single;   // 设置外设为单字节突发模式
                DMA_InitStructure.DMA_PeripheralDataSize =  DMA_PeripheralDataSize_Byte;  // 设置外设数据的位宽是字节
                DMA_InitStructure.DMA_PeripheralInc      =  DMA_PeripheralInc_Disable;    // 禁止外设地址自增
                DMA_InitStructure.DMA_Priority           =  DMA_Priority_Medium;          // 设置优先级
                // 配置 RX DMA
                DMA_InitStructure.DMA_Channel            =  UART2_RX_DMA_CHANNEL ;        // 配置接收通道
                DMA_InitStructure.DMA_DIR                =  DMA_DIR_PeripheralToMemory ;  // 设置从外设到内存
                DMA_InitStructure.DMA_Memory0BaseAddr    =  (uint32_t)Uart_Rx2Buf ;       // 设置内存地址
#endif
                DMA_Init(UART2_RX_DMA_STREAM,&DMA_InitStructure);
                //使能通道5
                DMA_Cmd(UART2_RX_DMA_STREAM,ENABLE);
            }
            //-----串口初始化
            {
                USART_InitTypeDef   USART_InitStructure;
                //配置UART
                USART_InitStructure.USART_BaudRate            = baudrate;            // 波特率
                USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
                USART_InitStructure.USART_StopBits            = USART_StopBits_1;
                USART_InitStructure.USART_Parity              = USART_Parity_No;
                USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
                USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
                USART_Init(USART2, &USART_InitStructure);
                //使能中断
                USART_ITConfig(USART2,USART_IT_TC   ,DISABLE);
                USART_ITConfig(USART2,USART_IT_RXNE ,DISABLE);
#if   (UART2_RX_OVERTIME_MS==0)
                USART_ITConfig(USART2,USART_IT_IDLE ,ENABLE);
#else
                USART_ITConfig(USART2,USART_IT_IDLE ,DISABLE);
#endif
                //配置中断
                {
                    NVIC_InitTypeDef    NVIC_InitStructure;
                    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
                    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
                    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
                    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
                    NVIC_Init(&NVIC_InitStructure);
                }
                //采用DMA方式发送
                USART_DMACmd(USART2, USART_DMAReq_Rx | USART_DMAReq_Tx, ENABLE);
                //启动串口
                USART_Cmd(USART2, ENABLE);
            }
            //-----配置GPIO
            {
                GPIO_InitTypeDef    GPIO_InitStructure;
                UART2_GPIO_RCC_ENABLE;
                UART2_GPIO_TX_REMAP;
                UART2_GPIO_RX_REMAP;
#if   (defined(STM32F1))
                GPIO_InitStructure.GPIO_Pin  = UART2_GPIO_RX_PIN;
                GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
                GPIO_Init(UART2_GPIO_RX_PORT, &GPIO_InitStructure);
                GPIO_InitStructure.GPIO_Pin   = UART2_GPIO_TX_PIN;
                GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
                GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
                //GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_OD;
                GPIO_Init(UART2_GPIO_TX_PORT, &GPIO_InitStructure);
#elif (defined(STM32F4))
                GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_AF;
                GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_2MHz;
                GPIO_InitStructure.GPIO_OType   =  GPIO_OType_PP;
                GPIO_InitStructure.GPIO_PuPd    =  GPIO_PuPd_UP;
                GPIO_InitStructure.GPIO_Pin     =  UART2_GPIO_TX_PIN;
                GPIO_Init(UART2_GPIO_TX_PORT, &GPIO_InitStructure);
                GPIO_InitStructure.GPIO_Pin     =  UART2_GPIO_RX_PIN;
                GPIO_Init(UART2_GPIO_RX_PORT, &GPIO_InitStructure);
#endif
            }
#elif (defined(NRF51)||defined(NRF52))
#endif
            //------------------------------UART2(RS485)------------------------------End
            break;
#endif
#ifdef BSP_UART3_ENABLE
        case 3:
            //------------------------------UART3(RS485)------------------------------Begin
#if   (defined(STM32F1)||defined(STM32F4))
            //关闭串口
            USART_Cmd(USART3, DISABLE);
            //-----启动相应时钟
            UART3_RCC_ENABLE;
            //-----DMA发送
            {
                //启动时钟
                UART3_DMA_TX_RCC_ENABLE;
                //中断设置
                {
                    NVIC_InitTypeDef    NVIC_InitStructure;
                    NVIC_InitStructure.NVIC_IRQChannel = UART3_DMA_TX_IRQn;
                    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
                    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
                    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
                    NVIC_Init(&NVIC_InitStructure);
                }
                //配置
                {
                    DMA_InitTypeDef     DMA_InitStructure;
                    DMA_DeInit(UART3_TX_DMA_STREAM);
#if   (defined(STM32F1))
                    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART3->DR);
                    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)Uart_Tx3Buf;
                    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                          //方向：内存-->外设
                    DMA_InitStructure.DMA_BufferSize = 0;
                    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
                    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
                    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
                    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
                    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                               //DMA模式：一次传输，非循环
                    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
                    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
#elif (defined(STM32F4))
                    DMA_InitStructure.DMA_BufferSize         =  0;                            //配置DMA大小
                    DMA_InitStructure.DMA_FIFOMode           =  DMA_FIFOMode_Disable;         //在这个程序里面使能或者禁止都可以的
                    DMA_InitStructure.DMA_FIFOThreshold      =  DMA_FIFOThreshold_Full;       //设置阀值
                    DMA_InitStructure.DMA_MemoryBurst        =  DMA_MemoryBurst_Single ;      //设置内存为单字节突发模式
                    DMA_InitStructure.DMA_MemoryDataSize     =  DMA_MemoryDataSize_Byte;      //设置内存数据的位宽是字节
                    DMA_InitStructure.DMA_MemoryInc          =  DMA_MemoryInc_Enable;         //使能地址自增
                    DMA_InitStructure.DMA_Mode               =  DMA_Mode_Normal;              //设置DMA是正常模式
                    DMA_InitStructure.DMA_PeripheralBaseAddr =  (uint32_t) (&(USART3->DR)) ;  //设置外设地址
                    DMA_InitStructure.DMA_PeripheralBurst    =  DMA_PeripheralBurst_Single;   //设置外设为单字节突发模式
                    DMA_InitStructure.DMA_PeripheralDataSize =  DMA_PeripheralDataSize_Byte;; //设置外设数据的位宽是字节
                    DMA_InitStructure.DMA_PeripheralInc      =  DMA_PeripheralInc_Disable;    //禁止外设地址自增
                    DMA_InitStructure.DMA_Priority           =  DMA_Priority_High;            //设置优先级
                    // 配置 TX DMA
                    DMA_InitStructure.DMA_Channel            =  UART3_TX_DMA_CHANNEL ;       //配置发送通道
                    DMA_InitStructure.DMA_DIR                =  DMA_DIR_MemoryToPeripheral ;  //设置从内存到外设
                    DMA_InitStructure.DMA_Memory0BaseAddr    =  (uint32_t)Uart_Tx3Buf;        //设置内存地址
#endif
                    DMA_Init(UART3_TX_DMA_STREAM, &DMA_InitStructure);
                    //开启中断
                    DMA_ITConfig(UART3_TX_DMA_STREAM,DMA_IT_TC,ENABLE);
                }
            }
            //-----DMA接收
            {
                DMA_InitTypeDef     DMA_InitStructure;
                //启动时钟
                UART3_DMA_RX_RCC_ENABLE;
                //DMA1通道5配置
                DMA_DeInit(UART3_RX_DMA_STREAM);
#if   (defined(STM32F1))
                //外设地址
                DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART3->DR);
                //内存地址
                DMA_InitStructure.DMA_MemoryBaseAddr = (u32)Uart_Rx3Buf;
                //dma传输方向单向
                DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
                //设置DMA在传输时缓冲区的长度
                DMA_InitStructure.DMA_BufferSize = UART3_RX_BUF_SIZE;
                //设置DMA的外设递增模式，一个外设
                DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
                //设置DMA的内存递增模式
                DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
                //外设数据字长
                DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
                //内存数据字长
                DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
                //设置DMA的传输模式
                //DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
                DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
                //设置DMA的优先级别
                DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
                //设置DMA的2个memory中的变量互相访问
                DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
#elif (defined(STM32F4))
                DMA_InitStructure.DMA_BufferSize         =  UART3_RX_BUF_SIZE;                          // 配置DMA大小
                DMA_InitStructure.DMA_FIFOMode           =  DMA_FIFOMode_Disable;         // 在这个程序里面使能或者禁止都可以的
                DMA_InitStructure.DMA_FIFOThreshold      =  DMA_FIFOThreshold_Full;       // 设置阀值
                DMA_InitStructure.DMA_MemoryBurst        =  DMA_MemoryBurst_Single ;      // 设置内存为单字节突发模式
                DMA_InitStructure.DMA_MemoryDataSize     =  DMA_MemoryDataSize_Byte;      // 设置内存数据的位宽是字节
                DMA_InitStructure.DMA_MemoryInc          =  DMA_MemoryInc_Enable;         // 使能地址自增
                DMA_InitStructure.DMA_Mode               =  DMA_Mode_Circular;            // 设置DMA是正常模式
                DMA_InitStructure.DMA_PeripheralBaseAddr =  (uint32_t) (&(USART3->DR)) ;  // 设置外设地址
                DMA_InitStructure.DMA_PeripheralBurst    =  DMA_PeripheralBurst_Single;   // 设置外设为单字节突发模式
                DMA_InitStructure.DMA_PeripheralDataSize =  DMA_PeripheralDataSize_Byte;  // 设置外设数据的位宽是字节
                DMA_InitStructure.DMA_PeripheralInc      =  DMA_PeripheralInc_Disable;    // 禁止外设地址自增
                DMA_InitStructure.DMA_Priority           =  DMA_Priority_Medium;          // 设置优先级
                // 配置 RX DMA
                DMA_InitStructure.DMA_Channel            =  UART3_RX_DMA_CHANNEL ;        // 配置接收通道
                DMA_InitStructure.DMA_DIR                =  DMA_DIR_PeripheralToMemory ;  // 设置从外设到内存
                DMA_InitStructure.DMA_Memory0BaseAddr    =  (uint32_t)Uart_Rx3Buf ;       // 设置内存地址
#endif
                DMA_Init(UART3_RX_DMA_STREAM,&DMA_InitStructure);
                //使能通道5
                DMA_Cmd(UART3_RX_DMA_STREAM,ENABLE);
            }
            //-----串口初始化
            {
                USART_InitTypeDef   USART_InitStructure;
                //配置UART
                USART_InitStructure.USART_BaudRate            = baudrate;            // 波特率
                USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
                USART_InitStructure.USART_StopBits            = USART_StopBits_1;
                USART_InitStructure.USART_Parity              = USART_Parity_No;
                USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
                USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
                USART_Init(USART3, &USART_InitStructure);
                //使能中断
                USART_ITConfig(USART3,USART_IT_TC   ,DISABLE);
                USART_ITConfig(USART3,USART_IT_RXNE ,DISABLE);
#if   (UART3_RX_OVERTIME_MS==0)
                USART_ITConfig(USART3,USART_IT_IDLE ,ENABLE);
#else
                USART_ITConfig(USART3,USART_IT_IDLE ,DISABLE);
#endif
                //配置中断
                {
                    NVIC_InitTypeDef    NVIC_InitStructure;
                    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
                    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
                    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
                    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
                    NVIC_Init(&NVIC_InitStructure);
                }
                //采用DMA方式发送
                USART_DMACmd(USART3, USART_DMAReq_Rx | USART_DMAReq_Tx, ENABLE);
                //启动串口
                USART_Cmd(USART3, ENABLE);
            }
            //-----配置GPIO
            {
                /*
                RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB , ENABLE);
                GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_11;
                GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
                GPIO_Init(GPIOB, &GPIO_InitStructure);
                GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
                GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
                GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
                GPIO_Init(GPIOB, &GPIO_InitStructure);
                */
                GPIO_InitTypeDef    GPIO_InitStructure;
                UART3_GPIO_RCC_ENABLE;
                UART3_GPIO_TX_REMAP;
                UART3_GPIO_RX_REMAP;
#if   (defined(STM32F1))
                GPIO_AFIODeInit();
                GPIO_PinRemapConfig(GPIO_FullRemap_USART3, ENABLE);
                GPIO_InitStructure.GPIO_Pin     =  UART3_GPIO_RX_PIN;
                GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_IN_FLOATING;
                GPIO_Init(UART3_GPIO_RX_PORT, &GPIO_InitStructure);
                GPIO_InitStructure.GPIO_Pin     =  UART3_GPIO_TX_PIN;
                GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_50MHz;
                GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_AF_PP;
                GPIO_Init(UART3_GPIO_TX_PORT, &GPIO_InitStructure);
                //
                UART3_MAX485_R;
#elif (defined(STM32F4))
                GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_AF;
                GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_2MHz;
                GPIO_InitStructure.GPIO_OType   =  GPIO_OType_PP;
                GPIO_InitStructure.GPIO_PuPd    =  GPIO_PuPd_UP;
                GPIO_InitStructure.GPIO_Pin     =  UART3_GPIO_TX_PIN;
                GPIO_Init(UART3_GPIO_TX_PORT, &GPIO_InitStructure);
                GPIO_InitStructure.GPIO_Pin     =  UART3_GPIO_RX_PIN;
                GPIO_Init(UART3_GPIO_RX_PORT, &GPIO_InitStructure);
#endif
            }
#elif (defined(NRF51)||defined(NRF52))
#endif
            //------------------------------UART3(RS485)------------------------------End
            break;
#endif
#ifdef BSP_UART4_ENABLE
        case 4:
            //------------------------------UART4------------------------------Begin
#if   (defined(STM32F1)||defined(STM32F4))
            //关闭串口
            USART_Cmd(UART4, DISABLE);
            //-----启动相应时钟
            UART4_RCC_ENABLE;
            //-----DMA发送
            {
                //启动时钟
                UART4_DMA_TX_RCC_ENABLE;
                //中断设置(DMA发送)
                {
                    NVIC_InitTypeDef    NVIC_InitStructure;
                    NVIC_InitStructure.NVIC_IRQChannel = UART4_DMA_TX_IRQn;
                    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
                    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
                    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
                    NVIC_Init(&NVIC_InitStructure);
                }
                //配置
                {
                    DMA_InitTypeDef     DMA_InitStructure;
                    DMA_DeInit(UART4_TX_DMA_STREAM);
#if   (defined(STM32F1))
                    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&UART4->DR);
                    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)Uart_Tx4Buf;
                    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                          //方向：内存-->外设
                    DMA_InitStructure.DMA_BufferSize = 0;
                    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
                    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
                    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
                    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
                    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                               //DMA模式：一次传输，非循环
                    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
                    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
#elif (defined(STM32F4))
                    DMA_InitStructure.DMA_BufferSize         =  0;                            //配置DMA大小
                    DMA_InitStructure.DMA_FIFOMode           =  DMA_FIFOMode_Disable;         //在这个程序里面使能或者禁止都可以的
                    DMA_InitStructure.DMA_FIFOThreshold      =  DMA_FIFOThreshold_Full;       //设置阀值
                    DMA_InitStructure.DMA_MemoryBurst        =  DMA_MemoryBurst_Single ;      //设置内存为单字节突发模式
                    DMA_InitStructure.DMA_MemoryDataSize     =  DMA_MemoryDataSize_Byte;      //设置内存数据的位宽是字节
                    DMA_InitStructure.DMA_MemoryInc          =  DMA_MemoryInc_Enable;         //使能地址自增
                    DMA_InitStructure.DMA_Mode               =  DMA_Mode_Normal;              //设置DMA是正常模式
                    DMA_InitStructure.DMA_PeripheralBaseAddr =  (uint32_t) (&(UART4->DR)) ;  //设置外设地址
                    DMA_InitStructure.DMA_PeripheralBurst    =  DMA_PeripheralBurst_Single;   //设置外设为单字节突发模式
                    DMA_InitStructure.DMA_PeripheralDataSize =  DMA_PeripheralDataSize_Byte;; //设置外设数据的位宽是字节
                    DMA_InitStructure.DMA_PeripheralInc      =  DMA_PeripheralInc_Disable;    //禁止外设地址自增
                    DMA_InitStructure.DMA_Priority           =  DMA_Priority_High;            //设置优先级
                    // 配置 TX DMA
                    DMA_InitStructure.DMA_Channel            =  UART4_TX_DMA_CHANNEL ;       //配置发送通道
                    DMA_InitStructure.DMA_DIR                =  DMA_DIR_MemoryToPeripheral ;  //设置从内存到外设
                    DMA_InitStructure.DMA_Memory0BaseAddr    =  (uint32_t)Uart_Tx4Buf;        //设置内存地址
#endif
                    DMA_Init(UART4_TX_DMA_STREAM, &DMA_InitStructure);
                    //开启中断
                    DMA_ITConfig(UART4_TX_DMA_STREAM,DMA_IT_TC,ENABLE);
                }
            }
            //-----DMA接收
            {
                DMA_InitTypeDef     DMA_InitStructure;
                //启动时钟
                UART4_DMA_RX_RCC_ENABLE;
                //DMA通道配置
                DMA_DeInit(UART4_RX_DMA_STREAM);
#if   (defined(STM32F1))
                //外设地址
                DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&UART4->DR);
                //内存地址
                DMA_InitStructure.DMA_MemoryBaseAddr = (u32)Uart_Rx4Buf;
                //dma传输方向单向
                DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
                //设置DMA在传输时缓冲区的长度
                DMA_InitStructure.DMA_BufferSize = UART4_RX_BUF_SIZE;
                //设置DMA的外设递增模式，一个外设
                DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
                //设置DMA的内存递增模式
                DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
                //外设数据字长
                DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
                //内存数据字长
                DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
                //设置DMA的传输模式
                //DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
                DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
                //设置DMA的优先级别
                DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
                //设置DMA的2个memory中的变量互相访问
                DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
#elif (defined(STM32F4))
                DMA_InitStructure.DMA_BufferSize         =  UART4_RX_BUF_SIZE;                          // 配置DMA大小
                DMA_InitStructure.DMA_FIFOMode           =  DMA_FIFOMode_Disable;         // 在这个程序里面使能或者禁止都可以的
                DMA_InitStructure.DMA_FIFOThreshold      =  DMA_FIFOThreshold_Full;       // 设置阀值
                DMA_InitStructure.DMA_MemoryBurst        =  DMA_MemoryBurst_Single ;      // 设置内存为单字节突发模式
                DMA_InitStructure.DMA_MemoryDataSize     =  DMA_MemoryDataSize_Byte;      // 设置内存数据的位宽是字节
                DMA_InitStructure.DMA_MemoryInc          =  DMA_MemoryInc_Enable;         // 使能地址自增
                DMA_InitStructure.DMA_Mode               =  DMA_Mode_Circular;            // 设置DMA是正常模式
                DMA_InitStructure.DMA_PeripheralBaseAddr =  (uint32_t) (&(UART4->DR)) ;  // 设置外设地址
                DMA_InitStructure.DMA_PeripheralBurst    =  DMA_PeripheralBurst_Single;   // 设置外设为单字节突发模式
                DMA_InitStructure.DMA_PeripheralDataSize =  DMA_PeripheralDataSize_Byte;  // 设置外设数据的位宽是字节
                DMA_InitStructure.DMA_PeripheralInc      =  DMA_PeripheralInc_Disable;    // 禁止外设地址自增
                DMA_InitStructure.DMA_Priority           =  DMA_Priority_Medium;          // 设置优先级
                // 配置 RX DMA
                DMA_InitStructure.DMA_Channel            =  UART4_RX_DMA_CHANNEL ;        // 配置接收通道
                DMA_InitStructure.DMA_DIR                =  DMA_DIR_PeripheralToMemory ;  // 设置从外设到内存
                DMA_InitStructure.DMA_Memory0BaseAddr    =  (uint32_t)Uart_Rx4Buf ;       // 设置内存地址
#endif
                DMA_Init(UART4_RX_DMA_STREAM,&DMA_InitStructure);
                //使能通道
                DMA_Cmd(UART4_RX_DMA_STREAM,ENABLE);
            }
            //-----串口初始化
            {
                USART_InitTypeDef   USART_InitStructure;
                //配置UART
                USART_InitStructure.USART_BaudRate            = baudrate;            // 波特率
                USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
                USART_InitStructure.USART_StopBits            = USART_StopBits_1;
                USART_InitStructure.USART_Parity              = USART_Parity_No;
                USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
                USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
                USART_Init(UART4, &USART_InitStructure);
                //使能中断
                USART_ITConfig(UART4,USART_IT_TC   ,DISABLE);
                USART_ITConfig(UART4,USART_IT_RXNE ,DISABLE);
#if   (UART4_RX_OVERTIME_MS==0)
                USART_ITConfig(UART4,USART_IT_IDLE ,ENABLE);
#else
                USART_ITConfig(UART4,USART_IT_IDLE ,DISABLE);
#endif
                //配置中断
                {
                    NVIC_InitTypeDef    NVIC_InitStructure;
                    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
                    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
                    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
                    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
                    NVIC_Init(&NVIC_InitStructure);
                }
                //采用DMA方式发送
                USART_DMACmd(UART4, USART_DMAReq_Rx | USART_DMAReq_Tx, ENABLE);
                //启动串口
                USART_Cmd(UART4, ENABLE);
            }
            //-----配置GPIO
            {
                GPIO_InitTypeDef    GPIO_InitStructure;
                UART4_GPIO_RCC_ENABLE;
                UART4_GPIO_TX_REMAP;
                UART4_GPIO_RX_REMAP;
#if   (defined(STM32F1))
                //GPIO_PinRemapConfig(GPIO_FullRemap_UART4, ENABLE);
                //Rx
                GPIO_InitStructure.GPIO_Pin  = UART4_GPIO_RX_PIN;
                GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
                GPIO_Init(UART4_GPIO_RX_PORT, &GPIO_InitStructure);
                //Tx
                GPIO_InitStructure.GPIO_Pin   = UART4_GPIO_TX_PIN;
                GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
                GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
                GPIO_Init(UART4_GPIO_TX_PORT, &GPIO_InitStructure);
                //
                UART4_MAX485_R;
#elif (defined(STM32F4))
                GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_AF;
                GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_2MHz;
                GPIO_InitStructure.GPIO_OType   =  GPIO_OType_PP;
                GPIO_InitStructure.GPIO_PuPd    =  GPIO_PuPd_UP;
                GPIO_InitStructure.GPIO_Pin     =  UART4_GPIO_TX_PIN;
                GPIO_Init(UART4_GPIO_TX_PORT, &GPIO_InitStructure);
                GPIO_InitStructure.GPIO_Pin     =  UART4_GPIO_RX_PIN;
                GPIO_Init(UART4_GPIO_RX_PORT, &GPIO_InitStructure);
#endif
            }
#elif (defined(NRF51)||defined(NRF52))
#endif
            //------------------------------UART4------------------------------End
            break;
#endif
#ifdef BSP_UART5_ENABLE
        case 5:
            //------------------------------UART5------------------------------Begin
#if   (defined(STM32F1)||defined(STM32F4))
            //关闭串口
            USART_Cmd(UART5, DISABLE);
            //-----启动相应时钟
            UART5_RCC_ENABLE;
            //STM32F103系列芯片串口5无DMA
#if 0
            //-----DMA发送
            {
                //启动时钟
                UART5_DMA_TX_RCC_ENABLE;
                //中断设置
                NVIC_InitStructure.NVIC_IRQChannel = UART5_DMA_TX_IRQn;
                NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
                NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
                NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
                NVIC_Init(&NVIC_InitStructure);
                //配置
                DMA_DeInit(DMA1_Channel2);
                DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART3->DR);
                DMA_InitStructure.DMA_MemoryBaseAddr = (u32)Uart_Tx3Buf;
                DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                          //方向：内存-->外设
                DMA_InitStructure.DMA_BufferSize = 0;
                DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
                DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
                DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
                DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
                DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                               //DMA模式：一次传输，非循环
                DMA_InitStructure.DMA_Priority = DMA_Priority_High;
                DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
                DMA_Init(DMA1_Channel2, &DMA_InitStructure);
                //开启中断
                DMA_ITConfig(DMA1_Channel2,DMA_IT_TC,ENABLE);
            }
            //-----DMA接收
            {
                //启动时钟
                RCC_AHBPeriphClockCmd( RCC_AHBPeriph_DMA1,ENABLE);
                //DMA1通道5配置
                DMA_DeInit(DMA1_Channel3);
                //外设地址
                DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART3->DR);
                //内存地址
                DMA_InitStructure.DMA_MemoryBaseAddr = (u32)Uart_Rx3Buf;
                //dma传输方向单向
                DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
                //设置DMA在传输时缓冲区的长度
                DMA_InitStructure.DMA_BufferSize = 256;
                //设置DMA的外设递增模式，一个外设
                DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
                //设置DMA的内存递增模式
                DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
                //外设数据字长
                DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
                //内存数据字长
                DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
                //设置DMA的传输模式
                //DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
                DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
                //设置DMA的优先级别
                DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
                //设置DMA的2个memory中的变量互相访问
                DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
                DMA_Init(DMA1_Channel3,&DMA_InitStructure);
                //使能通道5
                DMA_Cmd(DMA1_Channel3,ENABLE);
            }
#endif
            //-----串口初始化
            {
                USART_InitTypeDef   USART_InitStructure;
                //配置UART
                USART_InitStructure.USART_BaudRate            = baudrate;            // 波特率
                USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
                USART_InitStructure.USART_StopBits            = USART_StopBits_1;
                USART_InitStructure.USART_Parity              = USART_Parity_No;
                USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
                USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
                USART_Init(UART5, &USART_InitStructure);
                //使能中断
                /*
                USART_ITConfig(UART5,USART_IT_TC   ,DISABLE);
                USART_ITConfig(UART5,USART_IT_RXNE ,ENABLE);
                USART_ITConfig(UART5,USART_IT_IDLE ,ENABLE);
                */
                //
                USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);       //使能UART5接收中断
                USART_ITConfig(UART5, USART_IT_PE, ENABLE);
                USART_ITConfig(UART5, USART_IT_NE, ENABLE);
                USART_ITConfig(UART5, USART_IT_ORE, ENABLE);
                USART_ITConfig(UART5, USART_IT_FE, ENABLE);
                //配置中断
                {
                    NVIC_InitTypeDef    NVIC_InitStructure;
                    NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
                    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
                    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
                    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
                    NVIC_Init(&NVIC_InitStructure);
                }
                //采用DMA方式发送
                //STM32F103系列芯片串口5无DMA
#if 0
                USART_DMACmd(UART5, USART_DMAReq_Rx | USART_DMAReq_Tx, ENABLE);
#endif
                //启动串口
                USART_Cmd(UART5, ENABLE);
            }
            //-----配置GPIO
            {
                GPIO_InitTypeDef    GPIO_InitStructure;
                UART5_GPIO_RCC_ENABLE;
                UART5_GPIO_TX_REMAP;
                UART5_GPIO_RX_REMAP;
#if   (defined(STM32F1))
                GPIO_InitStructure.GPIO_Pin  = UART5_GPIO_RX_PIN;
                GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
                GPIO_Init(UART5_GPIO_RX_PORT, &GPIO_InitStructure);
                GPIO_InitStructure.GPIO_Pin   = UART5_GPIO_TX_PIN;
                GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
                GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
                GPIO_Init(UART5_GPIO_TX_PORT, &GPIO_InitStructure);
                //
                UART5_MAX485_R;
#elif (defined(STM32F4))
                GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_AF;
                GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_2MHz;
                GPIO_InitStructure.GPIO_OType   =  GPIO_OType_PP;
                GPIO_InitStructure.GPIO_PuPd    =  GPIO_PuPd_UP;
                GPIO_InitStructure.GPIO_Pin     =  UART5_GPIO_TX_PIN;
                GPIO_Init(UART5_GPIO_TX_PORT, &GPIO_InitStructure);
                GPIO_InitStructure.GPIO_Pin     =  UART5_GPIO_RX_PIN;
                GPIO_Init(UART5_GPIO_RX_PORT, &GPIO_InitStructure);
#endif
            }
#elif (defined(NRF51)||defined(NRF52))
#endif
            //------------------------------UART5------------------------------End
            break;
#endif
        default:
            break;
    }
}
/*******************************************************************************
函数功能: 串口发送固定长度数据
*******************************************************************************/
uint8_t UART_DMA_Tx(uint8_t ch,uint8_t *buf,uint16_t len)
{
    if(len==0 && buf==NULL)
    {
        return ERR;
    }
    if      (ch==0)
    {
#ifdef BSP_UART0_USB_ENABLE
#if   (defined(STM32F1)||defined(STM32F4))
        //USB
        USB_TxWrite(buf, len);
#endif
        return OK;
#endif
    }
#ifdef BSP_UART1_ENABLE
    else if (ch==1)
    {
        //
#if   (defined(STM32F1)||defined(STM32F4))
        //清标志
        while(Uart_Tx1SendEnable!=2)
        {
            MODULE_OS_DELAY_MS(1);
        }
        Uart_Tx1SendEnable=0;
        UART1_MAX485_T;
        //复制数据
        memcpy(Uart_Tx1Buf,buf,len);
        DMA_SetCurrDataCounter(UART1_TX_DMA_STREAM,len);
        //启动DMA发送
        DMA_Cmd(UART1_TX_DMA_STREAM, ENABLE);
#elif (defined(NRF51)||defined(NRF52))
        {
            uint16_t i16;
            for(i16=0; i16<len; i16++)
            {
                app_uart_put(buf[i16]);
            }
        }
#endif
        //
        return OK;
    }
#endif
#ifdef BSP_UART2_ENABLE
    else if (ch==2)
    {
        //清标志
        while(Uart_Tx2SendEnable!=2)
        {
            MODULE_OS_DELAY_MS(5);
        }
        Uart_Tx2SendEnable=0;
        //
        UART2_MAX485_T;
#if   (defined(STM32F1)||defined(STM32F4))
        //复制数据
        memcpy(Uart_Tx2Buf,buf,len);
        DMA_SetCurrDataCounter(UART2_TX_DMA_STREAM,len);
        //启动DMA发送
        DMA_Cmd(UART2_TX_DMA_STREAM, ENABLE);
#endif
        //
        return OK;
    }
#endif
#ifdef BSP_UART3_ENABLE
    else if (ch==3)
    {
        //清标志
        while(Uart_Tx3SendEnable!=2)
        {
            MODULE_OS_DELAY_MS(5);
        }
        Uart_Tx3SendEnable=0;
        //
        UART3_MAX485_T;
#if   (defined(STM32F1)||defined(STM32F4))
        //复制数据
        memcpy(Uart_Tx3Buf,buf,len);
        DMA_SetCurrDataCounter(UART3_TX_DMA_STREAM,len);
        //启动DMA发送
        DMA_Cmd(UART3_TX_DMA_STREAM, ENABLE);
#endif
        //
        return OK;
    }
#endif
#ifdef BSP_UART4_ENABLE
    else if (ch==4)
    {
        //清标志
        while(Uart_Tx4SendEnable!=2)
        {
            MODULE_OS_DELAY_MS(5);
        }
        Uart_Tx4SendEnable=0;
        //
        UART4_MAX485_T;
#if   (defined(STM32F1)||defined(STM32F4))
        //复制数据
        memcpy(Uart_Tx4Buf,buf,len);
        DMA_SetCurrDataCounter(UART4_TX_DMA_STREAM,len);
        //启动DMA发送
        DMA_Cmd(UART4_TX_DMA_STREAM, ENABLE);
#endif
        //
        return OK;
    }
#endif
#ifdef BSP_UART5_ENABLE
    else if(ch==5)
    {
        //清标志
        while(Uart_Tx5SendEnable!=2)
        {
            MODULE_OS_DELAY_MS(5);
        }
        Uart_Tx5SendEnable=0;
        //
        UART5_MAX485_T;
        //复制数据
        memcpy(Uart_Tx5Buf,buf,len);
        //
        UART_TX5_IRQ(len);
        //
        return OK;
    }
#endif
    return ERR;
}
/*******************************************************************************
函数功能: 串口测试
*******************************************************************************/
void BspUart_DebugTestOnOff(uint8_t OnOff)
{
    if(OnOff==ON)
    {
        BspUart_DebugTest_Enable=1;
    }
    else
    {
        BspUart_DebugTest_Enable=0;
    }
}
//-------------------------------------------------------------------------------
