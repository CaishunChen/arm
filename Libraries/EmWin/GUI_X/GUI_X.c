/*
*********************************************************************************************************
*                                                uC/GUI
*                        Universal graphic software for embedded applications
*
*                       (c) Copyright 2002, Micrium Inc., Weston, FL
*                       (c) Copyright 2002, SEGGER Microcontroller Systeme GmbH
*
*              礐/GUI is protected by international copyright laws. Knowledge of the
*              source code may not be used to write a similar product. This file may
*              only be used in accordance with a license and should not be redistributed
*              in any way. We appreciate your understanding and fairness.
*
----------------------------------------------------------------------
File        : GUI_X.C
Purpose     : Config / System dependent externals for GUI
---------------------------END-OF-HEADER------------------------------
*/

#include "GUI.h"
#include "stm32f4xx.h"


/*
********************************************************************
*
*       Global data
*
********************************************************************
*/


/*********************************************************************
*
*      Timing:
*                 GUI_X_GetTime()
*                 GUI_X_Delay(int)

  Some timing dependent routines require a GetTime
  and delay function. Default time unit (tick), normally is
  1 ms.
*/
extern __IO int32_t g_iRunTime;
int GUI_X_GetTime(void)
{

	return g_iRunTime;
}

void GUI_X_Delay(int ms)
{
	int tEnd = g_iRunTime + ms;
	while ((tEnd - g_iRunTime) > 0);
}

/*
*********************************************************************************************************
*                                          GUI_X_ExecIdle()
*********************************************************************************************************
*/
/*WM空闲时调用*/
void GUI_X_ExecIdle(void) 
{
  GUI_X_Delay(1);
}


/*
*********************************************************************************************************
*                                    MULTITASKING INTERFACE FUNCTIONS
*
* Note(1): 1) The following routines are required only if uC/GUI is used in a true multi task environment, 
*             which means you have more than one thread using the uC/GUI API.  In this case the #define 
*             GUI_OS 1   needs to be in GUIConf.h
*********************************************************************************************************
*/

void GUI_X_InitOS (void)
{ 
//  DispSem = OSSemCreate(1);              //建立一个互斥型信号量
//  EventMbox = OSMboxCreate((void *)0);	//建立一个邮箱
}


void GUI_X_Lock(void)
{ 
//  INT8U err;
//  OSSemPend(DispSem,0,&err);
}


void GUI_X_Unlock(void)
{ 
//  OSSemPost(DispSem);
}


U32 GUI_X_GetTaskId(void)
{ 
//  return ((U32)(OSTCBCur->OSTCBPrio));
	return 0;
}

/*
*********************************************************************************************************
*                                        GUI_X_WaitEvent()
*                                        GUI_X_SignalEvent()
*********************************************************************************************************
*/


void GUI_X_WaitEvent(void)
{
//   INT8U err;
//   (void)OSMboxPend(EventMbox,0,&err);
}


void GUI_X_SignalEvent(void)
{
//  (void)OSMboxPost(EventMbox,(void *)1);
}

/*
*********************************************************************************************************
*                                      KEYBOARD INTERFACE FUNCTIONS
*
* Purpose: The keyboard routines are required only by some widgets.
*          If widgets are not used, they may be eliminated.
*
* Note(s): If uC/OS-II is used, characters typed into the log window will be placed	in the keyboard buffer. 
*          This is a neat feature which allows you to operate your target system without having to use or 
*          even to have a keyboard connected to it. (useful for demos !)
*********************************************************************************************************
*/

void CheckInit(void)
{
//  if(KeyIsInited==0)
//    {
//      KeyIsInited = 1;
//      GUI_X_Init();
//    }
}


/*被GUI_Init()调用,用来初始化一些GUI运行之前需要用的硬件,如键盘或者鼠标之类的.如果不需要的话,可以为空*/
void GUI_X_Init(void)
{
//  KeySem = OSSemCreate(0);
}


int GUI_X_GetKey(void)
{
    int r;
//    r = KeyPressed;
//    CheckInit();
//    KeyPressed = 0;
    return (r);
}


int GUI_X_WaitKey(void)
{
     int  r;
//    INT8U  err;
//    CheckInit();
//    if(KeyPressed==0)
//      {
//        OSSemPend(KeySem,0,&err); //等待信号量
//      }
//    r= KeyPressed;
//    KeyPressed = 0;
    return (r);
}


void GUI_X_StoreKey(int k)
{
//    KeyPressed = k;
//    OSSemPost(KeySem); //释放信号量
}

void GUI_X_Log(const char *s) 
{ 
	GUI_USE_PARA(s); 
}

void GUI_X_Warn(const char *s) 
{
	GUI_USE_PARA(s); 
}

void GUI_X_ErrorOut(const char *s)
{ 
	GUI_USE_PARA(s); 
}
