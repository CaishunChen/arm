/*
***********************************************************************************
*                    ��    ��: ������
*                    ����ʱ��: 2015-06-03
***********************************************************************************
*/
/*
�㷨��Ƶĳ���˼��
   ̰����(��Ϊ"����",�ܶ����̰�����ò������Ž�)
      ��Ʋ���:
         1, ���������⾫ȷ��������ѧģ��,�����������Ž��ģ��.
         2, ������ֽ�Ϊһϵ��������,ͬʱ��������������Ž�ṹ.
         3, ��̰��ԭ��ȷ��ÿ��������ֲ����Ž�,�ٶѵ���ȫ�����Ž�.
      ʵ��:
         1, 0-1����(ÿ����Ʒֻ����1��)
   ���η�(�ֽ�,���,�ϲ�)
      ˵��
         1, ������ѧ���ɷ�֤��������,һ��Ϳ����÷��η����.
         2, �ݹ�ͷ�����һ�Ժ�����.
   ��̬�滮����:
      ��Ʋ���:
         1, ��������������
         2, ����״̬
         3, ������ߺ�״̬ת������
         4, ȷ���߽�����
      ʵ��:
ʵ��:
   �ַ����༭����(����/ɾ��/�滻)
*/
//------------------------------ Includes ---------------------------------------
#include  <stdio.h>
#include  <stdint.h>
#include  <string.h>
#include  <ctype.h>
#include  <stdlib.h>
#include  <stdarg.h>

//------------------------------ �̶��� -----------------------------------------
#define min(a, b)  (((a) < (b)) ? (a) : (b))
/*******************************************************************************
��������: ԭʼ�㷨1(���صĵݹ��㷨)
*******************************************************************************/
int EditDistance1(char *src, char *dest)
{
    int edIns,edDel,edRep;
    //�鵽��β,�򷵻��ַ������Ȳ�
    if((strlen(src) == 0) || (strlen(dest) == 0))
        return abs((int)strlen(src) - (int)strlen(dest));
    //�ַ���ͬ,��Ƚ���һ���ַ�
    if(src[0] == dest[0])
        return EditDistance1(src + 1, dest + 1);
    //�����ַ�
    edIns = EditDistance1(src, dest + 1) + 1;
    //ɾ���ַ�
    edDel = EditDistance1(src + 1, dest) + 1;
    //�滻�ַ�
    edRep = EditDistance1(src + 1, dest + 1) + 1;
    return min(min(edIns, edDel), edRep);
}
/*******************************************************************************
��������: �Ľ��㷨2(����ĵݹ��㷨)
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
    //���ֱ�ӷ���
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
            //source �����ַ�
            edIns = EditDistance2(src, dest, i, j + 1) + 1;
            //source ɾ���ַ�
            edDel = EditDistance2(src, dest, i+1, j) + 1;
            //source �滻�ַ�
            edRep = EditDistance2(src, dest, i+1, j+1) + 1;
            distance = min(min(edIns, edDel), edRep);
        }
    }
    memo[i][j].distance = distance;
    memo[i][j].refCount = 1;
    return distance;
}
/*******************************************************************************
��������: �Ľ��㷨3(����ǵݹ��㷨)
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
                //����Ҫ�༭����
                d[i][j] = d[i - 1][j - 1];
            }
            else
            {
                int edIns,edDel,edRep;
                //source �����ַ�
                edIns = d[i][j - 1] + 1;
                //source ɾ���ַ�
                edDel = d[i - 1][j] + 1;
                //source �滻�ַ�
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
//ð��
void Count_Sort_Buble_U32(uint32_t data[], uint16_t n) 
{
    uint16_t i,j;
	uint32_t temp;
    //����forѭ����ÿ��ȡ��һ��Ԫ�ظ����������Ԫ�رȽ�
    //������Ԫ���ŵ����
    for(j=0;j<n-1;j++) {
        //��ѭ��һ�Σ����ź�һ�����������ں��棬
        //���ԱȽ�ǰ��n-j-1��Ԫ�ؼ���
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
