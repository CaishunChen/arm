/*
*********************************************************************************************************
*
*   模块名称 : 应用配置函数
*   文件名称 : app_cfg.c
*   版    本 : V1.0
*   说    明 : 主要用于任务堆栈和优先级的配置，以及uC/SERIAL配置。
*   修改记录 :
*       版本号    日期          作者              说明
*       v1.0    2013-03-26    Eric2013      ST固件库版本 V1.0.2版本
*       v2.0    2014-02-23    Eric2013      ST固件库V1.3.0版本
*
*      Copyright (C), 2013-2014, 安富莱电子 www.armfly.com
*********************************************************************************************************
*/

#ifndef  APP_OS_CFG_MODULE_PRESENT
#define  APP_OS_CFG_MODULE_PRESENT

/*
*********************************************************************************************************
*                                       MODULE ENABLE / DISABLE
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            TASK PRIORITIES
*********************************************************************************************************
*/
#if		(defined(OS_UCOSIII)||defined(OS_FREERTOS))
#define  APP_CFG_TASK_START_PRIO             (2u)
#define  APP_TASK_RFMS_TX_PRIO               (5u)
#define  APP_TASK_USERTIMER_10MS_PRIO        (6u)
#define  APP_TASK_DEBUG_PRIO                 (7u)
#define  APP_TASK_AD_PRIO                    (8u)
#define  APP_TASK_SENSOR_PRIO                (9u)
#define  APP_TASK_HCI_PRIO                   (10u)
#define  APP_TASK_BLUETOOTH_PRIO             (11u)
#define  APP_TASK_GSM_TEST_PRIO              (12u)
#define  APP_TASK_GSM_SEND_PRIO              (13u)
#define  APP_TASK_GSM_PARSE_PRIO             (14u)
#define  APP_TASK_GPRSAPPXKAP_PRIO           (15u)
#define  APP_TASK_USERTIMER_100MS_PRIO       (16u)
#define  APP_TASK_RFMS_PRIO                  (17u)
#define  APP_TASK_RFMS_TEST_PRIO             (18u)
#define  APP_TASK_COMMPC_PRIO                (19u)
#define  APP_TASK_UARTICAM_PRIO              (19u)
#define  APP_TASK_GASMODULE_PRIO             (9u)
#define  APP_TASK_SPI_PRIO                   (10u)
#elif	(defined(OS_FREERTOS))
#define  APP_CFG_TASK_START_PRIO             (configMAX_PRIORITIES-2u)
#define  APP_TASK_RFMS_TX_PRIO               (configMAX_PRIORITIES-5u)
#define  APP_TASK_USERTIMER_10MS_PRIO        (configMAX_PRIORITIES-6u)
#define  APP_TASK_DEBUG_PRIO                 (configMAX_PRIORITIES-7u)
#define  APP_TASK_AD_PRIO                    (configMAX_PRIORITIES-8u)
#define  APP_TASK_SENSOR_PRIO                (configMAX_PRIORITIES-9u)
#define  APP_TASK_HCI_PRIO                   (configMAX_PRIORITIES-10u)
#define  APP_TASK_BLUETOOTH_PRIO             (configMAX_PRIORITIES-11u)
#define  APP_TASK_GSM_TEST_PRIO              (configMAX_PRIORITIES-12u)
#define  APP_TASK_GSM_SEND_PRIO              (configMAX_PRIORITIES-13u)
#define  APP_TASK_GSM_PARSE_PRIO             (configMAX_PRIORITIES-14u)
#define  APP_TASK_GPRSAPPXKAP_PRIO           (configMAX_PRIORITIES-15u)
#define  APP_TASK_USERTIMER_100MS_PRIO       (configMAX_PRIORITIES-16u)
#define  APP_TASK_RFMS_PRIO                  (configMAX_PRIORITIES-17u)
#define  APP_TASK_RFMS_TEST_PRIO             (configMAX_PRIORITIES-18u)
#define  APP_TASK_COMMPC_PRIO                (configMAX_PRIORITIES-19u)
#define  APP_TASK_UARTICAM_PRIO              (configMAX_PRIORITIES-19u)
#define  APP_TASK_GASMODULE_PRIO             (configMAX_PRIORITIES-9u)
#define  APP_TASK_SPI_PRIO                   (configMAX_PRIORITIES-10u)
#endif
/*
*********************************************************************************************************
*                                            TASK STACK SIZES
*                             Size of the task stacks (# of OS_STK entries)
*********************************************************************************************************
*/
#if		(defined(XKAP_ICARE_B_C)||defined(XKAP_ICARE_B_D)||defined(PROJECT_NRF5X_BLE)\
		||(defined(XKAP_ICARE_B_M)&&(!defined(HAIER))))
#define  APP_CFG_TASK_START_STK_SIZE         (128u-32u)
#else
#define  APP_CFG_TASK_START_STK_SIZE         (128u)
#endif

#if		(defined(PROJECT_BIB_TEST1))
#define  APP_CFG_TASK_DEBUG_STK_SIZE         (128u)
#elif (defined(STM32F1)||defined(STM32F4))
#define  APP_CFG_TASK_DEBUG_STK_SIZE         (1024u)
#elif (defined(NRF51)||defined(NRF52))
#define  APP_CFG_TASK_DEBUG_STK_SIZE         (128u)
#endif
//
#if		(defined(PROJECT_BIB_TEST1))
#define  APP_TASK_HCI_STK_SIZE               (128u)
#else
#define  APP_TASK_HCI_STK_SIZE               (1024u)/*(256u)*/
#endif
//
#if		(defined(XKAP_ICARE_B_C)||defined(PROJECT_NRF5X_BLE))	/*eMD Test*/
#define  APP_TASK_USERTIMER_100MS_STK_SIZE   (156u)
#elif	(defined(XKAP_ICARE_B_M)&&(!defined(HAIER)))
#define  APP_TASK_USERTIMER_100MS_STK_SIZE   (64u)
#else
#define  APP_TASK_USERTIMER_100MS_STK_SIZE   (128u)
#endif
//
#if		(defined(XKAP_ICARE_B_M)&&(!defined(HAIER)))
#define  APP_TASK_USERTIMER_10MS_STK_SIZE    (64u)
#else
#define  APP_TASK_USERTIMER_10MS_STK_SIZE    (128u)
#endif
//
#if		(defined(XKAP_ICARE_B_M)&&(!defined(HAIER)))
#define  APP_TASK_GPRSAPPXKAP_STK_SIZE    	(64u)
#else
#define  APP_TASK_GPRSAPPXKAP_STK_SIZE    	(128u)
#endif
//
#if		(defined(XKAP_ICARE_B_M)&&(!defined(HAIER)))
#define  APP_TASK_GSM_PARSE_STK_SIZE    	(64u)
#else
#define  APP_TASK_GSM_PARSE_STK_SIZE    	(128u)
#endif
//
#if		(defined(XKAP_ICARE_B_M)&&(!defined(HAIER)))
#define  APP_TASK_GSM_SEND_STK_SIZE    		(96u)
#else
#define  APP_TASK_GSM_SEND_STK_SIZE    		(128u)
#endif


#define  APP_TASK_SENSOR_STK_SIZE            (128u)
#define  APP_TASK_BLUETOOTH_STK_SIZE         (128u)
#define  APP_TASK_GSM_TEST_STK_SIZE          (128u)
#define  APP_TASK_AD_STK_SIZE                (128u)
#define  APP_TASK_RFMS_STK_SIZE              (256u)
#define  APP_TASK_COMMPC_STK_SIZE            (128u)
#define  APP_TASK_RFMS_TEST_STK_SIZE         (128u)
#define  APP_TASK_RFMS_TX_STK_SIZE           (64u)
#define  APP_TASK_GASMODULE_STK_SIZE         (64u)
#define  APP_TASK_UARTICAM_STK_SIZE          (128u)
#define  APP_TASK_SPI_STK_SIZE               (128u)
#endif
/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

