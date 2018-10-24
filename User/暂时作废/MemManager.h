//-------------------------------------------------------------------------------//
//                           丹东华通测控有限公司                                //
//                                版权所有                                       //
//                    企业网址：http://www.htong.com                             //
//                    开发环境: RealView MDK-ARM Version 4.14                    //
//                    编 译 器: RealView MDK-ARM Version 4.14                    //
//                    芯片型号: STM32F103ZET6                                    //
//                    项目名称: HH-SPS 安防项目                                  //
//                    文件名称: MemManager.h                                     //
//                    作    者: 徐松亮                                           //
//                    时    间: 2014-01-17    版本:  0.1                         //
//-------------------------------------------------------------------------------//
//--------------------------------
#ifdef MEMMANAGER_GLOBAL
#define MEMMANAGER_EXT
#else
#define MEMMANAGER_EXT extern
#endif
//--------------------------------
//-------------------------------------------------------------------------------
typedef struct S_MEMMANAGER_VAR
{
    INT8U   i;
    INT8U   j;
    INT8U   k;
    INT8U   m;
    INT8U   *pi;
    INT8U   *pj;
    INT8U   *pk;
    INT8U   *pm;
    INT16U  i16;
    INT16U  j16;
    INT16U  k16;
    INT16U  m16;
    INT16U  *pi16;
    INT16U  *pj16;
    INT16U  *pk16;
    INT16U  *pm16;
    INT32U  i32;
    INT32U  j32;
    INT32U  k32;
    INT32U  m32;
    INT32U  *pi32;
    INT32U  *pj32;
    INT32U  *pk32;
    INT32U  *pm32;
    //76B
    INT8U  buf1[20];
    INT8U  buf2[20];
    INT8U  buf3[20];
    INT8U  buf4[20];
    //76+80=156B
    INT8U  buf5[100];
    //156+100=256B
} S_MEMMANAGER_VAR;
//------------------------------------------------------------
extern INT8U MemManager_Debug_Num;
//------------------------------------------------------------
//小缓存申请
MEMMANAGER_EXT void *MemManager_Get(INT32U size,INT8U *res);
MEMMANAGER_EXT void MemManager_Free(void *pmem_blk,INT8U *res); 
//大缓存申请
MEMMANAGER_EXT void *Mem1Manager_Get(INT32U size,INT8U *res);
MEMMANAGER_EXT void Mem1Manager_Free(void *pmem_blk,INT8U *res); 
//测试
MEMMANAGER_EXT void MemManager_Test(void);
//-------------------------------------------------------------------------------
