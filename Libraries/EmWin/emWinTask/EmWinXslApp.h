/*
***********************************************************************************
*                    作    者: 徐松亮
*                    更新时间: 2015-05-29
***********************************************************************************
*/
#ifndef  __EMWIN_XSL_APP_H__
#define  __EMWIN_XSL_APP_H__
//-------------------加载库函数------------------------------
#include "includes.h"
#include "GUI.h"
//-------------------接口宏定义(硬件相关)--------------------
//-------------------接口宏定义(硬件无关)--------------------
//-------------------接口变量--------------------------------
extern GUI_FONT   XBF_Font;
//-------------------接口函数--------------------------------
//void _ShowXBF(uint8_t OnOff);
//void _ShowBMPEx(const char * sFilename,uint16_t xpos,uint16_t ypos);
//void _ShowJPGEx(const char * sFilename,uint16_t xpos,uint16_t ypos);
//void _ShowGIFEx(const char * sFilename,uint16_t xpos,uint16_t ypos);
//void _ShowPNGEx(const char * sFilename,uint16_t xpos,uint16_t ypos);
//
uint8_t EmWinXslApp_ShowBMP(const char * sFilename,uint16_t xpos,uint16_t ypos);
uint8_t EmWinXslApp_ShowStreamedBMP(const char * sFilename,uint16_t xpos,uint16_t ypos);
//
void EmWinXslApp_Test(void);
//-----------------------------------------------------------
#endif

