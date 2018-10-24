/**
  ******************************************************************************
  * @file    Bsp_Uart.h 
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  * @brief   bottom-driven -->   串口驱动.
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
#ifndef __BSP_UART_H
#define __BSP_UART_H
//-------------------加载库函数------------------------------
#include "includes.h"
//-------------------编译开关--------------------------------
#if   (defined(PROJECT_BASE_STM32F1)||defined(PROJECT_BASE_STM32F4))
#define BSP_UART0_USB_ENABLE
#elif (defined(PROJECT_BIB_TEST1))
#define BSP_UART2_ENABLE
#elif (defined(PROJECT_XKAP_V3)||defined(XKAP_ICARE_B_D_M))
#define BSP_UART0_USB_ENABLE
#define BSP_UART1_ENABLE
#define BSP_UART2_ENABLE
#define BSP_UART3_ENABLE
#elif (defined(PROJECT_TCI_V30))
#define BSP_UART0_USB_ENABLE
#elif (defined(PROJECT_SPI_SLAVE))
#define BSP_UART0_USB_ENABLE
#define BSP_UART1_ENABLE
#define BSP_UART2_ENABLE
#define BSP_UART3_ENABLE
#define BSP_UART4_ENABLE
#define BSP_UART5_ENABLE
#elif (defined(PROJECT_ARMFLY_V5_XSL))
//#define BSP_UART0_USB_ENABLE
#define BSP_UART1_ENABLE
#elif (defined(XKAP_ICARE_A_M)||defined(XKAP_ICARE_A_S)\
      ||defined(BASE_NRF51)   ||defined(BASE_NRF52)\
      ||defined(XKAP_ICARE_B_M)\
      ||defined(XKAP_ICARE_B_D))
#define BSP_UART1_ENABLE
#else
#error Please Set Project to Bsp_Uart.h
#endif
//-------------------接口宏定义(硬件相关)--------------------
//端口
#define UART1_GPIO_TX_PORT             GPIOA
#define UART1_GPIO_TX_PIN              GPIO_Pin_9
#define UART1_GPIO_TX_SOURCE           GPIO_PinSource9
#define UART1_GPIO_RX_PORT             GPIOA
#define UART1_GPIO_RX_PIN              GPIO_Pin_10
#define UART1_GPIO_RX_SOURCE           GPIO_PinSource10
#define UART2_GPIO_TX_PORT             GPIOA
#define UART2_GPIO_TX_PIN              GPIO_Pin_2
#define UART2_GPIO_TX_SOURCE           GPIO_PinSource2
#define UART2_GPIO_RX_PORT             GPIOA
#define UART2_GPIO_RX_PIN              GPIO_Pin_3
#define UART2_GPIO_RX_SOURCE           GPIO_PinSource3
#define UART3_GPIO_TX_PORT             GPIOD
#define UART3_GPIO_TX_PIN              GPIO_Pin_8
#define UART3_GPIO_TX_SOURCE           GPIO_PinSource8
#define UART3_GPIO_RX_PORT             GPIOD
#define UART3_GPIO_RX_PIN              GPIO_Pin_9
#define UART3_GPIO_RX_SOURCE           GPIO_PinSource9
#define UART4_GPIO_TX_PORT             GPIOC
#define UART4_GPIO_TX_PIN              GPIO_Pin_10
#define UART4_GPIO_TX_SOURCE           GPIO_PinSource10
#define UART4_GPIO_RX_PORT             GPIOC
#define UART4_GPIO_RX_PIN              GPIO_Pin_11
#define UART4_GPIO_RX_SOURCE           GPIO_PinSource11
#define UART5_GPIO_TX_PORT             GPIOC
#define UART5_GPIO_TX_PIN              GPIO_Pin_12
#define UART5_GPIO_TX_SOURCE           GPIO_PinSource12
#define UART5_GPIO_RX_PORT             GPIOD
#define UART5_GPIO_RX_PIN              GPIO_Pin_2
#define UART5_GPIO_RX_SOURCE           GPIO_PinSource2
//UART-DMA
#if   (defined(STM32F1))
//-----UART1
#define UART1_GPIO_TX_REMAP
#define UART1_GPIO_RX_REMAP
#define UART1_GPIO_RCC_ENABLE          RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE)
#define UART1_RCC_ENABLE               RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE)
#define UART1_DMA_TX_RCC_ENABLE        RCC_AHBPeriphClockCmd( RCC_AHBPeriph_DMA1,ENABLE)
#define UART1_DMA_RX_RCC_ENABLE        RCC_AHBPeriphClockCmd( RCC_AHBPeriph_DMA1,ENABLE);
#define UART1_TX_DMA_CHANNEL           DMA1_Channel4
#define UART1_TX_DMA_STREAM            UART1_TX_DMA_CHANNEL
#define UART1_TX_DMA_FLAG_TCIF         DMA1_FLAG_TC4
#define UART1_RX_DMA_CHANNEL           DMA1_Channel5
#define UART1_RX_DMA_STREAM            UART1_RX_DMA_CHANNEL
#define UART1_DMA_TX_IRQn              DMA1_Channel4_IRQn
#define UART1_DMA_TX_IRQHandler        DMA1_Channel4_IRQHandler
#define UART1_RxTx_IRQHandler          USART1_IRQHandler
//-----UART2
#define UART2_GPIO_TX_REMAP
#define UART2_GPIO_RX_REMAP
#define UART2_GPIO_RCC_ENABLE          RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA , ENABLE);
#define UART2_RCC_ENABLE               RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE)
#define UART2_DMA_TX_RCC_ENABLE        RCC_AHBPeriphClockCmd( RCC_AHBPeriph_DMA1,ENABLE)
#define UART2_DMA_RX_RCC_ENABLE        RCC_AHBPeriphClockCmd( RCC_AHBPeriph_DMA1,ENABLE)
#define UART2_TX_DMA_CHANNEL           DMA1_Channel7
#define UART2_TX_DMA_STREAM            UART2_TX_DMA_CHANNEL
#define UART2_TX_DMA_FLAG_TCIF         DMA1_FLAG_TC7
#define UART2_RX_DMA_CHANNEL           DMA1_Channel6
#define UART2_RX_DMA_STREAM            UART2_RX_DMA_CHANNEL
#define UART2_DMA_TX_IRQn              DMA1_Channel7_IRQn
#define UART2_DMA_TX_IRQHandler        DMA1_Channel7_IRQHandler
#define UART2_RxTx_IRQHandler          USART2_IRQHandler
//-----UART3
#define UART3_GPIO_TX_REMAP
#define UART3_GPIO_RX_REMAP
#define UART3_GPIO_RCC_ENABLE          RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE)
#define UART3_RCC_ENABLE               RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE)
#define UART3_DMA_TX_RCC_ENABLE        RCC_AHBPeriphClockCmd( RCC_AHBPeriph_DMA1,ENABLE)
#define UART3_DMA_RX_RCC_ENABLE        RCC_AHBPeriphClockCmd( RCC_AHBPeriph_DMA1,ENABLE)
#define UART3_TX_DMA_CHANNEL           DMA1_Channel2
#define UART3_TX_DMA_STREAM            UART3_TX_DMA_CHANNEL
#define UART3_TX_DMA_FLAG_TCIF         DMA1_FLAG_TC2
#define UART3_RX_DMA_CHANNEL           DMA1_Channel3
#define UART3_RX_DMA_STREAM            UART3_RX_DMA_CHANNEL
#define UART3_DMA_TX_IRQn              DMA1_Channel2_IRQn
#define UART3_DMA_TX_IRQHandler        DMA1_Channel2_IRQHandler
#define UART3_RxTx_IRQHandler          USART3_IRQHandler
//-----UART4
#define UART4_GPIO_TX_REMAP
#define UART4_GPIO_RX_REMAP
#define UART4_GPIO_RCC_ENABLE          RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC , ENABLE);
#define UART4_RCC_ENABLE               RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE)
#define UART4_DMA_TX_RCC_ENABLE        RCC_AHBPeriphClockCmd( RCC_AHBPeriph_DMA2,ENABLE)
#define UART4_DMA_RX_RCC_ENABLE        RCC_AHBPeriphClockCmd( RCC_AHBPeriph_DMA2,ENABLE)
#define UART4_TX_DMA_CHANNEL           DMA2_Channel5
#define UART4_TX_DMA_STREAM            UART4_TX_DMA_CHANNEL
#define UART4_TX_DMA_FLAG_TCIF         DMA2_FLAG_TC5
#define UART4_RX_DMA_CHANNEL           DMA2_Channel3
#define UART4_RX_DMA_STREAM            UART4_RX_DMA_CHANNEL
#define UART4_DMA_TX_IRQn              DMA2_Channel4_5_IRQn
#define UART4_DMA_TX_IRQHandler        DMA2_Channel4_5_IRQHandler
#define UART4_RxTx_IRQHandler          UART4_IRQHandler
//-----UART5
#define UART5_GPIO_TX_REMAP
#define UART5_GPIO_RX_REMAP
#define UART5_GPIO_RCC_ENABLE          RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);
#define UART5_RCC_ENABLE               RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE)
#define UART5_DMA_RCC_ENABLE
#define UART5_DMA_TX_IRQn
#define UART5_DMA_TX_IRQHandler         //DMA1_Channel4_IRQHandler
#define UART5_RxTx_IRQHandler          UART5_IRQHandler
//-----UART6
#define UART6_GPIO_TX_REMAP
#define UART6_GPIO_RX_REMAP
#define UART6_GPIO_RCC_ENABLE
#define UART6_RCC_ENABLE
#define UART6_DMA_RCC_ENABLE
#define UART6_DMA_TX_IRQn
#define UART6_DMA_TX_IRQHandler        //DMA1_Channel4_IRQHandler
#define UART6_RxTx_IRQHandler          USART6_IRQHandler
#elif (defined(STM32F4))
//-----UART1
#define UART1_GPIO_TX_REMAP            GPIO_PinAFConfig(UART1_GPIO_TX_PORT, UART1_GPIO_TX_SOURCE, GPIO_AF_USART1);
#define UART1_GPIO_RX_REMAP            GPIO_PinAFConfig(UART1_GPIO_RX_PORT, UART1_GPIO_RX_SOURCE, GPIO_AF_USART1);
#define UART1_GPIO_RCC_ENABLE          RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE)
#define UART1_RCC_ENABLE               RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE)
#define UART1_DMA_TX_RCC_ENABLE        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE)
#define UART1_DMA_RX_RCC_ENABLE        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE)
#define UART1_DMA                      DMA2
#define UART1_TX_DMA_CHANNEL           DMA_Channel_4
#define UART1_TX_DMA_STREAM            DMA2_Stream7
#define UART1_TX_DMA_FLAG_FEIF         DMA_FLAG_FEIF7
#define UART1_TX_DMA_FLAG_DMEIF        DMA_FLAG_DMEIF7
#define UART1_TX_DMA_FLAG_TEIF         DMA_FLAG_TEIF7
#define UART1_TX_DMA_FLAG_HTIF         DMA_FLAG_HTIF7
#define UART1_TX_DMA_FLAG_TCIF         DMA_FLAG_TCIF7
#define UART1_RX_DMA_CHANNEL           DMA_Channel_4
#define UART1_RX_DMA_STREAM            DMA2_Stream2
#define UART1_RX_DMA_FLAG_FEIF         DMA_FLAG_FEIF2
#define UART1_RX_DMA_FLAG_DMEIF        DMA_FLAG_DMEIF2
#define UART1_RX_DMA_FLAG_TEIF         DMA_FLAG_TEIF2
#define UART1_RX_DMA_FLAG_HTIF         DMA_FLAG_HTIF2
#define UART1_RX_DMA_FLAG_TCIF         DMA_FLAG_TCIF2
#define UART1_DMA_TX_IRQn              DMA2_Stream7_IRQn
#define UART1_DMA_RX_IRQn              DMA2_Stream2_IRQn
#define UART1_DMA_TX_IRQHandler        DMA2_Stream7_IRQHandler
#define UART1_DMA_RX_IRQHandler        DMA2_Stream2_IRQHandler
#define UART1_RxTx_IRQHandler          USART1_IRQHandler
//-----UART2
#define UART2_GPIO_TX_REMAP            GPIO_PinAFConfig(UART2_GPIO_TX_PORT, UART2_GPIO_TX_SOURCE, GPIO_AF_USART2);
#define UART2_GPIO_RX_REMAP            GPIO_PinAFConfig(UART2_GPIO_RX_PORT, UART2_GPIO_RX_SOURCE, GPIO_AF_USART2);
#define UART2_GPIO_RCC_ENABLE          RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE)
#define UART2_RCC_ENABLE               RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE)
#define UART2_DMA_TX_RCC_ENABLE        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE)
#define UART2_DMA_RX_RCC_ENABLE        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE)
#define UART2_DMA                      DMA1
#define UART2_TX_DMA_CHANNEL           DMA_Channel_4
#define UART2_TX_DMA_STREAM            DMA1_Stream6
#define UART2_TX_DMA_FLAG_FEIF         DMA_FLAG_FEIF6
#define UART2_TX_DMA_FLAG_DMEIF        DMA_FLAG_DMEIF6
#define UART2_TX_DMA_FLAG_TEIF         DMA_FLAG_TEIF6
#define UART2_TX_DMA_FLAG_HTIF         DMA_FLAG_HTIF6
#define UART2_TX_DMA_FLAG_TCIF         DMA_FLAG_TCIF6
#define UART2_RX_DMA_CHANNEL           DMA_Channel_4
#define UART2_RX_DMA_STREAM            DMA1_Stream5
#define UART2_RX_DMA_FLAG_FEIF         DMA_FLAG_FEIF5
#define UART2_RX_DMA_FLAG_DMEIF        DMA_FLAG_DMEIF5
#define UART2_RX_DMA_FLAG_TEIF         DMA_FLAG_TEIF5
#define UART2_RX_DMA_FLAG_HTIF         DMA_FLAG_HTIF5
#define UART2_RX_DMA_FLAG_TCIF         DMA_FLAG_TCIF5
#define UART2_DMA_TX_IRQn              DMA1_Stream6_IRQn
#define UART2_DMA_RX_IRQn              DMA1_Stream5_IRQn
#define UART2_DMA_TX_IRQHandler        DMA1_Stream6_IRQHandler
#define UART2_DMA_RX_IRQHandler        DMA1_Stream5_IRQHandler
#define UART2_RxTx_IRQHandler          USART2_IRQHandler
//-----UART3
#define UART3_GPIO_TX_REMAP            GPIO_PinAFConfig(UART3_GPIO_TX_PORT, UART3_GPIO_TX_SOURCE, GPIO_AF_USART3);
#define UART3_GPIO_RX_REMAP            GPIO_PinAFConfig(UART3_GPIO_RX_PORT, UART3_GPIO_RX_SOURCE, GPIO_AF_USART3);
#define UART3_GPIO_RCC_ENABLE          RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE)
#define UART3_RCC_ENABLE               RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE)
#define UART3_DMA_TX_RCC_ENABLE        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE)
#define UART3_DMA_RX_RCC_ENABLE        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE)
#define UART3_DMA                      DMA1
#define UART3_TX_DMA_CHANNEL           DMA_Channel_4
#define UART3_TX_DMA_STREAM            DMA1_Stream3
#define UART3_TX_DMA_FLAG_FEIF         DMA_FLAG_FEIF3
#define UART3_TX_DMA_FLAG_DMEIF        DMA_FLAG_DMEIF3
#define UART3_TX_DMA_FLAG_TEIF         DMA_FLAG_TEIF3
#define UART3_TX_DMA_FLAG_HTIF         DMA_FLAG_HTIF3
#define UART3_TX_DMA_FLAG_TCIF         DMA_FLAG_TCIF3
#define UART3_RX_DMA_CHANNEL           DMA_Channel_4
#define UART3_RX_DMA_STREAM            DMA1_Stream1
#define UART3_RX_DMA_FLAG_FEIF         DMA_FLAG_FEIF1
#define UART3_RX_DMA_FLAG_DMEIF        DMA_FLAG_DMEIF1
#define UART3_RX_DMA_FLAG_TEIF         DMA_FLAG_TEIF1
#define UART3_RX_DMA_FLAG_HTIF         DMA_FLAG_HTIF1
#define UART3_RX_DMA_FLAG_TCIF         DMA_FLAG_TCIF1
#define UART3_DMA_TX_IRQn              DMA1_Stream3_IRQn
#define UART3_DMA_RX_IRQn              DMA1_Stream1_IRQn
#define UART3_DMA_TX_IRQHandler        DMA1_Stream3_IRQHandler
#define UART3_DMA_RX_IRQHandler        DMA1_Stream1_IRQHandler
#define UART3_RxTx_IRQHandler          USART3_IRQHandler
//-----UART4
#define UART4_GPIO_TX_REMAP            GPIO_PinAFConfig(UART4_GPIO_TX_PORT, UART4_GPIO_TX_SOURCE, GPIO_AF_UART4);
#define UART4_GPIO_RX_REMAP            GPIO_PinAFConfig(UART4_GPIO_RX_PORT, UART4_GPIO_RX_SOURCE, GPIO_AF_UART4);
#define UART4_GPIO_RCC_ENABLE          RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE)
#define UART4_RCC_ENABLE               RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE)
#define UART4_DMA_TX_RCC_ENABLE        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE)
#define UART4_DMA_RX_RCC_ENABLE        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE)
#define UART4_DMA                      DMA1
#define UART4_TX_DMA_CHANNEL           DMA_Channel_4
#define UART4_TX_DMA_STREAM            DMA1_Stream4
#define UART4_TX_DMA_FLAG_FEIF         DMA_FLAG_FEIF4
#define UART4_TX_DMA_FLAG_DMEIF        DMA_FLAG_DMEIF4
#define UART4_TX_DMA_FLAG_TEIF         DMA_FLAG_TEIF4
#define UART4_TX_DMA_FLAG_HTIF         DMA_FLAG_HTIF4
#define UART4_TX_DMA_FLAG_TCIF         DMA_FLAG_TCIF4
#define UART4_RX_DMA_CHANNEL           DMA_Channel_4
#define UART4_RX_DMA_STREAM            DMA1_Stream2
#define UART4_RX_DMA_FLAG_FEIF         DMA_FLAG_FEIF2
#define UART4_RX_DMA_FLAG_DMEIF        DMA_FLAG_DMEIF2
#define UART4_RX_DMA_FLAG_TEIF         DMA_FLAG_TEIF2
#define UART4_RX_DMA_FLAG_HTIF         DMA_FLAG_HTIF2
#define UART4_RX_DMA_FLAG_TCIF         DMA_FLAG_TCIF2
#define UART4_DMA_TX_IRQn              DMA1_Stream4_IRQn
#define UART4_DMA_RX_IRQn              DMA1_Stream2_IRQn
#define UART4_DMA_TX_IRQHandler        DMA1_Stream4_IRQHandler
#define UART4_DMA_RX_IRQHandler        DMA1_Stream2_IRQHandler
#define UART4_RxTx_IRQHandler          UART4_IRQHandler
//-----UART5
#define UART5_GPIO_TX_REMAP            GPIO_PinAFConfig(UART5_GPIO_TX_PORT, UART5_GPIO_TX_SOURCE, GPIO_AF_UART5);
#define UART5_GPIO_RX_REMAP            GPIO_PinAFConfig(UART5_GPIO_RX_PORT, UART5_GPIO_RX_SOURCE, GPIO_AF_UART5);
#define UART5_GPIO_RCC_ENABLE          RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD , ENABLE)
#define UART5_RCC_ENABLE               RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE)
#define UART5_DMA_TX_RCC_ENABLE        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE)
#define UART5_DMA_RX_RCC_ENABLE        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE)
#define UART5_DMA                      DMA1
#define UART5_TX_DMA_CHANNEL           DMA_Channel_4
#define UART5_TX_DMA_STREAM            DMA1_Stream7
#define UART5_TX_DMA_FLAG_FEIF         DMA_FLAG_FEIF7
#define UART5_TX_DMA_FLAG_DMEIF        DMA_FLAG_DMEIF7
#define UART5_TX_DMA_FLAG_TEIF         DMA_FLAG_TEIF7
#define UART5_TX_DMA_FLAG_HTIF         DMA_FLAG_HTIF7
#define UART5_TX_DMA_FLAG_TCIF         DMA_FLAG_TCIF7
#define UART5_RX_DMA_CHANNEL           DMA_Channel_4
#define UART5_RX_DMA_STREAM            DMA1_Stream0
#define UART5_RX_DMA_FLAG_FEIF         DMA_FLAG_FEIF2
#define UART5_RX_DMA_FLAG_DMEIF        DMA_FLAG_DMEIF2
#define UART5_RX_DMA_FLAG_TEIF         DMA_FLAG_TEIF2
#define UART5_RX_DMA_FLAG_HTIF         DMA_FLAG_HTIF2
#define UART5_RX_DMA_FLAG_TCIF         DMA_FLAG_TCIF2
#define UART5_DMA_TX_IRQn              DMA1_Stream7_IRQn
#define UART5_DMA_RX_IRQn              DMA1_Stream0_IRQn
#define UART5_DMA_TX_IRQHandler        DMA1_Stream7_IRQHandler
#define UART5_DMA_RX_IRQHandler        DMA1_Stream0_IRQHandler
#define UART5_RxTx_IRQHandler          UART5_IRQHandler
//-----UART6
#define UART6_GPIO_TX_REMAP
#define UART6_GPIO_RX_REMAP
#define UART6_GPIO_RCC_ENABLE
#define UART6_RCC_ENABLE               RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE)
#define UART6_DMA_TX_RCC_ENABLE        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE)
#define UART6_DMA_RX_RCC_ENABLE        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE)
#define UART6_DMA                      DMA2
#define UART6_TX_DMA_CHANNEL           DMA_Channel_5
#define UART6_TX_DMA_STREAM            DMA2_Stream6
#define UART6_TX_DMA_FLAG_FEIF         DMA_FLAG_FEIF6
#define UART6_TX_DMA_FLAG_DMEIF        DMA_FLAG_DMEIF6
#define UART6_TX_DMA_FLAG_TEIF         DMA_FLAG_TEIF6
#define UART6_TX_DMA_FLAG_HTIF         DMA_FLAG_HTIF6
#define UART6_TX_DMA_FLAG_TCIF         DMA_FLAG_TCIF6
#define UART6_RX_DMA_CHANNEL           DMA_Channel_5
#define UART6_RX_DMA_STREAM            DMA2_Stream1
#define UART6_RX_DMA_FLAG_FEIF         DMA_FLAG_FEIF1
#define UART6_RX_DMA_FLAG_DMEIF        DMA_FLAG_DMEIF1
#define UART6_RX_DMA_FLAG_TEIF         DMA_FLAG_TEIF1
#define UART6_RX_DMA_FLAG_HTIF         DMA_FLAG_HTIF1
#define UART6_RX_DMA_FLAG_TCIF         DMA_FLAG_TCIF1
#define UART6_DMA_TX_IRQn              DMA2_Stream6_IRQn
#define UART6_DMA_RX_IRQn              DMA2_Stream1_IRQn
#define UART6_DMA_TX_IRQHandler        DMA2_Stream6_IRQHandler
#define UART6_DMA_RX_IRQHandler        DMA2_Stream1_IRQHandler
#define UART6_RxTx_IRQHandler          USART6_IRQHandler
#endif
//----------485物理引脚
#define UART1_MAX485_EN
#define UART1_MAX485_T
#define UART1_MAX485_R
#define UART2_MAX485_EN
#define UART2_MAX485_T           //GPIO_SetBits(GPIOD, GPIO_Pin_15)
#define UART2_MAX485_R           //GPIO_ResetBits(GPIOD, GPIO_Pin_15)
#define UART3_MAX485_EN
#define UART3_MAX485_T           //GPIO_SetBits(GPIOD, GPIO_Pin_1)
#define UART3_MAX485_R           //GPIO_ResetBits(GPIOD, GPIO_Pin_1)
#define UART4_MAX485_EN
#define UART4_MAX485_T           //GPIO_SetBits(GPIOD, GPIO_Pin_7)
#define UART4_MAX485_R           //GPIO_ResetBits(GPIOD, GPIO_Pin_7)
#define UART5_MAX485_EN
#define UART5_MAX485_T
#define UART5_MAX485_R
//-------------------接口宏定义(硬件无关)--------------------
//----------发送超时(引脚切换延时)
#define UART1_TX_OVERTIME_MS        3
#define UART2_TX_OVERTIME_MS        3
#define UART3_TX_OVERTIME_MS        3
#define UART4_TX_OVERTIME_MS        3
#define UART5_TX_OVERTIME_MS        3
//----------接收超时(为0表示应用空闲中断)
#if   (defined(PROJECT_XKAP_V3)||defined(XKAP_ICARE_B_D_M))
#include "uctsk_RFMS.h"
#include "uctsk_BluetoothDTU.h"
#include "uctsk_GprsNet.h"
//#include "uctsk_GasModule.h"
#include "uctsk_UartIcam.h"
#define UART1_TX_BUF_SIZE           256
#define UART2_TX_BUF_SIZE           256
#define UART4_TX_BUF_SIZE           256
#define UART5_TX_BUF_SIZE           256
#define UART1_RX_BUF_SIZE           256
#define UART2_RX_BUF_SIZE           256
#define UART4_RX_BUF_SIZE           256
#define UART5_RX_BUF_SIZE           256
#define UART1_RX_OVERTIME_MS        0
#define UART2_RX_OVERTIME_MS        0
#define UART4_RX_OVERTIME_MS        0
#define UART5_RX_OVERTIME_MS        0

#if (HARDWARE_VER!=7)
#define UART3_TX_BUF_SIZE           1000/*300*/
#define UART3_RX_BUF_SIZE           1500
#define UART3_RX_OVERTIME_MS        100
#else
#define UART3_TX_BUF_SIZE           512
#define UART3_RX_BUF_SIZE           256
#define UART3_RX_OVERTIME_MS        0
#endif

#define UART1_DMA_RX_PARSE_PRO      uctsk_Rfms_RxIrq
#define UART2_DMA_RX_PARSE_PRO      BluetoothDtu_RxIrq
#if 	(HARDWARE_VER==7)
#define UART3_DMA_RX_PARSE_PRO      uctsk_UartIcam_Rx
#elif	(HARDWARE_VER==0xF1)
#include "Bsp_Esp8266.h"
#define UART3_DMA_RX_PARSE_PRO      BspEsp8266_InterruptRx
#else
#define UART3_DMA_RX_PARSE_PRO      GprsNet_InterruptRx
#endif
//#define UART4_DMA_RX_PARSE_PRO
//#define UART5_DMA_RX_PARSE_PRO
//#define UART6_DMA_RX_PARSE_PRO
#elif (defined(PROJECT_SPI_SLAVE))
#include "uctsk_SpiSlave.h"
#define UART1_TX_BUF_SIZE           256
#define UART2_TX_BUF_SIZE           256
#define UART3_TX_BUF_SIZE           256
#define UART4_TX_BUF_SIZE           256
#define UART5_TX_BUF_SIZE           256
#define UART1_RX_BUF_SIZE           256
#define UART2_RX_BUF_SIZE           256
#define UART3_RX_BUF_SIZE           256
#define UART4_RX_BUF_SIZE           256
#define UART5_RX_BUF_SIZE           256
#define UART1_RX_OVERTIME_MS        0
#define UART2_RX_OVERTIME_MS        0
#define UART3_RX_OVERTIME_MS        0
#define UART4_RX_OVERTIME_MS        0
#define UART5_RX_OVERTIME_MS        0
#define UART1_DMA_RX_PARSE_PRO      SpiSlave_UartIrq_Rx1
#define UART2_DMA_RX_PARSE_PRO      SpiSlave_UartIrq_Rx2
#define UART3_DMA_RX_PARSE_PRO      SpiSlave_UartIrq_Rx3
#define UART4_DMA_RX_PARSE_PRO      SpiSlave_UartIrq_Rx4
#define UART5_DMA_RX_PARSE_PRO      SpiSlave_UartIrq_Rx5
#elif (defined(PROJECT_ARMFLY_V5_XSL))
#include "uctsk_Debug.h"
#define UART1_TX_BUF_SIZE           256
#define UART1_RX_BUF_SIZE           256
#define UART1_RX_OVERTIME_MS        0
#define UART2_RX_OVERTIME_MS        0
#define UART3_RX_OVERTIME_MS        0
#define UART4_RX_OVERTIME_MS        0
#define UART5_RX_OVERTIME_MS        0
#define UART1_DMA_RX_PARSE_PRO      Debug_InterruptRx
//
#elif (defined(PROJECT_BIB_TEST1))
#define UART2_TX_BUF_SIZE           256
#define UART2_RX_BUF_SIZE           256
#define UART1_RX_OVERTIME_MS        0
#define UART2_RX_OVERTIME_MS        0
#define UART3_RX_OVERTIME_MS        0
#define UART4_RX_OVERTIME_MS        0
#define UART5_RX_OVERTIME_MS        0
//#define UART1_DMA_RX_PARSE_PRO      Debug_InterruptRx
//
#elif (defined(XKAP_ICARE_A_M))
//#include "uctsk_Debug.h"
#include "uctsk_GprsNet.h"
#define UART1_TX_BUF_SIZE           256
#define UART1_RX_BUF_SIZE           256
//#define UART1_TX_PIN                30
//#define UART1_RX_PIN                29
#define UART1_TX_PIN                22
#define UART1_RX_PIN                21
#define UART1_RX_OVERTIME_MS        100
#define UART1_DMA_RX_PARSE_PRO      GprsNet_InterruptRx
//
#elif (defined(XKAP_ICARE_B_M))
#include "uctsk_GprsNet.h"
#define UART1_TX_BUF_SIZE           256
#define UART1_RX_BUF_SIZE           256
#if   (HARDWARE_SUB_VER==1)
#define UART1_TX_PIN                11
#define UART1_RX_PIN                12
#elif (HARDWARE_SUB_VER==2)
#define UART1_TX_PIN                20
#define UART1_RX_PIN                19
#endif
#define UART1_RX_OVERTIME_MS        100
#define UART1_DMA_RX_PARSE_PRO      GprsNet_InterruptRx
//
#elif (defined(XKAP_ICARE_B_D))
#include "Bsp_NrfBle.h"
#define UART1_TX_BUF_SIZE           64
#define UART1_RX_BUF_SIZE           64
#define UART1_TX_PIN                5
#define UART1_RX_PIN                6
#define UART1_RX_OVERTIME_MS        100
#define UART1_DMA_RX_PARSE_PRO      BspNrfBle_ProtocolParse
//
#elif (defined(XKAP_ICARE_A_S)||defined(BASE_NRF51)||defined(BASE_NRF52))
#include "uctsk_Debug.h"
#define UART1_TX_BUF_SIZE           256
#define UART1_RX_BUF_SIZE           256
#define UART1_TX_PIN                30
#define UART1_RX_PIN                29
#define UART1_RX_OVERTIME_MS        2000
#define UART1_DMA_RX_PARSE_PRO      Debug_InterruptRx
//
#else
#define UART1_RX_OVERTIME_MS        0
#define UART2_RX_OVERTIME_MS        0
#define UART3_RX_OVERTIME_MS        0
#define UART4_RX_OVERTIME_MS        0
#define UART5_RX_OVERTIME_MS        0
//#define UART2_DMA_RX_PARSE_PRO
//#define UART3_DMA_RX_PARSE_PRO
//#define UART4_DMA_RX_PARSE_PRO
//#define UART5_DMA_RX_PARSE_PRO
//#define UART6_DMA_RX_PARSE_PRO
#endif
//-------------------接口函数--------------------------------
extern void UART_INIT(uint8_t UartX,uint32_t baudrate);
extern uint8_t UART_DMA_Tx(uint8_t ch,uint8_t *buf,uint16_t len);
extern void BspUart_DebugTestOnOff(uint8_t OnOff);
extern void UART_1ms_IRQ(void);

#if   (defined(BSP_UART0_USB_ENABLE)&&defined(STM32F1))
extern void BspUart_UsbVpcRx(void);
#endif
//-----------------------------------------------------------
#endif

