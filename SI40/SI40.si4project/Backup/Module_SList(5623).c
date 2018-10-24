/*
***********************************************************************************
*                    作    者: 徐松亮
*                    更新时间: 2016-03-24
***********************************************************************************
*/
/*********************************************************************************
-------------------- 链表特点
   1.优点:           插入快,删除快
   2.缺点:           查找慢
-------------------- 关于线性表链接存储（单链表）操作的19种算法
   1.初始化:         线性表，即置单链表的表头指针为空
   2.创建:           创建线性表，此函数输入负数终止读取数据
   3.遍历:           打印链表，链表的遍历
   4.清除:           清除线性表L中的所有元素，即释放单链表L中所有结点使之成为空表
   5.获取长度:       返回单链表的长度
   6.查空:           检查单链表是否为空，若为空则返回１，否则返回０
   7.按位置获取信息: 返回单链表中第pos个结点中的元素，若pos超范围，则停止程序运行
   8.按键值获取信息: 从单链表中查找具有给定值x的第一个元素，若成功则返回该结点data域的存储地址，否则返回NULL
   9.按位置修改键值: 把单链表中第pos个结点的值修改为x的值，若成功返回１，否则返回０
   10.头部插入:      向单链表的表头插入一个元素
   11.尾部插入:      向单链表的末尾添加一个元素
   12.按位置插入:    向单链表中第pos个结点位置插入元素为x的结点，成功返１，否则返０
   13.有序插入:      向有序单链表中插入元素x结点，使得插入后仍然有序
   14.头部删除:      从单链表中删除表头结点，并把该结点的值返回，若删除失败则停止程序运行
   15.尾部删除:      从单链表中删除表尾结点并返回它的值，若删除失败则停止程序运行
   16.按位置删除:    从单链表中删除第pos个结点并返回它的值，若删除失败则停止程序运行
   17.按键值删除:    从单链表中删除值为x的第一个结点，若删除成功则返回1,否则返回0
   18.交换:          交换2个元素的位置
   19.排序:          将线性表进行冒排序
   注意检查分配到的动态内存是否为空
**********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Module_SList.h"
#include "uctsk_Debug.h"
#include "MemManager.h"
//定义存入的数据的类型可以是int char
//定义链表的结构类型
typedef struct NODE
{
    //键值(用于排序与查找)
    uint32_t KeyValue;
    //参数指针
    void *pPara;
    //链表下一个结点
    struct NODE *pNextNode;
} Node;
//
static uint8_t ModuleSList_DebugTest_Enable=0;
/*
*********************************************************************************
* 函数功能: 1,初始化线性表，即置单链表的表头指针为空
*********************************************************************************
*/
void ModuleSList_InitList(Node **pNode)
{
    *pNode=NULL;
    if(ModuleSList_DebugTest_Enable==1)
    {
        ModuleSList_DebugPrintf("SList Init OK.\r\n");
    }
}
/*
*********************************************************************************
* 函数功能: 2.创建线性表，此函数输入负数终止读取数据
*********************************************************************************
*/
Node *ModuleSList_CreatList(Node *pHead,uint32_t KeyValue)
{
    Node *p1,*p2;
    p1=p2=(Node *)malloc(sizeof(Node));
    if(p1 == NULL || p2 ==NULL)
    {
        if(ModuleSList_DebugTest_Enable==1)
        {
            ModuleSList_DebugPrintf("SList CreatList GetMem Err.\r\n");
        }
        return NULL;
    }
    memset(p1,0,sizeof(Node));
    p1->KeyValue = KeyValue;
    p1->pNextNode=NULL;
    //输入的值大于0则继续，否则停止
    while(p1->KeyValue >0)
    {
        //空表，接入表头
        if(pHead == NULL)
        {
            pHead=p1;
        }
        else
        {
            p2->pNextNode=p1;
        }

        p2=p1;
        p1=(Node *)malloc(sizeof(Node));

        if(p1==NULL||p2==NULL)
        {
            if(ModuleSList_DebugTest_Enable==1)
            {
                ModuleSList_DebugPrintf("SList CreatList GetMem Err.\r\n");
            }
            return NULL;
        }
        memset(p1,0,sizeof(Node));
        //scanf("%d",&p1->element);
        p1->KeyValue = 0;
        p1->pNextNode=NULL;
    }
    if(ModuleSList_DebugTest_Enable==1)
    {
        ModuleSList_DebugPrintf("SList CreatList OK.\r\n");
    }
    return pHead;
}
/*
*********************************************************************************
* 函数功能: 3.打印链表，链表的遍历
*********************************************************************************
*/
void ModuleSList_PrintList(Node *pHead)
{
    if(NULL==pHead)
    {
        if(ModuleSList_DebugTest_Enable==1)
        {
            ModuleSList_DebugPrintf("SList PrintList NULL.\r\n");
        }
    }
    else
    {
        if(ModuleSList_DebugTest_Enable==1)
        {
            int8_t *pbuf;
            pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
            while(NULL!=pHead)
            {
                sprintf((char*)pbuf,"KeyValue=%d\r\n",pHead->KeyValue);
                ModuleSList_DebugPrintf(pbuf);
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
void ModuleSList_ClearList(Node *pHead)
{
    Node *pNext;
    if(pHead==NULL)
    {
        if(ModuleSList_DebugTest_Enable==1)
        {
            ModuleSList_DebugPrintf("SList ClearList NULL.\r\n");
        }
        return;
    }
    while(pHead->pNextNode!=NULL)
    {
        pNext=pHead->pNextNode;
        free(pHead);
        pHead=pNext;
    }
    if(ModuleSList_DebugTest_Enable==1)
    {
        ModuleSList_DebugPrintf("SList ClearList OK.\r\n");
    }
}
/*
*********************************************************************************
* 函数功能: 5.返回链表的长度
*********************************************************************************
*/
int32_t ModuleSList_SizeList(Node *pHead)
{
    int32_t size=0;
    while(pHead!=NULL)
    {
        size++;
        pHead=pHead->pNextNode;
    }
    if(ModuleSList_DebugTest_Enable==1)
    {
        int8_t *pbuf;
        pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        sprintf((char*)pbuf,"SList Size:%d\r\n",size);
        ModuleSList_DebugPrintf(pbuf);
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
    return size;
}
/*
*********************************************************************************
* 函数功能: 6.检查单链表是否为空，若为空则返回１，否则返回０
*********************************************************************************
*/
int32_t ModuleSList_IsEmptyList(Node *pHead)
{
    if(pHead==NULL)
    {
        if(ModuleSList_DebugTest_Enable==1)
        {
            ModuleSList_DebugPrintf("SList IsEmptyList NULL.\r\n");
        }
        return 1;
    }
    else
    {
        if(ModuleSList_DebugTest_Enable==1)
        {
            ModuleSList_DebugPrintf("SList IsEmptyList NON-NULL.\r\n");
        }
    }
    return 0;

}
/*
*********************************************************************************
* 函数功能: 7.返回链表中第post节点的数据，若post超出范围，则停止程序运行
*********************************************************************************
*/
int32_t ModuleSList_GetElement(Node *pHead,int32_t pos)
{
    int32_t i=0;
    if(pos<1)
    {
        if(ModuleSList_DebugTest_Enable==1)
        {
            ModuleSList_DebugPrintf("SList GetElement Pos Err(Pos<1)\r\n");
        }
        return 0;
    }
    if(pHead==NULL)
    {
        if(ModuleSList_DebugTest_Enable==1)
        {
            ModuleSList_DebugPrintf("SList GetElement NULL.\r\n");
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
        if(ModuleSList_DebugTest_Enable==1)
        {
            ModuleSList_DebugPrintf("SList GetElement Pos Err(Pos>max)\r\n");
        }
        return 0;
    }
    if(ModuleSList_DebugTest_Enable==1)
    {
        int8_t *pbuf;
        pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        sprintf((char*)pbuf,"SList GetElement:%d(Pos)=%d(KeyValue)\r\n",pos,pHead->KeyValue);
        ModuleSList_DebugPrintf(pbuf);
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
uint32_t *ModuleSList_GetElemAddr(Node *pHead,uint32_t KeyValue)
{
    if(NULL==pHead)
    {
        if(ModuleSList_DebugTest_Enable==1)
        {
            ModuleSList_DebugPrintf("SList GetElemAddr NULL.\r\n");
        }
        return NULL;
    }
    /*
    if(KeyValue<0)
    {
        if(ModuleSList_DebugTest_Enable==1)
        {
            ModuleSList_DebugPrintf("SList GetElemAddr elemType Err(elemType<1)\r\n");
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
        if(ModuleSList_DebugTest_Enable==1)
        {
            ModuleSList_DebugPrintf("SList GetElemAddr elemType Err(NO elemType)\r\n");
        }
        return NULL;
    }
    else
    {
        if(ModuleSList_DebugTest_Enable==1)
        {
            int8_t *pbuf;
            pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
            sprintf((char*)pbuf,"SList GetElemAddr:%d(KeyValue)=0x%x(Addr)\r\n",KeyValue,&(pHead->KeyValue));
            ModuleSList_DebugPrintf(pbuf);
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
int32_t ModuleSList_ModifyElem(Node *pNode,int32_t pos,uint32_t KeyValue)
{
    int32_t i=0;
    Node *pHead;
    pHead=pNode;
    if(NULL==pHead)
    {
        if(ModuleSList_DebugTest_Enable==1)
        {
            ModuleSList_DebugPrintf("SList ModifyElem NULL.\r\n");
        }
        return 0;
    }

    if(pos<1)
    {
        if(ModuleSList_DebugTest_Enable==1)
        {
            ModuleSList_DebugPrintf("SList ModifyElem Pos Err(Pos<1)\r\n");
        }
        return 0;
    }

    while(pHead!= NULL)
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
        if(ModuleSList_DebugTest_Enable==1)
        {
            ModuleSList_DebugPrintf("SList ModifyElem Pos Err(Pos>max)\r\n");
        }
        return 0;
    }
    pNode=pHead;
    pNode->KeyValue=KeyValue;
    if(ModuleSList_DebugTest_Enable==1)
    {
        int8_t *pbuf;
        pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        sprintf((char*)pbuf,"SList ModifyElem: %d(Pos)=%d(KeyValue)\r\n",pos,KeyValue);
        ModuleSList_DebugPrintf(pbuf);
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
    return 1;

}
/*
*********************************************************************************
* 函数功能: 10.向单链表的表头插入一个元素
*********************************************************************************
*/
int32_t ModuleSList_InsertHeadList(Node **pNode,uint32_t KeyValue)
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
    *pNode=pInsert;
    if(ModuleSList_DebugTest_Enable==1)
    {
        int8_t *pbuf;
        pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        sprintf((char*)pbuf,"SList InsertHeadList OK:%d\r\n",KeyValue);
        ModuleSList_DebugPrintf(pbuf);
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
    return 1;
}
/*
*********************************************************************************
* 函数功能: 11.向单链表的末尾添加一个元素
*********************************************************************************
*/
int32_t ModuleSList_InsertLastList(Node *pNode,uint32_t KeyValue)
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
    while(pHead->pNextNode!=NULL)
    {
        pHead=pHead->pNextNode;
    }
    pHead->pNextNode=pInsert;
    if(ModuleSList_DebugTest_Enable==1)
    {
        int8_t *pbuf;
        pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        sprintf((char*)pbuf,"SList InsertLastList OK:%d\r\n",KeyValue);
        ModuleSList_DebugPrintf(pbuf);
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
int32_t ModuleSList_IsAddPos(Node *pNode,int32_t pos,uint32_t KeyValue)
{
    Node *pHead=pNode;
    Node *pTmp;

    int32_t i=0;

    if(NULL==pHead)
    {
        if(ModuleSList_DebugTest_Enable==1)
        {
            ModuleSList_DebugPrintf("SList IsAddPos NULL.\r\n");
        }
        return 0;
    }

    if(pos<1)
    {
        if(ModuleSList_DebugTest_Enable==1)
        {
            ModuleSList_DebugPrintf("SList IsAddPos Err(Pos<1).\r\n");
        }
        return 0;
    }

    while(pHead!=NULL)
    {
        ++i;
        if(i==pos)
            break;
        pHead=pHead->pNextNode;
    }

    if(i<pos)
    {
        if(ModuleSList_DebugTest_Enable==1)
        {
            ModuleSList_DebugPrintf("SList IsAddPos Err(Pos>max)\r\n");
        }
        return 0;
    }

    pTmp=(Node *)malloc(sizeof(Node));
    if(pTmp==NULL)
    {
        return NULL;
    }
    memset(pTmp,0,sizeof(Node));
    pTmp->pNextNode=pHead->pNextNode;
    pHead->pNextNode=pTmp;
    pTmp->KeyValue=KeyValue;
    if(ModuleSList_DebugTest_Enable==1)
    {
        int8_t *pbuf;
        pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        sprintf((char*)pbuf,"SList IsAddPos OK: %d(Pos)=%d(KeyValue)\r\n",pos,KeyValue);
        ModuleSList_DebugPrintf(pbuf);
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
int32_t ModuleSList_OrrderList(Node *pNode,uint32_t KeyValue)
{

    Node *pHead=pNode;
    Node *pTmp;

    if(NULL==pHead)
    {
        if(ModuleSList_DebugTest_Enable==1)
        {
            ModuleSList_DebugPrintf("SList OrrderList NULL.\r\n");
        }
        return 0;
    }

    if(KeyValue<1)
    {
        if(ModuleSList_DebugTest_Enable==1)
        {
            ModuleSList_DebugPrintf("SList OrrderList Err(KeyValue<1)\r\n");
        }
        return 0;
    }

    while(pHead!=NULL)
    {
        if((pHead->KeyValue)>=KeyValue)
            break;
        pHead=pHead->pNextNode;
    }


    if(pHead==NULL)
    {
        if(ModuleSList_DebugTest_Enable==1)
        {
            ModuleSList_DebugPrintf("SList OrrderList Err(KeyValue>max)\r\n");
        }
        return 0;
    }


    pTmp=(Node *)malloc(sizeof(Node));
    if(pTmp==NULL)
    {
        return NULL;
    }
    memset(pTmp,0,sizeof(Node));
    pTmp->pNextNode=pHead->pNextNode;
    pHead->pNextNode=pTmp;
    pTmp->KeyValue=KeyValue;
    if(ModuleSList_DebugTest_Enable==1)
    {
        int8_t *pbuf;
        pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        sprintf((char*)pbuf,"SList OrrderList OK:KeyValue=%d\r\n",KeyValue);
        ModuleSList_DebugPrintf(pbuf);
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
int32_t ModuleSList_DelHeadList(Node **pList)
{
    Node *pHead;
    pHead=*pList;
    if(pHead!=NULL)
    {
        if(ModuleSList_DebugTest_Enable==1)
        {
            int8_t *pbuf;
            pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
            sprintf((char*)pbuf,"SList DelHeadList OK: KeyValue=%d\r\n",pHead->KeyValue);
            ModuleSList_DebugPrintf(pbuf);
            MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
        }
    }
    else
    {
        if(ModuleSList_DebugTest_Enable==1)
        {
            ModuleSList_DebugPrintf("SList DelHeadList NULL.\r\n");
        }
        return 0;
    }
    *pList=pHead->pNextNode;
    return 1;
}
/*
*********************************************************************************
* 函数功能: 15.从单链表中删除表尾结点并返回它的值，若删除失败则停止程序运行
*********************************************************************************
*/
int32_t ModuleSList_DelLastList(Node *pNode)
{
    Node *pHead=pNode;
    Node *pTmp;

    while(pHead->pNextNode!=NULL)
    {
        pTmp=pHead;
        pHead=pHead->pNextNode;
    }
    if(ModuleSList_DebugTest_Enable==1)
    {
        int8_t *pbuf;
        pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        sprintf((char*)pbuf,"SList DelLastList OK: KeyValue=%d\r\n",pHead->KeyValue);
        ModuleSList_DebugPrintf(pbuf);
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
int32_t ModuleSList_DelPos(Node *pNode,int32_t pos)
{
    Node *pHead=pNode;
    Node *pTmp;

    int32_t i=0;

    if(NULL==pHead)
    {
        if(ModuleSList_DebugTest_Enable==1)
        {
            ModuleSList_DebugPrintf("SList DelPos NULL.\r\n");
        }
        return 0;
    }

    if(pos<1)
    {
        if(ModuleSList_DebugTest_Enable==1)
        {
            ModuleSList_DebugPrintf("SList DelPos Err(Pos<1)\r\n");
        }
        return 0;
    }

    while(pHead!=NULL)
    {
        ++i;
        if(i==pos)
            break;
        pTmp=pHead;
        pHead=pHead->pNextNode;
    }

    if(i<pos)
    {
        if(ModuleSList_DebugTest_Enable==1)
        {
            ModuleSList_DebugPrintf("SList DelPos Err(Pos>max)\r\n");
        }
        return 0;
    }
    if(ModuleSList_DebugTest_Enable==1)
    {
        int8_t *pbuf;
        pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        sprintf((char*)pbuf,"SList DelPos OK:%d(Pos)=%d(KeyValue)\r\n",pos,pHead->KeyValue);
        ModuleSList_DebugPrintf(pbuf);
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
    pTmp->pNextNode=pHead->pNextNode;
    free(pHead);
    return 1;
}
/*
*********************************************************************************
* 函数功能: 17.从单链表中删除值为x的第一个结点，若删除成功则返回1,否则返回0
*********************************************************************************
*/
int32_t ModuleSList_Delx(Node **pNode,int32_t KeyValue)
{
    Node *pHead=*pNode;
    Node *pTmp;
    int32_t i=0;

    if(NULL==pHead)
    {
        if(ModuleSList_DebugTest_Enable==1)
        {
            ModuleSList_DebugPrintf("SList Delx NULL.\r\n");
        }
        return 0;
    }
    if(KeyValue<0)
    {
        if(ModuleSList_DebugTest_Enable==1)
        {
            ModuleSList_DebugPrintf("SList Delx Err(KeyValue<0).\r\n");
        }
        return 0;
    }
    //判断链表是否为空，并且是否存在所查找的元素
    while((pHead->KeyValue!=KeyValue)&&(NULL!=pHead->pNextNode))
    {
        ++i;
        pTmp=pHead;
        pHead=pHead->pNextNode;
    }
    if(pHead->KeyValue!=KeyValue)
    {
        if(ModuleSList_DebugTest_Enable==1)
        {
            ModuleSList_DebugPrintf("SList Delx Err(No KeyValue)\r\n");
        }
        return 0;
    }
    if((i==0)&&(NULL!=pHead->pNextNode))
    {
        if(ModuleSList_DebugTest_Enable==1)
        {
            ModuleSList_DebugPrintf("SList Delx OK.\r\n");
        }
        *pNode=pHead->pNextNode;
        free(pHead);
        return 1;
    }
    if(ModuleSList_DebugTest_Enable==1)
    {
        int8_t *pbuf;
        pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        sprintf((char*)pbuf,"SList Delx OK.(KeyValue=%d)\r\n",KeyValue);
        ModuleSList_DebugPrintf(pbuf);
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
    pTmp->pNextNode=pHead->pNextNode;
    free(pHead);
    return 1;
}
/*
*********************************************************************************
* 函数功能: 18.交换2个元素的位置
*********************************************************************************
*/
int32_t ModuleSList_Exchange2Pos(Node *pNode,int32_t pos1,int32_t pos2)
{
    Node *pHead;
    uint32_t *pTmp;
    uint32_t *pInsert;
    uint32_t a;
    int32_t i=0;

    if(pos1<1||pos2<1)
    {
        if(ModuleSList_DebugTest_Enable==1)
        {
            ModuleSList_DebugPrintf("SList Exchange2Pos Err.(Pos1<1 || Pos2<1)\r\n");
        }
        return 0;
    }

    pHead=pNode;
    while(pHead!=NULL)
    {
        ++i;
        if(i==pos1)
            break;
        pHead=pHead->pNextNode;
    }

    if(i<pos1)
    {
        if(ModuleSList_DebugTest_Enable==1)
        {
            ModuleSList_DebugPrintf("SList Exchange2Pos Err.(Pos1>max)\r\n");
        }
        return 0;
    }

    pTmp=&(pHead->KeyValue);
    i=0;
    pHead=pNode;
    while(pHead!=NULL)
    {
        ++i;
        if(i==pos2)
            break;
        pHead=pHead->pNextNode;
    }

    if(i<pos2)
    {
        if(ModuleSList_DebugTest_Enable==1)
        {
            ModuleSList_DebugPrintf("SList Exchange2Pos Err.(Pos2>max)\r\n");
        }
        return 0;
    }

    pInsert=&(pHead->KeyValue);
    a=*pTmp;
    *pTmp=*pInsert;
    *pInsert=a;
    if(ModuleSList_DebugTest_Enable==1)
    {
        int8_t *pbuf;
        pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        sprintf((char*)pbuf,"SList Exchange2Pos OK.(Pos1=%d Pos2=%d\r\n",pos1,pos2);
        ModuleSList_DebugPrintf(pbuf);
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
    return 1;
}
/*
*********************************************************************************
* 函数功能: 19.将线性表进行冒泡排序
*********************************************************************************
*/
static int32_t ModuleSList_Swap(uint32_t *p1,uint32_t *p2)
{
    uint32_t a;
    if(*p1>*p2)
    {
        a=*p1;
        *p1=*p2;
        *p2=a;
    }
    return 0;
}
int32_t ModuleSList_Arrange(Node *pNode)
{
    Node *pHead=pNode;
    int32_t a=0,i,j;

    if(NULL==pHead)
    {
        if(ModuleSList_DebugTest_Enable==1)
        {
            ModuleSList_DebugPrintf("SList Arrange NULL.\r\n");
        }
        return 0;
    }

    while(pHead!=NULL)
    {
        ++a;
        pHead=pHead->pNextNode;
    }

    pHead=pNode;
    for(i=0; i<a-1; i++)
    {
        for(j=1; j<a-i; j++)
        {
            ModuleSList_Swap(&(pHead->KeyValue),&(pHead->pNextNode->KeyValue));
            pHead=pHead->pNextNode;
        }
        pHead=pNode;
    }
    if(ModuleSList_DebugTest_Enable==1)
    {
        ModuleSList_DebugPrintf("SList Arrange OK.\r\n");
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
    ModuleSList_DebugTest_Enable=1;
    //初始化链表
    ModuleSList_InitList(&pList);
    ModuleSList_PrintList(pList);
    //创建链表
    pList=ModuleSList_CreatList(pList,1);
    ModuleSList_InsertLastList(pList,2);
    ModuleSList_InsertLastList(pList,3);
    ModuleSList_InsertLastList(pList,4);
    ModuleSList_InsertLastList(pList,5);
    ModuleSList_InsertLastList(pList,6);
    ModuleSList_InsertLastList(pList,7);
    ModuleSList_InsertLastList(pList,8);
    ModuleSList_InsertLastList(pList,9);
    ModuleSList_InsertLastList(pList,10);
    ModuleSList_PrintList(pList);
    //获取链表长度
    ModuleSList_SizeList(pList);
    ModuleSList_PrintList(pList);
    //检查链表是否为空
    ModuleSList_IsEmptyList(pList);
    //按位置返回键值
    ModuleSList_GetElement(pList,3);
    ModuleSList_PrintList(pList);
    //按键值获取键值地址
    ModuleSList_GetElemAddr(pList,5);
    //按位置修改键值
    ModuleSList_ModifyElem(pList,4,1);
    ModuleSList_PrintList(pList);
    //插入---链表头节点
    ModuleSList_InsertHeadList(&pList,5);
    ModuleSList_PrintList(pList);
    //插入---链表尾节点
    ModuleSList_InsertLastList(pList,10);
    ModuleSList_PrintList(pList);
    //插入---按地址插入节点
    ModuleSList_IsAddPos(pList,4,5);
    ModuleSList_PrintList(pList);
    //插入---按键值有序插入
    ModuleSList_OrrderList(pList,6);
    ModuleSList_PrintList(pList);
    //删除---头节点
    ModuleSList_DelHeadList(&pList);
    ModuleSList_PrintList(pList);
    //删除---尾节点
    ModuleSList_DelLastList(pList);
    ModuleSList_PrintList(pList);
    //删除---按位置
    ModuleSList_DelPos(pList,5);
    ModuleSList_PrintList(pList);
    //删除---按键值
    ModuleSList_Delx(&pList,5);
    ModuleSList_PrintList(pList);
    //交换---按地址
    ModuleSList_Exchange2Pos(pList,2,5);
    ModuleSList_PrintList(pList);
    //排序
    ModuleSList_Arrange(pList);
    ModuleSList_PrintList(pList);
    //删除链表
    ModuleSList_ClearList(pList);
    ModuleSList_DebugTest_Enable=0;
}
