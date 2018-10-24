 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
| *Copyright (c) 2011, TCI All right reserved
| *�ļ�����		: LocalMem.c
| *�ļ����ݸ���	:ʵ�־ֲ������ڴ�Ķ�̬����
| *�ļ���������	:2011.05.09
| *�ļ�������	:	������
| *�ļ�����޸�����	:
| *�ļ�����޸���	:
| *��ע��
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#include <includes.h>
#ifndef STM32
#include "user.h"
#endif
/***********************
*  �ڴ��ݴ����ṹ��
***********************/
typedef struct{
   INT8U m_Lock :1;
   INT8U m_Res :7;
}TmpMemFlagStu;
typedef union {
   TmpMemFlagStu m_TmpMemFlag;
   INT8U m_Value;
}TmpMemFlagUni;
//����ĵ����ڴ浥Ԫ
typedef struct{
   INT8U*  starAddr;
   INT16U  areaSize;
}TmpMemStu;
//�ڴ�ʹ����Ϣ�ṹ��
typedef struct{
   INT16U refCount; //����������ü���
   INT8U* idleAreaPt;
   TmpMemFlagUni m_Flag;
}TmpMemUseInfStu;

static TmpMemUseInfStu m_tmpMemInf;
static INT8U ShareBuff[1024];
extern INT32U SharBuffSizeGet(void);
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*
*     ��֤�ݴ��ڴ�İ�ȫ�ԣ���������Ϣ�鶨��Ϊģ���ڲ���������������
*   Ҳ��Ӧ�����ڸ��ļ�
*
_______________________________________________________________________*/


/************************************************************************
*	����ԭ��:
*	��������:��ʼ����ǰ�ڴ����
*	����˵��:
*	ȫ�ֱ���:
*	����ֵ:
*	��ע:
************************************************************************/
void LocalMemInit(void)
{
   m_tmpMemInf.idleAreaPt = ShareBuff;
   m_tmpMemInf.refCount = 0;
   m_tmpMemInf.m_Flag.m_Value = 0;
   m_tmpMemInf.m_Flag.m_TmpMemFlag.m_Lock = FALSE;
}

/************************************************************************
*	����ԭ��:
*	��������:����һƬ��ʱ�ڴ�
*	����˵��:
*	ȫ�ֱ���:
*	����ֵ:
*	��ע:
************************************************************************/
void* LocalMalloc(INT16U byteSize)
{
   //������Ϣͷָ��
   TmpMemStu* tmpUseInfHeadPt = (TmpMemStu*)((INT8U*)ShareBuff + SharBuffSizeGet());
   //�Ƿ�����
   if(m_tmpMemInf.m_Flag.m_TmpMemFlag.m_Lock)
   {
      return NULL;
   }
   //��������
   //��1ΪΪ��ǰ������������Ŀ���ͷ��Ϣ
   if((INT8U*)m_tmpMemInf.idleAreaPt + byteSize + \
      (m_tmpMemInf.refCount+1)*sizeof(TmpMemStu) < \
      (INT8U*)tmpUseInfHeadPt)
   {
      //���¿��ƿ���Ϣ
      m_tmpMemInf.refCount++;
      tmpUseInfHeadPt -= m_tmpMemInf.refCount;
      tmpUseInfHeadPt->areaSize = byteSize;
      tmpUseInfHeadPt->starAddr = (INT8U*)m_tmpMemInf.idleAreaPt;
      //����ָ����Ϣ
      m_tmpMemInf.idleAreaPt += byteSize;
      return tmpUseInfHeadPt->starAddr;
   }
   else
   {
      return NULL;
   }
}

/************************************************************************
*	����ԭ��:
*	��������:�ͷ���ʱ�ڴ�
*	����˵��:
*	ȫ�ֱ���:
*	����ֵ:
*	��ע:���ڴ���ƿ�ṹ�����������ڴ�ֻ��������ʱ�������ݴ������
*     �����ͷ�ʱΪ���Ƶ�ջ�����ͷ��м��ĳһ��Ĭ�Ͻ������������ڴ�Ҳ�ͷ�
*     ���ͷ�ʱ��ù淶�����������ͷųɶԳ���
************************************************************************/
BOOL LocalMemFree(void* tmpMemPt)
{
   INT8U tmpCnt = 0;
   //������Ϣͷָ��
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
*	����ԭ��:
*	��������:�����ֲ��������Է�������
*	����˵��:
*	ȫ�ֱ���:
*	����ֵ:����
*	��ע:��Ҫ���ڱ����д���ֱ��ʹ����Ƭȫ������ģ�������ֲ������ͻ��
*  ������ֱ��ʹ��ʱ�Ծֲ�������̬����������
************************************************************************/
BOOL LocalMemLock(void)
{
   m_tmpMemInf.m_Flag.m_TmpMemFlag.m_Lock = TRUE;
   return TRUE;
}

/************************************************************************
*	����ԭ��:
*	��������:�ͷŶ�ȫ�ֹ������������
*	����˵��:
*	ȫ�ֱ���:
*	����ֵ:����
*	��ע:
************************************************************************/
BOOL LocalMemUnLock(void)
{
   m_tmpMemInf.m_Flag.m_TmpMemFlag.m_Lock = FALSE;
   return TRUE;
}

/************************************************************************
*	����ԭ��:
*	��������:
*	����˵��:
*	ȫ�ֱ���:
*	����ֵ:
*	��ע:
************************************************************************/

