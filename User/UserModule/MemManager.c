/*
***********************************************************************************
*                    作    者: 徐松亮
*                    更新时间: 2015-06-03
***********************************************************************************
*/
#include "includes.h"
//#include "MemManager.h"
//-------------------------------------------------------------------------------
#if (defined(STM32F1)||defined(STM32F4))
#include "uctsk_Debug.h"
#define MEM_MANAGER_DEBUG_OUT(str) DebugOutStr(str)
#else
#define MEM_MANAGER_DEBUG_OUT(str)
#endif
//-------------------------------------------------------------------------------
//小数据块
#if   (defined(STM32F4))
static uint32_t *mem_256B_buf =  (uint32_t*)MEM_256B_BLK_ADDR_BEGIN;
static uint32_t *mem_1KB_buf  =  (uint32_t*)MEM_1KB_BLK_ADDR_BEGIN;
#if   (MEM_10KB_BLK_NBR!=0)
static uint32_t *mem_10KB_buf =  (uint32_t*)MEM_10KB_BLK_ADDR_BEGIN;
#endif
#else
static uint32_t mem_256B_buf[MEM_256B_BLK_NBR*MEM_256B_BLK_SIZE/4];
#if   (MEM_1KB_BLK_NBR!=0)
static uint32_t mem_1KB_buf[MEM_1KB_BLK_NBR*MEM_1KB_BLK_SIZE/4];
#endif
#if   (MEM_10KB_BLK_NBR!=0)
static uint32_t mem_10KB_buf[MEM_10KB_BLK_NBR*MEM_10KB_BLK_SIZE/4];
#endif
#endif

#if   (MEM_2KB_BASIC_BLK_NBR!=0)
static uint32_t mem_2KB_Basci_buf[MEM_2KB_BASIC_BLK_NBR*MEM_2KB_BASIC_BLK_SIZE/4];
#endif

#if   (MEM_5KB_BASIC_BLK_NBR!=0)
static uint32_t mem_5KB_Basci_buf[MEM_5KB_BASIC_BLK_NBR*MEM_5KB_BASIC_BLK_SIZE/4];
#endif
//
static  uint8_t Mem_First=1;
uint8_t MemManager_Mem_256B_Num     =  0;
uint8_t MemManager_Mem_256B_MaxNum  =  0;
uint8_t MemManager_Mem_1KB_Num      =  0;
uint8_t MemManager_Mem_1KB_MaxNum   =  0;
uint8_t MemManager_Mem_2KB_Basic_Num      =  0;
uint8_t MemManager_Mem_2KB_Basic_MaxNum   =  0;
uint8_t MemManager_Mem_5KB_Basic_Num      =  0;
uint8_t MemManager_Mem_5KB_Basic_MaxNum   =  0;
uint8_t MemManager_Mem_10KB_Num     =  0;
uint8_t MemManager_Mem_10KB_MaxNum  =  0;
//
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#if     (defined(OS_UCOSII) || defined(OS_UCOSIII))
//
MODULE_OS_MEM(Mem_256B);
MODULE_OS_MEM(Mem_1KB);
MODULE_OS_MEM(Mem_2KB_Basic);
MODULE_OS_MEM(Mem_5KB_Basic);
#if   (MEM_10KB_BLK_NBR!=0)
MODULE_OS_MEM(Mem_10KB);
#endif
/*******************************************************************************
函数功能: 缓存池初始化
参    数:
返 回 值:
*******************************************************************************/
static void MemManager_Init(void)
{
    MODULE_OS_MEM_INIT();
#if   (MEM_256B_BLK_NBR!=0) 
    MODULE_OS_MEM_CREATE(Mem_256B,"Mem_256B",mem_256B_buf,MEM_256B_BLK_NBR,MEM_256B_BLK_SIZE);
#endif
#if   (MEM_1KB_BLK_NBR!=0) 
    MODULE_OS_MEM_CREATE(Mem_1KB,"Mem_1KB",mem_1KB_buf,MEM_1KB_BLK_NBR,MEM_1KB_BLK_SIZE);
#endif
#if   (MEM_2KB_BASIC_BLK_NBR!=0) 
    MODULE_OS_MEM_CREATE(Mem_2KB_Basic,"Mem_2KB Basic",mem_2KB_Basci_buf,MEM_2KB_BASIC_BLK_NBR,MEM_2KB_BASIC_BLK_SIZE);
#endif
#if   (MEM_5KB_BASIC_BLK_NBR!=0) 
    MODULE_OS_MEM_CREATE(Mem_5KB_Basic,"Mem_5KB Basic",mem_5KB_Basci_buf,MEM_5KB_BASIC_BLK_NBR,MEM_5KB_BASIC_BLK_SIZE);
#endif
#if   (MEM_10KB_BLK_NBR!=0)    
    MODULE_OS_MEM_CREATE(Mem_10KB,"Mem_10KB",mem_10KB_buf,MEM_10KB_BLK_NBR,MEM_10KB_BLK_SIZE);
#endif
}
/*******************************************************************************
函数功能: 申请缓存
参    数:
返 回 值:
*******************************************************************************/
void *MemManager_Get(E_MEM_MANAGER_TYPE mem_type)
{
    void *p;
    MODULE_OS_ERR err;
    if(Mem_First==1)
    {
        MemManager_Init();
        Mem_First=0;
    }
    switch(mem_type)
    {
        case E_MEM_MANAGER_TYPE_256B:

            //
            while(1)
            {
                MODULE_OS_MEM_GET(Mem_256B,p,err);
                if(MODULE_OS_ERR_NONE==err)
                {
                    break;
                }
                else
                {
                    MODULE_OS_DELAY_MS(100);
                    p=NULL;
                }
            }
            //
            MemManager_Mem_256B_Num++;
            if(MemManager_Mem_256B_MaxNum<MemManager_Mem_256B_Num)
            {
                MemManager_Mem_256B_MaxNum=MemManager_Mem_256B_Num;
            }
            break;
        case E_MEM_MANAGER_TYPE_1KB:
            //
            while(1)
            {
                MODULE_OS_MEM_GET(Mem_1KB,p,err);
                if(MODULE_OS_ERR_NONE==err)
                {
                    break;
                }
                else
                {
                    MODULE_OS_DELAY_MS(100);
                    p=NULL;
                }
            }
            //
            MemManager_Mem_1KB_Num++;
            if(MemManager_Mem_1KB_MaxNum<MemManager_Mem_1KB_Num)
            {
                MemManager_Mem_1KB_MaxNum=MemManager_Mem_1KB_Num;
            }
            break;
        case E_MEM_MANAGER_TYPE_2KB_BASIC:
            //
            while(1)
            {
                MODULE_OS_MEM_GET(Mem_2KB_Basic,p,err);
                if(MODULE_OS_ERR_NONE==err)
                {
                    break;
                }
                else
                {
                    MODULE_OS_DELAY_MS(100);
                    p=NULL;
                }
            }
            //
            MemManager_Mem_2KB_Basic_Num++;
            if(MemManager_Mem_2KB_Basic_MaxNum<MemManager_Mem_2KB_Basic_Num)
            {
                MemManager_Mem_2KB_Basic_MaxNum=MemManager_Mem_2KB_Basic_Num;
            }
            break;
        case E_MEM_MANAGER_TYPE_5KB_BASIC:
            //
            while(1)
            {
                MODULE_OS_MEM_GET(Mem_5KB_Basic,p,err);
                if(MODULE_OS_ERR_NONE==err)
                {
                    break;
                }
                else
                {
                    MODULE_OS_DELAY_MS(100);
                    p=NULL;
                }
            }
            //
            MemManager_Mem_5KB_Basic_Num++;
            if(MemManager_Mem_5KB_Basic_MaxNum<MemManager_Mem_5KB_Basic_Num)
            {
                MemManager_Mem_5KB_Basic_MaxNum=MemManager_Mem_5KB_Basic_Num;
            }
            break;
#if   (MEM_10KB_BLK_NBR!=0)            
        case E_MEM_MANAGER_TYPE_10KB:
            while(1)
            {
                MODULE_OS_MEM_GET(Mem_10KB,p,err);
                if(MODULE_OS_ERR_NONE==err)
                {
                    break;
                }
                else
                {
                    MODULE_OS_DELAY_MS(100);
                    p=NULL;
                }
            }
            //
            MemManager_Mem_10KB_Num++;
            if(MemManager_Mem_10KB_MaxNum<MemManager_Mem_10KB_Num)
            {
                MemManager_Mem_10KB_MaxNum=MemManager_Mem_10KB_Num;
            }
            break;
#endif            
        default:
            break;
    }
    return p;
}
/*******************************************************************************
函数功能: 释放缓存
参    数:
返 回 值:
*******************************************************************************/
void MemManager_Free(E_MEM_MANAGER_TYPE mem_type,void *pmem_blk)
{
    MODULE_OS_ERR err;
	  err = err;
    //
    if(Mem_First==1)
    {
        MemManager_Init();
        Mem_First=0;
    }
    switch(mem_type)
    {
        case E_MEM_MANAGER_TYPE_256B:
            //
            MODULE_OS_MEM_PUT(Mem_256B,pmem_blk,err);
            MemManager_Mem_256B_Num--;
            break;
        case E_MEM_MANAGER_TYPE_1KB:
            //
            MODULE_OS_MEM_PUT(Mem_1KB,pmem_blk,err);
            MemManager_Mem_1KB_Num--;
            break;
        case E_MEM_MANAGER_TYPE_2KB_BASIC:
            //
            MODULE_OS_MEM_PUT(Mem_2KB_Basic,pmem_blk,err);
            MemManager_Mem_2KB_Basic_Num--;
            break;
        case E_MEM_MANAGER_TYPE_5KB_BASIC:
            //
            MODULE_OS_MEM_PUT(Mem_5KB_Basic,pmem_blk,err);
            MemManager_Mem_5KB_Basic_Num--;
            break;
#if   (MEM_10KB_BLK_NBR!=0)            
        case E_MEM_MANAGER_TYPE_10KB:
            //
            MODULE_OS_MEM_PUT(Mem_10KB,pmem_blk,err);
            MemManager_Mem_10KB_Num--;
            break;
#endif            
        default:
            break;
    }
}
#elif     (defined(OS_NULL) || defined(OS_FREERTOS))
//内存使用信息结构体
typedef struct
{
    uint32_t  useBitmap;
} MemManager_S_Init;

#if   (MEM_256B_BLK_NBR!=0)
static MemManager_S_Init Mem_256B;
#endif
#if   (MEM_1KB_BLK_NBR!=0)
static MemManager_S_Init Mem_1KB;
#endif
#if   (MEM_2KB_BASIC_BLK_NBR!=0)
static MemManager_S_Init Mem_2KB_Basic;
#endif
#if   (MEM_5KB_BASIC_BLK_NBR!=0)
static MemManager_S_Init Mem_5KB_Basic;
#endif
#if   (MEM_10KB_BLK_NBR!=0)
static MemManager_S_Init Mem_10KB;
#endif
/*******************************************************************************
函数功能: 缓存池初始化
参    数:
返 回 值:
*******************************************************************************/
static void MemManager_Init(void)
{
#if   (MEM_256B_BLK_NBR!=0)
    Mem_256B.useBitmap        =  0;
#endif
#if   (MEM_1KB_BLK_NBR!=0)
    Mem_1KB.useBitmap         =  0;
#endif
#if   (MEM_2KB_BASIC_BLK_NBR!=0)
    Mem_2KB_Basic.useBitmap   =  0;
#endif
#if   (MEM_5KB_BASIC_BLK_NBR!=0)
    Mem_5KB_Basic.useBitmap   =  0;
#endif
#if   (MEM_10KB_BLK_NBR!=0)    
    Mem_10KB.useBitmap        =  0;
#endif
}
/*******************************************************************************
函数功能: 申请缓存
参    数:
返 回 值:
*******************************************************************************/
void *MemManager_Get(E_MEM_MANAGER_TYPE mem_type)
{
    void *p;
    uint8_t i=0;
    //
    if(Mem_First==1)
    {
        MemManager_Init();
        Mem_First=0;
    }
    //
    switch(mem_type)
    {
#if	(MEM_256B_BLK_NBR!=0)
        case E_MEM_MANAGER_TYPE_256B:
            //提取空余
            while(1)
            {
                if((Mem_256B.useBitmap & (1L<<i))==0)
                {
                    Mem_256B.useBitmap |= (1L<<i);  
                    break;
                }
                else
                {
                    i++;
                }
                if(i>=MEM_256B_BLK_NBR)
                {
                    break;
                }
            }
            if(i>=MEM_256B_BLK_NBR)
            {
               break;
            }
            else
            {
               p = (void*)(mem_256B_buf+(MEM_256B_BLK_SIZE*i)/4);
            }
            MemManager_Mem_256B_Num++;
            if(MemManager_Mem_256B_MaxNum<MemManager_Mem_256B_Num)
            {
                MemManager_Mem_256B_MaxNum=MemManager_Mem_256B_Num;
            }
            break;
#endif
#if	(MEM_1KB_BLK_NBR!=0)
        case E_MEM_MANAGER_TYPE_1KB:
            while(1)
            {
                if((Mem_1KB.useBitmap & (1L<<i))==0)
                {
                    Mem_1KB.useBitmap |= (1L<<i);  
                    break;
                }
                else
                {
                    i++;
                }
                if(i>=MEM_1KB_BLK_NBR)
                {
                    break;
                }
            }
            if(i>=MEM_1KB_BLK_NBR)
            {
               break;
            }
            else
            {
               p = (void*)(mem_1KB_buf+(MEM_1KB_BLK_SIZE*i)/4);
            }
            MemManager_Mem_1KB_Num++;
            if(MemManager_Mem_1KB_MaxNum<MemManager_Mem_1KB_Num)
            {
                MemManager_Mem_1KB_MaxNum=MemManager_Mem_1KB_Num;
            }
            break;
#endif
#if	(MEM_2KB_BASIC_BLK_NBR!=0)
        case E_MEM_MANAGER_TYPE_2KB_BASIC:
            while(1)
            {
                if((Mem_2KB_Basic.useBitmap & (1L<<i))==0)
                {
                    Mem_2KB_Basic.useBitmap |= (1L<<i);  
                    break;
                }
                else
                {
                    i++;
                }
                if(i>=MEM_2KB_BASIC_BLK_NBR)
                {
                    break;
                }
            }
            if(i>=MEM_2KB_BASIC_BLK_NBR)
            {
               break;
            }
            else
            {
               p = (void*)(mem_2KB_Basci_buf+(MEM_2KB_BASIC_BLK_SIZE*i)/4);
            }
            MemManager_Mem_2KB_Basic_Num++;
            if(MemManager_Mem_2KB_Basic_MaxNum<MemManager_Mem_2KB_Basic_Num)
            {
                MemManager_Mem_2KB_Basic_MaxNum=MemManager_Mem_2KB_Basic_Num;
            }
            break;
#endif
#if	(MEM_5KB_BASIC_BLK_NBR!=0)
        case E_MEM_MANAGER_TYPE_5KB_BASIC:
            while(1)
            {
                if((Mem_5KB_Basic.useBitmap & (1L<<i))==0)
                {
                    Mem_5KB_Basic.useBitmap |= (1L<<i);  
                    break;
                }
                else
                {
                    i++;
                }
                if(i>=MEM_5KB_BASIC_BLK_NBR)
                {
                    break;
                }
            }
            if(i>=MEM_5KB_BASIC_BLK_NBR)
            {
               break;
            }
            else
            {
               p = (void*)(mem_5KB_Basci_buf+(MEM_5KB_BASIC_BLK_SIZE*i)/4);
            }
            MemManager_Mem_5KB_Basic_Num++;
            if(MemManager_Mem_5KB_Basic_MaxNum<MemManager_Mem_5KB_Basic_Num)
            {
                MemManager_Mem_5KB_Basic_MaxNum=MemManager_Mem_5KB_Basic_Num;
            }
            break;
#endif
#if   (MEM_10KB_BLK_NBR!=0)            
        case E_MEM_MANAGER_TYPE_10KB:
            while(1)
            {
                if((Mem_10KB.useBitmap & (1L<<i))==0)
                {
                    Mem_10KB.useBitmap |= (1L<<i);  
                    break;
                }
                else
                {
                    i++;
                }
                if(i>=MEM_10KB_BLK_NBR)
                {
                    break;
                }
            }
            if(i>=MEM_10KB_BLK_NBR)
            {
               break;
            }
            else
            {
               p = (void*)(mem_10KB_buf + (MEM_10KB_BLK_SIZE*i)/4);
            }
            MemManager_Mem_10KB_Num++;
            if(MemManager_Mem_10KB_MaxNum<MemManager_Mem_10KB_Num)
            {
                MemManager_Mem_10KB_MaxNum=MemManager_Mem_10KB_Num;
            }
            break;
#endif            
        default:
            break;
    }
    return p;
}
/*******************************************************************************
函数功能: 释放缓存
参    数:
返 回 值:
*******************************************************************************/
//void MemManager_Free(void *pmem_blk,uint8_t *res)
void MemManager_Free(E_MEM_MANAGER_TYPE mem_type,void *pmem_blk)
{
    uint8_t i;
    //
    if(Mem_First==1)
    {
        MemManager_Init();
        Mem_First=0;
    }
    switch(mem_type)
    {
#if   (MEM_256B_BLK_NBR!=0) 
        case E_MEM_MANAGER_TYPE_256B:
            i= ((uint8_t*)pmem_blk - (uint8_t*)mem_256B_buf)/MEM_256B_BLK_SIZE;
            if(i<MEM_256B_BLK_NBR)
            {
               Mem_256B.useBitmap &= ~(1L<<i);  
            }
            MemManager_Mem_256B_Num--;
            break;
#endif
#if   (MEM_1KB_BLK_NBR!=0) 
        case E_MEM_MANAGER_TYPE_1KB:
            i= ((uint8_t*)pmem_blk - (uint8_t*)mem_1KB_buf)/MEM_1KB_BLK_SIZE;
            if(i<MEM_1KB_BLK_NBR)
            {
               Mem_1KB.useBitmap &= ~(1L<<i);  
            }
            MemManager_Mem_1KB_Num--;
            break;
#endif
#if   (MEM_2KB_BASIC_BLK_NBR!=0) 
        case E_MEM_MANAGER_TYPE_2KB_BASIC:
            i= ((uint8_t*)pmem_blk - (uint8_t*)mem_2KB_Basci_buf)/MEM_2KB_BASIC_BLK_SIZE;
            if(i<MEM_2KB_BASIC_BLK_NBR)
            {
               Mem_2KB_Basic.useBitmap &= ~(1L<<i);  
            }
            MemManager_Mem_2KB_Basic_Num--;
            break;
#endif
#if   (MEM_5KB_BASIC_BLK_NBR!=0) 
        case E_MEM_MANAGER_TYPE_5KB_BASIC:
            i= ((uint8_t*)pmem_blk - (uint8_t*)mem_5KB_Basci_buf)/MEM_5KB_BASIC_BLK_SIZE;
            if(i<MEM_5KB_BASIC_BLK_NBR)
            {
               Mem_5KB_Basic.useBitmap &= ~(1L<<i);  
            }
            MemManager_Mem_5KB_Basic_Num--;
            break;
#endif
#if   (MEM_10KB_BLK_NBR!=0)            
        case E_MEM_MANAGER_TYPE_10KB:
            i= ((uint8_t*)pmem_blk - (uint8_t*)mem_10KB_buf)/MEM_10KB_BLK_SIZE;
            if(i<MEM_10KB_BLK_NBR)
            {
               Mem_10KB.useBitmap &= ~(1L<<i);  
            }
            MemManager_Mem_10KB_Num--;
            break;
#endif      
        default:
            break;
    }
    pmem_blk = NULL;
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
    uint8_t *pbuf[MEM_256B_BLK_NBR];
    uint8_t err;
    uint8_t i,j;	
    MEM_MANAGER_DEBUG_OUT("-----缓存池测试开始-----\r\n");
    for(i=1; i<=100; i++)
    {
        //申请
        for(j=1; j<=MEM_256B_BLK_NBR/2; j++)
        {
            pbuf[j-1]=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
            if(err!=OK)
            {
                MEM_MANAGER_DEBUG_OUT("缓存池内存申请失败\r\n");
                return;
            }
        }
        //使用
        for(j=1; j<=MEM_256B_BLK_NBR/2; j++)
        {
            sprintf((char *)pbuf[j-1],"缓存池内存申请成功-%03d-%03d\r\n",i,j);
            MEM_MANAGER_DEBUG_OUT((int8_t*)pbuf[j-1]);

            //释放
            MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf[j-1]);

        }
    }
    if(i>100)
    {
        MEM_MANAGER_DEBUG_OUT("-----缓存池内存测试成功-----\r\n");
    }
    else
    {
        MEM_MANAGER_DEBUG_OUT("-----缓存池内存测试失败-----\r\n");
    }
}
//-------------------------------------------------------------------------------
