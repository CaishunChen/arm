 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
| *Copyright (c) 2011, TCI All right reserved
| *文件名称		: LocalMem.c
| *文件内容概述	:实现局部变量内存的动态分配
| *文件创建日期	:2011.05.09
| *文件创建者	:	徐松亮
| *文件最后修改日期	:
| *文件最后修改者	:
| *备注：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#include <includes.h>
#ifndef STM32
#include "user.h"
#endif
/***********************
*  内存暂存管理结构体
***********************/
typedef struct{
   INT8U m_Lock :1;
   INT8U m_Res :7;
}TmpMemFlagStu;
typedef union {
   TmpMemFlagStu m_TmpMemFlag;
   INT8U m_Value;
}TmpMemFlagUni;
//申请的单块内存单元
typedef struct{
   INT8U*  starAddr;
   INT16U  areaSize;
}TmpMemStu;
//内存使用信息结构体
typedef struct{
   INT16U refCount; //分配情况引用计数
   INT8U* idleAreaPt;
   TmpMemFlagUni m_Flag;
}TmpMemUseInfStu;

static TmpMemUseInfStu m_tmpMemInf;
static INT8U ShareBuff[1024];
extern INT32U SharBuffSizeGet(void);
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*
*     保证暂存内存的安全性，将管理信息块定义为模块内部变量，各管理函数
*   也相应定义在该文件
*
_______________________________________________________________________*/


/************************************************************************
*	函数原型:
*	函数功能:初始化当前内存管理
*	参数说明:
*	全局变量:
*	返回值:
*	备注:
************************************************************************/
void LocalMemInit(void)
{
   m_tmpMemInf.idleAreaPt = ShareBuff;
   m_tmpMemInf.refCount = 0;
   m_tmpMemInf.m_Flag.m_Value = 0;
   m_tmpMemInf.m_Flag.m_TmpMemFlag.m_Lock = FALSE;
}

/************************************************************************
*	函数原型:
*	函数功能:申请一片临时内存
*	参数说明:
*	全局变量:
*	返回值:
*	备注:
************************************************************************/
void* LocalMalloc(INT16U byteSize)
{
   //分配信息头指针
   TmpMemStu* tmpUseInfHeadPt = (TmpMemStu*)((INT8U*)ShareBuff + SharBuffSizeGet());
   //是否被锁定
   if(m_tmpMemInf.m_Flag.m_TmpMemFlag.m_Lock)
   {
      return NULL;
   }
   //允许申请
   //加1为为当前申请区域申请的控制头信息
   if((INT8U*)m_tmpMemInf.idleAreaPt + byteSize + \
      (m_tmpMemInf.refCount+1)*sizeof(TmpMemStu) < \
      (INT8U*)tmpUseInfHeadPt)
   {
      //更新控制块信息
      m_tmpMemInf.refCount++;
      tmpUseInfHeadPt -= m_tmpMemInf.refCount;
      tmpUseInfHeadPt->areaSize = byteSize;
      tmpUseInfHeadPt->starAddr = (INT8U*)m_tmpMemInf.idleAreaPt;
      //更新指针信息
      m_tmpMemInf.idleAreaPt += byteSize;
      return tmpUseInfHeadPt->starAddr;
   }
   else
   {
      return NULL;
   }
}

/************************************************************************
*	函数原型:
*	函数功能:释放临时内存
*	参数说明:
*	全局变量:
*	返回值:
*	备注:如内存控制块结构体所述，该内存只能用于临时中需求暂存大的情况
*     所以释放时为类似弹栈，当释放中间的某一段默认将其后面申请的内存也释放
*     但释放时最好规范操作，申请释放成对出现
************************************************************************/
BOOL LocalMemFree(void* tmpMemPt)
{
   INT8U tmpCnt = 0;
   //分配信息头指针
   TmpMemStu* tmpUseInfHeadPt = (TmpMemStu* )((INT8U*)ShareBuff + SharBuffSizeGet());
   if((INT8U*)tmpMemPt < (INT8U*)ShareBuff || (INT8U*)tmpMemPt >= \
      (INT8U*)tmpUseInfHeadPt)
   {
      return FALSE;
   }
   tmpCnt = m_tmpMemInf.refCount;
   tmpUseInfHeadPt -= tmpCnt;      
   while(tmpCnt--)
   {
      if(tmpUseInfHeadPt->starAddr == tmpMemPt)
      {
         m_tmpMemInf.idleAreaPt = tmpMemPt;
         m_tmpMemInf.refCount = tmpCnt;
         return TRUE;
      }
   }
   return FALSE;
}

/************************************************************************
*	函数原型:
*	函数功能:锁定局部变量可以分配区域
*	参数说明:
*	全局变量:
*	返回值:保留
*	备注:主要是在报表中存在直接使用这片全局区域的，避免与局部的相冲突，
*  所以在直接使用时对局部变量动态申请区加锁
************************************************************************/
BOOL LocalMemLock(void)
{
   m_tmpMemInf.m_Flag.m_TmpMemFlag.m_Lock = TRUE;
   return TRUE;
}

/************************************************************************
*	函数原型:
*	函数功能:释放对全局共享变量的锁定
*	参数说明:
*	全局变量:
*	返回值:保留
*	备注:
************************************************************************/
BOOL LocalMemUnLock(void)
{
   m_tmpMemInf.m_Flag.m_TmpMemFlag.m_Lock = FALSE;
   return TRUE;
}

/************************************************************************
*	函数原型:
*	函数功能:
*	参数说明:
*	全局变量:
*	返回值:
*	备注:
************************************************************************/

