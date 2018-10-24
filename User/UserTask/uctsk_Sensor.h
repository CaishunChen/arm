/*
***********************************************************************************
*                    作    者: 徐松亮
*                    更新时间: 2015-06-03
***********************************************************************************
*/
#ifndef __SENSOR_H
#define __SENSOR_H
//-------------------加载库函数------------------------------
#include "includes.h"
/* 供外部调用的函数声明 */
//传感器数值
extern int16_t  Sensor_Temp;   //温度值×10  (-550～+1250)
extern uint16_t Sensor_Humi;   //湿度值×10  (0-1000)
extern uint32_t Sensor_Illum;  //照度值      (0-10000 lx)
//错误码(0-OK，1-无芯片，2-数值无效)
extern uint8_t  Sensor_Err_Temp;
extern uint8_t  Sensor_Err_Humi;
extern uint8_t  Sensor_Err_Illim;
//
void App_SensorTaskCreate(void);

#endif


