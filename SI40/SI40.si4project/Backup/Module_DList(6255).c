/**
  ******************************************************************************
  * @file    Module_DList.c
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  ******************************************************************************
  * @attention
  *
  * GNU General Public License (GPL)
  *
  * <h2><center>&copy; COPYRIGHT 2017 XSLXHN</center></h2>
  ******************************************************************************
  */
  
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Module_DList.h"
#include "uctsk_Debug.h"
#include "MemManager.h"
/* Private typedef -----------------------------------------------------------*/
typedef struct NODE
{
    //键值(用于排序与查找)
    uint32_t KeyValue;
    //链表下一个结点
    struct NODE *pNextNode;
    //链表前一个结点
    struct NODE *pPrevNode;
	//-----
	//参数指针
    void *pPara;
} Node;
/* Private define ------------------------------------------------------------*/
#define	MODULE_DLIST_NODE_NUM_MAX			25600	// 	每个结点16B 25600占用400kB空间


#define MODULE_DLIST_LIST_MEM_ADDR_BEGIN	0
#define MODULE_DLIST_LIST_MEM_ADDR_END		MODULE_DLIST_LIST_MEM_ADDR_BEGIN+(MODULE_DLIST_NODE_NUM_MAX*sizeof(Node))-1
#define MODULE_DLIST_LIST_MEM_ADDR_SIZE		MODULE_DLIST_LIST_MEM_ADDR_END-MODULE_DLIST_LIST_MEM_ADDR_BEGIN+1

#define MODULE_DLIST_PARA_MEM_ADDR_BEGIN	MODULE_DLIST_LIST_MEM_ADDR_END+1
#define MODULE_DLIST_PARA_MEM_SIZE			16		//	参数每单元去占用空间
#define MODULE_DLIST_PARA_MEM_ADDR_END		MODULE_DLIST_PARA_MEM_ADDR_BEGIN+(MODULE_DLIST_NODE_NUM_MAX*MODULE_DLIST_PARA_MEM_SIZE)-1
#define MODULE_DLIST_PARA_MEM_ADDR_SIZE		MODULE_DLIST_PARA_MEM_ADDR_END-MODULE_DLIST_PARA_MEM_ADDR_BEGIN+1
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint32_t ModuleDList_ListMem_Num		=	0; 	//	有效数量 		1起始
static uint32_t ModuleDList_ListMem_EndNum	=	0;	//	末尾编号		1起始
static uint8_t ModuleDList_DebugTest_Enable	=	0;	//	
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @brief   格式化链表空间
 * @note    
 * @param   None
 * @return  None
 */
void ModuleDList_Format(void)
{
	MODULE_MEMORY_S_EXTFLASH s1; 
	// 格式化链表区
	s1.addr	=	MODULE_DLIST_LIST_MEM_ADDR_BEGIN;
	s1.len	=	MODULE_DLIST_LIST_MEM_ADDR_SIZE;
	Module_Memory_App(MODULE_MEMORY_APP_CMD_EXTFLASH_EARSE,NULL,(uint8_t*)&s1);
	// 格式化参数区
	s1.addr	=	MODULE_DLIST_PARA_MEM_ADDR_BEGIN;
	s1.len	=	MODULE_DLIST_PARA_MEM_ADDR_SIZE;
	Module_Memory_App(MODULE_MEMORY_APP_CMD_EXTFLASH_EARSE,NULL,(uint8_t*)&s1);
}

/**
 * @brief   获取链表空间
 * @note    计算两个变量值: ModuleDList_ListMem_Num ModuleDList_ListMem_EndNum
 * @param   None
 * @return  None
 */
void ModuleDList_Mem_Init(void)
{
	uint32_t i32;
	Node *ps_node;
	MODULE_MEMORY_S_EXTFLASH *ps_extFlash; 
	ps_node 	= 	MemManager_Get(E_MEM_MANAGER_TYPE_256B);
	ps_extFlash	=	(MODULE_MEMORY_S_EXTFLASH*)&ps_node[1];
	// 计算 ModuleDList_ListMem_Num
	// 计算 ModuleDList_ListMem_EndNum
	ModuleDList_ListMem_Num		=	0;
	ModuleDList_ListMem_EndNum	=	0;
	for(i32=0;i32<MODULE_DLIST_NODE_NUM_MAX;i32++)
	{
		// 提取结构体
		ps_extFlash->addr	=	MODULE_DLIST_LIST_MEM_ADDR_BEGIN	+	i32*sizeof(Node);
		ps_extFlash->len	=	sizeof(Node);
		Module_Memory_App(MODULE_MEMORY_APP_CMD_EXTFLASH_READ,(uint8_t*)ps_node,(uint8_t*)ps_extFlash);
		if(ps_node->pPara!=0xFFFFFFFF)
		{
			ModuleDList_ListMem_Num++;
			ModuleDList_ListMem_EndNum=i32+1;
		}
	}
	
	MemManager_Free(E_MEM_MANAGER_TYPE_256B, ps_node);
}

/**
 * @brief   获取指定编号的结点数据
 * @note    
 * @param   None
 * @return  OK ERR
 */

uint8_t ModuleDList_Node_Get(uint32_t addr,Node* pNode)
{
	MODULE_MEMORY_S_EXTFLASH s_extFlash; 
	
}

/**
 * @brief   获取链表空间
 * @note    
 * @param   None
 * @return  OK ERR
 */
uint8_t ModuleDList_Mem_Get(uint32_t *pAddr)
{
	// 如果 ModuleDList_ListMem_Num 未达到最大，则向后取空间
	if(ModuleDList_ListMem_EndNum < MODULE_DLIST_NODE_NUM_MAX)
	{
		*pAddr = MODULE_DLIST_LIST_MEM_ADDR_BEGIN	+	ModuleDList_ListMem_EndNum*sizeof(Node);
		ModuleDList_ListMem_EndNum++;
		return OK;
	}
	// 如果 ModuleDList_ListMem_Num 已达到最大，则选取空闲空间
	else if(ModuleDList_ListMem_Num<MODULE_DLIST_NODE_NUM_MAX)
	{
		// 整理碎片
		ModuleDList_Mem_Defrag();
		// 重新判定
		if(ModuleDList_ListMem_EndNum < MODULE_DLIST_NODE_NUM_MAX)
		{
			*pAddr = MODULE_DLIST_LIST_MEM_ADDR_BEGIN	+	ModuleDList_ListMem_EndNum*sizeof(Node);
			ModuleDList_ListMem_EndNum++;
			return OK;
		}
		else
		{
			return ERR;
		}
	}
	// 如果 已经存满
	else
	{
		return ERR;
	}
}

/**
 * @brief   释放链表空间
 * @note    
 * @param   None
 * @return  None
 */
uint8_t ModuleDList_Mem_Free(uint32_t addr)
{
	Node *ps_node;
	MODULE_MEMORY_S_EXTFLASH *ps_extFlash; 
	ps_node 	= 	MemManager_Get(E_MEM_MANAGER_TYPE_256B);
	ps_extFlash	=	(MODULE_MEMORY_S_EXTFLASH*)&ps_node[1];
	// 释放参数区(全部清0)
	ps_extFlash->addr	=	;
	ps_extFlash->len	=	sizeof(Node);
	// 释放链表区(全部清0)
	ps_extFlash->addr	=	addr;
	ps_extFlash->len	=	sizeof(Node);
	memset((char*)ps_node,0,sizeof(Node));
	Module_Memory_App(MODULE_MEMORY_APP_CMD_EXTFLASH_WRITE,(uint8_t*)ps_node,(uint8_t*)ps_extFlash);
	MemManager_Free(E_MEM_MANAGER_TYPE_256B, ps_node);
}

/**
 * @brief   整理碎片
 * @note    
 * @param   None
 * @return  None
 */
void ModuleDList_Mem_Defrag(void)
{
	// 恢复链表区碎片
	// 恢复参数区碎片
}

/*
*********************************************************************************
* 函数功能: 1,初始化线性表，即置单链表的表头指针为空
*********************************************************************************
*/
void ModuleDList_InitList(Node **pNode)
{
    *pNode=NULL;
    if(ModuleDList_DebugTest_Enable==1)
    {
        ModuleDList_DebugPrintf("DList Init OK.\r\n");
    }
}
/*
*********************************************************************************
* 函数功能: 2.创建线性表，此函数输入负数终止读取数据
*********************************************************************************
*/
Node *ModuleDList_CreatList(Node *pHead,uint32_t KeyValue)
{
    Node *p1,*p2;
    p1=p2=(Node *)malloc(sizeof(Node));
    if(p1 == NULL || p2 ==NULL)
    {
        if(ModuleDList_DebugTest_Enable==1)
        {
            ModuleDList_DebugPrintf("DList CreatList GetMem Err.\r\n");
        }
        return NULL;
    }
    memset(p1,0,sizeof(Node));
    p1->KeyValue = KeyValue;
    p1->pNextNode=NULL;
    p1->pPrevNode=NULL;
    //输入的值大于0则继续，否则停止
    while(p1->KeyValue >0)
    {
        //空表，接入表头
        if(pHead == NULL)
        {
            pHead=p1;
        }
        //非空表，则末尾插入
        else
        {
            p2->pNextNode=p1;
        }

        p2=p1;
        p1=(Node *)malloc(sizeof(Node));

        if(p1==NULL||p2==NULL)
        {
            if(ModuleDList_DebugTest_Enable==1)
            {
                ModuleDList_DebugPrintf("DList CreatList GetMem Err.\r\n");
            }
            return NULL;
        }
        memset(p1,0,sizeof(Node));
        //scanf("%d",&p1->element);
        p1->KeyValue = 0;
        p1->pNextNode=NULL;
        p1->pPrevNode=NULL;
    }
    if(ModuleDList_DebugTest_Enable==1)
    {
        ModuleDList_DebugPrintf("DList CreatList OK.\r\n");
    }
    return pHead;
}
/*
*********************************************************************************
* 函数功能: 3.打印链表，链表的遍历
*********************************************************************************
*/
void ModuleDList_PrintList(Node *pHead)
{
    if(NULL==pHead)
    {
        if(ModuleDList_DebugTest_Enable==1)
        {
            ModuleDList_DebugPrintf("DList PrintList NULL.\r\n");
        }
    }
    else
    {
        if(ModuleDList_DebugTest_Enable==1)
        {
            int8_t *pbuf;
            pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
            while(NULL!=pHead)
            {
                sprintf((char*)pbuf,"KeyValue=%d\r\n",pHead->KeyValue);
                ModuleDList_DebugPrintf(pbuf);
                pHead=pHead->pNextNode;
            }
            MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
        }
        else
        {
            while(NULL!=pHead)
            {
                //-----
                //-----
                pHead=pHead->pNextNode;
            }
        }

    }

}
/*
*********************************************************************************
* 函数功能: 4.清除线性表L中的所有元素，即释放单链表L中所有的结点，使之成为一个空表
*********************************************************************************
*/
void ModuleDList_ClearList(Node *pHead)
{
    Node *pNext;
    if(pHead==NULL)
    {
        if(ModuleDList_DebugTest_Enable==1)
        {
            ModuleDList_DebugPrintf("DList ClearList NULL.\r\n");
        }
        return;
    }
    while(pHead->pNextNode!=NULL)
    {
        pNext=pHead->pNextNode;
        free(pHead);
        pHead=pNext;
    }
    if(ModuleDList_DebugTest_Enable==1)
    {
        ModuleDList_DebugPrintf("DList ClearList OK.\r\n");
    }
}
/*
*********************************************************************************
* 函数功能: 5.返回链表的长度
*********************************************************************************
*/
int32_t ModuleDList_SizeList(Node *pHead)
{
    int32_t size=0;
    while(pHead!=NULL)
    {
        size++;
        pHead=pHead->pNextNode;
    }
    if(ModuleDList_DebugTest_Enable==1)
    {
        int8_t *pbuf;
        pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        sprintf((char*)pbuf,"DList Size:%d\r\n",size);
        ModuleDList_DebugPrintf(pbuf);
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
    return size;
}
/*
*********************************************************************************
* 函数功能: 6.检查单链表是否为空，若为空则返回１，否则返回０
*********************************************************************************
*/
int32_t ModuleDList_IsEmptyList(Node *pHead)
{
    if(pHead==NULL)
    {
        if(ModuleDList_DebugTest_Enable==1)
        {
            ModuleDList_DebugPrintf("SList IsEmptyList NULL.\r\n");
        }
        return 1;
    }
    else
    {
        if(ModuleDList_DebugTest_Enable==1)
        {
            ModuleDList_DebugPrintf("SList IsEmptyList NON-NULL.\r\n");
        }
    }
    return 0;

}
/*
*********************************************************************************
* 函数功能: 7.返回链表中第post节点的数据，若post超出范围，则停止程序运行
*********************************************************************************
*/
int32_t ModuleDList_GetElement(Node *pHead,int32_t pos)
{
    int32_t i=0;
    if(pos<1)
    {
        if(ModuleDList_DebugTest_Enable==1)
        {
            ModuleDList_DebugPrintf("DList GetElement Pos Err(Pos<1)\r\n");
        }
        return 0;
    }
    if(pHead==NULL)
    {
        if(ModuleDList_DebugTest_Enable==1)
        {
            ModuleDList_DebugPrintf("DList GetElement NULL.\r\n");
        }
    }

    while (pHead!=NULL)
    {
        ++i;
        if(i==pos)
        {
            break;
        }
        pHead=pHead->pNextNode;
    }
    if(i<pos)
    {
        if(ModuleDList_DebugTest_Enable==1)
        {
            ModuleDList_DebugPrintf("DList GetElement Pos Err(Pos>max)\r\n");
        }
        return 0;
    }
    if(ModuleDList_DebugTest_Enable==1)
    {
        int8_t *pbuf;
        pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        sprintf((char*)pbuf,"DList GetElement:%d(Pos)=%d(KeyValue)\r\n",pos,pHead->KeyValue);
        ModuleDList_DebugPrintf(pbuf);
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }

    return 1;
}
/*
*********************************************************************************
* 函数功能: 8.从单一链表中查找具有给定值x的第一个元素，若查找成功后，返回该节点
            data域的存储位置，否则返回NULL
*********************************************************************************
*/
uint32_t *ModuleDList_GetElemAddr(Node *pHead,uint32_t KeyValue)
{
    if(NULL==pHead)
    {
        if(ModuleDList_DebugTest_Enable==1)
        {
            ModuleDList_DebugPrintf("DList GetElemAddr NULL.\r\n");
        }
        return NULL;
    }
    /*
    if(KeyValue<0)
    {
        if(ModuleDList_DebugTest_Enable==1)
        {
            ModuleDList_DebugPrintf("DList GetElemAddr elemType Err(elemType<1)\r\n");
        }
        return NULL;
    }
    */
    //判断链表是否为空，并且是否存在所查找的元素
    while((pHead->KeyValue!=KeyValue)&&(NULL!=pHead->pNextNode))
    {
        pHead=pHead->pNextNode;
    }
    if(pHead->KeyValue!=KeyValue)
    {
        if(ModuleDList_DebugTest_Enable==1)
        {
            ModuleDList_DebugPrintf("DList GetElemAddr elemType Err(NO elemType)\r\n");
        }
        return NULL;
    }
    else
    {
        if(ModuleDList_DebugTest_Enable==1)
        {
            int8_t *pbuf;
            pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
            sprintf((char*)pbuf,"DList GetElemAddr:%d(KeyValue)=0x%x(Addr)\r\n",KeyValue,&(pHead->KeyValue));
            ModuleDList_DebugPrintf(pbuf);
            MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
        }
    }
    return &(pHead->KeyValue);

}
/*
*********************************************************************************
* 函数功能: 9.修改链表中第pos个点X的值，如果修改成功，则返回1,否则返回0
*********************************************************************************
*/
int32_t ModuleDList_ModifyElem(Node *pNode,int32_t pos,uint32_t KeyValue)
{
    int32_t i=0;
    Node *pHead;
    pHead=pNode;
    if(NULL==pHead)
    {
        if(ModuleDList_DebugTest_Enable==1)
        {
            ModuleDList_DebugPrintf("DList ModifyElem NULL.\r\n");
        }
        return 0;
    }

    if(pos<1)
    {
        if(ModuleDList_DebugTest_Enable==1)
        {
            ModuleDList_DebugPrintf("DList ModifyElem Pos Err(Pos<1)\r\n");
        }
        return 0;
    }
    // 找到响应位置---i
    while(pHead!= NULL)
    {
        ++i;
        if(i==pos)
        {
            break;
        }
        pHead=pHead->pNextNode;
    }
    // 位置容错
    if(i<pos)
    {
        if(ModuleDList_DebugTest_Enable==1)
        {
            ModuleDList_DebugPrintf("SList ModifyElem Pos Err(Pos>max)\r\n");
        }
        return 0;
    }
    // 修改键值
    pNode=pHead;
    pNode->KeyValue=KeyValue;
    if(ModuleDList_DebugTest_Enable==1)
    {
        int8_t *pbuf;
        pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        sprintf((char*)pbuf,"SList ModifyElem: %d(Pos)=%d(KeyValue)\r\n",pos,KeyValue);
        ModuleDList_DebugPrintf(pbuf);
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
    return 1;

}
/*
*********************************************************************************
* 函数功能: 10.向单链表的表头插入一个元素
*********************************************************************************
*/
int32_t ModuleDList_InsertHeadList(Node **pNode,uint32_t KeyValue)
{
    Node *pInsert;
    pInsert=(Node *)malloc(sizeof(Node));
    if(pInsert==NULL)
    {
        return NULL;
    }
    memset(pInsert,0,sizeof(Node));
    pInsert->KeyValue=KeyValue;
    pInsert->pNextNode=*pNode;
    (*pNode)->pPrevNode=pInsert;
    *pNode=pInsert;
    if(ModuleDList_DebugTest_Enable==1)
    {
        int8_t *pbuf;
        pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        sprintf((char*)pbuf,"DList InsertHeadList OK:%d\r\n",KeyValue);
        ModuleDList_DebugPrintf(pbuf);
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
    return 1;
}
/*
*********************************************************************************
* 函数功能: 11.向单链表的末尾添加一个元素
*********************************************************************************
*/
int32_t ModuleDList_InsertLastList(Node *pNode,uint32_t KeyValue)
{
    Node *pInsert;
    Node *pHead;
    //Node *pTmp;

    pHead=pNode;
    //pTmp=pHead;
    pInsert=(Node *)malloc(sizeof(Node));
    if(pInsert==NULL)
    {
        return NULL;
    }
    memset(pInsert,0,sizeof(Node));
    pInsert->KeyValue=KeyValue;
    pInsert->pNextNode=NULL;
    pInsert->pPrevNode=NULL;
    // 最终最后结点
    while(pHead->pNextNode!=NULL)
    {
        pHead=pHead->pNextNode;
    }
    // 赋值上一节点指针链
    pHead->pNextNode=pInsert;
    pInsert->pPrevNode=pHead;
    if(ModuleDList_DebugTest_Enable==1)
    {
        int8_t *pbuf;
        pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        sprintf((char*)pbuf,"DList InsertLastList OK:%d\r\n",KeyValue);
        ModuleDList_DebugPrintf(pbuf);
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
    return 1;
}
/*
*********************************************************************************
* 函数功能: 12.向单链表中第pos个结点位置插入元素为x的结点.
            若插入成功返回１，否则返回０
*********************************************************************************
*/
int32_t ModuleDList_IsAddPos(Node *pNode,int32_t pos,uint32_t KeyValue)
{
    Node *pHead=pNode;
    Node *pTmp;

    int32_t i=0;

    if(NULL==pHead)
    {
        if(ModuleDList_DebugTest_Enable==1)
        {
            ModuleDList_DebugPrintf("DList IsAddPos NULL.\r\n");
        }
        return 0;
    }

    if(pos<1)
    {
        if(ModuleDList_DebugTest_Enable==1)
        {
            ModuleDList_DebugPrintf("DList IsAddPos Err(Pos<1).\r\n");
        }
        return 0;
    }
    // 找出指定位置---i
    while(pHead!=NULL)
    {
        ++i;
        if(i==pos)
            break;
        pHead=pHead->pNextNode;
    }
    // 位置容错
    if(i<pos)
    {
        if(ModuleDList_DebugTest_Enable==1)
        {
            ModuleDList_DebugPrintf("DList IsAddPos Err(Pos>max)\r\n");
        }
        return 0;
    }
    // 申请缓存
    pTmp=(Node *)malloc(sizeof(Node));
    if(pTmp==NULL)
    {
        return NULL;
    }
    memset(pTmp,0,sizeof(Node));
    // 更新next指针
    pTmp->pNextNode=pHead->pNextNode;
    pHead->pNextNode=pTmp;
    // 更新prew指针
    pTmp->pPrevNode=pHead;
    (pTmp->pNextNode)->pPrevNode=pTmp;
    // 更新键值
    pTmp->KeyValue=KeyValue;
    if(ModuleDList_DebugTest_Enable==1)
    {
        int8_t *pbuf;
        pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        sprintf((char*)pbuf,"DList IsAddPos OK: %d(Pos)=%d(KeyValue)\r\n",pos,KeyValue);
        ModuleDList_DebugPrintf(pbuf);
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
    return 1;
}
/*
*********************************************************************************
* 函数功能: 13.向有序单链表中插入元素x结点，使得插入后仍然有序
* 说    明: 如果此数值要排到行尾要修改本代码
*********************************************************************************
*/
int32_t ModuleDList_OrrderList(Node *pNode,uint32_t KeyValue)
{

    Node *pHead=pNode;
    Node *pTmp;

    if(NULL==pHead)
    {
        if(ModuleDList_DebugTest_Enable==1)
        {
            ModuleDList_DebugPrintf("DList OrrderList NULL.\r\n");
        }
        return 0;
    }

    if(KeyValue<1)
    {
        if(ModuleDList_DebugTest_Enable==1)
        {
            ModuleDList_DebugPrintf("DList OrrderList Err(KeyValue<1)\r\n");
        }
        return 0;
    }
    // 获取位置---pHead
    while(pHead!=NULL)
    {
        if((pHead->KeyValue)>=KeyValue)
            break;
        pHead=pHead->pNextNode;
    }

    //位置容错
    if(pHead==NULL)
    {
        if(ModuleDList_DebugTest_Enable==1)
        {
            ModuleDList_DebugPrintf("DList OrrderList Err(KeyValue>max)\r\n");
        }
        return 0;
    }


    pTmp=(Node *)malloc(sizeof(Node));
    if(pTmp==NULL)
    {
        return NULL;
    }
    memset(pTmp,0,sizeof(Node));
    // 更新next指针
    pTmp->pNextNode=pHead->pNextNode;
    pHead->pNextNode=pTmp;
    // 更新prew指针
    pTmp->pPrevNode=pHead;
    (pTmp->pNextNode)->pPrevNode=pTmp;
    // 更新键值
    pTmp->KeyValue=KeyValue;
    if(ModuleDList_DebugTest_Enable==1)
    {
        int8_t *pbuf;
        pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        sprintf((char*)pbuf,"DList OrrderList OK:KeyValue=%d\r\n",KeyValue);
        ModuleDList_DebugPrintf(pbuf);
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
    return 1;
}
/*
*********************************************************************************
* 函数功能: 14.从单链表中删除表头结点，并把该结点的值返回.
            若删除失败则停止程序运行
*********************************************************************************
*/
int32_t ModuleDList_DelHeadList(Node **pList)
{
    Node *pHead;
    pHead=*pList;
    if(pHead!=NULL)
    {
        if(ModuleDList_DebugTest_Enable==1)
        {
            int8_t *pbuf;
            pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
            sprintf((char*)pbuf,"SList DelHeadList OK: KeyValue=%d\r\n",pHead->KeyValue);
            ModuleDList_DebugPrintf(pbuf);
            MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
        }
    }
    else
    {
        if(ModuleDList_DebugTest_Enable==1)
        {
            ModuleDList_DebugPrintf("SList DelHeadList NULL.\r\n");
        }
        return 0;
    }
    *pList=pHead->pNextNode;
    (*pList)->pPrevNode=NULL;
    // 释放空间
    free(pHead);
    return 1;
}
/*
*********************************************************************************
* 函数功能: 15.从单链表中删除表尾结点并返回它的值，若删除失败则停止程序运行
*********************************************************************************
*/
int32_t ModuleDList_DelLastList(Node *pNode)
{
    Node *pHead=pNode;
    Node *pTmp;

    while(pHead->pNextNode!=NULL)
    {
        pTmp=pHead;
        pHead=pHead->pNextNode;
    }
    if(ModuleDList_DebugTest_Enable==1)
    {
        int8_t *pbuf;
        pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        sprintf((char*)pbuf,"SList DelLastList OK: KeyValue=%d\r\n",pHead->KeyValue);
        ModuleDList_DebugPrintf(pbuf);
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
    free(pHead);
    pTmp->pNextNode=NULL;
    return     1;
}
/*
*********************************************************************************
* 函数功能: 16.从单链表中删除第pos个结点并返回它的值，若删除失败则停止程序运行
*********************************************************************************
*/
int32_t ModuleDList_DelPos(Node *pNode,int32_t pos)
{
    Node *pHead=pNode;
    //Node *pTmp;

    int32_t i=0;

    if(NULL==pHead)
    {
        if(ModuleDList_DebugTest_Enable==1)
        {
            ModuleDList_DebugPrintf("DList DelPos NULL.\r\n");
        }
        return 0;
    }

    if(pos<1)
    {
        if(ModuleDList_DebugTest_Enable==1)
        {
            ModuleDList_DebugPrintf("DList DelPos Err(Pos<1)\r\n");
        }
        return 0;
    }

    while(pHead!=NULL)
    {
        ++i;
        if(i==pos)
            break;
        //pTmp=pHead;
        pHead=pHead->pNextNode;
    }

    if(i<pos)
    {
        if(ModuleDList_DebugTest_Enable==1)
        {
            ModuleDList_DebugPrintf("DList DelPos Err(Pos>max)\r\n");
        }
        return 0;
    }
    if(ModuleDList_DebugTest_Enable==1)
    {
        int8_t *pbuf;
        pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        sprintf((char*)pbuf,"DList DelPos OK:%d(Pos)=%d(KeyValue)\r\n",pos,pHead->KeyValue);
        ModuleDList_DebugPrintf(pbuf);
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
    //pTmp->pNextNode=pHead->pNextNode;
    if((pHead->pPrevNode)!=NULL)
    {
        (pHead->pPrevNode)->pNextNode   =   (pHead->pNextNode);
    }
    if((pHead->pNextNode)!=NULL)
    {
        (pHead->pNextNode)->pPrevNode   =   (pHead->pPrevNode);
    }
    free(pHead);
    return 1;
}
/*
*********************************************************************************
* 函数功能: 17.从单链表中删除值为x的第一个结点，若删除成功则返回1,否则返回0
*********************************************************************************
*/
int32_t ModuleDList_Delx(Node **pNode,int32_t KeyValue)
{
    Node *pHead=*pNode;
    //Node *pTmp;
    int32_t i=0;

    if(NULL==pHead)
    {
        if(ModuleDList_DebugTest_Enable==1)
        {
            ModuleDList_DebugPrintf("SList Delx NULL.\r\n");
        }
        return 0;
    }
    if(KeyValue<0)
    {
        if(ModuleDList_DebugTest_Enable==1)
        {
            ModuleDList_DebugPrintf("SList Delx Err(KeyValue<0).\r\n");
        }
        return 0;
    }
    //判断链表是否为空，并且是否存在所查找的元素
    while((pHead->KeyValue!=KeyValue)&&(NULL!=pHead->pNextNode))
    {
        ++i;
        //pTmp=pHead;
        pHead=pHead->pNextNode;
    }
    if(pHead->KeyValue!=KeyValue)
    {
        if(ModuleDList_DebugTest_Enable==1)
        {
            ModuleDList_DebugPrintf("SList Delx Err(No KeyValue)\r\n");
        }
        return 0;
    }
    if((i==0)&&(NULL!=pHead->pNextNode))
    {
        if(ModuleDList_DebugTest_Enable==1)
        {
            ModuleDList_DebugPrintf("SList Delx OK.\r\n");
        }
        *pNode=pHead->pNextNode;
        free(pHead);
        return 1;
    }
    if(ModuleDList_DebugTest_Enable==1)
    {
        int8_t *pbuf;
        pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        sprintf((char*)pbuf,"SList Delx OK.(KeyValue=%d)\r\n",KeyValue);
        ModuleDList_DebugPrintf(pbuf);
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
    //pTmp->pNextNode=pHead->pNextNode;
    if((pHead->pPrevNode)!=NULL)
    {
        (pHead->pPrevNode)->pNextNode   =   (pHead->pNextNode);
    }
    if((pHead->pNextNode)!=NULL)
    {
        (pHead->pNextNode)->pPrevNode   =   (pHead->pPrevNode);
    }
    free(pHead);
    return 1;
}
/*
*********************************************************************************
* 函数功能: 18.交换2个元素的位置,有优化速度空间(同时查找)
*********************************************************************************
*/
int32_t ModuleDList_Exchange2Pos(Node *pNode,int32_t pos1,int32_t pos2)
{
    Node *pNodePos1,*pNodePos2;
    //uint32_t *pTmp;
    //uint32_t *pInsert;
    //uint32_t a;
    int32_t i=0;
    Node temp_node;

    if(pos1<1||pos2<1)
    {
        if(ModuleDList_DebugTest_Enable==1)
        {
            ModuleDList_DebugPrintf("SList Exchange2Pos Err.(Pos1<1 || Pos2<1)\r\n");
        }
        return 0;
    }
    // 找出pos1结构---pHead
    pNodePos1=pNode;
    while(pNodePos1!=NULL)
    {
        ++i;
        if(i==pos1)
            break;
        pNodePos1=pNodePos1->pNextNode;
    }
    // pos1位置容错
    if(i<pos1)
    {
        if(ModuleDList_DebugTest_Enable==1)
        {
            ModuleDList_DebugPrintf("DList Exchange2Pos Err.(Pos1>max)\r\n");
        }
        return 0;
    }
    // 找出pos2结构---pHead
    i=0;
    pNodePos2=pNode;
    while(pNodePos2!=NULL)
    {
        ++i;
        if(i==pos2)
            break;
        pNodePos2=pNodePos2->pNextNode;
    }
    // pos2位置容错
    if(i<pos2)
    {
        if(ModuleDList_DebugTest_Enable==1)
        {
            ModuleDList_DebugPrintf("DList Exchange2Pos Err.(Pos2>max)\r\n");
        }
        return 0;
    }
    // 交换具体内容
    temp_node.KeyValue  =   pNodePos1->KeyValue;
    temp_node.pPara     =   pNodePos1->pPara;
    pNodePos1->KeyValue =   pNodePos2->KeyValue;
    pNodePos1->pPara    =   pNodePos2->pPara;
    pNodePos1->KeyValue =   temp_node.KeyValue;
    pNodePos1->pPara    =   temp_node.pPara;
    // 打印
    if(ModuleDList_DebugTest_Enable==1)
    {
        int8_t *pbuf;
        pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        sprintf((char*)pbuf,"DList Exchange2Pos OK.(Pos1=%d Pos2=%d\r\n",pos1,pos2);
        ModuleDList_DebugPrintf(pbuf);
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
    return 1;
}
/*
*********************************************************************************
* 函数功能: 19.将线性表进行冒泡排序
*********************************************************************************
*/
static int32_t ModuleDList_Swap(Node *pNodePos1,Node *pNodePos2)
{
    Node temp_node;
    if((pNodePos1->KeyValue) > (pNodePos2->KeyValue))
    {
        temp_node.KeyValue  =   pNodePos1->KeyValue;
        temp_node.pPara     =   pNodePos1->pPara;
        pNodePos1->KeyValue =   pNodePos2->KeyValue;
        pNodePos1->pPara    =   pNodePos2->pPara;
        pNodePos1->KeyValue =   temp_node.KeyValue;
        pNodePos1->pPara    =   temp_node.pPara;
    }
    return 0;
}
int32_t ModuleDList_Arrange(Node *pNode)
{
    Node *pHead=pNode;
    int32_t a=0,i,j;

    if(NULL==pHead)
    {
        if(ModuleDList_DebugTest_Enable==1)
        {
            ModuleDList_DebugPrintf("DList Arrange NULL.\r\n");
        }
        return 0;
    }
    // 获取数量
    while(pHead!=NULL)
    {
        ++a;
        pHead=pHead->pNextNode;
    }
    //
    pHead=pNode;
    for(i=0; i<a-1; i++)
    {
        for(j=1; j<a-i; j++)
        {
            ModuleDList_Swap(pHead,pHead->pNextNode);
            pHead=pHead->pNextNode;
        }
        pHead=pNode;
    }
    if(ModuleDList_DebugTest_Enable==1)
    {
        ModuleDList_DebugPrintf("DList Arrange OK.\r\n");
    }
    return 0;
}
/*
*********************************************************************************
* 函数功能: 测试
*********************************************************************************
*/
void ModuleSList_DebugTestOnOff(uint8_t OnOff)
{
    Node *pList=NULL;
    //int32_t length=0;
    //elemType posElem;
    ModuleDList_DebugTest_Enable=1;
    //初始化链表
    ModuleDList_InitList(&pList);
    ModuleDList_PrintList(pList);
    //创建链表
    pList=ModuleDList_CreatList(pList,1);
    ModuleDList_InsertLastList(pList,2);
    ModuleDList_InsertLastList(pList,3);
    ModuleDList_InsertLastList(pList,4);
    ModuleDList_InsertLastList(pList,5);
    ModuleDList_InsertLastList(pList,6);
    ModuleDList_InsertLastList(pList,7);
    ModuleDList_InsertLastList(pList,8);
    ModuleDList_InsertLastList(pList,9);
    ModuleDList_InsertLastList(pList,10);
    ModuleDList_PrintList(pList);
    //获取链表长度
    ModuleDList_SizeList(pList);
    ModuleDList_PrintList(pList);
    //检查链表是否为空
    ModuleDList_IsEmptyList(pList);
    //按位置返回键值
    ModuleDList_GetElement(pList,3);
    ModuleDList_PrintList(pList);
    //按键值获取键值地址
    ModuleDList_GetElemAddr(pList,5);
    //按位置修改键值
    ModuleDList_ModifyElem(pList,4,1);
    ModuleDList_PrintList(pList);
    //插入---链表头节点
    ModuleDList_InsertHeadList(&pList,5);
    ModuleDList_PrintList(pList);
    //插入---链表尾节点
    ModuleDList_InsertLastList(pList,10);
    ModuleDList_PrintList(pList);
    //插入---按地址插入节点
    ModuleDList_IsAddPos(pList,4,5);
    ModuleDList_PrintList(pList);
    //插入---按键值有序插入
    ModuleDList_OrrderList(pList,6);
    ModuleDList_PrintList(pList);
    //删除---头节点
    ModuleDList_DelHeadList(&pList);
    ModuleDList_PrintList(pList);
    //删除---尾节点
    ModuleDList_DelLastList(pList);
    ModuleDList_PrintList(pList);
    //删除---按位置
    ModuleDList_DelPos(pList,5);
    ModuleDList_PrintList(pList);
    //删除---按键值
    ModuleDList_Delx(&pList,5);
    ModuleDList_PrintList(pList);
    //交换---按地址
    ModuleDList_Exchange2Pos(pList,2,5);
    ModuleDList_PrintList(pList);
    //排序
    ModuleDList_Arrange(pList);
    ModuleDList_PrintList(pList);
    //删除链表
    ModuleDList_ClearList(pList);
    ModuleDList_DebugTest_Enable=0;
}
