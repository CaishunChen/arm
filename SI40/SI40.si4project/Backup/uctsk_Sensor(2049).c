/*
***********************************************************************************
*                    ��    ��: ������
*                    ����ʱ��: 2015-06-03
***********************************************************************************
*/
#include "uctsk_Sensor.h"
#include "Bsp_IllumSensor.h"
#include "Bsp_TempHumidSensor.h"
//-------------------��̬����--------------------------------
MODULE_OS_TASK_TAB(App_TaskSensorTCB);
MODULE_OS_TASK_STK(App_TaskSensorStk,APP_TASK_SENSOR_STK_SIZE);
//------------------------------- ��̬���� ---------------------------------------
static void uctsk_Sensor(void *pvParameters);
//------------------------------- �ӿڱ��� ---------------------------------------
//��������ֵ
int16_t  Sensor_Temp;   //�¶�ֵ��10  (-550��+1250)
uint16_t Sensor_Humi;   //ʪ��ֵ��10  (0-1000)
uint32_t Sensor_Illum;  //�ն�ֵ      (0-10000 lx)
//������(0-OK��1-��оƬ��2-��ֵ��Ч)
uint8_t  Sensor_Err_Temp;
uint8_t  Sensor_Err_Humi;
uint8_t  Sensor_Err_Illim;
//------------------------------- ���ñ��� ---------------------------------------
/*******************************************************************************
��������: Sensor���񴴽�
*******************************************************************************/
void  App_SensorTaskCreate (void)
{
    MODULE_OS_TASK_CREATE("App Task Sensor",
                          uctsk_Sensor,
                          APP_TASK_SENSOR_PRIO,
                          App_TaskSensorStk,
                          APP_TASK_SENSOR_STK_SIZE,
                          App_TaskSensorTCB,
                          NULL);
}
/*******************************************************************************
��������: Sensor����ʵ��
*******************************************************************************/
static void uctsk_Sensor (void *pvParameters)
{
    //�նȳ�ʼ��
    BspTempHumidSensor_Init();
    for(;;)
    {
        //-----------------------
        //OSTimeDlyHMSM(0, 0, 1, 0);
        MODULE_OS_DELAY_MS(1000);
			
        //�����¶ȣ�ʪ��
        if(OK==BspTempHumidSensor_Read(&Sensor_Temp,&Sensor_Humi))
        {
            Sensor_Err_Temp=0;
            Sensor_Err_Humi=0;
        }
        else
        {
            Sensor_Err_Temp=1;
            Sensor_Err_Humi=1;
            BspTempHumidSensor_Init();
        }
#if   (defined(PROJECT_XKAP_V3)||defined(XKAP_ICARE_B_D_M))
        //�����ն�
        if(OK==BspIllumSensor_Read(&Sensor_Illum))
        {
            Sensor_Err_Illim=0;
        }
        else
        {
            Sensor_Err_Illim=1;
        }
#endif
    }
}
//------------------------------- �������� ---------------------------------------

