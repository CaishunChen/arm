/**
  ******************************************************************************
  * @file    main.c 
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
/* Includes ------------------------------------------------------------------*/
#include "includes.h"
#include "main.h"
#include "Bsp_CpuFlash.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
MODULE_OS_TASK_TAB(AppTaskStartTCB);
MODULE_OS_TASK_STK(AppTaskStartStk,APP_CFG_TASK_START_STK_SIZE);
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static  void  App_TaskStart  (void *p_arg);

/**
 * @brief   主函数入口
 * @note    初始化时钟，并建立start任务
 * @param   None
 * @return  None
 */
int32_t main (void)
{
    // 设置中断向量
    BSP_CPU_FLASH_SET_VECTOR_TABLE;
    // 启动时钟
    MODULE_OS_SYSTICK();
    // 系统初始化
    MODULE_OS_INIT();
    // 创建Start任务
    MODULE_OS_TASK_CREATE("Task-Start",\
                          App_TaskStart,\
                          APP_CFG_TASK_START_PRIO,\
                          AppTaskStartStk,\
                          APP_CFG_TASK_START_STK_SIZE,\
                          AppTaskStartTCB,\
                          NULL);
    // 启动任务
    MODULE_OS_START();
    return (0);
}
/**
 * @brief   start任务实体
 * @note    初始化各种模块和任务，并实现事件处理
 * @param   None
 * @return  None
 */
static  void  App_TaskStart (void *p_arg)
{
    (void)p_arg;
    //PWR初始化
    Bsp_Pwr_Init();
    //初始化Flash
    Module_Memory_Init();
    //算法初始化
    Count_Init();
    //RTC初始化
    BspRtc_Init();
    //
#ifdef   MAIN_BSP_INIT_1
    MAIN_BSP_INIT_1;
#endif
#ifdef   MAIN_BSP_INIT_2
    MAIN_BSP_INIT_2;
#endif
#ifdef   MAIN_BSP_INIT_3
    MAIN_BSP_INIT_3;
#endif
#ifdef   MAIN_BSP_INIT_4
    MAIN_BSP_INIT_4;
#endif
#ifdef   MAIN_BSP_INIT_5
    MAIN_BSP_INIT_5;
#endif
#ifdef   MAIN_BSP_INIT_6
    MAIN_BSP_INIT_6;
#endif
#ifdef   MAIN_BSP_INIT_7
    MAIN_BSP_INIT_7;
#endif
#ifdef   MAIN_BSP_INIT_8
    MAIN_BSP_INIT_8;
#endif
#ifdef   MAIN_BSP_INIT_9
    MAIN_BSP_INIT_9;
#endif
#ifdef   MAIN_BSP_INIT_10
    MAIN_BSP_INIT_10;
#endif
#ifdef   MAIN_BSP_INIT_11
    MAIN_BSP_INIT_11;
#endif
#ifdef   MAIN_BSP_INIT_12
    MAIN_BSP_INIT_12;
#endif
#ifdef   MAIN_BSP_INIT_13
    MAIN_BSP_INIT_13;
#endif
#ifdef   MAIN_BSP_INIT_14
    MAIN_BSP_INIT_14;
#endif
#ifdef   MAIN_BSP_INIT_15
    MAIN_BSP_INIT_15;
#endif
#ifdef   MAIN_BSP_INIT_16
    MAIN_BSP_INIT_16;
#endif
#ifdef   MAIN_BSP_INIT_17
    MAIN_BSP_INIT_17;
#endif
#ifdef   MAIN_BSP_INIT_18
    MAIN_BSP_INIT_18;
#endif
#ifdef   MAIN_BSP_INIT_19
    MAIN_BSP_INIT_19;
#endif
#ifdef   MAIN_BSP_INIT_20
    MAIN_BSP_INIT_20;
#endif
    //-----创建系统状态任务
    MODULE_OS_STAT();
    //-----创建应用任务
    App_DebugTaskCreate();
    //
#ifdef MAIN_TASK_1
    MAIN_TASK_1;
#endif
#ifdef MAIN_TASK_2
    MAIN_TASK_2;
#endif
#ifdef MAIN_TASK_3
    MAIN_TASK_3;
#endif
#ifdef MAIN_TASK_4
    MAIN_TASK_4;
#endif
#ifdef MAIN_TASK_5
    MAIN_TASK_5;
#endif
#ifdef MAIN_TASK_6
    MAIN_TASK_6;
#endif
#ifdef MAIN_TASK_7
    MAIN_TASK_7;
#endif
#ifdef MAIN_TASK_8
    MAIN_TASK_8;
#endif
#ifdef MAIN_TASK_9
    MAIN_TASK_9;
#endif
#ifdef MAIN_TASK_10
    MAIN_TASK_10;
#endif
#ifdef MAIN_TASK_11
    MAIN_TASK_11;
#endif
#ifdef MAIN_TASK_12
    MAIN_TASK_12;
#endif
#ifdef MAIN_TASK_13
    MAIN_TASK_13;
#endif
#ifdef MAIN_TASK_14
    MAIN_TASK_14;
#endif
#ifdef MAIN_TASK_15
    MAIN_TASK_15;
#endif
#ifdef MAIN_TASK_16
    MAIN_TASK_16;
#endif
#ifdef MAIN_TASK_17
    MAIN_TASK_17;
#endif
#ifdef MAIN_TASK_18
    MAIN_TASK_18;
#endif
#ifdef MAIN_TASK_19
    MAIN_TASK_19;
#endif
#ifdef MAIN_TASK_20
    MAIN_TASK_20;
#endif
    //
    for(;;)
    {
        BspPwr_Cmd_parse();
    }
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

