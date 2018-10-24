//-------------------------------------------------------------------------------//
//                           ������ͨ������޹�˾                                //
//                                ��Ȩ����                                       //
//                    ��ҵ��ַ��http://www.htong.com                             //
//                    ��������: RealView MDK-ARM Version 4.14                    //
//                    �� �� ��: RealView MDK-ARM Version 4.14                    //
//                    оƬ�ͺ�: STM32F103ZET6                                    //
//                    ��Ŀ����: HH-SPS ������Ŀ                                  //
//                    �ļ�����: MemManager.c                                     //
//                    ��    ��: ������                                           //
//                    ʱ    ��: 2014-01-17    �汾:  0.1                         //
//-------------------------------------------------------------------------------//
// �޸ļ�¼��
//    ���  ����     ����     ����
//    1     20140319 ������   STM32ƽ̨����С�黺��غʹ�黺���
//-------------------------------------------------------------------------------
#define MEMMANAGER_GLOBAL
#include <includes.h>
#ifndef STM32
#include "user.h"
#endif
//-------------------------------------------------------------------------------
#ifdef STM32
#define UCOS_LIB
#else
#endif
//-------------------------------------------------------------------------------STM32(uCOSƽ̨)
//С���ݿ�
#define MEM_BLK_NBR  16
#define MEM_BLK_SIZE 256L
static INT32U mem_buf[MEM_BLK_NBR*MEM_BLK_SIZE/4];
//
static  INT8U Mem_First=1;
INT8U MemManager_Debug_Num=0;
//------------------------------------------------------------------------------
#ifdef UCOS_LIB
//�����ݿ�
#define MEM1_BLK_NBR  2
#define MEM1_BLK_SIZE 1024L
static INT32U mem1_buf[MEM_BLK_NBR*MEM_BLK_SIZE/4];
static MEM_POOL MemPool;
static MEM_POOL Mem1Pool;
/*******************************************************************************
��������: ����س�ʼ��
��    ��:
�� �� ֵ:
*******************************************************************************/
static void MemManager_Init(INT8U *res)
{
    LIB_ERR err;
    CPU_SIZE_T allocation_err;
    //
    Mem_Init();
    //������һ�������
    Mem_PoolCreate (&MemPool,mem_buf,\
                    sizeof(mem_buf),\
                    MEM_BLK_NBR,\
                    MEM_BLK_SIZE,\
                    0,\
                    &allocation_err,\
                    &err);
    if(err==LIB_MEM_ERR_NONE)
    {
        *res=OK;
    }
    if(allocation_err!=0)
    {
        *res=ERR;
        return;
    }
    //�����ڶ��������
    Mem_PoolCreate (&Mem1Pool,mem1_buf,\
                    sizeof(mem1_buf),\
                    MEM1_BLK_NBR,\
                    MEM1_BLK_SIZE,\
                    0,\
                    &allocation_err,\
                    &err);
    if(err==LIB_MEM_ERR_NONE)
    {
        *res=OK;
    }
    if(allocation_err!=0)
    {
        *res=ERR;
        return;
    }
}
/*******************************************************************************
��������: ���뻺��
��    ��:
�� �� ֵ:
*******************************************************************************/
void *MemManager_Get(INT32U size,INT8U *res)
{
    void *p;
    LIB_ERR err;
    //
    if(Mem_First==1)
    {
        MemManager_Init(res);
        Mem_First=0;
    }
    //�������
    if(size&0x00000003)
    {
        size=size>>2;//����4
        size+=1;
        size=size<<2;//����4
    }
    //
    while(1)
    {
        p=Mem_PoolBlkGet (&MemPool,(CPU_SIZE_T)size,&err);
        if(LIB_MEM_ERR_NONE==err)
        {
            *res=OK;
            break;
        }
        else
        {
            OSTimeDlyHMSM(0, 0, 0, 100);
            *res=ERR;
            p=NULL;
        }
    }
    //
    MemManager_Debug_Num++;
    return p;
}
/*******************************************************************************
��������: �ͷŻ���
��    ��:
�� �� ֵ:
*******************************************************************************/
void MemManager_Free(void *pmem_blk,INT8U *res)
{
    LIB_ERR err;
    //
    if(Mem_First==1)
    {
        MemManager_Init(res);
        Mem_First=0;
    }
    //
    Mem_PoolBlkFree(&MemPool,pmem_blk,&err);
    if(LIB_MEM_ERR_NONE==err)
    {
        *res=OK;
    }
    else
    {
        *res=ERR;
    }
    MemManager_Debug_Num--;
}
/*******************************************************************************
��������: ���뻺��
��    ��:
�� �� ֵ:
*******************************************************************************/
void *Mem1Manager_Get(INT32U size,INT8U *res)
{
    void *p;
    LIB_ERR err;
    //
    if(Mem_First==1)
    {
        MemManager_Init(res);
        Mem_First=0;
    }
    //�������
    if(size&0x00000003)
    {
        size=size>>2;//����4
        size+=1;
        size=size<<2;//����4
    }
    //
    while(1)
    {
        p=Mem_PoolBlkGet (&Mem1Pool,(CPU_SIZE_T)size,&err);
        if(LIB_MEM_ERR_NONE==err)
        {
            *res=OK;
            break;
        }
        else
        {
            OSTimeDlyHMSM(0, 0, 0, 100);
            *res=ERR;
            p=NULL;
        }
    }
    return p;
}
/*******************************************************************************
��������: �ͷŻ���
��    ��:
�� �� ֵ:
*******************************************************************************/
void Mem1Manager_Free(void *pmem_blk,INT8U *res)
{
    LIB_ERR err;
    //
    if(Mem_First==1)
    {
        MemManager_Init(res);
        Mem_First=0;
    }
    //
    Mem_PoolBlkFree(&Mem1Pool,pmem_blk,&err);
    if(LIB_MEM_ERR_NONE==err)
    {
        *res=OK;
    }
    else
    {
        *res=ERR;
    }
}
//-------------------------------------------------------------------------------PIC
#else
/***********************
*  �ڴ��ݴ����ṹ��
***********************/
typedef struct
{
    INT8U m_Lock :1;
    INT8U m_Res :7;
} TmpMemFlagStu;
typedef union
{
    TmpMemFlagStu m_TmpMemFlag;
    INT8U m_Value;
} TmpMemFlagUni;
//����ĵ����ڴ浥Ԫ
typedef struct
{
    INT8U*  starAddr;
    INT16U  areaSize;
} TmpMemStu;
//�ڴ�ʹ����Ϣ�ṹ��
typedef struct
{
    INT16U refCount; //����������ü���
    INT8U* idleAreaPt;
    TmpMemFlagUni m_Flag;
} TmpMemUseInfStu;

static TmpMemUseInfStu m_tmpMemInf;
/************************************************************************
*   ����ԭ��:
*   ��������:��ʼ����ǰ�ڴ����
*   ����˵��:
*   ȫ�ֱ���:
*   ����ֵ:
*   ��ע:
************************************************************************/
static INT8U LocalMemInit(void)
{
    m_tmpMemInf.idleAreaPt = (INT8U*)mem_buf;
    m_tmpMemInf.refCount = 0;
    m_tmpMemInf.m_Flag.m_Value = 0;
    m_tmpMemInf.m_Flag.m_TmpMemFlag.m_Lock = ERR;
    return OK;
}

/************************************************************************
*   ����ԭ��:
*   ��������:����һƬ��ʱ�ڴ�
*   ����˵��:
*   ȫ�ֱ���:
*   ����ֵ:
*   ��ע:
************************************************************************/
static void* LocalMalloc(INT16U byteSize)
{
    //������Ϣͷָ��
    TmpMemStu* tmpUseInfHeadPt = (TmpMemStu*)((INT8U*)mem_buf + sizeof(mem_buf));
    //�Ƿ�����
    if(m_tmpMemInf.m_Flag.m_TmpMemFlag.m_Lock==OK)
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
*   ����ԭ��:
*   ��������:�ͷ���ʱ�ڴ�
*   ����˵��:
*   ȫ�ֱ���:
*   ����ֵ:
*   ��ע:���ڴ���ƿ�ṹ�����������ڴ�ֻ��������ʱ�������ݴ������
*     �����ͷ�ʱΪ���Ƶ�ջ�����ͷ��м��ĳһ��Ĭ�Ͻ������������ڴ�Ҳ�ͷ�
*     ���ͷ�ʱ��ù淶�����������ͷųɶԳ���
************************************************************************/
static INT8U LocalMemFree(void* tmpMemPt)
{
    INT8U tmpCnt = 0;
    //������Ϣͷָ��
    TmpMemStu* tmpUseInfHeadPt = (TmpMemStu* )((INT8U*)mem_buf + sizeof(mem_buf));
    if((INT8U*)tmpMemPt < (INT8U*)mem_buf || (INT8U*)tmpMemPt >= \
       (INT8U*)tmpUseInfHeadPt)
    {
        return ERR;
    }
    tmpCnt = m_tmpMemInf.refCount;
    tmpUseInfHeadPt -= tmpCnt;
    while(tmpCnt--)
    {
        if(tmpUseInfHeadPt->starAddr == tmpMemPt)
        {
            m_tmpMemInf.idleAreaPt = tmpMemPt;
            m_tmpMemInf.refCount = tmpCnt;
            return OK;
        }
    }
    return ERR;
}

/************************************************************************
*   ����ԭ��:
*   ��������:�����ֲ��������Է�������
*   ����˵��:
*   ȫ�ֱ���:
*   ����ֵ:����
*   ��ע:��Ҫ���ڱ����д���ֱ��ʹ����Ƭȫ������ģ�������ֲ������ͻ��
*  ������ֱ��ʹ��ʱ�Ծֲ�������̬����������
************************************************************************/
/*
static INT8U LocalMemLock(void)
{
    m_tmpMemInf.m_Flag.m_TmpMemFlag.m_Lock = OK;
    return OK;
}
*/
/************************************************************************
*   ����ԭ��:
*   ��������:�ͷŶ�ȫ�ֹ������������
*   ����˵��:
*   ȫ�ֱ���:
*   ����ֵ:����
*   ��ע:
************************************************************************/
static INT8U LocalMemUnLock(void)
{
    m_tmpMemInf.m_Flag.m_TmpMemFlag.m_Lock = ERR;
    return OK;
}
/*******************************************************************************
��������: ����س�ʼ��
��    ��:
�� �� ֵ:
*******************************************************************************/
static void MemManager_Init(INT8U *res)
{
    *res = LocalMemInit();
    LocalMemUnLock();
}
/*******************************************************************************
��������: ���뻺��
��    ��:
�� �� ֵ:
*******************************************************************************/
void *MemManager_Get(INT32U size,INT8U *res)
{
    void *p;
    //
    if(Mem_First==1)
    {
        MemManager_Init(res);
        Mem_First=0;
    }
    //�������
    if(size&0x00000003)
    {
        size=size>>2;//����4
        size+=1;
        size=size<<2;//����4
    }
    //
    p = LocalMalloc((INT16U)size);
    *res=OK;
    return p;
}
/*******************************************************************************
��������: �ͷŻ���
��    ��:
�� �� ֵ:
*******************************************************************************/
void MemManager_Free(void *pmem_blk,INT8U *res)
{
    //
    if(Mem_First==1)
    {
        MemManager_Init(res);
        Mem_First=0;
    }
    //
    *res = LocalMemFree(pmem_blk);
}
/*******************************************************************************
��������: ���뻺��
��    ��:
�� �� ֵ:
*******************************************************************************/
void *Mem1Manager_Get(INT32U size,INT8U *res)
{
    return MemManager_Get(size,res);
}
/*******************************************************************************
��������: �ͷŻ���
��    ��:
�� �� ֵ:
*******************************************************************************/
void Mem1Manager_Free(void *pmem_blk,INT8U *res)
{
    MemManager_Free(pmem_blk,res);
}
#endif
//-------------------------------------------------------------------------------
/*******************************************************************************
��������: �ڴ�ز���
��    ��:
�� �� ֵ:
*******************************************************************************/
void MemManager_Test(void)
{
    INT8U *pbuf[MEM_BLK_NBR];
    INT8U err;
    INT8U i,j;
    DebugOut("-----����ز��Կ�ʼ-----\r\n",strlen("-----����ز��Կ�ʼ-----\r\n"));
#ifdef UCOS_LIB
    for(i=1; i<=100; i++)
    {
        //����
        for(j=1; j<=MEM_BLK_NBR/2; j++)
        {
            pbuf[j-1]=MemManager_Get(100,&err);
            if(err!=OK)
            {
                DebugOut("������ڴ�����ʧ��\r\n",strlen("������ڴ�����ʧ��\r\n"));
                return;
            }
        }
        //ʹ��
        for(j=1; j<=MEM_BLK_NBR/2; j++)
        {
            sprintf((char *)pbuf[j-1],"������ڴ�����ɹ�-%03d-%03d\r\n",i,j);
            DebugOut((INT8S*)pbuf[j-1],strlen((char *)pbuf[j-1]));

            //�ͷ�
            MemManager_Free(pbuf[j-1],&err);

        }
    }
#else
    for(i=1; i<=100; i++)
    {
        //����
        for(j=1; j<=MEM_BLK_NBR/2; j++)
        {
            pbuf[j-1]=MemManager_Get(100,&err);
            if(err!=OK)
            {
                DebugOut("������ڴ�����ʧ��\r\n",strlen("������ڴ�����ʧ��\r\n"));
                return;
            }
        }
        //ʹ��
        for(j=1; j<=MEM_BLK_NBR/2; j++)
        {
            sprintf((char *)pbuf[j-1],"������ڴ�����ɹ�-%03d-%03d\r\n",i,j);
            DebugOut((INT8S*)pbuf[j-1],strlen((char *)pbuf[j-1]));
        }
        for(j=MEM_BLK_NBR; j>=1; j--)
        {
            MemManager_Free(pbuf[j-1],&err);
        }
    }
#endif
    if(i>100)
    {
        DebugOut("-----������ڴ���Գɹ�-----\r\n",strlen("-----������ڴ���Գɹ�-----\r\n"));
    }
    else
    {
        DebugOut("-----������ڴ����ʧ��-----\r\n",strlen("-----������ڴ����ʧ��-----\r\n"));
    }
}
//-------------------------------------------------------------------------------
