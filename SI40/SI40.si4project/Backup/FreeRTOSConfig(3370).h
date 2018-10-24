/*
    FreeRTOS V8.0.1 - Copyright (C) 2014 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that has become a de facto standard.             *
     *                                                                       *
     *    Help yourself get started quickly and support the FreeRTOS         *
     *    project by purchasing a FreeRTOS tutorial book, reference          *
     *    manual, or both from: http://www.FreeRTOS.org/Documentation        *
     *                                                                       *
     *    Thank you!                                                         *
     *                                                                       *
    ***************************************************************************

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.

    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available from the following
    link: http://www.freertos.org/a00114.html

    1 tab == 4 spaces!

    ***************************************************************************
     *                                                                       *
     *    Having a problem?  Start by reading the FAQ "My application does   *
     *    not run, what could be wrong?"                                     *
     *                                                                       *
     *    http://www.FreeRTOS.org/FAQHelp.html                               *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org - Documentation, books, training, latest versions,
    license and Real Time Engineers Ltd. contact details.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.OpenRTOS.com - Real Time Engineers ltd license FreeRTOS to High
    Integrity Systems to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/


#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#ifdef SOFTDEVICE_PRESENT
#include "nrf_soc.h"
#endif

/*-----------------------------------------------------------
 * Possible configurations for system timer
 */
#define FREERTOS_USE_RTC      0 /**< Use real time clock for the system */
#define FREERTOS_USE_SYSTICK  1 /**< Use SysTick timer for system */

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html.
 *----------------------------------------------------------*/

#define configTICK_SOURCE                                                       FREERTOS_USE_RTC    /* 用RTC做时间片驱动 */

#define configUSE_PREEMPTION                                                    1        			/*0-使能合作式调度器    1-使能抢占式调度器*/
#if (defined(NRF52))
#define configUSE_PORT_OPTIMISED_TASK_SELECTION                                 1               	/*优化算法  	1-专用    部分平台支持 效率高    优先级数量限制为32个 */
#else
#define configUSE_PORT_OPTIMISED_TASK_SELECTION                                 0               	/*优化算法      0-通用    纯C编写 比专用模式效率低 优先级数量无限制*/
#endif
#define configUSE_TICKLESS_IDLE                                                 1               	/*低功耗模式   1-使能*/
#define configUSE_TICKLESS_IDLE_SIMPLE_DEBUG                                    1             		/* See into vPortSuppressTicksAndSleep source code for explanation */
#define configCPU_CLOCK_HZ                                                      ( SystemCoreClock )	/*系统主频*/
#define configTICK_RATE_HZ                                                      (1000)    			/*系统时钟节拍*/
#define configMAX_PRIORITIES                                                    (20)      			/*最大优先级 e.g:10(0-9)*/
#define configMINIMAL_STACK_SIZE                                                (30+30)   			/* IDLE 空闲任务的堆栈大小 单位-4B*/

#if (defined(XKAP_ICARE_B_M)&&!defined(HAIER))
//#define configTOTAL_HEAP_SIZE                                                   (4096+2048)  		/*定义堆大小,内核/用户动态内存申请、任务堆栈都将使用*/
#define configTOTAL_HEAP_SIZE                                                   (4096+2048+1024)  		/*定义堆大小,内核/用户动态内存申请、任务堆栈都将使用*/

#else
#define configTOTAL_HEAP_SIZE                                                   (4096)    			/*定义堆大小,内核/用户动态内存申请、任务堆栈都将使用*/
#endif

#define configMAX_TASK_NAME_LEN                                                 (20)      			/*任务名称最大字符数(包括末尾\0)*/
#define configUSE_16_BIT_TICKS                                                  0             		/*1-系统时钟节拍计数使用16bit数据类型 对于32位CPU 固定为0*/
#define configIDLE_SHOULD_YIELD                                                 0             		/*1-使能与空闲任务同优先级任务 建议配置为0*/
#define configUSE_MUTEXES                                                       1            		/*1-使能互斥信号量*/
#define configUSE_RECURSIVE_MUTEXES                                             1             		/*1-使能递归互斥信号量*/
#define configUSE_COUNTING_SEMAPHORES                                           1             		/*1-使能计数信号量*/
#define configUSE_ALTERNATIVE_API                                               0         			/* Deprecated! */
#define configQUEUE_REGISTRY_SIZE                                               2             		/*可以注册的信号量和消息队列个数*/
#define configUSE_QUEUE_SETS                                                    1             		/*1-使能消息队列*/
#define configUSE_TIME_SLICING                                                  1             		/*1-使能时间片调度*/
#define configUSE_NEWLIB_REENTRANT                                              0             		/*1-每个任务创建的时候分配Newlib的重入结构体*/
#define configENABLE_BACKWARD_COMPATIBILITY                                     0             		/*1-新版本对老版本的兼容性*/

/* 回调函数 Hook function related definitions. */
#define configUSE_IDLE_HOOK                                                     1             		/*1-使能空闲任务的钩子函数*/
#define configUSE_TICK_HOOK                                                     0             		/*1-使能滴答定时器中断里面执行的钩子函数*/
#define configCHECK_FOR_STACK_OVERFLOW                                          1             		/*0-禁止栈溢出监测   1-栈溢出监测使用方法1  2-栈溢出监测使用方法2*/
#define configUSE_MALLOC_FAILED_HOOK                                            1

/* Run time and task stats gathering related definitions. */
#define configGENERATE_RUN_TIME_STATS                                           1             		/*1-使能任务运行状态参数统计*/
#define configUSE_TRACE_FACILITY                                                1             		/*1-添加额外结构体成员和函数来协助可视化跟踪(IAR的FREERTOS插件要使用这个配置)*/
#define configUSE_STATS_FORMATTING_FUNCTIONS                                    1             		/*与configUSE_TRACE_FACILITY共同配置为1时,相关功能才被启用*/
/* -----XSL-----ADD-Begin*/
#include <stdint.h>
extern uint32_t ulHighFrequencyTimerTicks;
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()    							(ulHighFrequencyTimerTicks  =   0ul)
#define portGET_RUN_TIME_COUNTER_VALUE()                    					ulHighFrequencyTimerTicks
/* -----XSL-----ADD-End*/

/* Co-routine definitions. */                                                                       /*合作调度器配置*/
#define configUSE_CO_ROUTINES                                                   0             		/*1-使能合作式调度相关函数*/
#define configMAX_CO_ROUTINE_PRIORITIES                                         ( 2 )     			/*最大的合作式任务优先级数*/

/* Software timer definitions. */                                                                   /*软件定时器配置*/
#define configUSE_TIMERS                                                        1               	/*使能软件定时器*/
#define configTIMER_TASK_PRIORITY                                               ( 2 )     			/*配置软件定时器任务的优先级*/
#define configTIMER_QUEUE_LENGTH                                                32            		/*配置软件定时器命令队列的长度*/

#if		(defined(XKAP_ICARE_B_M)&&(!defined(HAIER)))
#define  configTIMER_TASK_STACK_DEPTH    										(64u)				/*配置软件定时器任务的栈空间*/
#else
#define  configTIMER_TASK_STACK_DEPTH    										(100u)				/*配置软件定时器任务的栈空间*/
#endif

/* Tickless Idle configuration. */
#define configEXPECTED_IDLE_TIME_BEFORE_SLEEP                                   2

/* Tickless idle/low power functionality. */


/* Define to trap errors during development. */                                                     /*断言配置*/
#if defined(DEBUG_NRF) || defined(DEBUG_NRF_USER)
#define configASSERT( x )                                                       ASSERT(x)
#else
//#define configASSERT( x )                                                       if((x)==0){taskDISABLE_INTERRUPTS();for(;;);}
#endif

/* FreeRTOS MPU specific definitions. */
#define configINCLUDE_APPLICATION_DEFINED_PRIVILEGED_FUNCTIONS                  1

/* Optional functions - most linkers will remove unused functions anyway. */
#define INCLUDE_vTaskPrioritySet                                                1
#define INCLUDE_uxTaskPriorityGet                                               1
#define INCLUDE_vTaskDelete                                                     1
#define INCLUDE_vTaskSuspend                                                    1
#define INCLUDE_xResumeFromISR                                                  1
#define INCLUDE_vTaskDelayUntil                                                 1
#define INCLUDE_vTaskDelay                                                      1
#define INCLUDE_xTaskGetSchedulerState                                          1
#define INCLUDE_xTaskGetCurrentTaskHandle                                       1
#define INCLUDE_uxTaskGetStackHighWaterMark                                     1
#define INCLUDE_xTaskGetIdleTaskHandle                                          1
#define INCLUDE_xTimerGetTimerDaemonTaskHandle                                  1
#define INCLUDE_pcTaskGetTaskName                                               1
#define INCLUDE_eTaskGetState                                                   1
#define INCLUDE_xEventGroupSetBitFromISR                                        1
#define INCLUDE_xTimerPendFunctionCall                                          1

/*-----------------------------------------------------------
 * Settings that are generated automatically
 * basing on the settings above
 */
#if (configTICK_SOURCE == FREERTOS_USE_SYSTICK)
// do not define configSYSTICK_CLOCK_HZ for SysTick to be configured automatically
// to CPU clock source
#define xPortSysTickHandler     SysTick_Handler
#elif (configTICK_SOURCE == FREERTOS_USE_RTC)
#define configSYSTICK_CLOCK_HZ  ( 32768UL )
#define xPortSysTickHandler     RTC1_IRQHandler
#else
#error  Unsupported configTICK_SOURCE value
#endif

/* Code below should be only used by the compiler, and not the assembler. */
#if !(defined(__ASSEMBLY__) || defined(__ASSEMBLER__))
#include "nrf.h"
#include "nrf_assert.h"

/* This part of definitions may be problematic in assembly - it uses definitions from files that are not assembly compatible. */
/* Cortex-M specific definitions. */
#ifdef __NVIC_PRIO_BITS
/* __BVIC_PRIO_BITS will be specified when CMSIS is being used. */
#define configPRIO_BITS             __NVIC_PRIO_BITS
#elif	(defined(STM32F1)||defined(STM32F4))
#define configPRIO_BITS             4																/*STM32的中断优先级分组为组4，即15级抢占式优先级*/
#else
#error "This port requires __NVIC_PRIO_BITS to be defined"
#endif

/* Access to current system core clock is required only if we are ticking the system by systimer */
#if (configTICK_SOURCE == FREERTOS_USE_SYSTICK)
#include <stdint.h>
extern uint32_t SystemCoreClock;
#endif
#endif /* !assembler */

//-------------------------------------------------------------------------------中断
/* The lowest interrupt priority that can be used in a call to a "set priority"
function. */
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY         						0xf					/*最低优先级，用于SysTick与PendSV中断优先级*/

/* The highest interrupt priority that can be used by any interrupt service
routine that makes calls to interrupt safe FreeRTOS API functions.  DO NOT CALL
INTERRUPT SAFE FREERTOS API FUNCTIONS FROM ANY INTERRUPT THAT HAS A HIGHER
PRIORITY THAN THIS! (higher priorities are lower numeric values. */
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY    						1					/*最高优先级，抢占式优先级0不允许调用freeRTOS的API函数*/


#if		(defined(NRF51)||defined(NRF52))
/* Interrupt priorities used by the kernel port layer itself.  These are generic
to all Cortex-M ports, and do not rely on any particular library functions. */						/*内核中断优先级*/
#define configKERNEL_INTERRUPT_PRIORITY                 						configLIBRARY_LOWEST_INTERRUPT_PRIORITY
/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */												/*用于开关中断*/
#define configMAX_SYSCALL_INTERRUPT_PRIORITY            						configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY
#elif	(defined(STM32F1)||defined(STM32F4))
#define configKERNEL_INTERRUPT_PRIORITY                 						(configLIBRARY_LOWEST_INTERRUPT_PRIORITY<<(8-configPRIO_BITS))
#define configMAX_SYSCALL_INTERRUPT_PRIORITY            						(configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY<<(8-configPRIO_BITS))
#endif

//-------------------------------------------------------------------------------
/* Definitions that map the FreeRTOS port interrupt handlers to their CMSIS
standard names - or at least those used in the unmodified vector table. */

#define vPortSVCHandler                                                         SVC_Handler
#define xPortPendSVHandler                                                      PendSV_Handler


#endif /* FREERTOS_CONFIG_H */
