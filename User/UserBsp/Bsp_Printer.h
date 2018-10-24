/**
  ******************************************************************************
  * @file    Bsp_Printer.h 
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  * @brief   bottom-driven -->   串口打印机驱动.
  * @note    
  * @verbatim
    
 ===============================================================================
                     ##### How to use this driver #####
 ===============================================================================
   1,    适用芯片
         STM      :  STM32F1  STM32F4
         Nordic   :  Nrf51    Nrf52
   2,    移植步骤
   3,    验证方法
   4,    使用方法
   5,    其他说明
  @endverbatim      
  ******************************************************************************
  * @attention
  *
  * GNU General Public License (GPL) 
  *
  * <h2><center>&copy; COPYRIGHT 2017 XSLXHN</center></h2>
  ******************************************************************************
  */

#ifndef __BSP_PRINTER_H
#define __BSP_PRINTER_H
//-------------------加载库函数------------------------------
#include "includes.h"

//-------------------接口宏定义(硬件无关)--------------------

enum PRINTER_UARTX_APP_CMD
{    
   PRINTER_UARTX_APP_CMD_WORD_12SIZE12 = 0,      //12*12
   PRINTER_UARTX_APP_CMD_WORD_24SIZE24,          //24*24
   PRINTER_UARTX_APP_CMD_WORD_WIDTH_UNABLE,      
   PRINTER_UARTX_APP_CMD_WORD_WIDTH_ANABLE,      //粗体
   PRINTER_UARTX_APP_CMD_WORD_DOUBLE_UNABLE,           
   PRINTER_UARTX_APP_CMD_WORD_DOUBLE_ANABLE,     //放大2倍
   PRINTER_UARTX_APP_CMD_LINE_SPACE,             //行间距
   PRINTER_UARTX_APP_CMD_RIGHT_LIMIT,            //右限
   PRINTER_UARTX_APP_CMD_LEFT_LIMIT,             //左侧不打印的字符
   PRINTER_UARTX_APP_CMD_UNDERLINE_UNABLE,
   PRINTER_UARTX_APP_CMD_UNDERLINE_ENABLE,      //下划线
   PRINTER_UARTX_APP_CMD_INIT,                  //初始化
   PRINTER_UARTX_APP_CMD_BAUD_115200, 
	 PRINTER_UARTX_APP_CMD_BAUD_9600,
   PRINTER_UARTX_APP_CMD_ENTER,
	 PRINTER_UARTX_APP_CMD_IMAGE,
};
//-------------------接口宏定义(硬件相关)--------------------
#if   (defined(PROJECT_STLH_V10))
//---->
#define  BSP_PRINTER_UARTX    (3)
typedef struct BSP_PRINTER_LCD_INFO
{
   //时间(年月日时分秒)
   uint8_t TimeBuf[16];
   //经度半球E（东经）或W（西经）
	char     JingDu_EW;
   //纬度半球N（北半球）或S（南半球）
   char     WeiDu_NS;
   //-----
   //经度 度
   uint16_t JingDu_Du;
   //纬度 度
   uint16_t WeiDu_Du;
   //-----
   //经度 分
   uint32_t JingDu_Fen;
   //纬度 分
   uint32_t WeiDu_Fen;
   //-----
   //温度
   float  Temp;
   //湿度
   float  Humi;
   //-----
}BSP_PRINTER_LCD_INFO;
#else
#error Please Set Project to Bsp_Printer.h
#endif
//-------------------接口宏定义(硬件无关)--------------------
//-------------------接口变量--------------------------------
//-------------------接口函数--------------------------------
void BspPrinter_Init(void);
void BspPrinter_InterruptRx(uint8_t *pbuf,uint16_t len);
void BspPrinter_PrintInfo(BSP_PRINTER_LCD_INFO pinfo);
void BspPrinter_DebugTestOnOff(uint8_t OnOff);
//-----------------------------------------------------------
#endif

