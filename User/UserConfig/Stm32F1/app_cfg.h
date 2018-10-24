/****************************************Copyright (c)****************************************************
** File name:               app_cfg.h
** Descriptions:            ucosii configuration
*********************************************************************************************************/
#ifndef  __APP_CFG_H__
#define  __APP_CFG_H__
/********************************************************************************************************
*                                       MODULE ENABLE / DISABLE
********************************************************************************************************/
#define  OS_VIEW_MODULE                  DEF_DISABLED            /* DEF_ENABLED = Present, DEF_DISABLED = Not Present        */
/********************************************************************************************************
*                                           缓存池容量定义
********************************************************************************************************/
#define  LIB_MEM_CFG_ARG_CHK_EXT_EN     DEF_ENABLED
#define  LIB_MEM_CFG_OPTIMIZE_ASM_EN    DEF_ENABLED
#define  LIB_MEM_CFG_ALLOC_EN           DEF_ENABLED
#define  LIB_MEM_CFG_HEAP_SIZE           (8*128)
/********************************************************************************************************
*                                           任务名称
********************************************************************************************************/
/********************************************************************************************************
*                                           任务优先级
********************************************************************************************************/
#define  APP_TASK_START_PRIO                               3   //启动任务
#define  APP_TASK_OSVIEW_TERMINAL_PRIO   (OS_LOWEST_PRIO - 20) //
#define  OS_VIEW_TASK_PRIO               (OS_LOWEST_PRIO - 19)  //VIEW任务
#define  OS_TASK_TMR_PRIO                (OS_LOWEST_PRIO - 2)  //定时器任务

//#define  APP_TASK_RF_PRIO                (OS_LOWEST_PRIO - 6)  //RF任务
#define  APP_TASK_SDFAT_PRIO             (OS_LOWEST_PRIO - 7)  //SDFAT任务
#define  APP_TASK_USERTIMER_10MS_PRIO    (OS_LOWEST_PRIO - 5)  //KIN任务
#define  APP_TASK_USERTIMER_100MS_PRIO   (OS_LOWEST_PRIO - 4)  //KIN任务
//#define  APP_TASK_CAN_PRIO               (OS_LOWEST_PRIO - 13) //CAN任务
#define  APP_TASK_MODBUS_PRIO            (OS_LOWEST_PRIO - 14) //MODBUS任务
#define  APP_TASK_AD_PRIO                (OS_LOWEST_PRIO - 15) //AD任务
#define  APP_TASK_W5500APP_PRIO          (OS_LOWEST_PRIO - 12) //W5500APP任务
#define  APP_TASK_GPRSAPPXKAP_PRIO       (OS_LOWEST_PRIO - 12)  //GPRS-XKAP任务
#define  APP_TASK_GSM_PARSE_PRIO         (OS_LOWEST_PRIO - 17) //GSM模块M10的AT指令解析任务
#define  APP_TASK_GSM_SEND_PRIO          (OS_LOWEST_PRIO - 18) //GSM模块发送任务
#define  APP_TASK_GSM_TEST_PRIO          (OS_LOWEST_PRIO - 19) //GSM模块测试数据接收任务
#define  APP_TASK_GSM_SUBSECTION_PRIO    (OS_LOWEST_PRIO - 20) //GSM模块分段任务
#define  APP_TASK_SPI_PRIO               (OS_LOWEST_PRIO - 8)  //SPI任务
#define  APP_TASK_HCI_PRIO               (OS_LOWEST_PRIO - 9)  //HCI任务
#define  APP_TASK_SENSOR_PRIO            (OS_LOWEST_PRIO - 10) //Sensor任务
//#define  APP_TASK_RFMS_PRIO              (OS_LOWEST_PRIO - 11) //Rfms任务
#define  APP_TASK_BLUETOOTH_PRIO         (OS_LOWEST_PRIO - 6)  //蓝牙任务
#define  APP_TASK_COMMPC_PRIO            (OS_LOWEST_PRIO - 13) //PC通讯任务
#define  APP_TASK_GASMODULE_PRIO         (OS_LOWEST_PRIO - 11)  //蓝牙任务
#define  APP_TASK_UARTICAM_PRIO          (OS_LOWEST_PRIO - 13) //PC通讯任务
/********************************************************************************************************
*                                           任务堆栈设定(# of OS_STK entries)
********************************************************************************************************/
#define  APP_TASK_START_STK_SIZE                          128u //启动任务
#define  APP_TASK_OSVIEW_TERMINAL_STK_SIZE                64u  //
#define  OS_VIEW_TASK_STK_SIZE                            64u  //VIEW任务

#define  APP_TASK_RF_STK_SIZE                             128u //RF任务
#define  APP_TASK_SDFAT_STK_SIZE                          256u //SDFAT任务
#define  APP_TASK_USERTIMER_10MS_STK_SIZE                 128u //开关量输入任务
#define  APP_TASK_USERTIMER_100MS_STK_SIZE                128u //开关量输入任务
#define  APP_TASK_CAN_STK_SIZE                            128u //维尔数码公司应用程序
#define  APP_TASK_MODBUS_STK_SIZE                         128u //维尔数码公司应用程序
#define  APP_TASK_AD_STK_SIZE                             128u  //AD任务
#define  APP_TASK_W5500APP_STK_SIZE                       128u //开关量输入任务
#define  APP_TASK_GSM_SUBSECTION_STK_SIZE                 128u //GSM模块分段任务
#define  APP_TASK_GSM_PARSE_STK_SIZE                      128u //GSM模块解析任务
#define  APP_TASK_GSM_SEND_STK_SIZE                       128u //GSM模块发送任务
#define  APP_TASK_GSM_TEST_STK_SIZE                       128u //GSM模块测试任务
#define  APP_TASK_SPI_STK_SIZE                            128u //
#define  APP_TASK_HCI_STK_SIZE                            128u //人机界面任务
#define  APP_TASK_SENSOR_STK_SIZE                         64u  //传感器采集任务
//#define  APP_TASK_RFMS_STK_SIZE                           64u  //RFMS
#define  APP_TASK_BLUETOOTH_STK_SIZE                      64u  //蓝牙任务
#define  APP_TASK_GPRSAPPXKAP_STK_SIZE                    128u  //GPRS熙康任务
#define  APP_TASK_COMMPC_STK_SIZE	   			         (128u)
#define  APP_TASK_GASMODULE_STK_SIZE                      64u  //气体传感模块  
#define  APP_TASK_UARTICAM_STK_SIZE	   			         (128u)
/********************************************************************************************************
*                                           库
********************************************************************************************************/
#define  uC_CFG_OPTIMIZE_ASM_EN                 DEF_ENABLED
#define  LIB_STR_CFG_FP_EN                      DEF_DISABLED

#endif
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

