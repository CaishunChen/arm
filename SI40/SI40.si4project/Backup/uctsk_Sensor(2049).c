/*
***********************************************************************************
*                    作    者: 徐松亮
*                    更新时间: 2015-06-03
***********************************************************************************
*/
#include "uctsk_Sensor.h"
#include "Bsp_IllumSensor.h"
#include "Bsp_TempHumidSensor.h"
//-------------------静态变量--------------------------------
MODULE_OS_TASK_TAB(App_TaskSensorTCB);
MODULE_OS_TASK_STK(App_TaskSensorStk,APP_TASK_SENSOR_STK_SIZE);
//------------------------------- 静态函数 ---------------------------------------
static void uctsk_Sensor(void *pvParameters);
//------------------------------- 接口变量 ---------------------------------------
//传感器数值
int16_t  Sensor_Temp;   //温度值×10  (-550～+1250)
uint16_t Sensor_Humi;   //湿度值×10  (0-1000)
uint32_t Sensor_Illum;  //照度值      (0-10000 lx)
//错误码(0-OK，1-无芯片，2-数值无效)
uint8_t  Sensor_Err_Temp;
uint8_t  Sensor_Err_Humi;
uint8_t  Sensor_Err_Illim;
//------------------------------- 引用变量 ---------------------------------------
/*******************************************************************************
函数功能: Sensor任务创建
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
函数功能: Sensor任务实体
*******************************************************************************/
static void uctsk_Sensor (void *pvParameters)
{
    //照度初始化
    BspTempHumidSensor_Init();
    for(;;)
    {
        //-----------------------
        //OSTimeDlyHMSM(0, 0, 1, 0);
        MODULE_OS_DELAY_MS(1000);
			
        //更新温度，湿度
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
        //更新照度
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
//------------------------------- 驱动函数 ---------------------------------------

