/*
*********************************************************************************
*                    作    者: 徐松亮
*                    更新时间: 2016-04-26
*********************************************************************************
*/
#ifdef __cplusplus
extern "C" {
#endif
//
#include "Module_GameBucket.h"
#include "uctsk_Debug.h"
#include "MemManager.h"
//
#ifdef __cplusplus
}
#endif
//------------------------------------------------------------------------------库函数
//#include "targetver.h"
#include <stdio.h>
//#include <iostream>
#include <queue>
#include <cassert>
//------------------------------------------------------------------------------宏定义
// 水桶数量
const int BUCKETS_COUNT = 3;
//------------------------------------------------------------------------------数据结构
// 倒水动作
typedef struct tagACTION
{
    int from;
    int to;
    int water;
} ACTION;
// 水桶状态
struct BucketState
{
    BucketState();
    BucketState(const int *buckets);
    BucketState(const BucketState& state);
    BucketState& operator=(const BucketState& state);
    bool IsSameState(const BucketState& state) const;
    bool operator == (const BucketState& state);
    void SetAction(int w, int f, int t);
    void SetBuckets(const int *buckets);
    bool CanTakeDumpAction(int from, int to);
    bool IsBucketEmpty(int bucket);
    bool IsBucketFull(int bucket);
    void PrintStates();
    bool IsFinalState();
    bool DumpWater(int from, int to, BucketState& next);
    //----------
    // 水桶状态
    int bucket_s[BUCKETS_COUNT];
    // 倒水动作
    ACTION curAction;
};
//------------------------------------------------------------------------------静态变量
// 水桶容量
int bucket_capicity[BUCKETS_COUNT]      =   {8, 5, 3};
// 初始状态
int bucket_init_state[BUCKETS_COUNT]    =   {8, 0, 0};
// 终止状态
int bucket_final_state[BUCKETS_COUNT]   =   {4, 4, 0};
//------------------------------------------------------------------------------类定义

BucketState::BucketState()
{
    SetBuckets(bucket_init_state);
    SetAction(8, -1, 0);
}

BucketState::BucketState(const int *buckets)
{
    SetBuckets(buckets);
    SetAction(8, -1, 0);
}

BucketState::BucketState(const BucketState& state)
{
    SetBuckets((const int *)state.bucket_s);
    SetAction(state.curAction.water, state.curAction.from, state.curAction.to);
}

BucketState& BucketState::operator=(const BucketState& state)
{
    SetBuckets((const int *)state.bucket_s);
    SetAction(state.curAction.water, state.curAction.from, state.curAction.to);
    return *this;
}

bool BucketState::IsSameState(const BucketState& state) const
{
    for(int i = 0; i < BUCKETS_COUNT; ++i)
    {
        if(bucket_s[i] != state.bucket_s[i])
            return false;
    }

    return true;
}

bool BucketState::operator == (const BucketState& state)
{
    for(int i = 0; i < BUCKETS_COUNT; ++i)
    {
        if(bucket_s[i] != state.bucket_s[i])
            return false;
    }

    return true;
}

void BucketState::SetAction(int w, int f, int t)
{
    curAction.water = w;
    curAction.from  = f;
    curAction.to    = t;
}

void BucketState::SetBuckets(const int *buckets)
{
    for(int i = 0; i < BUCKETS_COUNT; ++i)
    {
        bucket_s[i] = buckets[i];
    }
}

bool BucketState::IsBucketEmpty(int bucket)
{
    assert((bucket >= 0) && (bucket < BUCKETS_COUNT));

    return (bucket_s[bucket] == 0);
}

bool BucketState::IsBucketFull(int bucket)
{
    assert((bucket >= 0) && (bucket < BUCKETS_COUNT));

    return (bucket_s[bucket] >= bucket_capicity[bucket]);
}

void BucketState::PrintStates()
{
#if 0
    std::cout << "Dump " << curAction.water << " water from "
       << curAction.from + 1 << " to " << curAction.to + 1 << ", ";
    std::cout << "buckets water states is : ";

    for(int i = 0; i < BUCKETS_COUNT; ++i)
    {
      std::cout << bucket_s[i] << " ";
    }
    std::cout << std::endl;
#else    
    char *pbuf;
    pbuf = (char *)MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    sprintf(pbuf,"Dump %d water from %d to %d, buckets water states is : %d %d %d\r\n",\
        curAction.water,curAction.from + 1,curAction.to + 1,bucket_s[0],bucket_s[1],bucket_s[2]);
    ModuleGameBucket_DebugPrintf((int8_t*)pbuf);
	  MODULE_OS_DELAY_MS(10);
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
#endif	
}

bool BucketState::IsFinalState()
{
    return IsSameState(BucketState(bucket_final_state));
}

bool BucketState::CanTakeDumpAction(int from, int to)
{
    assert((from >= 0) && (from < BUCKETS_COUNT));
    assert((to >= 0) && (to < BUCKETS_COUNT));
    /*不是同一个桶，且from桶中有水，且to桶中不满*/

    if( (from != to)
        && !IsBucketEmpty(from)
        && !IsBucketFull(to) )
    {
        return true;
    }

    return false;
}

/*从from到to倒水，返回实际倒水体积*/
bool BucketState::DumpWater(int from, int to, BucketState& next)
{
    next.SetBuckets(bucket_s);
    int dump_water = bucket_capicity[to] - next.bucket_s[to];
    if(next.bucket_s[from] >= dump_water)
    {
        next.bucket_s[to] += dump_water;
        next.bucket_s[from] -= dump_water;
    }
    else
    {
        next.bucket_s[to] += next.bucket_s[from];
        dump_water = next.bucket_s[from];
        next.bucket_s[from] = 0;
    }
    if(dump_water > 0) /*是一个有效的倒水动作?*/
    {
        next.SetAction(dump_water, from, to);
        return true;
    }

    return false;
}
//------------------------------------------------------------------------------函数
bool IsSameBucketState(BucketState state1, BucketState state2)
{
    return state1.IsSameState(state2);
}

bool IsProcessedState(std::deque<BucketState>& states, const BucketState& newState)
{
    std::deque<BucketState>::iterator it = states.end();

    it = find_if( states.begin(), states.end(),
                  std::bind2nd(std::ptr_fun(IsSameBucketState), newState) );

    //it = find_if(states.begin(), states.end(), std::bind1st(std::mem_fun_ref(&BucketState::IsSameState), newState));

    return (it != states.end());
}

void PrintResult(std::deque<BucketState>& states)
{
#if 0
    std::cout << "Find Result : " << std::endl;
    for_each(states.begin(), states.end(),
           std::mem_fun_ref(&BucketState::PrintStates));
    std::cout << std::endl << std::endl;
#else
    char *pbuf;
    pbuf = (char *)MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    sprintf(pbuf,"Find Result : \r\n");
    ModuleGameBucket_DebugPrintf((int8_t*)pbuf);
    //
    for_each(states.begin(), states.end(),\
             std::mem_fun_ref(&BucketState::PrintStates));
    //
    sprintf(pbuf,"\r\n\r\n");
    ModuleGameBucket_DebugPrintf((int8_t*)pbuf);
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
#endif
}

void SearchState(std::deque<BucketState>& states);
// 搜索递归
void SearchStateOnAction(std::deque<BucketState>& states, BucketState& current, int from, int to)
{
    if(current.CanTakeDumpAction(from, to))
    {
        BucketState next;
        /*从from到to倒水，如果成功，返回倒水后的状态*/
        bool bDump = current.DumpWater(from, to, next);
        if(bDump && !IsProcessedState(states, next))
        {
            states.push_back(next);
            SearchState(states);
            states.pop_back();
        }
    }
}
// 状态搜索核心算法
void SearchState(std::deque<BucketState>& states)
{
    // 每次都从当前状态开始
    BucketState current = states.back(); 
	// 如果是最终状态,则打印状态
    if(current.IsFinalState())
    {
        PrintResult(states);
        return;
    }
    // 使用两重循环排列组合6种倒水状态
    for(int j = 0; j < BUCKETS_COUNT; ++j)
    {
        for(int i = 0; i < BUCKETS_COUNT; ++i)
        {
            SearchStateOnAction(states, current, i, j);
        }
    }
}
#include <rt_heap.h>
#include <stdint.h>
void ModuleGameBucket_DebugTestOnOff(uint8_t OnOff)
{
    uintptr_t *pbuf;
    pbuf = (uintptr_t *)MemManager_Get(E_MEM_MANAGER_TYPE_1KB);
    _init_alloc((uintptr_t)pbuf, (uintptr_t)&pbuf[1024-1]);
	
    // 用deque容器 建立类型实体
    std::deque<BucketState> states;	
    // 建立一个初始化的水桶状态实体
    BucketState init;
    // 容器尾部加入初始化数据
    states.push_back(init);
	// 搜索倒水状态
    SearchState(states);
    // 如果无解则声明
    assert(states.size() == 1);
	//
	MemManager_Free(E_MEM_MANAGER_TYPE_1KB,pbuf);
}
//-------------------------------------------------------------------------------

