/*
***********************************************************************************
*                    ��    ��: ������
*                    ����ʱ��: 2015-05-29
***********************************************************************************
*/
#ifndef  __EMWIN_XSL_APP_H__
#define  __EMWIN_XSL_APP_H__
//-------------------���ؿ⺯��------------------------------
#include "includes.h"
#include "GUI.h"
//-------------------�ӿں궨��(Ӳ�����)--------------------
//-------------------�ӿں궨��(Ӳ���޹�)--------------------
//-------------------�ӿڱ���--------------------------------
extern GUI_FONT   XBF_Font;
//-------------------�ӿں���--------------------------------
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

