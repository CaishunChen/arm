/*
***********************************************************************************
*                    作    者: 徐松亮
*                    更新时间: 2015-06-03
***********************************************************************************
*/
//================================
#include "includes.h"
#include "uctsk_UserTimer.h"
//================================
//------------------------------- 用户变量 --------------------------
MODULE_OS_TASK_TAB(App_TaskUserTimer10msTCB);
MODULE_OS_TASK_STK(App_TaskUserTimer10msStk,APP_TASK_USERTIMER_10MS_STK_SIZE);
MODULE_OS_TASK_TAB(App_TaskUserTimer100msTCB);
MODULE_OS_TASK_STK(App_TaskUserTimer100msStk,APP_TASK_USERTIMER_100MS_STK_SIZE);
//
static void uctsk_UserTimer10ms(void *pvParameters);
static void uctsk_UserTimer100ms(void *pvParameters);
/*******************************************************************************
函数功能: AD任务创建
*******************************************************************************/
void  App_UserTimer10msTaskCreate (void)
{
    MODULE_OS_TASK_CREATE("Task-UserTimer10ms",
                          uctsk_UserTimer10ms,
                          APP_TASK_USERTIMER_10MS_PRIO,
                          App_TaskUserTimer10msStk,
                          APP_TASK_USERTIMER_10MS_STK_SIZE,
                          App_TaskUserTimer10msTCB,
                          NULL);
}
/*******************************************************************************
函数功能: 定时任务实体
*******************************************************************************/
static void uctsk_UserTimer10ms (void *pvParameters)
{
#if (defined(NRF51)||defined(NRF52))
    MODULE_OS_DELAY_MS(3000);
#endif
    for(;;)
    {
        MODULE_OS_DELAY_MS(10);
        //-----------------------
#ifdef   USER_TIMER_10MS_CH1
        USER_TIMER_10MS_CH1;
#endif
#ifdef   USER_TIMER_10MS_CH2
        USER_TIMER_10MS_CH2;
#endif
#ifdef   USER_TIMER_10MS_CH3
        USER_TIMER_10MS_CH3;
#endif
#ifdef   USER_TIMER_10MS_CH4
        USER_TIMER_10MS_CH4;
#endif
#ifdef   USER_TIMER_10MS_CH5
        USER_TIMER_10MS_CH5;
#endif
#ifdef   USER_TIMER_10MS_CH6
        USER_TIMER_10MS_CH6;
#endif
#ifdef   USER_TIMER_10MS_CH7
        USER_TIMER_10MS_CH7;
#endif
#ifdef   USER_TIMER_10MS_CH8
        USER_TIMER_10MS_CH8;
#endif
#ifdef   USER_TIMER_10MS_CH9
        USER_TIMER_10MS_CH9;
#endif
#ifdef   USER_TIMER_10MS_CH10
        USER_TIMER_10MS_CH10;
#endif
    }
}
/*******************************************************************************
函数功能: AD任务创建
*******************************************************************************/
void  App_UserTimer100msTaskCreate (void)
{
    MODULE_OS_TASK_CREATE("Task-Timer100ms",
                          uctsk_UserTimer100ms,
                          APP_TASK_USERTIMER_100MS_PRIO,
                          App_TaskUserTimer100msStk,
                          APP_TASK_USERTIMER_100MS_STK_SIZE,
                          App_TaskUserTimer100msTCB,
                          NULL);
}
/*******************************************************************************
函数功能: 定时任务实体
*******************************************************************************/
static void uctsk_UserTimer100ms (void *pvParameters)
{
    //测试
    for(;;)
    {
        MODULE_OS_DELAY_MS(100);
#ifdef   USER_TIMER_100MS_CH1
        USER_TIMER_100MS_CH1;
#endif
#ifdef   USER_TIMER_100MS_CH2
        USER_TIMER_100MS_CH2;
#endif
#ifdef   USER_TIMER_100MS_CH3
        USER_TIMER_100MS_CH3;
#endif
#ifdef   USER_TIMER_100MS_CH4
        USER_TIMER_100MS_CH4;
#endif
#ifdef   USER_TIMER_100MS_CH5
        USER_TIMER_100MS_CH5;
#endif
#ifdef   USER_TIMER_100MS_CH6
        USER_TIMER_100MS_CH6;
#endif
#ifdef   USER_TIMER_100MS_CH7
        USER_TIMER_100MS_CH7;
#endif
#ifdef   USER_TIMER_100MS_CH8
        USER_TIMER_100MS_CH8;
#endif
#ifdef   USER_TIMER_100MS_CH9
        USER_TIMER_100MS_CH9;
#endif
#ifdef   USER_TIMER_100MS_CH10
        USER_TIMER_100MS_CH10;
#endif
    }
}

