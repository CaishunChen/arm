/*
***********************************************************************************
*                    作    者: 徐松亮
*                    更新时间: 2015-06-03
* 当前问题: F4的10k内存申请数量如果设为1的视乎申请不了(大于1就好使,目前设为2).
***********************************************************************************
*/
//--------------------------------
#ifndef __MEMMANAGER_H
#define __MEMMANAGER_H
//--------------------------------
typedef enum E_MEM_MANAGER_TYPE
{
   E_MEM_MANAGER_TYPE_256B,
   E_MEM_MANAGER_TYPE_1KB,
   E_MEM_MANAGER_TYPE_2KB_BASIC,
   E_MEM_MANAGER_TYPE_5KB_BASIC,
   E_MEM_MANAGER_TYPE_10KB,
}E_MEM_MANAGER_TYPE;
//-------------------------------------------------------------------------------模式设定
#if   (defined(STM32F1))
//---->
#define MEM_256B_BLK_SIZE        256L
#define MEM_1KB_BLK_SIZE         1024L
#define MEM_2KB_BASIC_BLK_SIZE   (2*1024L)
#define MEM_5KB_BASIC_BLK_SIZE   (5*1024L)
#define MEM_10KB_BLK_SIZE        (10*1024L)
#if   (defined(PROJECT_SPI_SLAVE))
#define MEM_256B_BLK_NBR         10
#define MEM_1KB_BLK_NBR          5
#define MEM_2KB_BASIC_BLK_NBR    2
#define MEM_5KB_BASIC_BLK_NBR    2
#define MEM_10KB_BLK_NBR         0
#else
#define MEM_256B_BLK_NBR         16
#define MEM_1KB_BLK_NBR          5
#define MEM_2KB_BASIC_BLK_NBR    2
#define MEM_5KB_BASIC_BLK_NBR    2
#define MEM_10KB_BLK_NBR         1
#endif
//<----
#elif (defined(STM32F4))
//---->
// 4k
#define MEM_256B_BLK_NBR         16
#define MEM_256B_BLK_SIZE        256L
// 5k
#define MEM_1KB_BLK_NBR          5
#define MEM_1KB_BLK_SIZE         1024L
// 10k
#define MEM_2KB_BASIC_BLK_NBR    5
#define MEM_2KB_BASIC_BLK_SIZE   (2*1024L)
// 10k
#define MEM_5KB_BASIC_BLK_NBR    2
#define MEM_5KB_BASIC_BLK_SIZE   (5*1024L)
// 10k
#define MEM_10KB_BLK_NBR         2
#define MEM_10KB_BLK_SIZE        (10*1024L)
//
#define MEM_256B_BLK_ADDR_BEGIN  CCMDATARAM_BASE
#define MEM_256B_BLK_ADDR_END    (MEM_256B_BLK_ADDR_BEGIN+MEM_256B_BLK_NBR*MEM_256B_BLK_SIZE-1)
#define MEM_1KB_BLK_ADDR_BEGIN   (MEM_256B_BLK_ADDR_END+1)
#define MEM_1KB_BLK_ADDR_END     (MEM_1KB_BLK_ADDR_BEGIN+(MEM_1KB_BLK_NBR*MEM_1KB_BLK_SIZE)-1)
#define MEM_10KB_BLK_ADDR_BEGIN  (MEM_1KB_BLK_ADDR_END+1)
#define MEM_10KB_BLK_ADDR_END    (MEM_10KB_BLK_ADDR_BEGIN+(MEM_10KB_BLK_NBR*MEM_10KB_BLK_SIZE)-1)
//<----
#elif (defined(NRF51))
//---->

#if   ((defined(XKAP_ICARE_B_M)&&!defined(HAIER))||defined(PROJECT_NRF5X_BLE))
#define MEM_256B_BLK_NBR         6
#else
#define MEM_256B_BLK_NBR         4
#endif
#define MEM_256B_BLK_SIZE        256L

#if   ((defined(XKAP_ICARE_B_M)&&defined(HAIER))||defined(PROJECT_NRF5X_BLE))
#define MEM_1KB_BLK_NBR          0
#else
#define MEM_1KB_BLK_NBR          1
#endif
#define MEM_1KB_BLK_SIZE         1024L
#define MEM_2KB_BASIC_BLK_NBR    0
#define MEM_2KB_BASIC_BLK_SIZE   (2*1024L)
#define MEM_5KB_BASIC_BLK_NBR    0
#define MEM_5KB_BASIC_BLK_SIZE   (5*1024L)
#define MEM_10KB_BLK_NBR         0
#define MEM_10KB_BLK_SIZE        (10*1024L)
//<----
#elif (defined(NRF52))
//---->
#define MEM_256B_BLK_NBR         16
#define MEM_256B_BLK_SIZE        256L
#define MEM_1KB_BLK_NBR          5
#define MEM_1KB_BLK_SIZE         1024L
#define MEM_2KB_BASIC_BLK_NBR    2
#define MEM_2KB_BASIC_BLK_SIZE   (2*1024L)
#define MEM_5KB_BASIC_BLK_NBR    2
#define MEM_5KB_BASIC_BLK_SIZE   (5*1024L)
#define MEM_10KB_BLK_NBR         1
#define MEM_10KB_BLK_SIZE        (10*1024L)
//<----
#endif
//------------------------------------------------------------
//缓存的历史使用最大数量,与当前使用量
extern uint8_t MemManager_Mem_256B_MaxNum;
extern uint8_t MemManager_Mem_256B_Num;
extern uint8_t MemManager_Mem_1KB_MaxNum;
extern uint8_t MemManager_Mem_1KB_Num;
extern uint8_t MemManager_Mem_2KB_Basic_MaxNum;
extern uint8_t MemManager_Mem_2KB_Basic_Num;
extern uint8_t MemManager_Mem_5KB_Basic_MaxNum;
extern uint8_t MemManager_Mem_5KB_Basic_Num;
extern uint8_t MemManager_Mem_10KB_MaxNum;
extern uint8_t MemManager_Mem_10KB_Num;
//------------------------------------------------------------
//小缓存申请
void *MemManager_Get(E_MEM_MANAGER_TYPE mem_type);
void MemManager_Free(E_MEM_MANAGER_TYPE mem_type,void *pmem_blk);
//测试
void MemManager_Test(void);
//-------------------------------------------------------------------------------
#endif

