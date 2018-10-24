//-------------------------------------------------------------------------------//
//                           ������ͨ������޹�˾                                //
//                                ��Ȩ����                                       //
//                    ��ҵ��ַ��http://www.htong.com                             //
//                    ��������: RealView MDK-ARM Version 4.14                    //
//                    �� �� ��: RealView MDK-ARM Version 4.14                    //
//                    оƬ�ͺ�: STM32F103ZET6                                    //
//                    ��Ŀ����: HH-SPS ������Ŀ                                  //
//                    �ļ�����: MemManager.h                                     //
//                    ��    ��: ������                                           //
//                    ʱ    ��: 2014-01-17    �汾:  0.1                         //
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
//С��������
MEMMANAGER_EXT void *MemManager_Get(INT32U size,INT8U *res);
MEMMANAGER_EXT void MemManager_Free(void *pmem_blk,INT8U *res); 
//�󻺴�����
MEMMANAGER_EXT void *Mem1Manager_Get(INT32U size,INT8U *res);
MEMMANAGER_EXT void Mem1Manager_Free(void *pmem_blk,INT8U *res); 
//����
MEMMANAGER_EXT void MemManager_Test(void);
//-------------------------------------------------------------------------------
