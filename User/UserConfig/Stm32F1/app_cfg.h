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
*                                           �������������
********************************************************************************************************/
#define  LIB_MEM_CFG_ARG_CHK_EXT_EN     DEF_ENABLED
#define  LIB_MEM_CFG_OPTIMIZE_ASM_EN    DEF_ENABLED
#define  LIB_MEM_CFG_ALLOC_EN           DEF_ENABLED
#define  LIB_MEM_CFG_HEAP_SIZE           (8*128)
/********************************************************************************************************
*                                           ��������
********************************************************************************************************/
/********************************************************************************************************
*                                           �������ȼ�
********************************************************************************************************/
#define  APP_TASK_START_PRIO                               3   //��������
#define  APP_TASK_OSVIEW_TERMINAL_PRIO   (OS_LOWEST_PRIO - 20) //
#define  OS_VIEW_TASK_PRIO               (OS_LOWEST_PRIO - 19)  //VIEW����
#define  OS_TASK_TMR_PRIO                (OS_LOWEST_PRIO - 2)  //��ʱ������

//#define  APP_TASK_RF_PRIO                (OS_LOWEST_PRIO - 6)  //RF����
#define  APP_TASK_SDFAT_PRIO             (OS_LOWEST_PRIO - 7)  //SDFAT����
#define  APP_TASK_USERTIMER_10MS_PRIO    (OS_LOWEST_PRIO - 5)  //KIN����
#define  APP_TASK_USERTIMER_100MS_PRIO   (OS_LOWEST_PRIO - 4)  //KIN����
//#define  APP_TASK_CAN_PRIO               (OS_LOWEST_PRIO - 13) //CAN����
#define  APP_TASK_MODBUS_PRIO            (OS_LOWEST_PRIO - 14) //MODBUS����
#define  APP_TASK_AD_PRIO                (OS_LOWEST_PRIO - 15) //AD����
#define  APP_TASK_W5500APP_PRIO          (OS_LOWEST_PRIO - 12) //W5500APP����
#define  APP_TASK_GPRSAPPXKAP_PRIO       (OS_LOWEST_PRIO - 12)  //GPRS-XKAP����
#define  APP_TASK_GSM_PARSE_PRIO         (OS_LOWEST_PRIO - 17) //GSMģ��M10��ATָ���������
#define  APP_TASK_GSM_SEND_PRIO          (OS_LOWEST_PRIO - 18) //GSMģ�鷢������
#define  APP_TASK_GSM_TEST_PRIO          (OS_LOWEST_PRIO - 19) //GSMģ��������ݽ�������
#define  APP_TASK_GSM_SUBSECTION_PRIO    (OS_LOWEST_PRIO - 20) //GSMģ��ֶ�����
#define  APP_TASK_SPI_PRIO               (OS_LOWEST_PRIO - 8)  //SPI����
#define  APP_TASK_HCI_PRIO               (OS_LOWEST_PRIO - 9)  //HCI����
#define  APP_TASK_SENSOR_PRIO            (OS_LOWEST_PRIO - 10) //Sensor����
//#define  APP_TASK_RFMS_PRIO              (OS_LOWEST_PRIO - 11) //Rfms����
#define  APP_TASK_BLUETOOTH_PRIO         (OS_LOWEST_PRIO - 6)  //��������
#define  APP_TASK_COMMPC_PRIO            (OS_LOWEST_PRIO - 13) //PCͨѶ����
#define  APP_TASK_GASMODULE_PRIO         (OS_LOWEST_PRIO - 11)  //��������
#define  APP_TASK_UARTICAM_PRIO          (OS_LOWEST_PRIO - 13) //PCͨѶ����
/********************************************************************************************************
*                                           �����ջ�趨(# of OS_STK entries)
********************************************************************************************************/
#define  APP_TASK_START_STK_SIZE                          128u //��������
#define  APP_TASK_OSVIEW_TERMINAL_STK_SIZE                64u  //
#define  OS_VIEW_TASK_STK_SIZE                            64u  //VIEW����

#define  APP_TASK_RF_STK_SIZE                             128u //RF����
#define  APP_TASK_SDFAT_STK_SIZE                          256u //SDFAT����
#define  APP_TASK_USERTIMER_10MS_STK_SIZE                 128u //��������������
#define  APP_TASK_USERTIMER_100MS_STK_SIZE                128u //��������������
#define  APP_TASK_CAN_STK_SIZE                            128u //ά�����빫˾Ӧ�ó���
#define  APP_TASK_MODBUS_STK_SIZE                         128u //ά�����빫˾Ӧ�ó���
#define  APP_TASK_AD_STK_SIZE                             128u  //AD����
#define  APP_TASK_W5500APP_STK_SIZE                       128u //��������������
#define  APP_TASK_GSM_SUBSECTION_STK_SIZE                 128u //GSMģ��ֶ�����
#define  APP_TASK_GSM_PARSE_STK_SIZE                      128u //GSMģ���������
#define  APP_TASK_GSM_SEND_STK_SIZE                       128u //GSMģ�鷢������
#define  APP_TASK_GSM_TEST_STK_SIZE                       128u //GSMģ���������
#define  APP_TASK_SPI_STK_SIZE                            128u //
#define  APP_TASK_HCI_STK_SIZE                            128u //�˻���������
#define  APP_TASK_SENSOR_STK_SIZE                         64u  //�������ɼ�����
//#define  APP_TASK_RFMS_STK_SIZE                           64u  //RFMS
#define  APP_TASK_BLUETOOTH_STK_SIZE                      64u  //��������
#define  APP_TASK_GPRSAPPXKAP_STK_SIZE                    128u  //GPRS��������
#define  APP_TASK_COMMPC_STK_SIZE	   			         (128u)
#define  APP_TASK_GASMODULE_STK_SIZE                      64u  //���崫��ģ��  
#define  APP_TASK_UARTICAM_STK_SIZE	   			         (128u)
/********************************************************************************************************
*                                           ��
********************************************************************************************************/
#define  uC_CFG_OPTIMIZE_ASM_EN                 DEF_ENABLED
#define  LIB_STR_CFG_FP_EN                      DEF_DISABLED

#endif
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

