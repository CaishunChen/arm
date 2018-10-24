/*
***********************************************************************************
*                    作    者: 徐松亮
*                    更新时间: 2015-06-03
***********************************************************************************
*/
/*
算法设计的常用思想
   贪婪法(因为"短视",很多情况贪婪法得不到最优解)
      设计步骤:
         1, 建立对问题精确描述的数学模型,包括定义最优解的模型.
         2, 将问题分解为一系列子问题,同时定义子问题的最优解结构.
         3, 用贪心原则确定每个子问题局部最优解,再堆叠成全局最优解.
      实例:
         1, 0-1背包(每个物品只能有1件)
   分治法(分解,解决,合并)
      说明
         1, 能用数学归纳法证明的问题,一般就可以用分治法解决.
         2, 递归和分治是一对好朋友.
   动态规划步骤:
      设计步骤:
         1, 定义最优子问题
         2, 定义状态
         3, 定义决策和状态转换方程
         4, 确定边界条件
      实例:
实例:
   字符串编辑距离(插入/删除/替换)
*/
//------------------------------ Includes ---------------------------------------
#include  <stdio.h>
#include  <stdint.h>
#include  <string.h>
#include  <ctype.h>
#include  <stdlib.h>
#include  <stdarg.h>

//------------------------------ 固定宏 -----------------------------------------
#define min(a, b)  (((a) < (b)) ? (a) : (b))
/*******************************************************************************
函数功能: 原始算法1(朴素的递归算法)
*******************************************************************************/
int EditDistance1(char *src, char *dest)
{
    int edIns,edDel,edRep;
    //查到结尾,则返回字符串长度差
    if((strlen(src) == 0) || (strlen(dest) == 0))
        return abs((int)strlen(src) - (int)strlen(dest));
    //字符相同,则比较下一个字符
    if(src[0] == dest[0])
        return EditDistance1(src + 1, dest + 1);
    //插入字符
    edIns = EditDistance1(src, dest + 1) + 1;
    //删除字符
    edDel = EditDistance1(src + 1, dest) + 1;
    //替换字符
    edRep = EditDistance1(src + 1, dest + 1) + 1;
    return min(min(edIns, edDel), edRep);
}
/*******************************************************************************
函数功能: 改进算法2(建表的递归算法)
*******************************************************************************/
const int INVALID_VALUE = 0xFFFF;
const int MAX_STRING_LEN = 50;
typedef struct tagMemoRecord
{
    int distance;
    int refCount;
} MEMO_RECORD;
MEMO_RECORD memo[MAX_STRING_LEN][MAX_STRING_LEN];
static int recC = 0;
int EditDistance2(char *src, char *dest, int i, int j)
{
    int distance = 0;
    recC++;
    //查表，直接返回
    if(memo[i][j].refCount != 0)
    {
        memo[i][j].refCount++;
        return memo[i][j].distance;
    }
    if(strlen(src + i) == 0)
    {
        distance = strlen(dest + j);
    }
    else if(strlen(dest + j) == 0)
    {
        distance = strlen(src + i);
    }
    else
    {
        if(src[i] == dest[j])
        {
            distance = EditDistance2(src, dest, i + 1, j + 1);
        }
        else
        {
            int edIns,edDel,edRep;
            //source 插入字符
            edIns = EditDistance2(src, dest, i, j + 1) + 1;
            //source 删除字符
            edDel = EditDistance2(src, dest, i+1, j) + 1;
            //source 替换字符
            edRep = EditDistance2(src, dest, i+1, j+1) + 1;
            distance = min(min(edIns, edDel), edRep);
        }
    }
    memo[i][j].distance = distance;
    memo[i][j].refCount = 1;
    return distance;
}
/*******************************************************************************
函数功能: 改进算法3(建表非递归算法)
*******************************************************************************/
int EditDistance3(char *src, char *dest)
{
    int i,j;
    int d[MAX_STRING_LEN][MAX_STRING_LEN] = { 0xFFFF };

    for(i = 0; i <= strlen(src); i++)
        d[i][0] = i;
    for(j = 0; j <= strlen(dest); j++)
        d[0][j] = j;

    for(i = 1; i <= strlen(src); i++)
    {
        for(j = 1; j <= strlen(dest); j++)
        {
            if((src[i - 1] == dest[j - 1]))
            {
                //不需要编辑操作
                d[i][j] = d[i - 1][j - 1];
            }
            else
            {
                int edIns,edDel,edRep;
                //source 插入字符
                edIns = d[i][j - 1] + 1;
                //source 删除字符
                edDel = d[i - 1][j] + 1;
                //source 替换字符
                edRep = d[i - 1][j - 1] + 1;
                d[i][j] = min(min(edIns, edDel), edRep);
            }
        }
    }
    return d[strlen(src)][strlen(dest)];
}
//
int test1=0,test2=0,test3=0;
#define TEST(Name,Size)  const uint16_t Size;

//
//冒泡
void Count_Sort_Buble_U32(uint32_t data[], uint16_t n) 
{
    uint16_t i,j;
	uint32_t temp;
    //两个for循环，每次取出一个元素跟数组的其他元素比较
    //将最大的元素排到最后。
    for(j=0;j<n-1;j++) {
        //外循环一次，就排好一个数，并放在后面，
        //所以比较前面n-j-1个元素即可
        for(i=0;i<n-j-1;i++) {
            if(data[i]>data[i+1]) {
                temp = data[i];
                data[i] = data[i+1];
                data[i+1] = temp;
            }
        }
    }  
}
void Count_Sort_Buble_U32(uint32_t data[], uint16_t n);
uint32_t test_i32[10]={8,5,2,1,4,6,7,9,3,0};
int main(int argc, char* argv[])
{
    int i,j;
	  TEST(a,b);
    for(i = 0; i < MAX_STRING_LEN; i++)
    {
        for(j = 0; j < MAX_STRING_LEN; j++)
        {
            memo[i][j].distance = INVALID_VALUE;
            memo[i][j].refCount = 0;
        }
    }
    test1 = EditDistance1("SNOWY", "SUNNY");
    test2 = EditDistance2("SNOWY", "SUNNY", 0, 0);
    test3 = EditDistance3("SNOWY", "SUNNY");
		//
		
		Count_Sort_Buble_U32(test_i32,10);
		test_i32[0]=0;
		for(i=2;i<=7;i++)
		{
			test_i32[0]	+=	test_i32[i];
		}
		test_i32[0]	=	test_i32[0]/6;
		//
		test_i32[0] = test_i32[0];
		//
		return 0;
}
