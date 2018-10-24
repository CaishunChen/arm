/*
***********************************************************************************
*                    作    者: 徐松亮
*                    更新时间: 2015-06-03
*  uCOSIII
*  FREERTOS:
*     支持抢占式调度，合作式调度和时间片调度
*     用于低功耗的Tickless模式
*     设计的简单易用，典型的内核使用大小在4k-9k
*     支持消息队列、二值信号量、计数信号量、递归信号量和互斥信号量，
*     可用于任务与任务间的消息传 递和同步，任务与中断间的消息传递和同步
*     优先级继承方式的互斥信号量
*     高效的软件定时器
*     强大的跟踪执行函数
*     堆栈溢出检查
*     任务的数量不限,任务优先级数量不限
***********************************************************************************
*/
#ifndef __MODULE_OS_H
#define __MODULE_OS_H
//-------------------加载库函数------------------------------
#include "includes.h"
//-------------------错误码定义
typedef enum MODULE_OS_ERR
{
    MODULE_OS_ERR_NONE      =  0,
    MODULE_OS_ERR_TIMEOUT,
    MODULE_OS_ERR_OTHER,
} MODULE_OS_ERR;
//-------------------接口宏定义(配置相关)--------------------
/*
#if (defined(STM32F1)||defined(STM32F4))
#include "uctsk_Debug.h"
#define MODULE_OS_DEBUG_OUT_STR(str) DebugOutStr(str)
#else
#define MODULE_OS_DEBUG_OUT_STR(str)
#endif
*/
//-------------------接口宏定义(硬件相关)--------------------
#if (defined(OS_NULL))
//---->
//头文件-LIB
//头文件-OS
//临界保护
#define MODULE_OS_DISABLE_INT
#define MODULE_OS_ENABLE_INT
//系统
#define MODULE_OS_VER(ver)    NULL
#define MODULE_OS_INIT()      NULL
#define MODULE_OS_START()     NULL
#define MODULE_OS_SYSTICK()   NULL
#define MODULE_OS_STAT()      NULL
#define MODULE_OS_GET_CURRENT_TASK_NAME_STR()      NULL
//延时
#define MODULE_OS_DELAY_MS(ms)                     	Count_SysTickDelayUs(ms*1000)		
#define MODULE_OS_DELAY_ABS_MS(ms)					Count_SysTickDelayUs(ms*1000)					
//任务
#define MODULE_OS_TASK_TAB(Name)                   NULL
#define MODULE_OS_TASK_STK(Name,Size)              NULL
#define MODULE_OS_TASK_CREATE(TaskName,FunName,Prio,pStk,StkSize,Tcb,pPara) NULL
#define MODULE_OS_TASK_LOCK_ALL						NULL
#define MODULE_OS_TASK_UNLOCK_ALL					NULL

//信号量
#define MODULE_OS_SEM(sem)    NULL
#define MODULE_OS_SEM_CREATE(sem,name,init_value)  NULL
#define MODULE_OS_SEM_POST(sem)                    NULL
#define MODULE_OS_SEM_PEND(sem,timeout,block,err)  NULL
//互斥信号量
#define MODULE_OS_MUTEX(sem)  NULL
//消息队列
#define MODULE_OS_Q(q)        NULL
#define MODULE_OS_Q_CREATE(q,name,num)             NULL
#define MODULE_OS_Q_POST(q,buf,size)               NULL
#define MODULE_OS_Q_PEND(pbuf,q,timeout,block,err) NULL
#define MODULE_OS_Q_FLUSH(q)  NULL
//缓存
#define MODULE_OS_MEM(mem)    NULL
#define MODULE_OS_MEM_INIT()  NULL
#define MODULE_OS_MEM_CREATE(mem,name,buf,nbr,size)   NULL
#define MODULE_OS_MEM_GET(mem,pbuf,err)
#define MODULE_OS_MEM_PUT(mem,pbuf,err)            NULL
//<----
#elif (defined(OS_UCOSIII))
//---->
//头文件-LIB
#include  "cpu.h"
#include  "lib_def.h"
#include  "lib_ascii.h"
#include  "lib_math.h"
#include  "lib_mem.h"
#include  "lib_str.h"
//头文件-OS
#include  "os.h"
#include  "app_os_cfg.h"
#include  "app_ucos_cfg.h"
#include  "bsp_xsl.h"
//临界保护
#define MODULE_OS_DISABLE_INT \
         {\
            CPU_SR_ALLOC();\
            CPU_CRITICAL_ENTER();\
            OSIntNestingCtr++;\
            CPU_CRITICAL_EXIT();\
         }
#define MODULE_OS_ENABLE_INT \
         {\
            OSIntExit();\
         }
//系统
#define MODULE_OS_VER(ver) \
         {\
            OS_ERR  err1;\
            ver = OSVersion(&err1);\
         }
#define MODULE_OS_INIT() \
         {\
            OS_ERR  err1;\
            OSInit(&err1);\
         }
#define MODULE_OS_START() \
         {\
            OS_ERR  err1;\
            OSStart(&err1);\
            (void)&err1;\
         }
#define MODULE_OS_SYSTICK() \
         {\
            CPU_Init();\
            BSP_Tick_Init();\
         }
#define MODULE_OS_STAT() \
         {\
            OS_ERR err1;\
            OSStatTaskCPUUsageInit(&err1);\
         }
#define MODULE_OS_GET_CURRENT_TASK_NAME_STR() (uint8_t*)(OSTCBCurPtr->NamePtr)
//延时
#define MODULE_OS_DELAY_MS(ms) \
         {\
            OS_ERR err1;\
            OSTimeDlyHMSM(ms/(60*60*1000L), (ms%(60*60*1000L))/(60*1000), (ms%(60*1000L))/1000, ms%1000,OS_OPT_TIME_HMSM_STRICT,&err1);\
         }			
#define MODULE_OS_DELAY_ABS_MS(ms) \
	 	{\
            OS_ERR err1;\
			OSTimeDly(ms,OS_OPT_TIME_PERIODIC,&err);\
        }
//任务
#define MODULE_OS_TASK_TAB(Name)       OS_TCB Name;
#define MODULE_OS_TASK_STK(Name,Size)  __align(8) static CPU_STK Name[Size];
#define MODULE_OS_TASK_CREATE(TaskName,FunName,Prio,pStk,StkSize,Tcb,pPara) \
         {\
            OS_ERR  os_err;\
            OSTaskCreate((OS_TCB       *)&Tcb,\
                     (CPU_CHAR     *)TaskName,\
                     (OS_TASK_PTR   )FunName,\
                     (void         *)0,\
                     (OS_PRIO       )Prio,\
                     (CPU_STK      *)&pStk[0],\
                     (CPU_STK_SIZE  )StkSize / 10,\
                     (CPU_STK_SIZE  )StkSize,\
                     (OS_MSG_QTY    )0,\
                     (OS_TICK       )0,\
                     (void         *)pPara,\
                     (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),\
                     (OS_ERR       *)&os_err);\
         }
#define MODULE_OS_TASK_LOCK_ALL			OSSchedLock()
#define MODULE_OS_TASK_UNLOCK_ALL		OSSchedUnlock()
//信号量
#define MODULE_OS_SEM(sem)   static OS_SEM sem;
#define MODULE_OS_SEM_CREATE(sem,name,init_value) \
         {\
            OS_ERR err1;\
            OSSemCreate(&sem,name,init_value,&err1);\
         }
#define MODULE_OS_SEM_POST(sem) \
         {\
            OS_ERR err1;\
            OSSemPost (&sem,OS_OPT_POST_1,&err1);\
         }
#define MODULE_OS_SEM_PEND(sem,timeout,block,err) \
         {\
            OS_ERR err1;\
            if(block==TRUE)\
            {\
               OSSemPend (&sem,timeout,OS_OPT_PEND_BLOCKING,NULL,&err1);\
            }\
            else\
            {\
               OSSemPend (&sem,timeout,OS_OPT_PEND_NON_BLOCKING,NULL,&err1);\
            }\
            if(OS_ERR_NONE==err1)\
            {\
               err=MODULE_OS_ERR_NONE;\
            }\
            else if(OS_ERR_TIMEOUT==err1)\
            {\
               err=MODULE_OS_ERR_TIMEOUT;\
            }\
            else\
            {\
               err=MODULE_OS_ERR_OTHER;\
            }\
         }
//互斥信号量
#define MODULE_OS_MUTEX(sem)  static OS_MUTEX sem;
//消息队列
#define MODULE_OS_Q(q)     OS_Q q;
#define MODULE_OS_Q_CREATE(q,name,num) \
         {\
            OS_ERR err1;\
            OSQCreate (&q,name,num,&err1);\
         }
#define MODULE_OS_Q_POST(q,buf,size) \
         {\
            OS_ERR err1;\
            OSQPost ((OS_Q  *)&q,\
                 (void  *)&buf,\
                 (OS_MSG_SIZE)size,\
                 (OS_OPT )OS_OPT_POST_FIFO,\
                 (OS_ERR*)&err1);\
         }
#define MODULE_OS_Q_PEND(pbuf,q,timeout,block,err) \
         {\
            OS_ERR err1;\
            OS_MSG_SIZE msg_size;\
            CPU_TS ts;\
            if(block==TRUE)\
            {\
               pbuf = OSQPend (&q,timeout,OS_OPT_PEND_BLOCKING,&msg_size,&ts,&err1);\
            }\
            else\
            {\
               pbuf = OSQPend (&q,timeout,OS_OPT_PEND_NON_BLOCKING,&msg_size,&ts,&err1);\
            }\
            if(OS_ERR_NONE==err1)\
            {\
               err=MODULE_OS_ERR_NONE;\
            }\
            else if(OS_ERR_TIMEOUT==err1)\
            {\
               err=MODULE_OS_ERR_TIMEOUT;\
            }\
            else\
            {\
               err=MODULE_OS_ERR_OTHER;\
            }\
         }
#define MODULE_OS_Q_FLUSH(q) \
         {\
            OS_ERR  err1;\
            OSQFlush(&q,&err1);\
         }
//缓存
#define MODULE_OS_MEM(mem)   static OS_MEM mem;
#define MODULE_OS_MEM_INIT()  Mem_Init();
#define MODULE_OS_MEM_CREATE(mem,name,buf,nbr,size) \
         {\
            OS_ERR err1;\
            OSMemCreate(&mem,name,buf,nbr,size,&err1);\
         }
#define MODULE_OS_MEM_GET(mem,pbuf,err) \
         {\
            OS_ERR err1;\
            pbuf = OSMemGet(&mem,&err1);\
            if(OS_ERR_NONE==err1)\
            {\
               err=MODULE_OS_ERR_NONE;\
            }\
            else\
            {\
               err=MODULE_OS_ERR_OTHER;\
            }\
         }
#define MODULE_OS_MEM_PUT(mem,pbuf,err) \
         {\
            OS_ERR err1;\
            OSMemPut(&mem,pbuf,&err1);\
            if(OS_ERR_NONE==err1)\
            {\
               err=MODULE_OS_ERR_NONE;\
            }\
            else\
            {\
               err=MODULE_OS_ERR_OTHER;\
            }\
         }
//<----
#elif (defined(OS_FREERTOS))
//---->
//头文件-LIB
/*
#include  "lib_def.h"
#include  "lib_ascii.h"
#include  "lib_math.h"
#include  "lib_mem.h"
#include  "lib_str.h" 
*/
//头文件-OS
#include  "FreeRTOS.h"
#include  "task.h"
#include  "queue.h"
#include  "croutine.h"
#include  "semphr.h"
#include  "app_os_cfg.h"
//临界保护
//---任务使用
#define MODULE_OS_DISABLE_INT_TASK	taskENTER_CRITICAL()
#define MODULE_OS_ENABLE_INT_TASK	taskEXIT_CRITICAL()
//---精简版(不支持中断嵌套)
//#define MODULE_OS_DISABLE_INT 		taskDISABLE_INTERRUPTS()
//#define MODULE_OS_ENABLE_INT  		taskENABLE_INTERRUPTS()
//---中断使用
#define MODULE_OS_DISABLE_INT		UBaseType_t x;x=taskENTER_CRITICAL_FROM_ISR()
#define MODULE_OS_ENABLE_INT		taskEXIT_CRITICAL_FROM_ISR(x)
//系统
#define MODULE_OS_VER(ver) \
         {\
                      ver = tskKERNEL_VERSION_MAJOR*256*256+tskKERNEL_VERSION_MINOR*256+tskKERNEL_VERSION_BUILD;\
         }
#define MODULE_OS_INIT() \
         {\
            ;\
         }
#define MODULE_OS_START() \
         {\
            vTaskStartScheduler();\
         }
#define MODULE_OS_SYSTICK() \
         {\
            ;\
         }
#define MODULE_OS_STAT() \
         {\
            ;\
         }
#define MODULE_OS_GET_CURRENT_TASK_NAME_STR() pcTaskGetTaskName(xTaskGetCurrentTaskHandle())
//延时
#define MODULE_OS_DELAY_MS(ms) \
		{\
            vTaskDelay(ms);\
         }
#define MODULE_OS_DELAY_ABS_MS(ms) \
	 	{\
            TickType_t xLastWakeTime=0;\
			vTaskDelayUntil(&xLastWakeTime,ms);\
        }

//任务
#define MODULE_OS_TASK_TAB(Name)       static TaskHandle_t Name=NULL;
#define MODULE_OS_TASK_STK(Name,Size)  ;
#define MODULE_OS_TASK_CREATE(TaskName,FunName,Prio,pStk,StkSize,Tcb,pPara) \
         {\
         	xTaskCreate(FunName,\
                        TaskName,\
                        StkSize,\
                     	pPara,\
                     	Prio,\
                     	&Tcb);\
                 }
#define MODULE_OS_TASK_LOCK_ALL		vTaskSuspendAll()
#define MODULE_OS_TASK_UNLOCK_ALL	xTaskResumeAll()
//信号量
#define MODULE_OS_SEM(sem)   static SemaphoreHandle_t sem = NULL;
#define MODULE_OS_SEM_CREATE(sem,name,init_value) \
         {\
            sem = xSemaphoreCreateCounting(10, init_value);\
         }
#define MODULE_OS_SEM_POST(sem) \
         {\
            xSemaphoreGive(sem);\
         }
#define MODULE_OS_SEM_PEND(sem,timeout,block,err) \
         {\
            BaseType_t err1;\
            err1  =  xSemaphoreTake(sem, pdMS_TO_TICKS(timeout));\
            if(err1==pdPASS)\
            {\
               err = MODULE_OS_ERR_NONE;\
            }\
            else if(errQUEUE_EMPTY==err1)\
            {\
               err = MODULE_OS_ERR_TIMEOUT;\
            }\
            else\
            {\
               err = MODULE_OS_ERR_OTHER;\
            }\
         }
//互斥信号量
#define MODULE_OS_MUTEX(sem) static SemaphoreHandle_t sem=NULL;
#define MODULE_OS_MUTEX_CREATE(sem,name,init_value) \
         {\
            sem = xSemaphoreCreateMutex();\
         }
//消息队列
#define MODULE_OS_Q(q)       QueueHandle_t q;
#define MODULE_OS_Q_CREATE(q,name,num) \
         {\
            q = xQueueCreate(num, sizeof(uint8_t *));\
         }
#define MODULE_OS_Q_POST(q,buf,size) \
         {\
            uint32_t i32 = (uint32_t)&buf;\
            xQueueSend(q,\
                   (void *) &i32,\
                   (TickType_t)10);\
         }
#define MODULE_OS_Q_PEND(pbuf,q,timeout,block,err) \
         {\
            BaseType_t err1;\
            uint32_t i32;\
            err1 = xQueueReceive(q,\
                   (void *)&i32,\
                   (TickType_t)timeout);\
            pbuf = (void *)i32; \
            if(err1==pdPASS)\
            {\
               err = MODULE_OS_ERR_NONE;\
            }\
            else if(errQUEUE_EMPTY==err1)\
            {\
               err = MODULE_OS_ERR_TIMEOUT;\
            }\
            else\
            {\
               err = MODULE_OS_ERR_OTHER;\
            }\
         }
#define MODULE_OS_Q_FLUSH(q) \
         {\
            xQueueReset(q);\
         }
//缓存
#define MODULE_OS_MEM(mem)    NULL
#define MODULE_OS_MEM_INIT()    NULL
#define MODULE_OS_MEM_CREATE(mem,name,buf,nbr,size)   NULL
#define MODULE_OS_MEM_GET(mem,pbuf,err)               NULL
#define MODULE_OS_MEM_PUT(mem,pbuf,err)               NULL
//<----
#endif
//-------------------接口宏定义(硬件无关)--------------------
//-------------------接口变量--------------------------------
//-------------------接口函数--------------------------------
extern void Module_OS_Info_DebugTestOnOff(uint8_t OnOff);
//-----------------------------------------------------------
#endif

