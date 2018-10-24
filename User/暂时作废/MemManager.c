//-------------------------------------------------------------------------------//
//                           丹东华通测控有限公司                                //
//                                版权所有                                       //
//                    企业网址：http://www.htong.com                             //
//                    开发环境: RealView MDK-ARM Version 4.14                    //
//                    编 译 器: RealView MDK-ARM Version 4.14                    //
//                    芯片型号: STM32F103ZET6                                    //
//                    项目名称: HH-SPS 安防项目                                  //
//                    文件名称: MemManager.c                                     //
//                    作    者: 徐松亮                                           //
//                    时    间: 2014-01-17    版本:  0.1                         //
//-------------------------------------------------------------------------------//
// 修改记录：
//    序号  日期     姓名     内容
//    1     20140319 徐松亮   STM32平台区分小块缓存池和大块缓存池
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
//-------------------------------------------------------------------------------STM32(uCOS平台)
//小数据块
#define MEM_BLK_NBR  16
#define MEM_BLK_SIZE 256L
static INT32U mem_buf[MEM_BLK_NBR*MEM_BLK_SIZE/4];
//
static  INT8U Mem_First=1;
INT8U MemManager_Debug_Num=0;
//------------------------------------------------------------------------------
#ifdef UCOS_LIB
//大数据块
#define MEM1_BLK_NBR  2
#define MEM1_BLK_SIZE 1024L
static INT32U mem1_buf[MEM_BLK_NBR*MEM_BLK_SIZE/4];
static MEM_POOL MemPool;
static MEM_POOL Mem1Pool;
/*******************************************************************************
函数功能: 缓存池初始化
参    数:
返 回 值:
*******************************************************************************/
static void MemManager_Init(INT8U *res)
{
    LIB_ERR err;
    CPU_SIZE_T allocation_err;
    //
    Mem_Init();
    //创建第一个缓存池
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
    //创建第二个缓存池
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
函数功能: 申请缓存
参    数:
返 回 值:
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
    //对齐矫正
    if(size&0x00000003)
    {
        size=size>>2;//除以4
        size+=1;
        size=size<<2;//乘以4
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
函数功能: 释放缓存
参    数:
返 回 值:
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
函数功能: 申请缓存
参    数:
返 回 值:
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
    //对齐矫正
    if(size&0x00000003)
    {
        size=size>>2;//除以4
        size+=1;
        size=size<<2;//乘以4
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
函数功能: 释放缓存
参    数:
返 回 值:
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
*  内存暂存管理结构体
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
//申请的单块内存单元
typedef struct
{
    INT8U*  starAddr;
    INT16U  areaSize;
} TmpMemStu;
//内存使用信息结构体
typedef struct
{
    INT16U refCount; //分配情况引用计数
    INT8U* idleAreaPt;
    TmpMemFlagUni m_Flag;
} TmpMemUseInfStu;

static TmpMemUseInfStu m_tmpMemInf;
/************************************************************************
*   函数原型:
*   函数功能:初始化当前内存管理
*   参数说明:
*   全局变量:
*   返回值:
*   备注:
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
*   函数原型:
*   函数功能:申请一片临时内存
*   参数说明:
*   全局变量:
*   返回值:
*   备注:
************************************************************************/
static void* LocalMalloc(INT16U byteSize)
{
    //分配信息头指针
    TmpMemStu* tmpUseInfHeadPt = (TmpMemStu*)((INT8U*)mem_buf + sizeof(mem_buf));
    //是否被锁定
    if(m_tmpMemInf.m_Flag.m_TmpMemFlag.m_Lock==OK)
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
*   函数原型:
*   函数功能:释放临时内存
*   参数说明:
*   全局变量:
*   返回值:
*   备注:如内存控制块结构体所述，该内存只能用于临时中需求暂存大的情况
*     所以释放时为类似弹栈，当释放中间的某一段默认将其后面申请的内存也释放
*     但释放时最好规范操作，申请释放成对出现
************************************************************************/
static INT8U LocalMemFree(void* tmpMemPt)
{
    INT8U tmpCnt = 0;
    //分配信息头指针
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
*   函数原型:
*   函数功能:锁定局部变量可以分配区域
*   参数说明:
*   全局变量:
*   返回值:保留
*   备注:主要是在报表中存在直接使用这片全局区域的，避免与局部的相冲突，
*  所以在直接使用时对局部变量动态申请区加锁
************************************************************************/
/*
static INT8U LocalMemLock(void)
{
    m_tmpMemInf.m_Flag.m_TmpMemFlag.m_Lock = OK;
    return OK;
}
*/
/************************************************************************
*   函数原型:
*   函数功能:释放对全局共享变量的锁定
*   参数说明:
*   全局变量:
*   返回值:保留
*   备注:
************************************************************************/
static INT8U LocalMemUnLock(void)
{
    m_tmpMemInf.m_Flag.m_TmpMemFlag.m_Lock = ERR;
    return OK;
}
/*******************************************************************************
函数功能: 缓存池初始化
参    数:
返 回 值:
*******************************************************************************/
static void MemManager_Init(INT8U *res)
{
    *res = LocalMemInit();
    LocalMemUnLock();
}
/*******************************************************************************
函数功能: 申请缓存
参    数:
返 回 值:
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
    //对齐矫正
    if(size&0x00000003)
    {
        size=size>>2;//除以4
        size+=1;
        size=size<<2;//乘以4
    }
    //
    p = LocalMalloc((INT16U)size);
    *res=OK;
    return p;
}
/*******************************************************************************
函数功能: 释放缓存
参    数:
返 回 值:
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
函数功能: 申请缓存
参    数:
返 回 值:
*******************************************************************************/
void *Mem1Manager_Get(INT32U size,INT8U *res)
{
    return MemManager_Get(size,res);
}
/*******************************************************************************
函数功能: 释放缓存
参    数:
返 回 值:
*******************************************************************************/
void Mem1Manager_Free(void *pmem_blk,INT8U *res)
{
    MemManager_Free(pmem_blk,res);
}
#endif
//-------------------------------------------------------------------------------
/*******************************************************************************
函数功能: 内存池测试
参    数:
返 回 值:
*******************************************************************************/
void MemManager_Test(void)
{
    INT8U *pbuf[MEM_BLK_NBR];
    INT8U err;
    INT8U i,j;
    DebugOut("-----缓存池测试开始-----\r\n",strlen("-----缓存池测试开始-----\r\n"));
#ifdef UCOS_LIB
    for(i=1; i<=100; i++)
    {
        //申请
        for(j=1; j<=MEM_BLK_NBR/2; j++)
        {
            pbuf[j-1]=MemManager_Get(100,&err);
            if(err!=OK)
            {
                DebugOut("缓存池内存申请失败\r\n",strlen("缓存池内存申请失败\r\n"));
                return;
            }
        }
        //使用
        for(j=1; j<=MEM_BLK_NBR/2; j++)
        {
            sprintf((char *)pbuf[j-1],"缓存池内存申请成功-%03d-%03d\r\n",i,j);
            DebugOut((INT8S*)pbuf[j-1],strlen((char *)pbuf[j-1]));

            //释放
            MemManager_Free(pbuf[j-1],&err);

        }
    }
#else
    for(i=1; i<=100; i++)
    {
        //申请
        for(j=1; j<=MEM_BLK_NBR/2; j++)
        {
            pbuf[j-1]=MemManager_Get(100,&err);
            if(err!=OK)
            {
                DebugOut("缓存池内存申请失败\r\n",strlen("缓存池内存申请失败\r\n"));
                return;
            }
        }
        //使用
        for(j=1; j<=MEM_BLK_NBR/2; j++)
        {
            sprintf((char *)pbuf[j-1],"缓存池内存申请成功-%03d-%03d\r\n",i,j);
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
        DebugOut("-----缓存池内存测试成功-----\r\n",strlen("-----缓存池内存测试成功-----\r\n"));
    }
    else
    {
        DebugOut("-----缓存池内存测试失败-----\r\n",strlen("-----缓存池内存测试失败-----\r\n"));
    }
}
//-------------------------------------------------------------------------------
