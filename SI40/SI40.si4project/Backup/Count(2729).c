/*
***********************************************************************************
*                    作    者: 徐松亮
*                    更新时间: 2015-06-03
***********************************************************************************
*/
#define COUNT_GLOBAL

#if (defined(STM32F1))
#include "Count.h"
#include "uctsk_Debug.h"
#include "Bsp_Rtc.h"
#include "Bsp_Tim.h"
#include "math.h"
#include "stm32_dsp.h"
#include "stm32f10x.h"
#define COUNT_DEBUG_OUT_STR(str)    DebugOutStr(str)
#elif (defined(STM32F4))
#include "Count.h"
#include "uctsk_Debug.h"
#include "Bsp_Rtc.h"
#include "Bsp_Tim.h"
#include "math.h"
#include "stm32_dsp.h"
#include "stm32f4xx.h"
#define COUNT_DEBUG_OUT_STR(str)    DebugOutStr(str)
#elif (defined(NRF51))
#include "Count.h"
#include "Bsp_Tim.h"
#include "math.h"
#include "nrf51.h"
#include "uctsk_Debug.h"
#include "nrf_drv_rng.h"
#define COUNT_DEBUG_OUT_STR(str)    DebugOutStr(str)
#elif (defined(NRF52))
#include "Count.h"
#include "Bsp_Tim.h"
#include "math.h"
#include "nrf52.h"
#include "uctsk_Debug.h"
#include "nrf_drv_rng.h"
#define COUNT_DEBUG_OUT_STR(str)    DebugOutStr(str)
#endif

//
#ifdef   COUNT_NON_LINEAR_EQUATION
static void Count_NonLinearEquation_Test(void);
#endif
#ifdef   COUNT_SPLINE3_ENABLE
static void Count_Spline3_Test(void);
#endif
#ifdef   COUNT_GA_ENABLE
static void Count_GA_Test(void);
#endif
#ifdef   COUNT_ACO_ENABLE
static void Count_ACO_Test(void);
#endif
#if	(COUNT_SOFT_TIMER_NUM!=0)
static COUNT_S_SOFT_TIMER s_tTmr[COUNT_SOFT_TIMER_NUM];
static void Count_SoftTimer_Init(void);
static void Count_SoftTimer_Dec(COUNT_S_SOFT_TIMER *_tmr);
#endif

/*
******************************************************************************
* 函数功能: 初始化
******************************************************************************
*/
void Count_Init(void)
{
#if   (defined(STM32F1))
#elif (defined(STM32F4))
    // 使能RNG时钟源
    RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);
    // 使能RNG
    RNG_Cmd(ENABLE);
    //
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);
#elif (defined(NRF51)||defined(NRF52))
    uint32_t err_code;
    err_code = nrf_drv_rng_init(NULL);
    APP_ERROR_CHECK(err_code);
#endif
#if	(COUNT_SOFT_TIMER_NUM!=0)
	Count_SoftTimer_Init();
#endif
}
void Count_Transplant_1ms(void)
{
#if	(COUNT_SOFT_TIMER_NUM!=0)
	uint8_t i;
	// 每隔1ms，对软件定时器的计数器进行减一操作
	for (i = 0; i < COUNT_SOFT_TIMER_NUM; i++)
	{
		Count_SoftTimer_Dec(&s_tTmr[i]);
	}	
#endif	
}
void Count_DebugTestOnOff(uint8_t OnOff)
{
    uint8_t *pbuf;
    OnOff=OnOff;
    //申请缓存
    pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    //数据类型长度打印
    sprintf((char*)pbuf,"char    :  %01d\r\n",sizeof(char));
    COUNT_DEBUG_OUT_STR((int8_t*)pbuf);
    sprintf((char*)pbuf,"int     :  %01d\r\n",sizeof(int));
    COUNT_DEBUG_OUT_STR((int8_t*)pbuf);
    sprintf((char*)pbuf,"short   :  %01d\r\n",sizeof(short));
    COUNT_DEBUG_OUT_STR((int8_t*)pbuf);
    sprintf((char*)pbuf,"long    :  %01d\r\n",sizeof(long));
    COUNT_DEBUG_OUT_STR((int8_t*)pbuf);
    sprintf((char*)pbuf,"float   :  %01d\r\n",sizeof(float));
    COUNT_DEBUG_OUT_STR((int8_t*)pbuf);
    sprintf((char*)pbuf,"double  :  %01d\r\n",sizeof(double));
    COUNT_DEBUG_OUT_STR((int8_t*)pbuf);
    //math.h中绝对值测试
#if 0
    /*
    uint8_t i8=15,j8=20;
    uint8_t k8;
    k8=abs(i8-j8);
    k8=k8;
    */
    int8_t t=30;
    uint8_t h=50;
    uint8_t thi;
    while(1)
    {
        Count_THI(t,h,&thi);
        thi=thi;
    }
#endif
    // dsp 库测试
#if 0
    float32_t pSrc;
    float32_t pDst;
    pSrc -= 1.23f;
    arm_abs_f32(&pSrc, &pDst, 1);
#endif
    // 测试交换
    sprintf((char*)pbuf,"-----Exchange\r\n");
    COUNT_DEBUG_OUT_STR((int8_t*)pbuf);
    {
        uint16_t x16=100,y16=200;
        sprintf((char*)pbuf,"x16=%d,y16=%d\r\n",x16,y16);
        COUNT_DEBUG_OUT_STR((int8_t*)pbuf);
        COUNT_EXCHANGE(&x16,&y16);
        sprintf((char*)pbuf,"x16=%d,y16=%d\r\n",x16,y16);
        COUNT_DEBUG_OUT_STR((int8_t*)pbuf);
    }
    //测试随机数
#if 0
    {
        uint16_t i16;
        sprintf((char*)pbuf,"-----Rand\r\n");
        COUNT_DEBUG_OUT_STR((int8_t*)pbuf);
        for(i16=0; i16<100; i16++)
        {
            sprintf((char*)pbuf,"Rand(%05d):%ld\r\n",i16,Count_Rand());
            COUNT_DEBUG_OUT_STR((int8_t*)pbuf);
            MODULE_OS_DELAY_MS(10);
        }
    }
#endif
    //测试插值算法---一次样条
    {
        float32_t y;
        arm_linear_interp_instance_f32 *ps;
        float32_t *py;
        ps = (arm_linear_interp_instance_f32 *)&pbuf[100];
        py = (float32_t *)&pbuf[200];
        py[0]=0;
        py[1]=2;
        py[2]=1;
        ps->nValues=3;
        ps->x1=0;
        ps->xSpacing=2;
        ps->pYData=py;
        sprintf((char*)pbuf,"-----Interpolation(SpLine1)\r\n");
        COUNT_DEBUG_OUT_STR((int8_t*)pbuf);
        sprintf((char*)pbuf,"x=[0,2,4],y=[0,2,1]\r\n");
        COUNT_DEBUG_OUT_STR((int8_t*)pbuf);
        y = arm_linear_interp_f32(ps , 1);
        sprintf((char*)pbuf,"x=1,y=%f\r\n",y);
        COUNT_DEBUG_OUT_STR((int8_t*)pbuf);
        y = arm_linear_interp_f32(ps , 3);
        sprintf((char*)pbuf,"x=3,y=%f\r\n",y);
        COUNT_DEBUG_OUT_STR((int8_t*)pbuf);
    }
#ifdef   COUNT_NON_LINEAR_EQUATION
    //测试算法---非线性方程求解
    Count_NonLinearEquation_Test();
#endif
#ifdef   COUNT_SPLINE3_ENABLE
    //测试算法---插值---三次插条
    Count_Spline3_Test();
#endif
#ifdef   COUNT_GA_ENABLE
    //测试算法---遗传算法
    Count_GA_Test();
#endif
#ifdef   COUNT_ACO_ENABLE
    //测试算法---蚁群算法
    Count_ACO_Test();
#endif
    //释放缓存
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
}
/*
******************************************************************************
* 函数功能: 定时器
******************************************************************************
*/
#if	(COUNT_SOFT_TIMER_NUM!=0)
// 定于软件定时器结构体变量
static COUNT_S_SOFT_TIMER s_tTmr[COUNT_SOFT_TIMER_NUM];
static void Count_SoftTimer_Init(void)
{
	uint8_t i;
	// 清零所有的软件定时器
	for (i = 0; i < COUNT_SOFT_TIMER_NUM; i++)
	{
		s_tTmr[i].Count = 0;
		s_tTmr[i].PreLoad = 0;
		s_tTmr[i].Flag = 0;
		s_tTmr[i].Mode = COUNT_E_SOFT_TIMER_ONCE_MODE;
	}
}
static void Count_SoftTimer_Dec(COUNT_S_SOFT_TIMER *_tmr)
{
	if (_tmr->Count > 0)
	{
		/* 如果定时器变量减到1则设置定时器到达标志 */
		if (--_tmr->Count == 0)
		{
			_tmr->Flag = 1;

			/* 如果是自动模式，则自动重装计数器 */
			if(_tmr->Mode == COUNT_E_SOFT_TIMER_AUTO_MODE)
			{
				_tmr->Count = _tmr->PreLoad;
			}
		}
	}
}
/*
*********************************************************************************************************
*	函 数 名: bsp_StartTimer
*	功能说明: 启动一个定时器，并设置定时周期。
*	形    参:  	_id     : 定时器ID，值域【0,COUNT_SOFT_TIMER_NUM-1】。用户必须自行维护定时器ID，以避免定时器ID冲突。
*				_period : 定时周期，单位1ms
*	返 回 值: 无
*********************************************************************************************************
*/

void Count_SoftTimer_Start(uint8_t _id, uint32_t _period)
{
	if (_id >= COUNT_SOFT_TIMER_NUM)
	{
		/* 打印出错的源代码文件名、函数名称 */
		BSP_Printf("Error: file %s, function %s()\r\n", __FILE__, __FUNCTION__);
		while(1); /* 参数异常，死机等待看门狗复位 */
	}

	DISABLE_INT();  			/* 关中断 */

	s_tTmr[_id].Count = _period;		/* 实时计数器初值 */
	s_tTmr[_id].PreLoad = _period;		/* 计数器自动重装值，仅自动模式起作用 */
	s_tTmr[_id].Flag = 0;				/* 定时时间到标志 */
	s_tTmr[_id].Mode = COUNT_E_SOFT_TIMER_ONCE_MODE;	/* 1次性工作模式 */

	ENABLE_INT();  				/* 开中断 */
}
/*
*********************************************************************************************************
*	函 数 名: bsp_StartAutoTimer
*	功能说明: 启动一个自动定时器，并设置定时周期。
*	形    参:  	_id     : 定时器ID，值域【0,COUNT_SOFT_TIMER_NUM-1】。用户必须自行维护定时器ID，以避免定时器ID冲突。
*				_period : 定时周期，单位10ms
*	返 回 值: 无
*********************************************************************************************************
*/

void Count_SoftTimer_AutoStart(uint8_t _id, uint32_t _period)
{
	if (_id >= COUNT_SOFT_TIMER_NUM)
	{
		/* 打印出错的源代码文件名、函数名称 */
		BSP_Printf("Error: file %s, function %s()\r\n", __FILE__, __FUNCTION__);
		while(1); /* 参数异常，死机等待看门狗复位 */
	}

	DISABLE_INT();  		/* 关中断 */

	s_tTmr[_id].Count = _period;			/* 实时计数器初值 */
	s_tTmr[_id].PreLoad = _period;		/* 计数器自动重装值，仅自动模式起作用 */
	s_tTmr[_id].Flag = 0;				/* 定时时间到标志 */
	s_tTmr[_id].Mode = COUNT_E_SOFT_TIMER_AUTO_MODE;	/* 自动工作模式 */

	ENABLE_INT();  			/* 开中断 */
}
/*
*********************************************************************************************************
*	函 数 名: bsp_StopTimer
*	功能说明: 停止一个定时器
*	形    参:  	_id     : 定时器ID，值域【0,COUNT_SOFT_TIMER_NUM-1】。用户必须自行维护定时器ID，以避免定时器ID冲突。
*	返 回 值: 无
*********************************************************************************************************
*/
void Count_SoftTimer_Stop(uint8_t _id)
{
	if (_id >= COUNT_SOFT_TIMER_NUM)
	{
		/* 打印出错的源代码文件名、函数名称 */
		BSP_Printf("Error: file %s, function %s()\r\n", __FILE__, __FUNCTION__);
		while(1); /* 参数异常，死机等待看门狗复位 */
	}

	DISABLE_INT();  	/* 关中断 */

	s_tTmr[_id].Count = 0;				/* 实时计数器初值 */
	s_tTmr[_id].Flag = 0;				/* 定时时间到标志 */
	s_tTmr[_id].Mode = COUNT_E_SOFT_TIMER_ONCE_MODE;	/* 自动工作模式 */

	ENABLE_INT();  		/* 开中断 */
}

/*
*********************************************************************************************************
*	函 数 名: bsp_CheckTimer
*	功能说明: 检测定时器是否超时
*	形    参:  	_id     : 定时器ID，值域【0,COUNT_SOFT_TIMER_NUM-1】。用户必须自行维护定时器ID，以避免定时器ID冲突。
*				_period : 定时周期，单位1ms
*	返 回 值: 返回 0 表示定时未到， 1表示定时到
*********************************************************************************************************
*/
uint8_t Count_SoftTimer_Check(uint8_t _id)
{
	if (_id >= COUNT_SOFT_TIMER_NUM)
	{
		return 0;
	}

	if (s_tTmr[_id].Flag == 1)
	{
		s_tTmr[_id].Flag = 0;
		return 1;
	}
	else
	{
		return 0;
	}
}

#endif
/*
******************************************************************************
* 函数功能: 延时
******************************************************************************
*/
void Count_DelayUs(uint32_t x)
{
#if   (defined(STM32F1) || defined(STM32F4))
    uint32_t _dcnt;
    _dcnt=(x*10);
    while(_dcnt-- > 0) ;	
#elif   (defined(NRF51))
    uint32_t _dcnt;
    _dcnt=(x*4);
    while(_dcnt-- > 0) ;
#else
    uint32_t _dcnt;
    _dcnt=(x*10);
    while(_dcnt-- > 0) ;
#endif
}
/*
******************************************************************************
* 函数功能: 定时器延时us
******************************************************************************
*/
void Count_SysTickDelayUs(uint32_t n)
{
#if   (defined(STM32F1) || defined(STM32F4))
    uint32_t ticks;
    uint32_t told;
    uint32_t tnow;
    uint32_t tcnt = 0;
    uint32_t reload;

    reload = SysTick->LOAD;
    // 需要的节拍数
    ticks = n * (SystemCoreClock / 1000000);
    tcnt = 0;
    // 刚进入时的计数器值
    told = SysTick->VAL;

    while (1)
    {
        tnow = SysTick->VAL;
        if (tnow != told)
        {
            // SYSTICK是一个递减的计数器
            if (tnow < told)
            {
                tcnt += told - tnow;
            }
            // 重新装载递减
            else
            {
                tcnt += reload - tnow + told;
            }
            told = tnow;

            // 时间超过/等于要延迟的时间,则退出
            if (tcnt >= ticks)
            {
                break;
            }
        }
    }
#endif
}
void Count_GetTimeMs(uint32_t *pms)
{
    *pms=Tim_1ms_Count;
}
/*
******************************************************************************
* 函数功能: 随机数
******************************************************************************
*/
int Count_Rand(void)
{
    uint32_t i32=0;
#if   (defined(STM32F1))
    BspRtc_ReadRealTime(NULL,NULL,&i32,NULL);
    srand(i32);
    return rand();
    //
#elif (defined(STM32F4))
    // 等待随机信号继续
    while(RNG_GetFlagStatus(RNG_FLAG_DRDY)== RESET);
    // 得到32位随机数
    i32 = RNG_GetRandomNumber();
    i32=i32%COUNT_RAND_MAX;
    return i32;
#elif (defined(NRF51)||defined(NRF52))
    uint32_t err_code;
    uint8_t  available;

    nrf_drv_rng_bytes_available(&available);
    uint8_t length = MIN(4, available);

    err_code = nrf_drv_rng_rand((uint8_t*)&i32, length);
    APP_ERROR_CHECK(err_code);

    i32=i32%COUNT_RAND_MAX;
    return i32;
#else
    return i32;
#endif
}
/*******************************************************************************
* 函数功能: 不快指数
* 参    数: temp --- 温度(度)    humi --- 湿度(%)
* 输    出: *ThiValue --- 不快指数
* 返 回 值: OK/ERR
* 不快指数: 由温度与湿度换算的数值
* 公    式: 不快指数=0.81*温度+0.01*湿度*(0.99*温度-14.3)+46.3
* 含    义:   ~55 :  寒冷     需要室内加温
            55~60 :  肌寒     需要多穿衣服
            60~65 :  凉爽     无需改善
            65~70 :  舒适     无需改善
            70~75 :  温暖     无需改善
            75~80 :  稍热     减衣服，开风扇
            80~85 :  汗热
            85~   :  大热
* 查    表: 结果与上面公式等同
      湿度
      40 45 50 55 60 65 70 75 80 85 90
温度  --------------------------------
11 |  54 54 54 53 53 53 53 53 52 52 52
12 |  55 55 55 55 55 54 54 54 54 54 54
13 |  56 56 56 56 56 56 56 56 56 56 56
14 |  57 57 57 57 57 57 57 57 57 57 57
15 |  59 59 59 59 59 59 59 59 59 59 59
16 |  60 60 60 60 60 60 60 60 60 61 61
17 |  61 61 61 61 62 62 62 62 62 62 62
18 |  62 62 63 63 63 63 63 64 64 64 64
19 |  63 64 64 64 64 65 65 65 65 66 66
20 |  65 65 65 66 66 66 66 67 67 67 67
21 |  66 66 67 67 67 68 68 68 69 69 69
22 |  67 67 68 68 69 69 69 70 70 70 71
23 |  68 69 69 70 70 70 71 71 72 72 73
24 |  70 70 70 71 71 72 72 73 73 74 74
25 |  71 71 72 72 73 73 74 74 75 75 76
26 |  72 73 73 74 74 75 75 76 77 77 78
27 |  73 74 74 75 76 76 77 77 78 79 79
28 |  74 75 76 76 77 78 78 79 80 80 81
29 |  76 76 77 78 78 79 80 81 81 82 83
30 |  77 78 78 79 80 81 81 82 83 84 84
31 |  78 79 80 80 81 82 83 84 85 85 86
32 |  79 80 81 82 83 84 84 85 86 87 88
33 |  80 81 82 83 84 85 86 87 88 89 90
34 |  82 83 84 84 85 86 87 88 89 90 91
35 |  83 84 85 86 87 88 89 90 91 92 93
36 |  84 85 86 87 88 89 90 91 93 94 95
37 |  85 86 87 89 90 91 92 93 94 95 96
38 |  86 88 89 90 91 92 93 95 96 97 98
*******************************************************************************/
uint8_t Count_THI(int8_t temp,uint8_t humi,uint8_t *ThiValue)
{
    float f_THI;
    //原算法
    //f_THI=0.81*temp+0.01*humi*(0.99*temp-14.3)+46.3;
    //熙康算法
    f_THI=0.81*temp+0.01*humi*(0.99*temp-19)+46.3;
    *ThiValue = (uint8_t)f_THI;
    return OK;
}
/*******************************************************************************
* 函数功能: 16位大小端互转
* 参    数: val
* 返 回 值: 转换后的值
*******************************************************************************/
uint16_t Count_Int16ByteReversed(uint16_t val)
{
    uint16_t res = 0;
    res = val << 8;
    res += val >> 8;
    return res;
}

/*******************************************************************************
* 函数功能: 32位大小端互转
* 参    数: val
* 返 回 值: 转换后的值
*******************************************************************************/
uint32_t Count_Int32ByteReversed(uint32_t val)
{
    uint8_t *p = (uint8_t *)&val;
    uint8_t arr[4] = {0};
    arr[0] = p[3];
    arr[1] = p[2];
    arr[2] = p[1];
    arr[3] = p[0];
    return *(uint32_t *)arr;
}
/*******************************************************************************
* 函数功能: 循环加
* 参    数: val
* 返 回 值: 转换后的值
*******************************************************************************/
uint8_t Count_AddCyc(uint8_t degindata,uint8_t num,uint8_t maxdata)
{
    uint16_t i16;
    i16=degindata+num;
    if(i16<=maxdata)
    {
        return ((uint8_t)i16);
    }
    else
    {
        while(i16>maxdata)
        {
            i16=i16-maxdata-1;
        }
        return ((uint8_t)i16);
    }
}
/*******************************************************************************
* 函数功能: 循环减
* 返 回 值: 转换后的值
*******************************************************************************/
uint8_t Count_SubCyc(uint8_t degindata,uint8_t num,uint8_t maxdata)
{
    uint8_t i;
    num=num%(maxdata+1);
    if(degindata>=num)
    {
        i=degindata-num;
        return i;
    }
    else
    {
        return(degindata+maxdata+1-num);
    }
}
//-------------------------------------------------------------------------------
// 函数名称: uint16_t Count_Sum(uint16_t BeginSum,uint8_t *Buf,uint16_t len)
// 函数功能: 计算和
// 调用函数: 无
// 入口参数: *Buf 发送数据串 及 len数据长度
// 返回参数: 和结果
// 修改说明：
// 修改时间：
//-------------------------------------------------------------------------------
uint16_t Count_Sum(uint16_t BeginSum,uint8_t *pBuf,uint16_t len)
{
    while (len--)
    {
        BeginSum += *pBuf++;
    }
    return BeginSum;
}
uint32_t Count_SumMax(uint32_t BeginSum,uint8_t *pBuf,uint32_t len)
{
    while (len--)
    {
        BeginSum += *pBuf++;
    }
    return BeginSum;
}
uint32_t Count_Sum16(uint32_t BeginSum,uint16_t *pBuf,uint16_t len)
{
    while (len--)
    {
        BeginSum += *pBuf++;
    }
    return BeginSum;
}

/*******************************************************************************
函数功能: m^n函数
参    数:
返 回 值: 计算结果
*******************************************************************************/
uint32_t Count_Pow(uint8_t m,uint8_t n)
{
    uint32_t result=1;
    while(n--)result*=m;
    return result;
}
/*******************************************************************************
函数功能: 求最大,最小,平均值
参    数:
*******************************************************************************/
uint16_t Count_MaxMinAve_Byte(uint8_t* pbuf,uint16_t len,uint8_t filterMin,uint8_t filterMax,uint8_t* pmax,uint8_t* pmin,uint8_t* pave)
{
    uint32_t sum=0;
    uint32_t i32=0;
    uint8_t min=0xFF,max=0;
    uint16_t i16;
    i16 = len;

    for(i32=0; i32<len; i32++)
    {
        // 过滤大小值
        if(pbuf[i32]<filterMin || pbuf[i32]>filterMax)
        {
            i16--;
            continue;
        }
        //
        sum+=pbuf[i32];
        if(min>pbuf[i32])
        {
            min=pbuf[i32];
        }
        if(max<pbuf[i32])
        {
            max=pbuf[i32];
        }
    }
    if(pmax!=NULL)
    {
        *pmax=max;
    }
    if(pmin!=NULL)
    {
        *pmin=min;
    }
    if(pave!=NULL)
    {
        *pave=sum/i16;
    }
    return i16;
}
/*******************************************************************************
函数功能: 异或计算
参    数: p   - 待计算的数组头地址
          len - 要计算的字节数
返 回 值: 计算结果
*******************************************************************************/
uint8_t Count_Xor(uint8_t BeginXor,uint8_t *str,uint32_t len)
{
    while(len--) BeginXor^=*str++;
    return(BeginXor);
}
/*******************************************************************************
函数功能: 求平均
参    数: *p - 待求数组     len - 长度
返 回 值: 平均值结果
*******************************************************************************/
uint16_t Count_Average8(uint8_t *p,uint16_t len)
{
    uint16_t i;
    uint32_t sumjs=0;
    for (i=0; i<=len-1; i++) sumjs+=*(p+i);
    sumjs=sumjs/len;
    return sumjs;
}
uint16_t Count_Average16(uint16_t *p,uint16_t len)
{
    uint16_t i;
    uint32_t sumjs=0;
    for (i=0; i<=len-1; i++) sumjs+=*(p+i);
    sumjs=sumjs/len;
    return sumjs;
}
uint32_t Count_Average32(uint32_t *p,uint16_t len)
{
    uint16_t i;
    uint32_t sumjs=0;
    for (i=0; i<=len-1; i++) sumjs+=*(p+i);
    sumjs=sumjs/len;
    return sumjs;
}
/*******************************************************************************
函数功能: 颜色转换
参    数: cmd  -  0  565-->666   并取反
                  1  666-->565   先取反再转换
返 回 值: void
*******************************************************************************/
void Count_ColorConver(uint8_t cmd,uint32_t *p32,uint8_t *p8)
{
    p32=p32;
    p8=p8;
    switch(cmd)
    {
        case 0:
            p8[0] = (~((*p32)>>(11-3)))|0x07;
            p8[1] = (~((*p32)>>(5-2)))|0x03;
            p8[2] = (~((*p32)<<3))|0x07;
            break;
        default:
            break;
    }
}
#ifdef COUNT_CRC16_ENABLE
/*******************************************************************************
函数功能: 求CRC16校验和
参    数: *databuf - 待求数组     len - 长度
返 回 值: CRC16
注意事项: CRC16由于对计算速度要求，数据长度控制在256字节,计算完成后读取校验值
*******************************************************************************/
uint8_t const CRC16_Hi[] =                    // CRC高位字节值表
{
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};
//-------------------------------------------------------------------------------
uint8_t const CRC16_Lo[] =                    // CRC低位字节值表
{
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
    0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
    0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
    0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
    0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
    0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
    0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
    0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
    0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
    0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
    0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
    0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
    0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
    0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
    0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
    0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
    0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
    0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};
uint16_t Count_CRC16(uint8_t *pchMsg, uint16_t DataLen)
{
    uint8_t uIndex;
    uint8_t uchCRCHi = 0xFF;            //CRC校验 高位
    uint8_t uchCRCLo = 0xFF;            //CRC校验 低位
    uint16_t res;
    uchCRCHi = 0xFF;
    uchCRCLo = 0xFF;
    while (DataLen--)
    {
        uIndex = uchCRCLo ^ *pchMsg++;
        uchCRCLo = uchCRCHi ^ CRC16_Hi[uIndex];
        uchCRCHi = CRC16_Lo[uIndex];
    }
    res = uchCRCHi*256+uchCRCLo;
    return res;
}
#endif
#ifdef COUNT_CRC32_ENABLE
/*******************************************************************************
函数功能: 求CRC32校验和
参    数: *databuf - 待求数组     len - 长度
返 回 值: CRC32
*******************************************************************************/
uint32_t const crc_32_tab[] =   /* CRC polynomial 0xedb88320 */
{
    0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
    0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
    0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
    0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
    0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
    0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
    0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
    0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
    0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
    0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
    0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
    0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
    0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
    0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
    0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
    0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
    0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
    0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
    0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
    0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
    0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
    0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
    0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
    0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
    0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
    0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
    0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
    0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
    0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
    0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
    0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
    0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
    0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
    0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
    0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
    0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
    0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
    0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
    0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
    0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
    0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
    0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
    0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};
uint32_t Count_CRC32(uint8_t *databuf,uint32_t len)
{
    uint32_t oldcrc32,crc32,oldcrc;
    uint16_t charcnt;
    uint8_t c;
    uint32_t t;

    oldcrc32 = 0x00000000;
    charcnt = 0;
    while(len --)
    {
        t = (oldcrc32>> 24)&0xFF;
        oldcrc = crc_32_tab[t];
        c = databuf[charcnt];
        oldcrc32 = (oldcrc32 << 8)|c;
        oldcrc32 = oldcrc32 ^ oldcrc;
        charcnt ++;
    }
    crc32 = oldcrc32;
    return crc32;
}
#endif
//
#ifdef COUNT_AES128_ENABLE
static uint32_t Count_AES128_msCount=0;
/******************************************************************************
********************************原始说明***************************************
 Used to specify the AES algorithm version.
 There are two possible AES implementations:
 1 - version with unique key-schedule
     using 256 + 256 + 10*4 bytes data = 552 bytes of look-up tables.
 2 - version with different key-schedule for encryption and decryption
     using 256 + 256 + 10*4 + 256*4*2 bytes data = 2058 bytes of look-up table.
    Version 2 is faster than version 1 but requires more memory.
 The value of COUNT_AES128_IMPLEMENTATION is automatically set according to
 the define COUNT_AES128_ALGORITHM in Config.h.
*******************************************************************************/
#if COUNT_AES128_ALGORITHM==1
#define COUNT_AES128_IMPLEMENTATION 1
#elif COUNT_AES128_ALGORITHM==2
#define COUNT_AES128_IMPLEMENTATION 2
#endif

/* Matrix Sbox for the Count_Aes128_Sbox operation in encryption procedure */
static  uint8_t Count_Aes128_Sbox[256] =
{
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5,
    0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0,
    0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc,
    0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a,
    0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0,
    0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b,
    0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85,
    0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5,
    0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17,
    0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88,
    0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c,
    0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9,
    0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6,
    0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e,
    0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94,
    0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68,
    0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

/* Matrix INVSBOX for the procedure of decryption */
static uint8_t Count_Aes128_InvSbox[256] =
{
    0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38,
    0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
    0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87,
    0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
    0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d,
    0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
    0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2,
    0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
    0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16,
    0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
    0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda,
    0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
    0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a,
    0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
    0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02,
    0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
    0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea,
    0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
    0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85,
    0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
    0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89,
    0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
    0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20,
    0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
    0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31,
    0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
    0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d,
    0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
    0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0,
    0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26,
    0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d
};

/* Matrix of number necessary for the keyschedule procedure */
static uint32_t Count_Aes128_Rcon[10] =
{
    0x01000000, 0x02000000, 0x04000000, 0x08000000,
    0x10000000, 0x20000000, 0x40000000, 0x80000000,
    0x1b000000, 0x36000000
};

#if COUNT_AES128_IMPLEMENTATION==2

static  uint32_t Count_Aes128_EncTable[256] =
{
    0xc66363a5  , 0xf87c7c84  , 0xee777799  , 0xf67b7b8d  ,
    0xfff2f20d  , 0xd66b6bbd  , 0xde6f6fb1  , 0x91c5c554  ,
    0x60303050  , 0x02010103  , 0xce6767a9  , 0x562b2b7d  ,
    0xe7fefe19  , 0xb5d7d762  , 0x4dababe6  , 0xec76769a  ,
    0x8fcaca45  , 0x1f82829d  , 0x89c9c940  , 0xfa7d7d87  ,
    0xeffafa15  , 0xb25959eb  , 0x8e4747c9  , 0xfbf0f00b  ,
    0x41adadec  , 0xb3d4d467  , 0x5fa2a2fd  , 0x45afafea  ,
    0x239c9cbf  , 0x53a4a4f7  , 0xe4727296  , 0x9bc0c05b  ,
    0x75b7b7c2  , 0xe1fdfd1c  , 0x3d9393ae  , 0x4c26266a  ,
    0x6c36365a  , 0x7e3f3f41  , 0xf5f7f702  , 0x83cccc4f  ,
    0x6834345c  , 0x51a5a5f4  , 0xd1e5e534  , 0xf9f1f108  ,
    0xe2717193  , 0xabd8d873  , 0x62313153  , 0x2a15153f  ,
    0x0804040c  , 0x95c7c752  , 0x46232365  , 0x9dc3c35e  ,
    0x30181828  , 0x379696a1  , 0x0a05050f  , 0x2f9a9ab5  ,
    0x0e070709  , 0x24121236  , 0x1b80809b  , 0xdfe2e23d  ,
    0xcdebeb26  , 0x4e272769  , 0x7fb2b2cd  , 0xea75759f  ,
    0x1209091b  , 0x1d83839e  , 0x582c2c74  , 0x341a1a2e  ,
    0x361b1b2d  , 0xdc6e6eb2  , 0xb45a5aee  , 0x5ba0a0fb  ,
    0xa45252f6  , 0x763b3b4d  , 0xb7d6d661  , 0x7db3b3ce  ,
    0x5229297b  , 0xdde3e33e  , 0x5e2f2f71  , 0x13848497  ,
    0xa65353f5  , 0xb9d1d168  , 0x00000000  , 0xc1eded2c  ,
    0x40202060  , 0xe3fcfc1f  , 0x79b1b1c8  , 0xb65b5bed  ,
    0xd46a6abe  , 0x8dcbcb46  , 0x67bebed9  , 0x7239394b  ,
    0x944a4ade  , 0x984c4cd4  , 0xb05858e8  , 0x85cfcf4a  ,
    0xbbd0d06b  , 0xc5efef2a  , 0x4faaaae5  , 0xedfbfb16  ,
    0x864343c5  , 0x9a4d4dd7  , 0x66333355  , 0x11858594  ,
    0x8a4545cf  , 0xe9f9f910  , 0x04020206  , 0xfe7f7f81  ,
    0xa05050f0  , 0x783c3c44  , 0x259f9fba  , 0x4ba8a8e3  ,
    0xa25151f3  , 0x5da3a3fe  , 0x804040c0  , 0x058f8f8a  ,
    0x3f9292ad  , 0x219d9dbc  , 0x70383848  , 0xf1f5f504  ,
    0x63bcbcdf  , 0x77b6b6c1  , 0xafdada75  , 0x42212163  ,
    0x20101030  , 0xe5ffff1a  , 0xfdf3f30e  , 0xbfd2d26d  ,
    0x81cdcd4c  , 0x180c0c14  , 0x26131335  , 0xc3ecec2f  ,
    0xbe5f5fe1  , 0x359797a2  , 0x884444cc  , 0x2e171739  ,
    0x93c4c457  , 0x55a7a7f2  , 0xfc7e7e82  , 0x7a3d3d47  ,
    0xc86464ac  , 0xba5d5de7  , 0x3219192b  , 0xe6737395  ,
    0xc06060a0  , 0x19818198  , 0x9e4f4fd1  , 0xa3dcdc7f  ,
    0x44222266  , 0x542a2a7e  , 0x3b9090ab  , 0x0b888883  ,
    0x8c4646ca  , 0xc7eeee29  , 0x6bb8b8d3  , 0x2814143c  ,
    0xa7dede79  , 0xbc5e5ee2  , 0x160b0b1d  , 0xaddbdb76  ,
    0xdbe0e03b  , 0x64323256  , 0x743a3a4e  , 0x140a0a1e  ,
    0x924949db  , 0x0c06060a  , 0x4824246c  , 0xb85c5ce4  ,
    0x9fc2c25d  , 0xbdd3d36e  , 0x43acacef  , 0xc46262a6  ,
    0x399191a8  , 0x319595a4  , 0xd3e4e437  , 0xf279798b  ,
    0xd5e7e732  , 0x8bc8c843  , 0x6e373759  , 0xda6d6db7  ,
    0x018d8d8c  , 0xb1d5d564  , 0x9c4e4ed2  , 0x49a9a9e0  ,
    0xd86c6cb4  , 0xac5656fa  , 0xf3f4f407  , 0xcfeaea25  ,
    0xca6565af  , 0xf47a7a8e  , 0x47aeaee9  , 0x10080818  ,
    0x6fbabad5  , 0xf0787888  , 0x4a25256f  , 0x5c2e2e72  ,
    0x381c1c24  , 0x57a6a6f1  , 0x73b4b4c7  , 0x97c6c651  ,
    0xcbe8e823  , 0xa1dddd7c  , 0xe874749c  , 0x3e1f1f21  ,
    0x964b4bdd  , 0x61bdbddc  , 0x0d8b8b86  , 0x0f8a8a85  ,
    0xe0707090  , 0x7c3e3e42  , 0x71b5b5c4  , 0xcc6666aa  ,
    0x904848d8  , 0x06030305  , 0xf7f6f601  , 0x1c0e0e12  ,
    0xc26161a3  , 0x6a35355f  , 0xae5757f9  , 0x69b9b9d0  ,
    0x17868691  , 0x99c1c158  , 0x3a1d1d27  , 0x279e9eb9  ,
    0xd9e1e138  , 0xebf8f813  , 0x2b9898b3  , 0x22111133  ,
    0xd26969bb  , 0xa9d9d970  , 0x078e8e89  , 0x339494a7  ,
    0x2d9b9bb6  , 0x3c1e1e22  , 0x15878792  , 0xc9e9e920  ,
    0x87cece49  , 0xaa5555ff  , 0x50282878  , 0xa5dfdf7a  ,
    0x038c8c8f  , 0x59a1a1f8  , 0x09898980  , 0x1a0d0d17  ,
    0x65bfbfda  , 0xd7e6e631  , 0x844242c6  , 0xd06868b8  ,
    0x824141c3  , 0x299999b0  , 0x5a2d2d77  , 0x1e0f0f11  ,
    0x7bb0b0cb  , 0xa85454fc  , 0x6dbbbbd6  , 0x2c16163a
};

static  uint32_t Count_Aes128_DecTable[256] =
{
    0x51f4a750  , 0x7e416553  , 0x1a17a4c3  , 0x3a275e96  ,
    0x3bab6bcb  , 0x1f9d45f1  , 0xacfa58ab  , 0x4be30393  ,
    0x2030fa55  , 0xad766df6  , 0x88cc7691  , 0xf5024c25  ,
    0x4fe5d7fc  , 0xc52acbd7  , 0x26354480  , 0xb562a38f  ,
    0xdeb15a49  , 0x25ba1b67  , 0x45ea0e98  , 0x5dfec0e1  ,
    0xc32f7502  , 0x814cf012  , 0x8d4697a3  , 0x6bd3f9c6  ,
    0x038f5fe7  , 0x15929c95  , 0xbf6d7aeb  , 0x955259da  ,
    0xd4be832d  , 0x587421d3  , 0x49e06929  , 0x8ec9c844  ,
    0x75c2896a  , 0xf48e7978  , 0x99583e6b  , 0x27b971dd  ,
    0xbee14fb6  , 0xf088ad17  , 0xc920ac66  , 0x7dce3ab4  ,
    0x63df4a18  , 0xe51a3182  , 0x97513360  , 0x62537f45  ,
    0xb16477e0  , 0xbb6bae84  , 0xfe81a01c  , 0xf9082b94  ,
    0x70486858  , 0x8f45fd19  , 0x94de6c87  , 0x527bf8b7  ,
    0xab73d323  , 0x724b02e2  , 0xe31f8f57  , 0x6655ab2a  ,
    0xb2eb2807  , 0x2fb5c203  , 0x86c57b9a  , 0xd33708a5  ,
    0x302887f2  , 0x23bfa5b2  , 0x02036aba  , 0xed16825c  ,
    0x8acf1c2b  , 0xa779b492  , 0xf307f2f0  , 0x4e69e2a1  ,
    0x65daf4cd  , 0x0605bed5  , 0xd134621f  , 0xc4a6fe8a  ,
    0x342e539d  , 0xa2f355a0  , 0x058ae132  , 0xa4f6eb75  ,
    0x0b83ec39  , 0x4060efaa  , 0x5e719f06  , 0xbd6e1051  ,
    0x3e218af9  , 0x96dd063d  , 0xdd3e05ae  , 0x4de6bd46  ,
    0x91548db5  , 0x71c45d05  , 0x0406d46f  , 0x605015ff  ,
    0x1998fb24  , 0xd6bde997  , 0x894043cc  , 0x67d99e77  ,
    0xb0e842bd  , 0x07898b88  , 0xe7195b38  , 0x79c8eedb  ,
    0xa17c0a47  , 0x7c420fe9  , 0xf8841ec9  , 0x00000000  ,
    0x09808683  , 0x322bed48  , 0x1e1170ac  , 0x6c5a724e  ,
    0xfd0efffb  , 0x0f853856  , 0x3daed51e  , 0x362d3927  ,
    0x0a0fd964  , 0x685ca621  , 0x9b5b54d1  , 0x24362e3a  ,
    0x0c0a67b1  , 0x9357e70f  , 0xb4ee96d2  , 0x1b9b919e  ,
    0x80c0c54f  , 0x61dc20a2  , 0x5a774b69  , 0x1c121a16  ,
    0xe293ba0a  , 0xc0a02ae5  , 0x3c22e043  , 0x121b171d  ,
    0x0e090d0b  , 0xf28bc7ad  , 0x2db6a8b9  , 0x141ea9c8  ,
    0x57f11985  , 0xaf75074c  , 0xee99ddbb  , 0xa37f60fd  ,
    0xf701269f  , 0x5c72f5bc  , 0x44663bc5  , 0x5bfb7e34  ,
    0x8b432976  , 0xcb23c6dc  , 0xb6edfc68  , 0xb8e4f163  ,
    0xd731dcca  , 0x42638510  , 0x13972240  , 0x84c61120  ,
    0x854a247d  , 0xd2bb3df8  , 0xaef93211  , 0xc729a16d  ,
    0x1d9e2f4b  , 0xdcb230f3  , 0x0d8652ec  , 0x77c1e3d0  ,
    0x2bb3166c  , 0xa970b999  , 0x119448fa  , 0x47e96422  ,
    0xa8fc8cc4  , 0xa0f03f1a  , 0x567d2cd8  , 0x223390ef  ,
    0x87494ec7  , 0xd938d1c1  , 0x8ccaa2fe  , 0x98d40b36  ,
    0xa6f581cf  , 0xa57ade28  , 0xdab78e26  , 0x3fadbfa4  ,
    0x2c3a9de4  , 0x5078920d  , 0x6a5fcc9b  , 0x547e4662  ,
    0xf68d13c2  , 0x90d8b8e8  , 0x2e39f75e  , 0x82c3aff5  ,
    0x9f5d80be  , 0x69d0937c  , 0x6fd52da9  , 0xcf2512b3  ,
    0xc8ac993b  , 0x10187da7  , 0xe89c636e  , 0xdb3bbb7b  ,
    0xcd267809  , 0x6e5918f4  , 0xec9ab701  , 0x834f9aa8  ,
    0xe6956e65  , 0xaaffe67e  , 0x21bccf08  , 0xef15e8e6  ,
    0xbae79bd9  , 0x4a6f36ce  , 0xea9f09d4  , 0x29b07cd6  ,
    0x31a4b2af  , 0x2a3f2331  , 0xc6a59430  , 0x35a266c0  ,
    0x744ebc37  , 0xfc82caa6  , 0xe090d0b0  , 0x33a7d815  ,
    0xf104984a  , 0x41ecdaf7  , 0x7fcd500e  , 0x1791f62f  ,
    0x764dd68d  , 0x43efb04d  , 0xccaa4d54  , 0xe49604df  ,
    0x9ed1b5e3  , 0x4c6a881b  , 0xc12c1fb8  , 0x4665517f  ,
    0x9d5eea04  , 0x018c355d  , 0xfa877473  , 0xfb0b412e  ,
    0xb3671d5a  , 0x92dbd252  , 0xe9105633  , 0x6dd64713  ,
    0x9ad7618c  , 0x37a10c7a  , 0x59f8148e  , 0xeb133c89  ,
    0xcea927ee  , 0xb761c935  , 0xe11ce5ed  , 0x7a47b13c  ,
    0x9cd2df59  , 0x55f2733f  , 0x1814ce79  , 0x73c737bf  ,
    0x53f7cdea  , 0x5ffdaa5b  , 0xdf3d6f14  , 0x7844db86  ,
    0xcaaff381  , 0xb968c43e  , 0x3824342c  , 0xc2a3405f  ,
    0x161dc372  , 0xbce2250c  , 0x283c498b  , 0xff0d9541  ,
    0x39a80171  , 0x080cb3de  , 0xd8b4e49c  , 0x6456c190  ,
    0x7bcb8461  , 0xd532b670  , 0x486c5c74  , 0xd0b85742
};

#endif

//Multiply for 2 each byte of a WORD32 working in parallel mode on each one
#define Count_Aes128_Xtime(x)  ((((x) & 0x7f7f7f7f) << 1) ^ ((((x) & 0x80808080) >> 7) * 0x0000001b))
//Right shift x of n bytes
#define Count_Aes128_Upr(x,n) (x >> 8*n) | (x << (32 - 8*n))
//Develop of the matrix necessary for the MixColomn procedure
#define Count_Aes128_FwdMcol(x)  (Count_Aes128_Xtime(x)^(Count_Aes128_Upr((x^Count_Aes128_Xtime(x)),3)) ^ (Count_Aes128_Upr(x,2)) ^ (Count_Aes128_Upr(x,1)))
//Develop of the matrix necessary for the InvMixColomn procedure
#define Count_Aes128_InvMcol(x)  (f2=Count_Aes128_Xtime(x),f4=Count_Aes128_Xtime(f2),f8=Count_Aes128_Xtime(f4),(x)^=f8, f2^=f4^f8^(Count_Aes128_Upr((f2^(x)),3))^(Count_Aes128_Upr((f4^(x)),2))^(Count_Aes128_Upr((x),1)))
//Rotation macro
#define Count_Aes128_Rot3(x) ((x << 8 ) | (x >> 24)) /* rotate right by 24 bit */
#define Count_Aes128_Rot2(x) ((x << 16) | (x >> 16)) /* rotate right by 16 bit */
#define Count_Aes128_Rot1(x) ((x << 24) | (x >> 8 )) /* rotate right by 8 bit  */

#if COUNT_AES128_IMPLEMENTATION==1

/*******************************************************************************
* Function Name  : Count_AES128_KeyscheduleEnc
* Description    : According to key, computes the expanded key (expkey) for AES128
*                  encryption.
* Input          : key: user key
* Output         : expkey: expanded key
* Return         : None
*******************************************************************************/
void Count_AES128_KeyscheduleEnc(uint32_t* key, uint32_t* expkey)
{
    register uint32_t* local_pointer = expkey;
    register int i = 0;
    register uint32_t copy0;
    register uint32_t copy1;
    register uint32_t copy2;
    register uint32_t copy3;

    copy0 = key[0];
    copy1 = key[1];
    copy2 = key[2];
    copy3 = key[3];

    local_pointer[0] = copy0;
    local_pointer[1] = copy1;
    local_pointer[2] = copy2;
    local_pointer[3] = copy3;

    for (; i < 10;)
    {
        copy0 ^= Count_4ByteToLong( Count_Aes128_Sbox[Count_ByteMH(copy3)],
                                    Count_Aes128_Sbox[Count_ByteML(copy3)],
                                    Count_Aes128_Sbox[Count_ByteL(copy3)],
                                    Count_Aes128_Sbox[Count_ByteH(copy3)]) ^ Count_Aes128_Rcon[i++];
        copy1 ^= copy0;
        copy2 ^= copy1;
        copy3 ^= copy2;
        local_pointer += 4;
        local_pointer[0] = copy0;
        local_pointer[1] = copy1;
        local_pointer[2] = copy2;
        local_pointer[3] = copy3;
    }
}

/*******************************************************************************
* Function Name  : Count_AES128_KeyscheduleDec
* Description    : According to key computes the expanded key (expkey) for AES128
*                  decryption.
* Input          : key: user key
* Output         : expkey: expanded key
* Return         : None
*******************************************************************************/
void Count_AES128_KeyscheduleDec(uint32_t* key, uint32_t* exp)
{
    Count_AES128_KeyscheduleEnc(key, exp);
}

/*******************************************************************************
* Function Name  : Count_AES128_decrypt
* Description    : Decrypts one block of 16 bytes
* Input          : - input_pointer: input block address
*                  - expkey: decryption key
* Output         : output_pointer: output block address
* Return         : None
*******************************************************************************/
void Count_AES128_decrypt(uint32_t* input_pointer, uint32_t* output_pointer, uint32_t* expkey)
{
    /* Register: ask to the compiler to maintain this variable in the
     processor's registers and don't store it in RAM */
    register uint32_t t0;
    register uint32_t t1;
    register uint32_t t2;
    register uint32_t t3;
    register uint32_t s0;
    register uint32_t s1;
    register uint32_t s2;
    register uint32_t s3;
    register int r =  10; /* Count the round */
    register uint32_t* local_pointer = expkey + 40;
    uint32_t f2,f4,f8;

    s0 = input_pointer[0] ^ local_pointer[0];
    s1 = input_pointer[1] ^ local_pointer[1];
    s2 = input_pointer[2] ^ local_pointer[2];
    s3 = input_pointer[3] ^ local_pointer[3];

    /* First add key: before start the rounds */
    local_pointer -= 8;

    for (;;) /* Start round */
    {
        t0 = Count_4ByteToLong( Count_Aes128_InvSbox[Count_ByteH(s0)],
                                Count_Aes128_InvSbox[Count_ByteMH(s3)],
                                Count_Aes128_InvSbox[Count_ByteML(s2)],
                                Count_Aes128_InvSbox[Count_ByteL(s1)]) ^ local_pointer[4];
        t1 = Count_4ByteToLong( Count_Aes128_InvSbox[Count_ByteH(s1)],
                                Count_Aes128_InvSbox[Count_ByteMH(s0)],
                                Count_Aes128_InvSbox[Count_ByteML(s3)],
                                Count_Aes128_InvSbox[Count_ByteL(s2)]) ^ local_pointer[5];
        t2 = Count_4ByteToLong( Count_Aes128_InvSbox[Count_ByteH(s2)],
                                Count_Aes128_InvSbox[Count_ByteMH(s1)],
                                Count_Aes128_InvSbox[Count_ByteML(s0)],
                                Count_Aes128_InvSbox[Count_ByteL(s3)]) ^ local_pointer[6];
        t3 = Count_4ByteToLong( Count_Aes128_InvSbox[Count_ByteH(s3)],
                                Count_Aes128_InvSbox[Count_ByteMH(s2)],
                                Count_Aes128_InvSbox[Count_ByteML(s1)],
                                Count_Aes128_InvSbox[Count_ByteL(s0)]) ^ local_pointer[7];
        /*End of Count_Aes128_InvSbox,  INVshiftRow,  add key*/
        s0=Count_Aes128_InvMcol(t0);
        s1=Count_Aes128_InvMcol(t1);
        s2=Count_Aes128_InvMcol(t2);
        s3=Count_Aes128_InvMcol(t3);
        /*End of INVMix column */
        local_pointer -= 4; /*Follow the key sheduler to choose the right round key*/

        if (--r == 1)
        {
            break;
        }

    }/*End of round*/

    /*Start last round :is the only one different from the other*/
    t0 = Count_4ByteToLong( Count_Aes128_InvSbox[Count_ByteH(s0)],
                            Count_Aes128_InvSbox[Count_ByteMH(s3)],
                            Count_Aes128_InvSbox[Count_ByteML(s2)],
                            Count_Aes128_InvSbox[Count_ByteL(s1)]) ^ local_pointer[4];
    t1 = Count_4ByteToLong( Count_Aes128_InvSbox[Count_ByteH(s1)],
                            Count_Aes128_InvSbox[Count_ByteMH(s0)],
                            Count_Aes128_InvSbox[Count_ByteML(s3)],
                            Count_Aes128_InvSbox[Count_ByteL(s2)]) ^ local_pointer[5];
    t2 = Count_4ByteToLong( Count_Aes128_InvSbox[Count_ByteH(s2)],
                            Count_Aes128_InvSbox[Count_ByteMH(s1)],
                            Count_Aes128_InvSbox[Count_ByteML(s0)],
                            Count_Aes128_InvSbox[Count_ByteL(s3)]) ^ local_pointer[6];
    t3 = Count_4ByteToLong( Count_Aes128_InvSbox[Count_ByteH(s3)],
                            Count_Aes128_InvSbox[Count_ByteMH(s2)],
                            Count_Aes128_InvSbox[Count_ByteML(s1)],
                            Count_Aes128_InvSbox[Count_ByteL(s0)]) ^ local_pointer[7];
    output_pointer[0] = t0;
    output_pointer[1] = t1;
    output_pointer[2] = t2;
    output_pointer[3] = t3;
}

/*******************************************************************************
* Function Name  : Count_AES128_encrypt
* Description    : Encrypts one block of 16 bytes
* Input          : - input_pointer: input block address
*                  - expkey: encryption key
* Output         : output_pointer
* Return         : None
*******************************************************************************/
void Count_AES128_encrypt(uint32_t* input_pointer, uint32_t* output_pointer, uint32_t* expkey)
{
    register uint32_t t0;
    register uint32_t t1;
    register uint32_t t2;
    register uint32_t t3;
    register uint32_t s0;
    register uint32_t s1;
    register uint32_t s2;
    register uint32_t s3;
    register int r = 10;/*Round counter */
    register uint32_t* local_pointer = expkey;

    s0 = input_pointer[0] ^ local_pointer[0];
    s1 = input_pointer[1] ^ local_pointer[1];
    s2 = input_pointer[2] ^ local_pointer[2];
    s3 = input_pointer[3] ^ local_pointer[3];
    local_pointer += 4;
    /* ADD KEY before start round*/
    for (;;)
    {
        t0 = Count_4ByteToLong( Count_Aes128_Sbox[Count_ByteH(s0)],
                                Count_Aes128_Sbox[Count_ByteMH(s1)],
                                Count_Aes128_Sbox[Count_ByteML(s2)],
                                Count_Aes128_Sbox[Count_ByteL(s3)]);
        t1 = Count_4ByteToLong( Count_Aes128_Sbox[Count_ByteH(s1)],
                                Count_Aes128_Sbox[Count_ByteMH(s2)],
                                Count_Aes128_Sbox[Count_ByteML(s3)],
                                Count_Aes128_Sbox[Count_ByteL(s0)]);
        t2 = Count_4ByteToLong( Count_Aes128_Sbox[Count_ByteH(s2)],
                                Count_Aes128_Sbox[Count_ByteMH(s3)],
                                Count_Aes128_Sbox[Count_ByteML(s0)],
                                Count_Aes128_Sbox[Count_ByteL(s1)]);
        t3 = Count_4ByteToLong( Count_Aes128_Sbox[Count_ByteH(s3)],
                                Count_Aes128_Sbox[Count_ByteMH(s0)],
                                Count_Aes128_Sbox[Count_ByteML(s1)],
                                Count_Aes128_Sbox[Count_ByteL(s2)]);
        /*End of SBOX + Shift ROW*/
        s0 = Count_Aes128_FwdMcol(t0)^local_pointer[0];
        s1 = Count_Aes128_FwdMcol(t1)^local_pointer[1];
        s2 = Count_Aes128_FwdMcol(t2)^local_pointer[2];
        s3 = Count_Aes128_FwdMcol(t3)^local_pointer[3];
        /*End of mix colomn*/

        local_pointer += 4;
        if ( --r == 1)
        {
            break;
        }

    }/*End for(;;)*/

    /*Start Last round*/
    t0 = Count_4ByteToLong( Count_Aes128_Sbox[Count_ByteH(s0)],
                            Count_Aes128_Sbox[Count_ByteMH(s1)],
                            Count_Aes128_Sbox[Count_ByteML(s2)],
                            Count_Aes128_Sbox[Count_ByteL(s3)]);
    t1 = Count_4ByteToLong( Count_Aes128_Sbox[Count_ByteH(s1)],
                            Count_Aes128_Sbox[Count_ByteMH(s2)],
                            Count_Aes128_Sbox[Count_ByteML(s3)],
                            Count_Aes128_Sbox[Count_ByteL(s0)]);
    t2 = Count_4ByteToLong( Count_Aes128_Sbox[Count_ByteH(s2)],
                            Count_Aes128_Sbox[Count_ByteMH(s3)],
                            Count_Aes128_Sbox[Count_ByteML(s0)],
                            Count_Aes128_Sbox[Count_ByteL(s1)]);
    t3 = Count_4ByteToLong( Count_Aes128_Sbox[Count_ByteH(s3)],
                            Count_Aes128_Sbox[Count_ByteMH(s0)],
                            Count_Aes128_Sbox[Count_ByteML(s1)],
                            Count_Aes128_Sbox[Count_ByteL(s2)]);

    t0 ^= local_pointer[0];
    t1 ^= local_pointer[1];
    t2 ^= local_pointer[2];
    t3 ^= local_pointer[3];

    /*Store of the result of encryption*/
    output_pointer[0] = t0;
    output_pointer[1] = t1;
    output_pointer[2] = t2;
    output_pointer[3] = t3;
}


#elif COUNT_AES128_IMPLEMENTATION==2

/*******************************************************************************
* Function Name  : Count_AES128_KeyscheduleEnc
* Description    : According to key computes the expanded key exp for AES128
*                  encryption.
* Input          : key: user key
* Output         : expkey: expanded key
* Return         : None
*******************************************************************************/
void Count_AES128_KeyscheduleEnc(uint32_t* key, uint32_t* expkey)
{
    register uint32_t* local_pointer = expkey;
    register int i = 0;
    register uint32_t copy0;
    register uint32_t copy1;
    register uint32_t copy2;
    register uint32_t copy3;

    copy0 = key[0];
    copy1 = key[1];
    copy2 = key[2];
    copy3 = key[3];
    local_pointer[0] = copy0;
    local_pointer[1] = copy1;
    local_pointer[2] = copy2;
    local_pointer[3] = copy3;
    for (; i < 10;)
    {
        copy0 ^= Count_4ByteToLong( Count_Aes128_Sbox[Count_ByteMH(copy3)],
                                    Count_Aes128_Sbox[Count_ByteML(copy3)],
                                    Count_Aes128_Sbox[Count_ByteL(copy3)],
                                    Count_Aes128_Sbox[Count_ByteH(copy3)]) ^ Count_Aes128_Rcon[i++];
        copy1 ^= copy0;
        copy2 ^= copy1;
        copy3 ^= copy2;
        local_pointer += 4;
        local_pointer[0] = copy0;
        local_pointer[1] = copy1;
        local_pointer[2] = copy2;
        local_pointer[3] = copy3;
    }
}

/*******************************************************************************
* Function Name  : Count_AES128_KeyscheduleDec
* Description    : According to key computes the expanded key (expkey) for AES128
*                  decryption.
* Input          : key: user key
* Output         : expkey: expanded key
* Return         : None
*******************************************************************************/
void Count_AES128_KeyscheduleDec(uint32_t* key, uint32_t* expkey)
{
    register uint32_t* local_pointer = expkey;
    register int i = 0;
    register uint32_t copy0;
    register uint32_t copy1;
    register uint32_t copy2;
    register uint32_t copy3;

    Count_AES128_KeyscheduleEnc(key,expkey);

    local_pointer[0] =  key[0];
    local_pointer[1] =  key[1];
    local_pointer[2] =  key[2];
    local_pointer[3] =  key[3];

    for (i = 1; i <10; i++)
    {
        local_pointer += 4;
        copy0 = local_pointer[0];
        copy1 = local_pointer[1];
        copy2 = local_pointer[2];
        copy3 = local_pointer[3];
        local_pointer[0] =      Count_Aes128_DecTable[Count_Aes128_Sbox[Count_ByteH(copy0)]]  ^
                                Count_Aes128_Rot1(Count_Aes128_DecTable[Count_Aes128_Sbox[Count_ByteMH(copy0)]]) ^
                                Count_Aes128_Rot2(Count_Aes128_DecTable[Count_Aes128_Sbox[Count_ByteML(copy0)]]) ^
                                Count_Aes128_Rot3(Count_Aes128_DecTable[Count_Aes128_Sbox[Count_ByteL(copy0)]]);
        local_pointer[1] =      Count_Aes128_DecTable[Count_Aes128_Sbox[Count_ByteH(copy1)]]  ^
                                Count_Aes128_Rot1(Count_Aes128_DecTable[Count_Aes128_Sbox[Count_ByteMH(copy1)]]) ^
                                Count_Aes128_Rot2(Count_Aes128_DecTable[Count_Aes128_Sbox[Count_ByteML(copy1)]]) ^
                                Count_Aes128_Rot3(Count_Aes128_DecTable[Count_Aes128_Sbox[Count_ByteL(copy1)]]);
        local_pointer[2] =      Count_Aes128_DecTable[Count_Aes128_Sbox[Count_ByteH(copy2)]]  ^
                                Count_Aes128_Rot1(Count_Aes128_DecTable[Count_Aes128_Sbox[Count_ByteMH(copy2)]]) ^
                                Count_Aes128_Rot2(Count_Aes128_DecTable[Count_Aes128_Sbox[Count_ByteML(copy2)]]) ^
                                Count_Aes128_Rot3(Count_Aes128_DecTable[Count_Aes128_Sbox[Count_ByteL(copy2)]]);
        local_pointer[3] =      Count_Aes128_DecTable[Count_Aes128_Sbox[Count_ByteH(copy3)]]  ^
                                Count_Aes128_Rot1(Count_Aes128_DecTable[Count_Aes128_Sbox[Count_ByteMH(copy3)]]) ^
                                Count_Aes128_Rot2(Count_Aes128_DecTable[Count_Aes128_Sbox[Count_ByteML(copy3)]]) ^
                                Count_Aes128_Rot3(Count_Aes128_DecTable[Count_Aes128_Sbox[Count_ByteL(copy3)]]);
    }
}

/*******************************************************************************
* Function Name  : Count_AES128_encrypt
* Description    : Encrypts one block of 16 bytes
* Input          : - input_pointer: input block address
*                  - expkey: encryption key
* Output         : output_pointer
* Return         : None
*******************************************************************************/
void Count_AES128_encrypt(uint32_t* input_pointer, uint32_t* output_pointer, uint32_t* expkey)
{
    register uint32_t s0;
    register uint32_t s1;
    register uint32_t s2;
    register uint32_t s3;
    register uint32_t t0;
    register uint32_t t1;
    register uint32_t t2;
    register uint32_t t3;
    register int r = 10 >> 1;
    register uint32_t* local_pointer = expkey;

    s0 = input_pointer[0] ^ local_pointer[0];
    s1 = input_pointer[1] ^ local_pointer[1];
    s2 = input_pointer[2] ^ local_pointer[2];
    s3 = input_pointer[3] ^ local_pointer[3];

    for (;;)
    {
        t0 =      Count_Aes128_EncTable[Count_ByteH(s0)]  ^
                  Count_Aes128_Rot1(Count_Aes128_EncTable[Count_ByteMH(s1)]) ^
                  Count_Aes128_Rot2(Count_Aes128_EncTable[Count_ByteML(s2)]) ^
                  Count_Aes128_Rot3(Count_Aes128_EncTable[Count_ByteL(s3)]) ^
                  local_pointer[4];
        t1 =      Count_Aes128_EncTable[Count_ByteH(s1)]  ^
                  Count_Aes128_Rot1(Count_Aes128_EncTable[Count_ByteMH(s2)]) ^
                  Count_Aes128_Rot2(Count_Aes128_EncTable[Count_ByteML(s3)]) ^
                  Count_Aes128_Rot3(Count_Aes128_EncTable[Count_ByteL(s0)]) ^
                  local_pointer[5];
        t2 =      Count_Aes128_EncTable[Count_ByteH(s2)]  ^
                  Count_Aes128_Rot1(Count_Aes128_EncTable[Count_ByteMH(s3)]) ^
                  Count_Aes128_Rot2(Count_Aes128_EncTable[Count_ByteML(s0)]) ^
                  Count_Aes128_Rot3(Count_Aes128_EncTable[Count_ByteL(s1)]) ^
                  local_pointer[6];
        t3 =      Count_Aes128_EncTable[Count_ByteH(s3)]  ^
                  Count_Aes128_Rot1(Count_Aes128_EncTable[Count_ByteMH(s0)]) ^
                  Count_Aes128_Rot2(Count_Aes128_EncTable[Count_ByteML(s1)]) ^
                  Count_Aes128_Rot3(Count_Aes128_EncTable[Count_ByteL(s2)]) ^
                  local_pointer[7];

        local_pointer += 8;
        if (--r == 0)
        {
            break;
        }

        s0 =      Count_Aes128_EncTable[Count_ByteH(t0)]  ^
                  Count_Aes128_Rot1(Count_Aes128_EncTable[Count_ByteMH(t1)]) ^
                  Count_Aes128_Rot2(Count_Aes128_EncTable[Count_ByteML(t2)]) ^
                  Count_Aes128_Rot3(Count_Aes128_EncTable[Count_ByteL(t3)]) ^
                  local_pointer[0];
        s1 =      Count_Aes128_EncTable[Count_ByteH(t1)]  ^
                  Count_Aes128_Rot1(Count_Aes128_EncTable[Count_ByteMH(t2)]) ^
                  Count_Aes128_Rot2(Count_Aes128_EncTable[Count_ByteML(t3)]) ^
                  Count_Aes128_Rot3(Count_Aes128_EncTable[Count_ByteL(t0)]) ^
                  local_pointer[1];
        s2 =      Count_Aes128_EncTable[Count_ByteH(t2)]  ^
                  Count_Aes128_Rot1(Count_Aes128_EncTable[Count_ByteMH(t3)]) ^
                  Count_Aes128_Rot2(Count_Aes128_EncTable[Count_ByteML(t0)]) ^
                  Count_Aes128_Rot3(Count_Aes128_EncTable[Count_ByteL(t1)]) ^
                  local_pointer[2];
        s3 =      Count_Aes128_EncTable[Count_ByteH(t3)]  ^
                  Count_Aes128_Rot1(Count_Aes128_EncTable[Count_ByteMH(t0)]) ^
                  Count_Aes128_Rot2(Count_Aes128_EncTable[Count_ByteML(t1)]) ^
                  Count_Aes128_Rot3(Count_Aes128_EncTable[Count_ByteL(t2)]) ^
                  local_pointer[3];
    }

    output_pointer[0] = Count_4ByteToLong( Count_Aes128_Sbox[Count_ByteH(t0)],
                                           Count_Aes128_Sbox[Count_ByteMH(t1)],
                                           Count_Aes128_Sbox[Count_ByteML(t2)],
                                           Count_Aes128_Sbox[Count_ByteL(t3)]) ^ local_pointer[0];
    output_pointer[1] = Count_4ByteToLong( Count_Aes128_Sbox[Count_ByteH(t1)],
                                           Count_Aes128_Sbox[Count_ByteMH(t2)],
                                           Count_Aes128_Sbox[Count_ByteML(t3)],
                                           Count_Aes128_Sbox[Count_ByteL(t0)]) ^ local_pointer[1];
    output_pointer[2] = Count_4ByteToLong( Count_Aes128_Sbox[Count_ByteH(t2)],
                                           Count_Aes128_Sbox[Count_ByteMH(t3)],
                                           Count_Aes128_Sbox[Count_ByteML(t0)],
                                           Count_Aes128_Sbox[Count_ByteL(t1)]) ^ local_pointer[2];
    output_pointer[3] = Count_4ByteToLong( Count_Aes128_Sbox[Count_ByteH(t3)],
                                           Count_Aes128_Sbox[Count_ByteMH(t0)],
                                           Count_Aes128_Sbox[Count_ByteML(t1)],
                                           Count_Aes128_Sbox[Count_ByteL(t2)]) ^ local_pointer[3];
}


/*******************************************************************************
* Function Name  : Count_AES128_decrypt
* Description    : Decrypts one block of 16 bytes
* Input          : - input_pointer: input block address
*                  - expkey: decryption key
* Output         : output_pointer: output block address
* Return         : None
*******************************************************************************/
void Count_AES128_decrypt(uint32_t* input_pointer, uint32_t* output_pointer, uint32_t* expkey)
{
    register uint32_t s0;
    register uint32_t s1;
    register uint32_t s2;
    register uint32_t s3;
    register uint32_t t0;
    register uint32_t t1;
    register uint32_t t2;
    register uint32_t t3;
    register int r = 10 >> 1;
    register uint32_t* local_pointer = expkey + (40);

    s0 = input_pointer[0] ^ local_pointer[0];
    s1 = input_pointer[1] ^ local_pointer[1];
    s2 = input_pointer[2] ^ local_pointer[2];
    s3 = input_pointer[3] ^ local_pointer[3];

    for (;;)
    {
        local_pointer -= 8;

        t0 =      Count_Aes128_DecTable[Count_ByteH(s0)]  ^
                  Count_Aes128_Rot1(Count_Aes128_DecTable[Count_ByteMH(s3)]) ^
                  Count_Aes128_Rot2(Count_Aes128_DecTable[Count_ByteML(s2)]) ^
                  Count_Aes128_Rot3(Count_Aes128_DecTable[Count_ByteL(s1)]) ^
                  local_pointer[4];
        t1 =      Count_Aes128_DecTable[Count_ByteH(s1)]  ^
                  Count_Aes128_Rot1(Count_Aes128_DecTable[Count_ByteMH(s0)]) ^
                  Count_Aes128_Rot2(Count_Aes128_DecTable[Count_ByteML(s3)]) ^
                  Count_Aes128_Rot3(Count_Aes128_DecTable[Count_ByteL(s2)]) ^
                  local_pointer[5];
        t2 =      Count_Aes128_DecTable[Count_ByteH(s2)]  ^
                  Count_Aes128_Rot1(Count_Aes128_DecTable[Count_ByteMH(s1)]) ^
                  Count_Aes128_Rot2(Count_Aes128_DecTable[Count_ByteML(s0)]) ^
                  Count_Aes128_Rot3(Count_Aes128_DecTable[Count_ByteL(s3)]) ^
                  local_pointer[6];
        t3 =      Count_Aes128_DecTable[Count_ByteH(s3)]  ^
                  Count_Aes128_Rot1(Count_Aes128_DecTable[Count_ByteMH(s2)]) ^
                  Count_Aes128_Rot2(Count_Aes128_DecTable[Count_ByteML(s1)]) ^
                  Count_Aes128_Rot3(Count_Aes128_DecTable[Count_ByteL(s0)]) ^
                  local_pointer[7];

        if (--r == 0)
        {
            break;
        }

        s0 =      Count_Aes128_DecTable[Count_ByteH(t0)]  ^
                  Count_Aes128_Rot1(Count_Aes128_DecTable[Count_ByteMH(t3)]) ^
                  Count_Aes128_Rot2(Count_Aes128_DecTable[Count_ByteML(t2)]) ^
                  Count_Aes128_Rot3(Count_Aes128_DecTable[Count_ByteL(t1)]) ^
                  local_pointer[0];
        s1 =      Count_Aes128_DecTable[Count_ByteH(t1)]  ^
                  Count_Aes128_Rot1(Count_Aes128_DecTable[Count_ByteMH(t0)]) ^
                  Count_Aes128_Rot2(Count_Aes128_DecTable[Count_ByteML(t3)]) ^
                  Count_Aes128_Rot3(Count_Aes128_DecTable[Count_ByteL(t2)]) ^
                  local_pointer[1];
        s2 =      Count_Aes128_DecTable[Count_ByteH(t2)]  ^
                  Count_Aes128_Rot1(Count_Aes128_DecTable[Count_ByteMH(t1)]) ^
                  Count_Aes128_Rot2(Count_Aes128_DecTable[Count_ByteML(t0)]) ^
                  Count_Aes128_Rot3(Count_Aes128_DecTable[Count_ByteL(t3)]) ^
                  local_pointer[2];
        s3 =      Count_Aes128_DecTable[Count_ByteH(t3)]  ^
                  Count_Aes128_Rot1(Count_Aes128_DecTable[Count_ByteMH(t2)]) ^
                  Count_Aes128_Rot2(Count_Aes128_DecTable[Count_ByteML(t1)]) ^
                  Count_Aes128_Rot3(Count_Aes128_DecTable[Count_ByteL(t0)]) ^
                  local_pointer[3];
    }

    output_pointer[0] = Count_4ByteToLong( Count_Aes128_InvSbox[Count_ByteH(t0)],
                                           Count_Aes128_InvSbox[Count_ByteMH(t3)],
                                           Count_Aes128_InvSbox[Count_ByteML(t2)],
                                           Count_Aes128_InvSbox[Count_ByteL(t1)]) ^ local_pointer[0];
    output_pointer[1] = Count_4ByteToLong( Count_Aes128_InvSbox[Count_ByteH(t1)],
                                           Count_Aes128_InvSbox[Count_ByteMH(t0)],
                                           Count_Aes128_InvSbox[Count_ByteML(t3)],
                                           Count_Aes128_InvSbox[Count_ByteL(t2)]) ^ local_pointer[1];
    output_pointer[2] = Count_4ByteToLong( Count_Aes128_InvSbox[Count_ByteH(t2)],
                                           Count_Aes128_InvSbox[Count_ByteMH(t1)],
                                           Count_Aes128_InvSbox[Count_ByteML(t0)],
                                           Count_Aes128_InvSbox[Count_ByteL(t3)]) ^ local_pointer[2];
    output_pointer[3] = Count_4ByteToLong( Count_Aes128_InvSbox[Count_ByteH(t3)],
                                           Count_Aes128_InvSbox[Count_ByteMH(t2)],
                                           Count_Aes128_InvSbox[Count_ByteML(t1)],
                                           Count_Aes128_InvSbox[Count_ByteL(t0)]) ^ local_pointer[3];
}
#else
#error Please select AES algorithm version in config.h
#endif

//---------------------
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* User's secret key */
static uint32_t Count_Aes128_key[COUNT_AES128_KEY_SIZE]= {0x31323334,0x35363738,0x61626364,0x65666768};
/* Expanded key */
static uint32_t Count_Aes128_ExpKey[COUNT_AES128_EXPKEY_SIZE];
/* Two plaitext blosks */
//INT32U p1[COUNT_AES128_BLOCK_SIZE]={0x31323334,0x35363738,0x61626364,0x65666768};
//INT32U p2[COUNT_AES128_BLOCK_SIZE]={0x30313233,0x34353637,0x61626364,0x65666768};
//INT8U p1[COUNT_AES128_BLOCK_SIZE*4+1]="12345678abcdefgh";
//INT32U p2[COUNT_AES128_BLOCK_SIZE*4]= {0x30313233,0x34353637,0x61626364,0x65666768};
/* Two ciphertext blocks */
//INT32U c1[COUNT_AES128_BLOCK_SIZE];
//INT32U c2[COUNT_AES128_BLOCK_SIZE];
void  Count_AES128_Test(void)
{
#if 1
    uint8_t res;
    uint16_t i16,j16;
    uint8_t *p1,*p2,*pp;
    uint8_t *pi8,*pj8;
    uint32_t t32;
    //申请缓存
    p1=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    p2=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    pp=MemManager_Get(E_MEM_MANAGER_TYPE_1KB);
    //初始化缓存
    pi8=p1;
    pj8=p2;
    for(i16=0; i16<256; i16++)
    {
        pi8[i16]='0'+(i16%10);
    }
    //加密时间测试
    Count_AES128_KeyscheduleEnc((uint32_t*)Count_Aes128_key,(uint32_t*)Count_Aes128_ExpKey);
    Count_AES128_msCount=t32=0;
    for(i16=0; i16<256; i16+=16)
    {
        Count_AES128_encrypt((uint32_t*)&pi8[i16],(uint32_t*)&pj8[i16],(uint32_t*)Count_Aes128_ExpKey);
    }
    t32=Count_AES128_msCount;
    sprintf((char*)pp,"AES128方式加密%03d字节('0'~'9'循环),共耗时%05ldms\r\n",256,t32);
    DebugOut((int8_t*)pp,strlen((char*)pp));
    //打印加密数据
    sprintf((char*)pp,"打印加密后数据:\r\n");
    DebugOut((int8_t*)pp,strlen((char*)pp));
    pi8=pp;
    for(i16=0; i16<256; i16++)
    {
        Count_HexToAscii(pi8,&p2[i16],1);
        pi8=pi8+2;
        *pi8++=' ';
    }
    *pi8++='\r';
    *pi8++='\n';
    *pi8++=0;
    i16=strlen((char*)pp);
    for(j16=0; j16<i16;)
    {
        if((i16-j16)<=250)
        {
            DebugOut((int8_t*)&pp[j16],i16-j16);
            break;
        }
        else
        {
            DebugOut((int8_t*)&pp[j16],250);
            j16+=250;
        }
    }

    //初始化缓存
    memset((char*)p1,0,256);
    pi8=p1;
    pj8=p2;
    //解密时间测试
    Count_AES128_KeyscheduleDec((uint32_t*)Count_Aes128_key,(uint32_t*)Count_Aes128_ExpKey);
    Count_AES128_msCount=t32=0;
    for(i16=0; i16<256; i16+=16)
    {
        Count_AES128_decrypt((uint32_t*)&pj8[i16],(uint32_t*)&pi8[i16],(uint32_t*)Count_Aes128_ExpKey);
    }
    t32=Count_AES128_msCount;
    sprintf((char*)pp,"AES128方式解密%03d字节,共耗时%05ldms\r\n",256,t32);
    DebugOut((int8_t*)pp,strlen((char*)pp));
    //打印解密数据
    sprintf((char*)pp,"打印解密后数据:\r\n");
    DebugOut((int8_t*)pp,strlen((char*)pp));
    pi8=pp;
    for(i16=0; i16<256; i16++)
    {
        Count_HexToAscii(pi8,&p1[i16],1);
        pi8=pi8+2;
        *pi8++=' ';
    }
    *pi8++='\r';
    *pi8++='\n';
    *pi8++=0;
    i16=strlen((char*)pp);
    for(j16=0; j16<i16;)
    {
        if((i16-j16)<=250)
        {
            DebugOut((int8_t*)&pp[j16],i16-j16);
            break;
        }
        else
        {
            DebugOut((int8_t*)&pp[j16],250);
            j16+=250;
        }
    }
    //
    DebugOut((int8_t*)pp,strlen((char*)pp));
    //释放内存
    MemManager_Free(E_MEM_MANAGER_TYPE_1KB,pp);
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,p2);
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,p1);
#else
    INT32U p1[COUNT_AES128_BLOCK_SIZE]= {0x31323334,0x35363738,0x61626364,0x65666768};
    INT32U p2[COUNT_AES128_BLOCK_SIZE]= {0x30313233,0x34353637,0x61626364,0x65666768};
    INT32U c1[COUNT_AES128_BLOCK_SIZE];
    INT32U c2[COUNT_AES128_BLOCK_SIZE];
    /*******************************************************/
    /*                   AES ECB ENCRYPTION                */
    /*******************************************************/
    /* Encryption key scheduling, to be done once */
    Count_AES128_KeyscheduleEnc((u32*)Count_Aes128_key,(u32*)Count_Aes128_ExpKey);
    /* First block encryption */
    Count_AES128_encrypt((u32*)p1,(u32*)c1,(u32*)Count_Aes128_ExpKey);
    /* Second block encryption */
    Count_AES128_encrypt((u32*)p2,(u32*)c2,(u32*)Count_Aes128_ExpKey);
    /*******************************************************/
    /*                   清缓存                            */
    /*******************************************************/
    memset((char*)p1,0,sizeof(p1));
    memset((char*)p2,0,sizeof(p2));
    /*******************************************************/
    /*                   AES ECB DECRYPTION                */
    /*******************************************************/
    /* Decryption key scheduling, to be done once */
    Count_AES128_KeyscheduleDec((u32*)Count_Aes128_key,(u32*)Count_Aes128_ExpKey);
    /* First block decryption */
    Count_AES128_decrypt((u32*)c1,(u32*)p1,(u32*)Count_Aes128_ExpKey);
    /* Second block decryption */
    Count_AES128_decrypt((u32*)c2,(u32*)p2,(u32*)Count_Aes128_ExpKey);
#endif
}
//-------------------------------------------------------------------------------
//                     出口函数---按地址写数据
//使用提示: 调用于1mS中断
//-------------------------------------------------------------------------------
void Count_AES128_1msPro(void)
{
    if(Count_AES128_msCount!=0xFFFFFFFF)  //1mS计数器
    {
        Count_AES128_msCount++;
    }
}
//-------------------------------------------------------------------------------项目专用函数
/********************************************************************************
* 函数说明: 项目专用编码函数
* 参    数: *key   ---  钥匙(128b)
*           *sbuf  ---  源缓存区
*           *dbuf  ---  目的缓存区
*           *len   ---  进入加密前数据的有效字节长度,返回加密扩展后字节长度
* 说    明: 项目中第一个字节为有效字节长度
*********************************************************************************/
/*
void AES128_encrypt_Project(uint32_t* pkey,uint8_t *psbuf,uint8_t *pdbuf,uint8_t *plen)
{
#ifdef AES128_MODBUS_ENABLE
    uint8_t i;
    uint16_t i16;
    uint32_t *p_inbuf,*p_outbuf;
    //生成扩展密钥
    if(pkey==NULL)
        Count_AES128_KeyscheduleEnc((uint32_t*)key,(uint32_t*)exp_key);
    else
        Count_AES128_KeyscheduleEnc((uint32_t*)key,(uint32_t*)exp_key);
    //申请缓存
    p_inbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    p_outbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    //清空缓存
    memset((char*)p_inbuf,0,256);
    memset((char*)p_outbuf,0,256);
    //将有效字节长度加入到前面
    i=*plen;
    for(i=(*plen); i!=0; i--)
    {
        psbuf[i-1]=psbuf[i];
    }
    psbuf[0]=*plen;
    //拷贝用户数据到缓存
    memcpy((char*)p_inbuf,psbuf,*plen);
    //大端/小端存储高低字节转换
    //计算
    for(i16=0; i16<(256>>2); i16+=4)
    {
        Count_AES128_encrypt((uint32_t*)&p_inbuf[i16],(uint32_t*)&p_outbuf[i16],(uint32_t*)exp_key);
    }
    //大端/小端存储高低字节转换
    //长度矫正
    *plen -= -1;
    *plen =(*plen)%4;
    *plen +=1;
    *plen =(*plen)<<2;
    //拷贝缓存到用户数据
    memcpy(pdbuf,(char*)p_outbuf,*plen);
    //释放缓存
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,p_outbuf);
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,p_inbuf);
#else
    pkey=pkey;
    psbuf=psbuf;
    pdbuf=pdbuf;
    plen=plen;
#endif
}
*/
/********************************************************************************
* 函数说明: 项目专用解密函数
* 参    数: *key   ---  钥匙(128b)
*           *sbuf  ---  源缓存区
*           *dbuf  ---  目的缓存区
*           *len   ---  加密前数据的有效长度
* 说    明: 项目中第一个字节为有效字节长度
*********************************************************************************/
/*
void AES128_decrypt_Project(uint32_t* pkey,uint8_t *psbuf,uint8_t *pdbuf,uint8_t *plen)
{
#ifdef AES128_MODBUS_ENABLE
    uint8_t i;
    uint16_t i16;
    uint32_t *p_inbuf,*p_outbuf;
    //生成扩展密钥
    if(pkey==NULL)
        Count_AES128_KeyscheduleEnc((uint32_t*)key,(uint32_t*)exp_key);
    else
        Count_AES128_KeyscheduleEnc((uint32_t*)key,(uint32_t*)exp_key);
    //申请缓存
    p_inbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    p_outbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    //清空缓存
    memset((char*)p_inbuf,0,256);
    memset((char*)p_outbuf,0,256);
    //拷贝用户数据到缓存
    memcpy((char*)p_inbuf,psbuf,*plen);
    //大端/小端存储高低字节转换
    //计算
    for(i16=0; i16<(256>>2); i16+=4)
    {
        Count_AES128_decrypt((uint32_t*)&p_inbuf[i16],(uint32_t*)&p_outbuf[i16],(uint32_t*)exp_key);
    }
    //大端/小端存储高低字节转换
    //拷贝缓存到用户数据
    memcpy(pdbuf,(char*)&p_outbuf[1],(*plen)-1);
    //有效长度计算
    *plen = pdbuf[0];
    //加入地址1B和CRC16
    *plen = (*plen) +3;
    //释放缓存
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,p_outbuf);
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,p_inbuf);
#else
    pkey=pkey;
    psbuf=psbuf;
    pdbuf=pdbuf;
    plen=plen;
#endif
}
*/
#endif
//
/*******************************************************************************
* 函数功能: BCD字节转为整型
* 输    入: BCD值
* 返 回 值: INT8值
*******************************************************************************/
uint8_t Count_BcdToInt8(uint8_t val)
{
    return ((val >> 4) * 10 + (val & 0xF));
}
/*******************************************************************************
* 函数功能: INT8转为BCD字节
* 输    入: INT8值
* 返 回 值: BCD值
*******************************************************************************/
uint8_t Count_Int8ToBcd(uint8_t val)
{
    return (((val / 10) << 4) + (val % 10));
}
/*******************************************************************************
* 函数功能: 无符号BCD字符串转为整型    (BCD高字节在前)
* 参    数: uint8_t * str  BCD字符串
*           uint16_t len   BCD字符串长度
* 输    出: 转换后的整数
*******************************************************************************/
uint32_t Count_StringBcdToInt(uint8_t *str, uint16_t len)
{
    uint32_t res = 0;

    while (len--)
    {
        res *= 100;
        res += Count_BcdToInt8(*str++);
    }
    return res;
}

/*******************************************************************************
* 函数功能: 整数转为BCD字符串，字符串右对齐 (BCD高字节在前)
* 参    数: uint32_t val   整数
            uint8_t * str  转换后缓冲区
            uint16_t size  缓冲区空间大小
*******************************************************************************/
void Count_IntToStringBcd( uint8_t *str, uint16_t size, uint32_t val )
{
    while (size--)
    {
        *(str + size)  = Count_Int8ToBcd(val % 100);
        val /= 100;
    }
}
/*******************************************************************************
函数功能: ASCII 到 HEX的转换函数
参    数:
          O_data: 转换数据的入口指针，
          N_data: 转换后新数据的入口指针
          len : 需要转换的长度
返 回 值: -1: 转换失败
          其它：转换后数据长度
注    意: O_data[]数组中的数据在转换过程中会被修改。
****************************************/
int32_t Count_AsciiToHex(uint8_t *O_data,uint8_t *r_data,uint32_t len)
{
    uint32_t i,j;
    uint8_t tmpData;
    // ASC--BCD
    for(i = 0; i < len; i++)
    {
        if     ((O_data[i] >= '0') && (O_data[i] <= '9'))   tmpData = O_data[i] - '0';
        else if((O_data[i] >= 'A') && (O_data[i] <= 'F'))   tmpData = O_data[i] - 0x37;
        else if((O_data[i] >= 'a') && (O_data[i] <= 'f'))   tmpData = O_data[i] - 0x57;
        else return -1;
        r_data[i] = tmpData;
    }
    // BCD--HEX
    for(i = 0,j = 0; j < len; j+=2,i++)
    {
        tmpData = (r_data[j]<<4) | r_data[j+1];
        r_data[i] = tmpData;
    }
    return (len/2);
}
/*******************************************************************************
* 函数功能: HEX 到 ASCII的转换函数
* 参    数: data: 转换数据的入口指针
            buffer: 转换后数据入口指针
            len : 需要转换的长度
* 返 回 值：转换后数据长度
*******************************************************************************/
uint32_t Count_HexToAscii(uint8_t *data, uint8_t *buffer, uint32_t len)
{
    uint8_t ascTable[17] = {"0123456789ABCDEF"};
    uint32_t i, pos=0;
    for(i = 0; i < len; i++)
    {
        buffer[pos++] = ascTable[data[i] >> 4];
        buffer[pos++] = ascTable[data[i] & 0x0f];
    }
    buffer[pos] = '\0';
    return pos;
}
//把端口号变成字符串
void uint32_tToStr(uint32_t ui,char *pp)
{

    char k;
    //万
    k=0;
    *pp=ui/10000+0x30;
    if (*pp!=0x30)
    {
        pp++;
        k=1;
    }
    //千
    *pp=(ui%10000)/1000+0x30;
    if (k==1||*pp!=0x30)
    {
        pp++;
        k=1;
    }
    //佰
    *pp=(ui%1000)/100+0x30;
    if(k==1||*pp!=0x30)
    {
        pp++;
        k=1;
    }
    //十
    *pp=(ui%100)/10+0x30;
    if(k==1||*pp!=0x30)
    {
        pp++;
        k=1;
    }
    //个
    *pp++=ui%10+0x30;
    //补零
    *pp='\0';

}
/*******************************************************************************
* 函数功能: 把字符串变为数字
* 参    数: *str: 字符串地址
            len : 字符串长度
            *num: 需要转换的长度
* 返 回 值：转换成功或失败
* 说    明: 例如:字符串"1234"变为10进制：1234.
            遇到‘/0’或指定长度结束
*******************************************************************************/
uint8_t Count_StrToLong(int8_t *str,uint8_t len,long *num)
{
    //   定义返回值
    long   iNum=0;
    uint8_t bflag=1;
    uint8_t res=OK;
    //   如果传递字符串指针为空，返回0，atoi函数本身是不做这种类型判断
    //   这种判断应该有用户判断来避免。
    if(str   ==   NULL)
    {
        res = ERR;
        return res;
    }

    if((*str)   ==   '-')
    {
        bflag   =   0;
        ++str;
    }
    //   循环遍历，进行判断
    while((*str)   !=   '\0')
    {
        if(len==0)
            break;
        else len--;

        //如果字符串中存在字符组合，只返回转化好的数字
        if((*str)   <   '0'   ||   (*str)   >   '9')
        {
            res=ERR;
            break;
        }
        else
        {
            iNum*=10;//   转化为10进制数字
            iNum   +=   (*str)   -   '0';   //   有ascii码转换为数字
        }
        ++str;
    }
    //   为负值
    if(!bflag)
    {
        iNum   =   -iNum;
    }
    //   返回转换值
    *num=iNum;
    return res;
}
/*******************************************************************************
*函数功能:  把IP地址格式化成  - XXX.XXX.XXX.XXX(0)  - 的标准格式
*           例如: 192.168.1.2(0)--->192.168.001.002(0)
*返回值  :  原始格式不对,导致转换错误
*******************************************************************************/
uint8_t Count_IP_Format(uint8_t * ip_asc,uint8_t *ip_buf)
{
    uint8_t i;
    //验证IP格式
    uint8_t begin=0,end=0,num=0;
    uint16_t i16;
    uint8_t buf[4];
    //---搜索第一个'.'-->end
    for(i=0; i<4; i++)
    {
        for(; end<(begin+4); end++)
        {
            if(ip_asc[end]=='.')
            {
                break;
            }
            else if(i==3 && ip_asc[end]==0)
            {
                break;
            }
            else if(ip_asc[end]>='0'&&ip_asc[end]<='9')
            {
                continue;
            }
            else
            {
                return ERR;
            }
        }
        if(end!=(begin+4))
        {
            i16=0;
            for(num=begin; num<end; num++)
            {
                i16*=10;
                i16+=ip_asc[num]-'0';
            }
            if(i16<=255)
            {
                buf[i]=(uint8_t)i16;
                end++;
                begin=end;
            }
            else
            {
                return ERR;
            }
        }
        else
        {
            return ERR;
        }
    }
    //实施转换
    if(i==4)
    {
        sprintf((char*)ip_asc,"%03d.%03d.%03d.%03d",buf[0],buf[1],buf[2],buf[3]);
        if(ip_buf!=NULL)
        {
            ip_buf[0]=buf[0];
            ip_buf[1]=buf[1];
            ip_buf[2]=buf[2];
            ip_buf[3]=buf[3];
        }
        return OK;
    }
    return ERR;
}
/*******************************************************************************
*函数功能:把PORT地址格式化成  - XXXXX(0)  - 的标准格式
*******************************************************************************/
void PORT_Format1(unsigned char *port)
{
    unsigned char i;
    port[5]=0;
    while((port[4]<'0')||(port[4]>'9'))
    {
        for(i=3;; i--)
        {
            port[i+1]=port[i];
            if(i==0)
            {
                port[i]='0';
                break;
            }
        }
    }
}
/*******************************************************************************
函数功能: 字母小写变大写
*******************************************************************************/
uint8_t Count_Capital(uint8_t chr)
{
    if(chr>='a'&&chr<='z')
        chr=chr+'A'-'a';
    return chr;
}
/*******************************************************************************
* 函数功能: 信号过滤判定操作
* 参    数: pState      ---   信号判定变量
            newState    ---   新的状态
            judgeCount  ---   判定次数
* 返 回 值: 判定后的状态
*******************************************************************************/
int8_t Count_StatusFilter(COUNT_S_STATUS *pState, int8_t newState, int8_t judgeCount)
{
    if (pState->state != newState)
    {
        pState->judgeCount++;

        if (pState->judgeCount >= judgeCount)
        {
            pState->state = newState;

            pState->judgeCount = 0;
        }
    }
    else
    {
        pState->judgeCount = 0;
    }

    return pState->state;
}
/*******************************************************************************
* 函数功能: 缓存环求和
* 参    数:
* 返 回 值: 无
*******************************************************************************/
void Count_BufLoopSum(uint16_t *pbuf,uint16_t bufMaxNum,uint16_t value,uint32_t *psum)
{
    static uint16_t sbufnum=0;
    static uint32_t sbufsum=0;
    uint16_t firstvalue;
    //
    if(sbufnum>=(bufMaxNum-1))
    {
        sbufnum=0;
    }
    else
    {
        sbufnum++;
    }
    //提取最远一个值
    firstvalue=pbuf[sbufnum];
    //将最新的值更新到缓存环
    pbuf[sbufnum]=value;
    //计算缓存环的和
    sbufsum+=value;
    sbufsum-=firstvalue;
    //
    *psum=sbufsum;
}
/*******************************************************************************
* 函数功能: 分级
* 参    数:
* 返 回 值: 无
*******************************************************************************/
uint16_t Count_Classification(uint32_t input_data,uint32_t *ptable,uint16_t table_len)
{
    uint16_t i16=0;
    for(i16=0; i16<table_len; i16++)
    {
        if(input_data<ptable[i16])
        {
            break;
        }
    }
    return i16;
}
/*******************************************************************************
* Function Name  : Time_ConvUnixToCalendar(time_t t)
* Description    : 转换UNIX时间戳为日历时间
* Input          : u32 t  当前时间的UNIX时间戳
* Output         : None
* Return         : struct tm
*******************************************************************************/
struct tm Count_Time_ConvUnixToCalendar(time_t t)
{
    struct tm *t_tm;
    t_tm = localtime(&t);
    t_tm->tm_year += 1900;  //localtime转换结果的tm_year是相对值，需要转成绝对值
    return *t_tm;
}
/*******************************************************************************
* Function Name  : Time_ConvCalendarToUnix(struct tm t)
* Description    : 写入RTC时钟当前时间
* Input          : struct tm t
* Output         : None
* Return         : time_t
*******************************************************************************/
time_t Count_Time_ConvCalendarToUnix(struct tm t)
{
    t.tm_year -= 1900;  //外部tm结构体存储的年份为2008格式
    //而time.h中定义的年份格式为1900年开始的年份
    //所以，在日期转换时要考虑到这个因素。
    return mktime(&t);
}
/*******************************************************************************
* 函数功能: BCD转为整型时间
* 参    数: BCDTIME * time BCD时间
* 返 回 值: 整型时间
*******************************************************************************/
uint32_t Count_BCDTimeToSec(BCDTIME_t *time)
{
    uint32_t lTime;
    struct tm conv = {0};

    conv.tm_year = Count_BcdToInt8(time->year) + 2000 - 1900;
    conv.tm_mon = Count_BcdToInt8(time->month)-1;
    conv.tm_mday = Count_BcdToInt8(time->day);
    conv.tm_hour = Count_BcdToInt8(time->hour);
    conv.tm_min = Count_BcdToInt8(time->minute);
    conv.tm_sec = Count_BcdToInt8(time->seconds);

    lTime = (uint32_t)mktime(&conv);
    //convert +8:00 time to UTC from 1970-01-01 00:00:00
    //lTime -= g_TimeZone * 60 * 60;
    return lTime;
}

/*******************************************************************************
* 函数功能: 整型时间转为BCD
* 参    数: BCDTIME * tim  BCD缓冲区
*           uint32_t sec   整型时间
*******************************************************************************/
void Count_SecToBCDTime(BCDTIME_t *tim, uint32_t sec)
{
    struct tm conv;
    time_t cTim = sec;
    //convert UTC time to +8:00 time zone
    //cTim += g_TimeZone * 60 * 60;
#ifdef _OS_WINDOWS_
    localtime_s(&conv, &cTim);
#elif defined(_OS_UCOSII_) || defined(_OS_UCOS_III_) || defined(_OS_LINUX_)
    localtime_r(&cTim, &conv);
#else
    localtime_r(&cTim, &conv);
#endif
    tim->year = Count_Int8ToBcd((uint8_t)(conv.tm_year + 1900 - 2000));
    tim->month = Count_Int8ToBcd((uint8_t)conv.tm_mon+1);
    tim->day = Count_Int8ToBcd((uint8_t)conv.tm_mday);
    tim->hour = Count_Int8ToBcd((uint8_t)conv.tm_hour);
    tim->minute = Count_Int8ToBcd((uint8_t)conv.tm_min);
    tim->seconds = Count_Int8ToBcd((uint8_t)conv.tm_sec);
}
//-------------------------------------------------------------------------------
// 函数功能: 时间比对
// 入口参数: tm1,tm2
// 返    回: 秒差值
//-------------------------------------------------------------------------------
int32_t Count_TimeCompare(struct tm *tm1,struct tm *tm2)
{
    time_t t1,t2;
    t1 = Count_Time_ConvCalendarToUnix(*tm1);
    t2 = Count_Time_ConvCalendarToUnix(*tm2);
    return(t1-t2);
}
//-------------------------------------------------------------------------------
// 函数功能: 由年月日获取星期
// 入口参数: 年月日
// 返回参数: OK / ERR
//-------------------------------------------------------------------------------
uint8_t Count_YYMMDDToWeek(uint8_t yy,uint8_t mm,uint8_t dd)
{
    uint8_t week, day;
    //润年月星期表
    static const uint8_t WeekTab[] =
    {
        (3 << 5) + 31,
        (6 << 5) + 29,
        (0 << 5) + 31,
        (3 << 5) + 30,
        (5 << 5) + 31,
        (1 << 5) + 30,
        (3 << 5) + 31,
        (6 << 5) + 31,
        (1 << 5) + 30,
        (4 << 5) + 31,
        (0 << 5) + 30,
        (2 << 5) + 31
    };
    //月表
    day = WeekTab[mm];
    //月星期数
    week = day >> 5;
    //月天数
    day &= 0x1f;
    //平年
    if ((mm < 3) && (yy & 0x03))
    {
        //平年月天数
        if (mm == 2) day--;
        //平年月表+1
        week++;
    }
    //年+年/4
    yy = yy + (yy >> 2);
    //(星期=年+年/4+月表+2日)%7
    week = (week +  yy + dd + 2) % 7;
    //返回星期和月天数
    return week;
}
/*******************************************************************************
* 函数功能: (蔡勒公式)将年月日转换为星期
* 参    数: year  :  实际年,如2016等
            month :  1-12
            day   :  1-31
* 返 回 值：0-6(0为星期日)
* 注    意: 此版本蔡勒公式只适用与:1582年10月15日之后
*******************************************************************************/
int32_t Count_ZellerWeek(int32_t year,int32_t month,int32_t day)
{
    int32_t m,d,y,c,w;
    m=month;
    d=day;
    if(month<=2)
    {
        year--;
        m=month+12;
    }
    y=year%100;
    c=year/100;
    w=(y+y/4+c/4-2*c+(13*(m+1)/5)+d-1)%7;
    if(w<0)
    {
        w+=7;
    }
    return w;
}
//-------------------------------------------------------------------------------
// 函数名称: INT8U TimeJudge(INT8U TimeGroupNumber)
// 函数功能: tm时间转换为5字节短时间
// 入口参数:   mode = 1 : tm->time5;  mode=2:time5->tm ;
//             mode = 3 : tm->time6;  mode=4:time6->tm ;
// 返回参数:
// 修改说明：
// 修改时间：
// 注    意: 参数中的tm为项目专用的tm格式,比标准的少2000年
//-------------------------------------------------------------------------------
uint8_t Count_TimeTypeConvert(uint8_t mode,struct tm* tm_time,uint8_t* time)
{
    uint8_t i;
    //tm->time5
    if(mode==1)
    {
        //年
        i=tm_time->tm_year-2000;
        time[4]=(i<<1);
        //月
        i=tm_time->tm_mon;
        i++;
        time[4]+=(i>>3);
        time[3]=(i<<5);
        //日
        time[3]+=tm_time->tm_mday;
        //时
        time[2]=((tm_time->tm_hour)<<3);
        //分
        i=tm_time->tm_min;
        time[2]+=(i>>3);
        time[1]=(i<<5);
        //秒
        i=tm_time->tm_sec;
        time[1]+=(i>>1);
        time[0]=(i<<7);
        //毫秒
    }
    //time5->tm
    else if(mode==2)
    {
        tm_time->tm_year=  (time[4]>>1)+2000;                              // 年 since 2000
        tm_time->tm_mon= (((time[4]&0x01)<<3) |(time[3]>>5))-1;   // 月 0 to 11
        tm_time->tm_mday= (time[3]&0x1f);                           // 日 1 to 31
        tm_time->tm_hour= (time[2]>>3);                             // 时 0 to 23
        tm_time->tm_min= ((time[2]& 0x07)<<3)|(time[1]>>5);   // 分 0 to 59
        tm_time->tm_sec= ((time[1]&0x1f)<<1) |(time[0]>>7);  // 秒 0 to 60
    }
    //tm->time6
    else if(mode==3)
    {
        time[0]   =  tm_time->tm_year  -  2000;
        time[1]   =  tm_time->tm_mon   +  1;
        time[2]   =  tm_time->tm_mday;
        time[3]   =  tm_time->tm_hour;
        time[4]   =  tm_time->tm_min;
        time[5]   =  tm_time->tm_sec;
    }
    //time6->tm
    else if(mode==4)
    {
        tm_time->tm_year   =  time[0]+2000;
        tm_time->tm_mon    =  time[1]-1;
        tm_time->tm_mday   =  time[2];
        tm_time->tm_hour   =  time[3];
        tm_time->tm_min    =  time[4];
        tm_time->tm_sec    =  time[5];
    }
    return OK;
}
uint8_t Count_TimeOffset(uint8_t *pInTime6,uint8_t *pOutTime6,int32_t offset_second)
{
    struct tm* ptm1;
    time_t t1;
    ptm1=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    ptm1->tm_year =  pInTime6[0]+2000;
    ptm1->tm_mon  =  pInTime6[1]-1;
    ptm1->tm_mday =  pInTime6[2];
    ptm1->tm_hour =  pInTime6[3];
    ptm1->tm_min  =  pInTime6[4];
    ptm1->tm_sec  =  pInTime6[5];
    t1 = Count_Time_ConvCalendarToUnix(*ptm1);
    t1 = t1+offset_second;
    *ptm1 = Count_Time_ConvUnixToCalendar(t1);
    pOutTime6[0]  =  ptm1->tm_year-2000;
    pOutTime6[1]  =  ptm1->tm_mon+1;
    pOutTime6[2]  =  ptm1->tm_mday;
    pOutTime6[3]  =  ptm1->tm_hour;
    pOutTime6[4]  =  ptm1->tm_min;
    pOutTime6[5]  =  ptm1->tm_sec;
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,ptm1);
    return OK;
}
//-------------------------------------------------------------------------------
// 函数功能:   位图读
// 入口参数:   *bitmap  ---   位图表
//             bnum     ---   位号(1起始)
// 返回参数:   TRUE/FALSE
//-------------------------------------------------------------------------------
uint8_t Count_Bitmap_Read(uint8_t *bitmap,uint16_t bnum)
{
    uint16_t nByte;
    uint8_t offset;
    //位号1起始
    if(bnum<1)return FALSE;
    //根据位号获取字节编号nByte(0起始)
    nByte =  (bnum-1)>>3;
    //根据位号获取相对偏移量offset(0-7)
    offset = (bnum-1)&0x0007;
    //判断
    if(bitmap[nByte]&(1<<offset))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
//-------------------------------------------------------------------------------
// 函数功能:   位图写
// 入口参数:   *bitmap  ---   位图表
//             bnum     ---   位号(1起始)
//             state    ---   TRUE/FALSE
// 返回参数:  TRUE - 状态跳变 FALSE - 状态保持
//-------------------------------------------------------------------------------
uint8_t Count_Bitmap_Write(uint8_t *bitmap,uint16_t bnum,uint8_t state)
{
    uint16_t nByte;
    uint8_t offset;
    uint8_t res;
    //位号1起始
    if(bnum<1)return FALSE;
    //根据位号获取字节编号nByte(0起始)
    nByte =  (bnum-1)>>3;
    //根据位号获取相对偏移量offset(0-7)
    offset = (bnum-1)&0x0007;
    //读取
    if((0!=(bitmap[nByte]&(1<<offset)))&&(state==TRUE))
    {
        res=FALSE;
    }
    else if((0==(bitmap[nByte]&(1<<offset)))&&(state==FALSE))
    {
        res=FALSE;
    }
    else
    {
        res=TRUE;
    }
    //更新标志
    if(state==TRUE)
    {
        bitmap[nByte]|=(1<<offset);
    }
    else
    {
        bitmap[nByte]&=~(1<<offset);
    }
    return res;
}
//-------------------------------------------------------------------------------
// 函数功能:   位图搜索(从小到大)
// 入口参数:   *bitmap  ---   位图表
//             *bnum    ---   位号(1起始)
//             state    ---   TRUE/FALSE
//             maxbnum  ---   最大位号(1起始)
// 返回参数:   无
//-------------------------------------------------------------------------------
void Count_Bitmap_Search(uint8_t *bitmap,uint16_t *bnum,uint8_t state,uint16_t maxbnum)
{
    uint16_t bitmapsize;
    uint8_t  i8;
    uint16_t i16;
    uint8_t  res=0;
    //初始化
    *bnum = 0;
    //计算位图尺寸(0起始)
    bitmapsize=(maxbnum-1)>>3;
    if(state==TRUE)
    {
        for(i16=0; i16<=bitmapsize; i16++)
        {
            for(i8=0; i8<8; i8++)
            {
                if(0!=(bitmap[i16]&(1<<i8)))
                {
                    *bnum=(i16<<3);
                    *bnum+=(i8+1);
                    res=1;
                    break;
                }
            }
            if(res==1)
            {
                break;
            }
        }
    }
    else
    {
        for(i16=0; i16<=bitmapsize; i16++)
        {
            for(i8=0; i8<8; i8++)
            {
                if(0==(bitmap[i16]&(1<<i8)))
                {
                    *bnum=(i16<<3);
                    *bnum+=(i8+1);
                    res=1;
                    break;
                }
            }
            if(res==1)
            {
                break;
            }
        }
    }
    //矫正(超过判定区,则无效)
    if(*bnum>maxbnum)
    {
        *bnum = 0;
    }
}
//-------------------------------------------------------------------------------
// 函数功能:   计算线模块电阻关系
// 入口参数:
// 返回参数:   R_om[0]-R_om[5]
// 电路:
// V -----R0-----R1-----R2-----R3-----R4-----R5-----GNG
//            |- K1 -|- K2 -|- K3 -|      V(ad)
//V(ad): 0-350-700-1050-1400-1750-2100-2450-2800-3150
//-------------------------------------------------------------------------------
void Count_LineMode_R(void)
{
    uint32_t v_mv;
    uint32_t R_om[6];
    uint32_t R123_om;
    //
    uint8_t *buf;
    uint16_t *pi16;
    //
    //设置电压
    v_mv = 5000;
    //设置基准电阻阻值(R6)
    R_om[5]=10000;
    //当线模块短路时,可计算R0
    R_om[0]=v_mv*R_om[5]/3150-R_om[5];
    //当K1K2K3按下后,可以计算R4
    R_om[4]=v_mv*R_om[5]/3000-(R_om[0]+R_om[5]);
    //当按键都没有按下时,可以计算R1+R2+R3
    R123_om=v_mv*R_om[5]/1400-(R_om[0]+R_om[4]+R_om[5]);
    //
    R_om[1]=R123_om/7;
    R_om[2]=R_om[1]*2;
    R_om[3]=R_om[1]*4;
    //
    //5V(标准)
    R_om[0] = 5900;
    R_om[1] = 2700;
    R_om[2] = 5490;
    R_om[3] = 10700;
    R_om[4] = 787;
    R_om[5] = 10000;
    //5V(贴近)
    R_om[0] = (3000+3000);
    R_om[1] = 2400;
    R_om[2] = (3000+2400);
    R_om[3] = 10000;
    R_om[4] = 820;
    R_om[5] = 10000;
    //
    //贴近
    R_om[0] = (3000+3000);
    R_om[1] = 2400;
    R_om[2] = (3000+2400);
    R_om[3] = 11000;
    R_om[4] = 1000;
    R_om[5] = 11000;
    //贴近
    R_om[0] = 4020;
    R_om[1] = 2400;
    R_om[2] = 4020;
    R_om[3] = 11000;
    R_om[4] = 1000;
    R_om[5] = 11000;
    //申请缓存
    buf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    pi16 = (uint16_t*)&buf[100];
    //获取理论电压
    pi16[0] = (R_om[5]*v_mv)/(R_om[0]+R_om[4]+R_om[5]);
    pi16[1] = (R_om[5]*v_mv)/(R_om[0]+R_om[4]+R_om[1]+R_om[5]);
    pi16[2] = (R_om[5]*v_mv)/(R_om[0]+R_om[4]+R_om[2]+R_om[5]);
    pi16[3] = (R_om[5]*v_mv)/(R_om[0]+R_om[4]+R_om[3]+R_om[5]);
    pi16[4] = (R_om[5]*v_mv)/(R_om[0]+R_om[4]+R_om[1]+R_om[2]+R_om[5]);
    pi16[5] = (R_om[5]*v_mv)/(R_om[0]+R_om[4]+R_om[1]+R_om[3]+R_om[5]);
    pi16[6] = (R_om[5]*v_mv)/(R_om[0]+R_om[4]+R_om[2]+R_om[3]+R_om[5]);;
    pi16[7] = (R_om[5]*v_mv)/(R_om[0]+R_om[4]+R_om[1]+R_om[2]+R_om[3]+R_om[5]);;
    pi16[8] = (R_om[5]*v_mv)/(R_om[0]+R_om[5]);
    pi16[9] = 0;

    /*
    #define  AD_LINE_R1  3000
    #define  AD_LINE_R2  390
    #define  AD_LINE_R3  1000
    #define  AD_LINE_R4  2400
    #define  AD_LINE_R5  4020
    #define  AD_LINE_R6  820
    */
    //打印电阻
    sprintf((char *)buf,"R0:%lu,R1:%lu,R2:%lu,R3:%lu,R4:%lu,R5:%lu,\r\n",R_om[0],R_om[1],R_om[2],R_om[3],R_om[4],R_om[5]);
    COUNT_DEBUG_OUT_STR((int8_t*)buf);
    //打印理论电压
    sprintf((char *)buf,"SHORT_K1K2K3:%05dmV\r\n",pi16[0]);
    COUNT_DEBUG_OUT_STR((int8_t*)buf);
    sprintf((char *)buf,"BREAK_K1    :%05dmV\r\n",pi16[1]);
    COUNT_DEBUG_OUT_STR((int8_t*)buf);
    sprintf((char *)buf,"BREAK_K2    :%05dmV\r\n",pi16[2]);
    COUNT_DEBUG_OUT_STR((int8_t*)buf);
    sprintf((char *)buf,"BREAK_K3    :%05dmV\r\n",pi16[3]);
    COUNT_DEBUG_OUT_STR((int8_t*)buf);
    sprintf((char *)buf,"BREAK_K1K2  :%05dmV\r\n",pi16[4]);
    COUNT_DEBUG_OUT_STR((int8_t*)buf);
    sprintf((char *)buf,"BREAK_K1K3  :%05dmV\r\n",pi16[5]);
    COUNT_DEBUG_OUT_STR((int8_t*)buf);
    sprintf((char *)buf,"BREAK_K2K3  :%05dmV\r\n",pi16[6]);
    COUNT_DEBUG_OUT_STR((int8_t*)buf);
    sprintf((char *)buf,"BREAK_K1K2K3:%05dmV\r\n",pi16[7]);
    COUNT_DEBUG_OUT_STR((int8_t*)buf);
    sprintf((char *)buf,"SHORT       :%05dmV\r\n",pi16[8]);
    COUNT_DEBUG_OUT_STR((int8_t*)buf);
    sprintf((char *)buf,"BREAK       :%05dmV\r\n",pi16[9]);
    COUNT_DEBUG_OUT_STR((int8_t*)buf);
    //释放缓存
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,buf);
}
void arm_q30_to_float (q30_t *pSrc, float32_t  *pDst,uint32_t blockSize)
{
    uint32_t i32;
    for(i32=0; i32<blockSize; i32++)
    {
        pDst[i32]=pSrc[i32]/COUNT_Q30;
    }
}
uint8_t Count_Quaternion_To_EulerAngle(float *p_quat,float *p_ea)
{
    // 俯仰角
    *(p_ea+0)  = asin(-2 * p_quat[1] * p_quat[3] + 2 * p_quat[0]* p_quat[2])* 57.3;
    // 翻滚脚
    *(p_ea+1) = atan2(2 * p_quat[2] * p_quat[3] + 2 * p_quat[0] * p_quat[1], -2 * p_quat[1] * p_quat[1] - 2 * p_quat[2]* p_quat[2] + 1)* 57.3;
    // 航向角
    *(p_ea+2) =  atan2(2*(p_quat[1]*p_quat[2] + p_quat[0]*p_quat[3]),p_quat[0]*p_quat[0]+p_quat[1]*p_quat[1]-p_quat[2]*p_quat[2]-p_quat[3]*p_quat[3]) * 57.3;
    return OK;
}
//-------------------------------------------------------------------------------
// 函数功能:   傅立叶变换
// 入口参数:
// 返回参数:
//-------------------------------------------------------------------------------
#ifdef COUNT_FFT_ENABLE

#define PI2  6.28318530717959
#define NPT 64            //傅立叶采样点数量(NPT = No of FFT point)
long lBUFIN[NPT];         //输入向量
long lBUFOUT[NPT];        //输出向量
long lBUFMAG[NPT + NPT/2];//功率向量(频谱幅值)Magnitude vector
//-----将两个正弦波信号合成一个信号
//参数:  nfill -  输入信号序列长度
//       Fs    -  采样频率
//       Freq1 -  1号正弦波频率
//       Freq2 -  2号正弦波频率
//       Ampli -  比例因子
void Count_DoubleSin(long nfill, long Fs, long Freq1, long Freq2, long Ampli)
{

    uint32_t i;
    float fFs, fFreq1, fFreq2, fAmpli;
    float fZ,fY;

    fFs = (float) Fs;
    fFreq1 = (float) Freq1;
    fFreq2 = (float) Freq2;
    fAmpli = (float) Ampli;

    for (i=0; i < nfill; i++)
    {
        fY = 1+sin(PI2 * i * fFreq1/fFs) + sin(PI2 * i * fFreq2/fFs);
        fZ = fAmpli * fY;
        lBUFIN[i]= ((short)fZ) << 16 ;  /* sine_cosine  (cos=0x0) */
    }
}
//-----移除频谱别名(XSL:可能是频谱虚部)(Removes the aliased part of the spectrum (not tested))
//参数:  nfill -  功率向量序列长度
void onesided(long nfill)
{
    uint32_t i;

    lBUFMAG[0] = lBUFMAG[0];
    lBUFMAG[nfill/2] = lBUFMAG[nfill/2];
    for (i=1; i < nfill/2; i++)
    {
        lBUFMAG[i] = lBUFMAG[i] + lBUFMAG[nfill-i];
        lBUFMAG[nfill-i] = 0x0;
    }
}
//-----计算傅立叶变换后的功率
//参数:  nfill    -  输出信号序列长度
//       *strPara -  如果设为"1SIDED" ,则移除频谱别名(XSL:可能是频谱虚部)
void powerMag(long nfill, char* strPara)
{
    int32_t lX,lY;
    uint32_t i;

    for (i=0; i < nfill; i++)
    {
        lX= (lBUFOUT[i]<<16)>>16; /* sine_cosine --> cos */
        lY= (lBUFOUT[i] >> 16);   /* sine_cosine --> sin */
        {
            float X=  64*((float)lX)/32768;
            float Y = 64*((float)lY)/32768;
            float Mag = sqrt(X*X+ Y*Y)/nfill;
            lBUFMAG[i] = (uint32_t)(Mag*65536);
        }
    }
    if (strPara == "1SIDED") onesided(nfill);
}
//-----傅立叶实验(扫描双频率波形)
//参数:  freqinc1 -  波形1的步进频率
//       freqinc2 -  波形2的步进频率
void Count_FFT_Test(uint32_t freqinc1,uint32_t freqinc2)
{
    //uint32_t freq;
    uint16_t i16;
    //uint8_t res;
    uint8_t *p_i8;
    //申请缓存
    p_i8=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    //
    //for (freq=50; freq <1600; freq+=freqinc1)
    //{
    //生成波形(2双正弦波)
    Count_DoubleSin(NPT, 3200, 50, 1250, 4000);
    //执行傅立叶变换
    cr4_fft_64_stm32(lBUFOUT, lBUFIN, NPT);
    //获取频谱功率
    powerMag(NPT,"1SIDED");
    //显示时域波形
    //In_displayWaveform(DISPLAY_RIGHT);
    //显示频域波形
    //displayPowerMag(DISPLAY_RIGHT, 9);
    COUNT_DEBUG_OUT_STR("0,0\r\n");
    COUNT_DEBUG_OUT_STR("0,0\r\n");
    COUNT_DEBUG_OUT_STR("0,0\r\n");
    COUNT_DEBUG_OUT_STR("0,0\r\n");
    COUNT_DEBUG_OUT_STR("0,0\r\n");
    COUNT_DEBUG_OUT_STR("0,0\r\n");
    COUNT_DEBUG_OUT_STR("0,0\r\n");
    COUNT_DEBUG_OUT_STR("0,0\r\n");
    COUNT_DEBUG_OUT_STR("0,0\r\n");
    COUNT_DEBUG_OUT_STR("0,0\r\n");
    for(i16=0; i16<NPT; i16++)
    {
        if(i16<(NPT/2))
        {
            sprintf((char *)p_i8,"%d,%d\r\n",lBUFIN[i16]>>16,lBUFMAG[i16]);
        }
        else
        {
            sprintf((char *)p_i8,"%d,0\r\n",lBUFIN[i16]>>16);
        }
        COUNT_DEBUG_OUT_STR((int8_t*)p_i8);
    }
    //}
    /*
    for (freq=40; freq <4000; freq+=freqinc2)
    {
      //生成波形(2双正弦波)
      MygSin(NPT, 8000, freq, 160, 32767/2);
      //执行傅立叶变换
      cr4_fft_64_stm32(lBUFOUT, lBUFIN, NPT);
      //获取频谱功率
      powerMag(NPT,"2SIDED");
      //显示时域波形
      //In_displayWaveform(DISPLAY_LEFT);
      //显示频域波形
      //displayPowerMag(DISPLAY_LEFT, 8);
    }
    */
    //释放缓存
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,p_i8);
}

#endif
//---------------------------------------------------------------------------------------缓存环1
void Count_BufferLoopInit(COUNT_BUFFERLOOP_S *ps,uint8_t *pBuf,uint16_t len,COUNT_BUFFERLOOP_S_NODE *psNodeBuf,uint16_t snodeBufNum,COUNT_BUFFERLOOP_S_LIMIT *pslimit)
{
    //结构体赋值
    ps->pBuf=pBuf;
    ps->len=len;
    ps->psNodeBuf=psNodeBuf;
    ps->NodeNum=snodeBufNum;
    ps->pslimit=pslimit;
    //初始化结构体
    memset((char *)ps->pBuf,0,ps->len);
    memset((char *)ps->psNodeBuf,0,(ps->NodeNum)*sizeof(COUNT_BUFFERLOOP_S_NODE));
    memset((char *)ps->pslimit,0,sizeof(COUNT_BUFFERLOOP_S_LIMIT));
}
uint8_t Count_BufferLoopPush(COUNT_BUFFERLOOP_S *ps,uint8_t *pBuf,uint16_t len)
{
    uint8_t i;
    //uint8_t begin=0,end=0;
    uint16_t free_size=0;
    uint16_t i16;
    uint16_t new_addr=0;
    if(len>=(ps->len) || len==0)return ERR;
    //计算剩余空间
    if(ps->pslimit->endAddr>ps->pslimit->beginAddr)
    {
        free_size=(ps->len) -(ps->pslimit->endAddr-ps->pslimit->beginAddr+1);
    }
    else if(ps->pslimit->endAddr<ps->pslimit->beginAddr)
    {
        free_size=ps->pslimit->beginAddr-ps->pslimit->endAddr-1;
    }
    else
    {
        free_size=(ps->len);
    }
    if(len>free_size)return ERR;
    //获取新的缓存环地址
    if(ps->pslimit->endAddr!=ps->pslimit->beginAddr)
    {
        new_addr=ps->pslimit->endAddr+1;
        if(new_addr>=(ps->len))
        {
            new_addr=0;
        }
    }
    else
    {
        new_addr=ps->pslimit->endAddr;
    }
    //
    ps->pslimit->endAddr=new_addr;
    //复制数据,更新结束点
    if(ps->pslimit->endAddr+len<=(ps->len))
    {
        memcpy((char*)&((ps->pBuf)[ps->pslimit->endAddr]),pBuf,len);
        ps->pslimit->endAddr+=(len-1);
    }
    else
    {
        i16=(ps->len)-ps->pslimit->endAddr;
        memcpy((char*)&((ps->pBuf)[ps->pslimit->endAddr]),pBuf,i16);
        memcpy((char*)&((ps->pBuf)[0]),&pBuf[i16],len-i16);
        ps->pslimit->endAddr=len-i16-1;
    }
    //获取新结点-->i
    if(ps->pslimit->beginNode!=ps->pslimit->endNode)
    {
        i=ps->pslimit->endNode+1;
        if(i>=ps->NodeNum)
        {
            i=0;
        }
        if(i==ps->pslimit->beginNode)
        {
            if((ps->psNodeBuf)[i].TimeoutCount_ms==0)
            {
                i=ps->pslimit->beginNode;
            }
            else
            {
                //找不到新结点
                return ERR;
            }
        }
    }
    else
    {
        if((ps->psNodeBuf)[ps->pslimit->beginNode].TimeoutCount_ms!=0)
        {
            //当前节点既是起始结点又是结束结点
            i=ps->pslimit->endNode+1;
            if(i>=ps->NodeNum)
            {
                i=0;
            }
            ps->pslimit->endNode =i;
        }
        i=ps->pslimit->endNode;
    }
    ps->pslimit->endNode=i;
    //更新结点
    (ps->psNodeBuf)[ps->pslimit->endNode].buf=&((ps->pBuf)[new_addr]);
    (ps->psNodeBuf)[ps->pslimit->endNode].len=len;
    (ps->psNodeBuf)[ps->pslimit->endNode].TimeoutCount_ms=0xFFFF;
    return OK;
}

uint8_t Count_BufferLoopPop(COUNT_BUFFERLOOP_S *ps,uint8_t *pBuf,uint16_t* len,COUNT_BUFFERLOOP_E_POPMODE mode)
{
    uint16_t i16;
    uint32_t i32,j32;
    //查看结点有效性
    if((ps->psNodeBuf)[ps->pslimit->beginNode].TimeoutCount_ms==0)
    {
        *len=0;
        return ERR;
    }
    //如果缓存指针为空,则只返回长度信息,且不清空环数据
    if(pBuf==NULL)
    {
        *len=(ps->psNodeBuf)[ps->pslimit->beginNode].len;
        return OK;
    }
    if((ps->psNodeBuf)[ps->pslimit->beginNode].len==0)return ERR;
    //复制数据
    i32=(uint32_t)((ps->psNodeBuf)[ps->pslimit->beginNode].buf) + (ps->psNodeBuf)[ps->pslimit->beginNode].len;
    if(i32<=(uint32_t)(ps->pBuf)+(ps->len))
    {
        //没有越界
        i16=(ps->psNodeBuf)[ps->pslimit->beginNode].len;
        if(pBuf!=NULL)
        {
            memcpy(pBuf,(ps->psNodeBuf)[ps->pslimit->beginNode].buf,i16);
        }
    }
    else
    {
        //越界
        j32=(&((ps->pBuf)[0]))+(ps->len)-(ps->psNodeBuf)[ps->pslimit->beginNode].buf;
        i16=(ps->psNodeBuf)[ps->pslimit->beginNode].len;
        if(pBuf!=NULL)
        {
            memcpy(pBuf,(ps->psNodeBuf)[ps->pslimit->beginNode].buf,j32);
            memcpy(&pBuf[j32],ps->pBuf,i16-j32);
        }
    }
    if(len!=NULL)
    {
        *len=i16;
    }
    if(mode==COUNT_BUFFERLOOP_E_POPMODE_KEEP)
    {
        return OK;
    }
    //释放空间
    ps->pslimit->beginAddr+=(ps->psNodeBuf)[ps->pslimit->beginNode].len;
    if(ps->pslimit->beginAddr>=(ps->len))
    {
        ps->pslimit->beginAddr-=(ps->len);
    }
    i16=ps->pslimit->endAddr;
    i16++;
    if(i16>=(ps->len))
    {
        i16-=(ps->len);
    }
    if(i16==ps->pslimit->beginAddr)
    {
        //最后一包数据特殊处理
        ps->pslimit->endAddr=i16;
    }
    //释放记录结点
    (ps->psNodeBuf)[ps->pslimit->beginNode].TimeoutCount_ms=0;
    //释放极限结点
    i16=ps->pslimit->beginNode;
    i16++;
    if(ps->NodeNum==i16)
    {
        i16=0;
    }
    if(ps->pslimit->beginNode==ps->pslimit->endNode)
    {
        ps->pslimit->beginNode=ps->pslimit->endNode=i16;
    }
    else
    {
        ps->pslimit->beginNode=i16;
    }
    return OK;
}
//---------------------------------------------------------------------------------------modbus相关
#ifdef COUNT_MODBUS_ENABLE
/*******************************************************************************
函数功能:   Modbus数据整合
参    数:   *pOutBuf          ---   输出缓存
            *pOutLen          ---   输出缓存有效长度
            slaveAddr         ---   从机地址
            functionCode      ---   功能码
            *pDataBeginAddr   ---   寄存器首地址
            *pDataWordLen     ---   操作字长度
            *pDataByteLen     ---   操作字节长度
            *pDataBuf         ---   寄存器数据
            DataLen           ---   寄存器数据长度
返 回 值:   无
说    明:   上述部分指针为空,表示不参与数据整合
*******************************************************************************/
uint8_t Count_ModbusSlaveAddr=0;
void Count_Modbus_Array(uint8_t *pOutBuf,uint16_t *pOutLen,uint8_t slaveAddr,uint8_t functionCode,uint16_t *pDataBeginAddr,uint16_t *pDataWordLen,uint8_t *pDataByteLen,uint8_t *pDataBuf,uint16_t DataLen)
{
    uint16_t len=0;
    uint16_t i16;
    //设备地址
    pOutBuf[len++]=slaveAddr;
    //功能码
    pOutBuf[len++]=functionCode;
    //数据起始地址
    if(pDataBeginAddr!=NULL)
    {
        i16=*pDataBeginAddr;
#ifdef COUNT_MODBUS_TRANSFER_FIRST_HIGH
        i16=Count_Int16ByteReversed(i16);
#endif
        pOutBuf[len++]=i16;
        i16=i16>>8;
        pOutBuf[len++]=i16;
    }
    //数据字长度
    if(pDataWordLen!=NULL)
    {
        i16=*pDataWordLen;
#ifdef COUNT_MODBUS_TRANSFER_FIRST_HIGH
        i16=Count_Int16ByteReversed(i16);
#endif
        pOutBuf[len++]=i16;
        i16=i16>>8;
        pOutBuf[len++]=i16;
    }
    //数据字节长度
    if(pDataByteLen!=NULL)
    {
        pOutBuf[len++]=*pDataByteLen;
    }
    //数据
    if(pDataBuf!=NULL && DataLen!=0)
    {
        memcpy(&pOutBuf[len],pDataBuf,DataLen);
        len+=DataLen;
    }
    //CRC校验
    i16=Count_CRC16(pOutBuf,len);
    pOutBuf[len++]=i16;
    i16=i16>>8;
    pOutBuf[len++]=i16;
    //返回数据总有效长度
    *pOutLen=len;
}
/*******************************************************************************
函数功能:   校验Modbus的CRC
参    数:   *pBuf ---   缓存
            len   ---   长度
返 回 值:   OK/ERR
*******************************************************************************/
uint8_t Count_Modbus_Check(uint8_t *pBuf,uint16_t len,uint8_t addr)
{
    uint16_t i16,j16;
    //校验地址
    if(addr!=COUNT_MODBUS_BROADCAST_ADDR && addr!=Count_ModbusSlaveAddr)
    {
        return ERR;
    }
    //校验长度
    if(len<=2)
    {
        return ERR;
    }
    //计算CRC
    i16=Count_CRC16(pBuf,len-2);
    //获取CRC
    j16=pBuf[len-1];
    j16<<=8;
    j16+=pBuf[len-2];
    //比较
    if(i16!=j16)
    {
        return ERR;
    }
    //
    return OK;
}
#endif
//-------------------------------------------------------------------------------
#if (defined(STM32F1))
/*******************************************************************************
* Function Name : uint32_t QUEUE_PacketCreate(QUEUE8_t *pQ8, uint8_t *pBuf, uint32_t lenSize)
* Description   : 队列建立
* Input         :   pQ8:    队列
                    pBuf:   队列缓冲区地址
                    bufSize:队列缓冲区大小
* Output        :
* Other         :
* Date          : 2013.08.29
*******************************************************************************/
uint32_t QUEUE_PacketCreate(QUEUE8_t *pQ8, uint8_t *pBuf, uint32_t bufSize)
{
    ASSERT_PARAM(pQ8);
    ASSERT_PARAM(pBuf);
    ASSERT_PARAM(bufSize);

    pQ8->bufSize    = bufSize;
    pQ8->pBuf       = pBuf;
    pQ8->pStart     = pBuf;
    pQ8->pEnd       = pBuf;

    return 0;
}

/*******************************************************************************
* Function Name : uint32_t QUEUE_PacketIn(QUEUE8_t *pQ8, uint8_t *pData, uint32_t len)
* Description   : 数据载入队列
* Input         :   pQ8:    队列
                    pData:  要进队列的数据
                    len:    数据长度
* Output        :
* Other         :
* Date          : 2013.08.29
*******************************************************************************/
uint32_t QUEUE_PacketIn(QUEUE8_t *pQ8, uint8_t *pData, uint32_t len)
{
    volatile uint8_t    *pEnd   = NULL;

    uint32_t            index   = 0;

    ASSERT_PARAM(pData);
    ASSERT_PARAM(pQ8);
    ASSERT_PARAM(pQ8->pBuf);
    ASSERT_PARAM(pQ8->pStart);
    ASSERT_PARAM(pQ8->pEnd);

    pEnd = pQ8->pEnd;

    for (index = 0; index < len; index++)
    {
        if (++pEnd >= pQ8->pBuf + pQ8->bufSize)
        {
            pEnd = pQ8->pBuf;
        }
        if (pEnd == pQ8->pStart)
        {
            break;
        }

        *pQ8->pEnd = *pData++;

        pQ8->pEnd = pEnd;
    }

    return index;
}

/*******************************************************************************
* Function Name : uint32_t QUEUE_PacketOut(QUEUE8_t *pQ8, uint8_t *pData, uint32_t dataLen)
* Description   : 队列中取数据
* Input         :   pQ8:    队列
                    pData:  缓冲区
                    dataLen:缓冲区大小
* Output        :
* Other         :
* Date          : 2013.08.29
*******************************************************************************/
uint32_t QUEUE_PacketOut(QUEUE8_t *pQ8, uint8_t *pData, uint32_t dataLen)
{
    uint32_t index = 0;

    ASSERT_PARAM(pData);
    ASSERT_PARAM(pQ8);
    ASSERT_PARAM(pQ8->pBuf);
    ASSERT_PARAM(pQ8->pStart);
    ASSERT_PARAM(pQ8->pEnd);

    while ((pQ8->pStart != pQ8->pEnd) && (index < dataLen) && (index < pQ8->bufSize))
    {
        pData[index++] = *pQ8->pStart++;
        if (pQ8->pStart >= pQ8->pBuf + pQ8->bufSize)
        {
            pQ8->pStart = pQ8->pBuf;
        }
    }

    return index;
}

/*******************************************************************************
* Function Name : uint32_t QUEUE_PacketLengthGet(QUEUE8_TYPE *pQ8)
* Description   : 队列中的数据长度
* Input         :   pQ8:    队列
                    pData:  缓冲区
                    dataLen:缓冲区大小
* Output        :
* Other         :
* Date          : 2013.08.29
*******************************************************************************/
uint32_t QUEUE_PacketLengthGet(QUEUE8_t *pQ8)
{

    volatile uint8_t    *pStart     = NULL;
    uint32_t            index       = 0;

    ASSERT_PARAM(pQ8);
    ASSERT_PARAM(pQ8->pStart);
    ASSERT_PARAM(pQ8->pEnd);

    pStart = pQ8->pStart;

    while ((pStart != pQ8->pEnd) && (index < pQ8->bufSize))
    {
        index++;
        if (++pStart >= pQ8->pBuf + pQ8->bufSize)
        {
            pStart = pQ8->pBuf;
        }
    }

    return index;
}


/*******************************************************************************
* Function Name : uint32_t QUEUE_PacketStartEndDifferentCharSplit(QUEUE8_t *pQ8, uint8_t startChar, uint8_t endChar, uint8_t *pData, uint32_t dataLen)
* Description   : 以起始符和结束符取队列中的数据 (取出的数据 包括起始符 和分隔符)
* Input         :   pQ8:        队列
                    startChar:  起始符
                    endChar:    结束符
                    pData:      缓冲区
                    dataLen:    缓冲区大小
* Output        :
* Other         :
* Date          : 2013.08.29
*******************************************************************************/
uint32_t QUEUE_PacketStartEndDifferentCharSplit(QUEUE8_t *pQ8, uint8_t startChar, uint8_t endChar, uint8_t *pData, uint32_t dataLen)
{
    int32_t count;
    int32_t index;
    volatile uint8_t *pStart;
    volatile uint8_t *pEnd;

    ASSERT_PARAM(pData);
    ASSERT_PARAM(pQ8);
    ASSERT_PARAM(pQ8->pBuf);
    ASSERT_PARAM(pQ8->pStart);
    ASSERT_PARAM(pQ8->pEnd);

    pStart      = pQ8->pStart;
    count       = pQ8->bufSize;

    while ((pStart != pQ8->pEnd) && count--)        //查找起始字符
    {
        if (startChar == *pStart) break;
        if (++pStart >= pQ8->pBuf + pQ8->bufSize) pStart = pQ8->pBuf;
    }

    if (pStart == pQ8->pEnd) return 0;              //未找到起始符
    if (count == -1) return 0;
    pEnd = pStart;
    if (++pEnd >= pQ8->pBuf + pQ8->bufSize) pEnd = pQ8->pBuf;

    while ((pEnd != pQ8->pEnd) && count--)          //查找结束字符
    {
        if (endChar == *pEnd) break;
        if (++pEnd >= pQ8->pBuf + pQ8->bufSize) pEnd = pQ8->pBuf;
    }

    if (pEnd == pQ8->pEnd) return 0;                //未找结束符
    if (count == -1) return 0;
    if (++pEnd >= pQ8->pBuf + pQ8->bufSize) pEnd = pQ8->pBuf;

    count   = pQ8->bufSize - count;
    index   = 0;
    //获取从起始字符到结束字符的数据
    while ((pStart != pEnd) && (index < dataLen) && (index < pQ8->bufSize) && count--)
    {
        pData[index++] = *pStart++;
        if (pStart >= pQ8->pBuf + pQ8->bufSize) pStart = pQ8->pBuf;
    }

    pQ8->pStart = pEnd;
    return index;
}

/*******************************************************************************
* Function Name : uint32_t QUEUE_PacketStartEndCharSplit(QUEUE8_t *pQ8, uint8_t splitChar, uint8_t *pData, uint32_t dataLen)
* Description   : 提取首尾分隔符内的数据(包括分隔符)
* Input         :   pQ8:        队列
                    startChar:  起始符
                    endChar:    结束符
                    pData:      缓冲区
                    dataLen:    缓冲区大小
* Output        :
* Other         :
* Date          : 2013.08.30
*******************************************************************************/
uint32_t QUEUE_PacketStartEndCharSplit(QUEUE8_t *pQ8, uint8_t splitChar, uint8_t *pData, uint32_t dataLen)
{
    int32_t count;
    int32_t index;
    volatile uint8_t *pStart;
    volatile uint8_t *pEnd;

    ASSERT_PARAM(pData);
    ASSERT_PARAM(pQ8);
    ASSERT_PARAM(pQ8->pBuf);
    ASSERT_PARAM(pQ8->pStart);
    ASSERT_PARAM(pQ8->pEnd);

    pStart      = pQ8->pStart;
    count       = pQ8->bufSize;

    while ((pStart != pQ8->pEnd) && count--)        //查找起始字符
    {
        if (splitChar == *pStart) break;
        if (++pStart >= pQ8->pBuf + pQ8->bufSize) pStart = pQ8->pBuf;
    }

    if (pStart == pQ8->pEnd) return 0;              //未找到起始符
    if (count == -1) return 0;
    pEnd = pStart;
    if (++pEnd >= pQ8->pBuf + pQ8->bufSize) pEnd = pQ8->pBuf;

    while ((pEnd != pQ8->pEnd) && count--)          //查找结束字符
    {
        if (splitChar == *pEnd) break;
        if (++pEnd >= pQ8->pBuf + pQ8->bufSize) pEnd = pQ8->pBuf;
    }

    if (pEnd == pQ8->pEnd) return 0;                //未找结束符
    if (count == -1) return 0;
    if (++pEnd >= pQ8->pBuf + pQ8->bufSize) pEnd = pQ8->pBuf;

    count   = pQ8->bufSize - count;
    index   = 0;
    //获取从起始字符到结束字符的数据
    while ((pStart != pEnd) && (index < dataLen) && (index < pQ8->bufSize) && count--)
    {
        pData[index++] = *pStart++;
        if (pStart >= pQ8->pBuf + pQ8->bufSize) pStart = pQ8->pBuf;
    }

    //如果取出的数据只包括分隔符，有可能是上次结束符和下次起始符，因此放弃上次结束符。
    if (index <= 2)
    {
        index = 0;
        if (--pStart < pQ8->pBuf) pStart = pQ8->pBuf + pQ8->bufSize - 1;
    }

    pQ8->pStart = pStart;
    return index;
}

/*******************************************************************************
* Function Name : uint32_t QUEUE_PacketCharSplit(QUEUE8_t *pQ8, uint8_t splitChar, uint8_t *pData, uint32_t dataLen)
* Description   : 提取单结束分隔符的数据 (包括分隔符)
* Input         :
* Output        :
* Other         :
* Date          : 2013.10.20
*******************************************************************************/
uint32_t QUEUE_PacketCharSplit(QUEUE8_t *pQ8, uint8_t splitChar, uint8_t *pData, uint32_t dataLen)
{
    int32_t count;
    int32_t index;
    volatile uint8_t *pStart;
    volatile uint8_t *pEnd;

    ASSERT_PARAM(pData);
    ASSERT_PARAM(pQ8);
    ASSERT_PARAM(pQ8->pBuf);
    ASSERT_PARAM(pQ8->pStart);
    ASSERT_PARAM(pQ8->pEnd);

    pStart      = pQ8->pStart;
    count       = pQ8->bufSize;

    while ((pStart != pQ8->pEnd) && count--)        //查找起始字符
    {
        if (splitChar == *pStart) break;
        if (++pStart >= pQ8->pBuf + pQ8->bufSize) pStart = pQ8->pBuf;
    }

    if (pStart == pQ8->pEnd) return 0;              //未找到起始符
    if (count == -1) return 0;
    pEnd = pStart;
    if (++pEnd >= pQ8->pBuf + pQ8->bufSize) pEnd = pQ8->pBuf;

    pStart      = pQ8->pStart;
    count       = pQ8->bufSize;
    index       = 0;
    while ((pStart != pEnd) && (index < dataLen) && count--)        //查找起始字符
    {
        pData[index++] = *pStart;
        if (++pStart >= pQ8->pBuf + pQ8->bufSize) pStart = pQ8->pBuf;
    }

    pQ8->pStart = pStart;
    return index;
}

/*******************************************************************************
* Function Name :QUEUE_PacketDoubleEndCharSplit
* Description   :提取双结束分隔符的数据 (包括分隔符)
* Input         :QUEUE8_t * pQ8
* Input         :uint8_t splitChar1
* Input         :uint8_t splitChar2
* Input         :uint8_t * pData
* Input         :uint32_t dataLen
* Output        :uint32_t
* Other         :
* Date          :2014/03/27
*******************************************************************************/
uint32_t QUEUE_PacketDoubleEndCharSplit(QUEUE8_t *pQ8, uint8_t splitChar1, uint8_t splitChar2, uint8_t *pData, uint32_t dataLen)
{
    int32_t count;
    int32_t index;
    volatile uint8_t *pStart;
    volatile uint8_t *pEnd;
    uint8_t lastChar = 0;

    ASSERT_PARAM(pData);
    ASSERT_PARAM(pQ8);
    ASSERT_PARAM(pQ8->pBuf);
    ASSERT_PARAM(pQ8->pStart);
    ASSERT_PARAM(pQ8->pEnd);

    pStart      = pQ8->pStart;
    count       = pQ8->bufSize;

    while ((pStart != pQ8->pEnd) && count--)        //查找起始字符
    {
        if ((splitChar1 == lastChar) && (splitChar2 == *pStart)) break;

        lastChar = *pStart;

        if (++pStart >= pQ8->pBuf + pQ8->bufSize) pStart = pQ8->pBuf;
    }

    if (pStart == pQ8->pEnd) return 0;              //未找到起始符
    if (count == -1) return 0;
    pEnd = pStart;
    if (++pEnd >= pQ8->pBuf + pQ8->bufSize) pEnd = pQ8->pBuf;

    pStart      = pQ8->pStart;
    count       = pQ8->bufSize;
    index       = 0;
    while ((pStart != pEnd) && (index < dataLen) && count--)        //查找起始字符
    {
        pData[index++] = *pStart;
        if (++pStart >= pQ8->pBuf + pQ8->bufSize) pStart = pQ8->pBuf;
    }

    pQ8->pStart = pStart;
    return index;
}





/*******************************************************************************
* Function Name : uint32_t QUEUE_PacketCreate(QUEUE_STRUCT_t *pQueue, uint8_t *pBuf, uint32_t bufSize)
* Description   : 结构体队列
* Input         :   pQueue:     队列名
                    pBuf:       队列缓冲区
                    bufSize:    换冲区大小(字节)
                    blkSize:    单结构体大小(字节)
* Output        : 0: 成功
* Other         :
* Date          : 2014.08.13
*******************************************************************************/
uint32_t QUEUE_StructCreate(QUEUE_STRUCT_t *pQueue, void *pBuf, uint32_t bufSize, uint16_t blkSize)
{
    ASSERT_PARAM(pQueue);
    ASSERT_PARAM(pBuf);
    ASSERT_PARAM(bufSize);
    ASSERT_PARAM(blkSize);

    pQueue->elemSize    = blkSize;
    pQueue->sumCount    = bufSize / blkSize;
    pQueue->pBuf        = pBuf;
    pQueue->start       = 0;
    pQueue->end         = 0;
    return 0;
}

/*******************************************************************************
* Function Name : uint32_t QUEUE_StructIn(QUEUE_STRUCT_t *pQueue, void *pData, uint16_t blkCount)
* Description   : 结构体队列入栈 缓冲区中如果满则不载入
* Input         :   pQueue:     队列名
                    pData:      准备入栈的数据
                    blkCount:   准备入栈的结构体个数(单位:结构体个数)
* Output        : 成功入栈结构体的个数
* Other         :
* Date          : 2014.08.14
*******************************************************************************/
uint32_t QUEUE_StructIn(QUEUE_STRUCT_t *pQueue, void *pData, uint32_t blkCount)
{
    uint32_t i = blkCount;
    uint32_t end = 0;

    ASSERT_PARAM(pQueue);
    ASSERT_PARAM(pQueue->pBuf);
    ASSERT_PARAM(pData);

    end = pQueue->end;
    for (i = 0; i < blkCount; i++)
    {
        //再装一组数据后，指针是否指向栈尾
        if (++end >= pQueue->sumCount)
        {
            end = 0;
        }

        //缓冲区填满 直接退出
        if (end == pQueue->start)
        {
            break;
        }

        memcpy((uint8_t *)pQueue->pBuf + pQueue->end * pQueue->elemSize, pData, pQueue->elemSize);

        pData = (uint8_t *)pData + pQueue->elemSize;
        pQueue->end = end;
    }

    return i;
}

/*******************************************************************************
* Function Name : uint32_t QUEUE_StructOut(QUEUE_STRUCT_t *pQueue, void *pData, uint16_t blkCount)
* Description   : 结构体队列出栈
* Input         :   pQueue:     队列名
                    pData:      准备出栈的数据缓冲区
                    blkCount:   存放出栈结构体的最大个数(单位:结构体个数)
* Output        : 成功出栈结构体的个数
* Other         :
* Date          : 2014.08.14
*******************************************************************************/
uint32_t QUEUE_StructOut(QUEUE_STRUCT_t *pQueue, void *pData, uint32_t blkCount)
{
    uint32_t index = 0;

    ASSERT_PARAM(pQueue);
    ASSERT_PARAM(pQueue->pBuf);
    ASSERT_PARAM(pData);

    while ((pQueue->start != pQueue->end) && (index < pQueue->sumCount) && (index < blkCount))
    {
        memcpy(pData, (uint8_t *)pQueue->pBuf + pQueue->start * pQueue->elemSize, pQueue->elemSize);

        pData = (uint8_t *)pData + pQueue->elemSize;
        index++;
        if (++pQueue->start >= pQueue->sumCount)
        {
            pQueue->start = 0;
        }
    }

    return index;
}

/*******************************************************************************
* Function Name : uint32_t QUEUE_StructCountGet(QUEUE_STRUCT_t *pQueue)
* Description   : 获取结构体队列中的个数(结构体个数)
* Input         :
* Output        :
* Other         :
* Date          : 2014.08.14
*******************************************************************************/

uint32_t QUEUE_StructCountGet(QUEUE_STRUCT_t *pQueue)
{
    uint32_t index = 0;
    uint32_t start =0;

    ASSERT_PARAM(pQueue);
    ASSERT_PARAM(pQueue->pBuf);

    start = pQueue->start;
    while ((start != pQueue->end) && (index < pQueue->sumCount))
    {
        index++;
        if (++start >= pQueue->sumCount)
        {
            start = 0;
        }
    }

    return index;
}
#endif
/*******************************************************************************
* 函数功能: 三次样条曲线拟合算法
* 参    数:
* 返 回 值:
* 说    明: 可用于机器人路径生成
*******************************************************************************/
#ifdef   COUNT_SPLINE3_ENABLE
uint8_t Count_Spline3(COUNT_S_SPLINE3* ps)
{
    //步长
    float *pH;
    //导数
    float *pFi;
    //第一步 中间量
    float *pui,*pvi,*pdi;
    //第二步 中间量
    float *pB,*pY,*pM;
    //
    uint16_t i16=0,j16=0;
    uint8_t *pbuf;
    pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_10KB);
    memset((char*)pbuf,0,MEM_10KB_BLK_SIZE);
    pH   =  (float*)&pbuf[1024*0];
    pFi  =  (float*)&pbuf[1024*1];
    pui  =  (float*)&pbuf[1024*2];
    pvi  =  (float*)&pbuf[1024*3];
    pdi  =  (float*)&pbuf[1024*4];
    pB   =  (float*)&pbuf[1024*5];
    pY   =  (float*)&pbuf[1024*6];
    pM   =  (float*)&pbuf[1024*7];
    //容错
    //---输入数据点个数太少或太多
    if((ps->InNum < 3) || (ps->InNum > COUNT_SPLINE3_MAXNUM + 1))
    {
        MemManager_Free(E_MEM_MANAGER_TYPE_10KB,pbuf);
        return ERR;
    }
    //第一步: 计算系数矩阵(u v d)
    //---中间值
    for(i16 = 0; i16 <= ps->InNum - 2; i16++)
    {
        //求H[i] 步长
        pH[i16] = ps->pInX[i16+1] - ps->pInX[i16];
        //求F[x(i),x(i+1)] 导数
        pFi[i16] = (ps->pInY[i16+1] - ps->pInY[i16]) / pH[i16];
    }
    //---ui,vi,di
    for(i16 = 1; i16 <= ps->InNum - 2; i16++)
    {
        pui[i16] = pH[i16-1] / (pH[i16-1] + pH[i16]);
        pvi[i16] = pH[i16] / (pH[i16-1] + pH[i16]);
        pdi[i16] = 6 * (pFi[i16] - pFi[i16-1]) / (pH[i16-1] + pH[i16]);
    }
    //---第一类边界条件
    if(ps->BoundType==1)
    {
        pvi[0] = 1;
        pdi[0] = 6 * (pFi[0] - ps->BoundBegin) / pH[0];
        pui[ps->InNum - 1] = 1;
        pdi[ps->InNum - 1] = 6 * (ps->BoundEnd - pFi[ps->InNum - 2]) / pH[ps->InNum - 2];
    }
    //---第二类边界条件
    else if(ps->BoundType==2)
    {
        pvi[0] = 0;
        pdi[0] = 2 * ps->BoundBegin;
        pui[ps->InNum - 1] = 0;
        pdi[ps->InNum - 1] = 2 * ps->BoundEnd;
    }
    //第二部: 追赶法求解M矩阵
    pB[0] = pvi[0] / 2;
    for(i16 = 1; i16 <= ps->InNum - 2; i16++)
    {
        pB[i16] = pvi[i16] / (2 - pui[i16] * pB[i16-1]);
    }
    //---追
    pY[0] = pdi[0] / 2;
    for(i16 = 1; i16 <= ps->InNum - 1; i16++)
    {
        pY[i16] = (pdi[i16] - pui[i16] * pY[i16-1]) / (2 - pui[i16] * pB[i16-1]);
    }
    //---赶
    pM[ps->InNum - 1] = pY[ps->InNum - 1];
    for(i16 = ps->InNum - 1; i16 > 0; i16--)
    {
        pM[i16-1] = pY[i16-1] - pB[i16-1] * pM[i16];
    }
    //第三部: 计算方程组最终结果
    {
        float f1,f2;
        for(i16 = 0; i16 < ps->OutNum; i16++)
        {
            // 如果x超限，则y=0
            if(ps->pOutX[i16]<ps->pInX[0] || ps->pOutX[i16]>ps->pInX[ps->InNum-1])
            {
                ps->pOutY[i16]=0.0f;
                continue;
            }
            //找出最近前采样点索引-->j16
            for(j16=0; j16<ps->InNum-1; j16++)
            {
                if(ps->pOutX[i16]>=ps->pInX[j16] && ps->pOutX[i16]<ps->pInX[j16+1])
                {
                    break;
                }
            }
            //
            f1 = ps->pInX[j16+1] -  ps->pOutX[i16];
            f2 = ps->pOutX[i16]  -  ps->pInX[j16];
            ps->pOutY[i16] =  f1*f1*f1*(pM[i16] / (6 * pH[i16]));
            ps->pOutY[i16]+=  f2*f2*f2*(pM[i16+1] / (6 * pH[i16]));
            ps->pOutY[i16]+=  f1*((ps->pInY[i16] - pM[i16] * pH[i16] * pH[i16] / 6) / pH[i16]);
            ps->pOutY[i16]+=  f2*((ps->pInY[i16+1] - pM[i16+1] * pH[i16] * pH[i16] / 6) /pH[i16]);
        }
    }
    MemManager_Free(E_MEM_MANAGER_TYPE_10KB,pbuf);
    //
    return TRUE;
}
//uint8_t TestBuf[1000];
static void Count_Spline3_Test(void)
{
    /*
    验证公式: y=3/(1+x*x)
    与VC++的数值比对,完全相符
    01:x=0.100000 y=2.967449
    02:x=0.600000 y=2.211606
    03:x=1.100000 y=1.355111
    04:x=1.600000 y=0.843087
    05:x=2.100000 y=0.554375
    06:x=2.600000 y=0.386623
    07:x=3.100000 y=0.282738
    08:x=3.600000 y=0.214900
    09:x=4.100000 y=0.168442
    10:x=4.600000 y=0.135378
    */
    uint8_t i;
    COUNT_S_SPLINE3 *ps;
    uint8_t *pbuf;
    float const sourceX[17] = {0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0, 5.5, 6.0, 6.5, 7.0, 7.5, 8.0};
    pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_1KB);
    //
    sprintf((char*)pbuf,"-----Interpolation(SpLine3)\r\n");
    COUNT_DEBUG_OUT_STR((int8_t*)pbuf);
    //
    ps         =  (COUNT_S_SPLINE3*)&pbuf[500];
    ps->pInX   =  (float*)&pbuf[100];
    ps->pInY   =  (float*)&pbuf[200];
    ps->pOutX  =  (float*)&pbuf[300];
    ps->pOutY  =  (float*)&pbuf[400];
    for(i=0; i<17; i++)
    {
        ps->pInX[i]=sourceX[i];
        ps->pInY[i]=3.0f / (1.0f + ps->pInX[i] * ps->pInX[i]);
    }
    ps->InNum      =  17;
    ps->BoundBegin =  0.0;
    ps->BoundEnd   =  -0.01136;
    ps->BoundType  =  1;
    for(i=0; i<10; i++)
    {
        ps->pOutX[i]=sourceX[i]+0.1f;
    }
    ps->OutNum     =  10;
    //
    Count_Spline3(ps);
    //
    for(i=0; i<ps->OutNum; i++)
    {
        MODULE_OS_DELAY_MS(10);
        sprintf((char*)pbuf,"%02d:x=%f y=%f\r\n",i+1,ps->pOutX[i],ps->pOutY[i]);
        COUNT_DEBUG_OUT_STR((int8_t*)pbuf);
    }
    MemManager_Free(E_MEM_MANAGER_TYPE_1KB,pbuf);
}
#endif
#ifdef   COUNT_NON_LINEAR_EQUATION
/*******************************************************************************
* 函数功能: 非线性方程求解-->二分逼近法
* 参    数:
* 返 回 值:
* 说    明: 非线性方程y=0时，x的值既是所求解
*******************************************************************************/
double Count_DichotomyEquation(double a, double b, pCount_DichotomyEquationFunction f,uint16_t *pNum)
{
    uint16_t i16=0;
    double mid = (a + b) / 2.0;
    while((b - a) > COUNT_DICHOTOMY_EQUATION_PRECISION)
    {
        if(f(a) * f(mid) < 0.0)
        {
            b = mid;
        }
        else
        {
            a = mid;
        }
        mid = (a + b) / 2.0;
        i16++;
        if(i16==0xFFFF)
        {
            break;
        }
    }
    *pNum   =  i16;
    return mid;
}
/*******************************************************************************
* 函数功能: 非线性方程求解-->牛顿迭代法
* 参    数:
* 返 回 值:
*******************************************************************************/
static double Count_CalcDerivative(pCount_NewtonRaphsonFunction f, double x)
{
    return (f(x + 0.000005) - f(x - 0.000005)) / 0.00001;
}
double Count_NewtonRaphson(pCount_NewtonRaphsonFunction f, double x0,uint16_t *pNum)
{
    uint16_t i16=0;
    double x1 = x0 - f(x0) / Count_CalcDerivative(f, x0);
    while(fabs(x1 - x0) > COUNT_NEWTON_RAPHSON_PRECISION)
    {
        x0 = x1;
        x1 = x0 - f(x0) / Count_CalcDerivative(f, x0);
        i16++;
        if(i16==0xFFFF)
        {
            break;
        }
    }
    *pNum = i16;
    return x1;
}
static double Count_NonLinearEquationTestFun(double x)
{
    return (2.0*x*x + 3.2*x - 1.8);
}
static void Count_NonLinearEquation_Test(void)
{
    uint16_t i16;
    double x,y;
    uint8_t *pbuf;
    pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    memset((char*)pbuf,0,MEM_256B_BLK_SIZE);
    //
    sprintf((char*)pbuf,"-----NonLinearEquation\r\n");
    COUNT_DEBUG_OUT_STR((int8_t*)pbuf);
    //
    x=-0.8;
    y = Count_NonLinearEquationTestFun(x);
    sprintf((char*)pbuf,"x=%9.9f y=%9.9f\r\n",x,y);
    COUNT_DEBUG_OUT_STR((int8_t*)pbuf);
    x=8;
    y = Count_NonLinearEquationTestFun(x);
    sprintf((char*)pbuf,"x=%9.9f y=%9.9f\r\n",x,y);
    COUNT_DEBUG_OUT_STR((int8_t*)pbuf);
    x=0.44096736423671234;
    y = Count_NonLinearEquationTestFun(x);
    sprintf((char*)pbuf,"x=%9.9f y=%9.9f\r\n",x,y);
    COUNT_DEBUG_OUT_STR((int8_t*)pbuf);
    x=-2.040967365;
    y = Count_NonLinearEquationTestFun(x);
    sprintf((char*)pbuf,"x=%9.9f y=%9.9f\r\n",x,y);
    COUNT_DEBUG_OUT_STR((int8_t*)pbuf);
    //
    y = Count_DichotomyEquation(-0.8, 12.0, Count_NonLinearEquationTestFun,&i16);
    sprintf((char*)pbuf,"DichotomyEquation:(-0.8,12.0) x=%9.9f cmt=%d\r\n",y,i16);
    COUNT_DEBUG_OUT_STR((int8_t*)pbuf);
    y = Count_NewtonRaphson(Count_NonLinearEquationTestFun, 8.0 , &i16);
    sprintf((char*)pbuf,"NewtonRaphson:(8,0) x=%9.9f cmt=%d\r\n",y,i16);
    COUNT_DEBUG_OUT_STR((int8_t*)pbuf);
    y = Count_NewtonRaphson(Count_NonLinearEquationTestFun, -8.0 , &i16);
    sprintf((char*)pbuf,"NewtonRaphson:(-8,0)x=%9.9f cmt=%d\r\n",y,i16);
    COUNT_DEBUG_OUT_STR((int8_t*)pbuf);
    //
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
}
#endif
/*******************************************************************************
                        智能算法介绍
*  遗传算法
      缩写  GA
      含义  适者生存
      适合  连续与离散问题
      流程  选择-交叉->变异->适应度评价
      优点  快速全局搜索，收敛速度快，进化时间短
      缺点  无正反馈与负反馈机制，长时间只是做无用进化演算，存在汉明悬崖问题
*  蚁群算法
      缩写  ACO
      适合  路径搜索
            NP难度(非确定多项式)的旅行商问题
            车间调度算法
            二次指派
            多维背包
      优点  有正反馈机制(正确导向)
      缺点  全局搜索能力差，收敛慢，计算开销大
*  粒子群算法
      缩写  PSO
      含义  模拟鸟类觅食/人类认知等社会行为
      适合  实数问题，连续问题，如神经网络训练/函数优化
      优点  全局优化，记忆良种，算法简单
      缺点  陷入局部最优解
*  蜂群算法
      含义  蚁群优化算法
      优点  对比蚁群算法，减少了参数个数，提升全局寻优能力
*  差分进化算法
*  量子进化算法
*  退火算法
*******************************************************************************/
/*******************************************************************************
*                       遗传算法 解决  背包问题
*  背景：1个背包  能载重150
         7个物品  重量(35 30 60 50 40 10 25) 价值(10 40 30 50 35 40 30)
*  问题：不超重/价最高
*******************************************************************************/
#ifdef   COUNT_GA_ENABLE
//条件定义  ---   背包载重
const int COUNT_GA_KNAPSACK_CAPACITY = 150;
//条件定义  ---   7个物品
const int COUNT_GA_KNAPSACK_COUNT = 7;
//条件定义  ---   物品重量
const int Weight[COUNT_GA_KNAPSACK_COUNT] = {35,30,60,50,40,10,25};
//条件定义  ---   物品价值
const int Value[COUNT_GA_KNAPSACK_COUNT] = {10,40,30,50,35,40,30};
//算法定义  ---   种群数量
#define  COUNT_GA_POPULATION_SIZE   32
//算法定义  ---   配对次数
const int COUNT_GA_MAX_GENERATIONS = 500;
//算法定义  ---   交叉概率
const double COUNT_GA_P_XOVER = 0.8;
//算法定义  ---   变异概率
const double COUNT_GA_P_MUTATION = 0.15;
//数据结构  ---   个体类型
typedef struct COUNT_S_GA_UNIT
{
    //基因指针
    int32_t gene[COUNT_GA_KNAPSACK_COUNT];
    //适应度
    int32_t fitness;
    //选择概率
    double rf;
    //积累概率
    double cf;
} COUNT_S_GA_UNIT;

//函数   ---   初始化种群
static void Count_GA_Initialize(COUNT_S_GA_UNIT *pop)
{
    uint16_t i16,j16;
    //种群初始化
    for(i16 = 0; i16 < COUNT_GA_POPULATION_SIZE; i16++)
    {
        //基因初始化
        for(j16 = 0; j16 < COUNT_GA_KNAPSACK_COUNT; j16++)
        {
            pop[i16].gene[j16] = Count_Rand() % 2;
        }
        //适应度初始化
        pop[i16].fitness  =  0;
        //选择概率初始化
        pop[i16].rf       =  0.0;
        //积累概率初始化
        pop[i16].cf       =  0.0;
    }
}
//函数   ---   评价种群(非个体)适应度
//说明   ---   物品价值总和,即为适应度
static int Count_GA_EvaluateFitness(COUNT_S_GA_UNIT *pop)
{
    int32_t totalFitness = 0;
    int tw;
    uint16_t i,j;
    //分析每个个体
    for(i = 0; i < COUNT_GA_POPULATION_SIZE; i++)
    {
        //重量清零
        tw = 0;
        //适应度清零
        pop[i].fitness = 0;
        //分析每个基因
        for(j = 0; j < COUNT_GA_KNAPSACK_COUNT; j++)
        {
            if(pop[i].gene[j] == 1)
            {
                tw += Weight[j];
                pop[i].fitness += Value[j];
            }
        }
        //超重惩罚(超重则适应度评价固定为1)
        if(tw > COUNT_GA_KNAPSACK_CAPACITY)
        {
            pop[i].fitness = 1;
        }
        totalFitness += pop[i].fitness;
    }
    return totalFitness;
}
//函数   ---   选择算子 ---   获取最优个体
static int Count_GA_GetBestPopulation(COUNT_S_GA_UNIT *pop, COUNT_S_GA_UNIT *bestGene)
{
    int best = 0;
    int i;
    for(i = 0; i < COUNT_GA_POPULATION_SIZE; i++)
    {
        if(pop[i].fitness > pop[best].fitness)
        {
            best = i;
        }
    }
    *bestGene = pop[best];

    return best;
}
//函数   ---   选择算子
static void Count_GA_Select(int totalFitness, COUNT_S_GA_UNIT *pop)
{
    int i,j;
    COUNT_S_GA_UNIT best;
    double lastCf = 0.0;
    double p;
    //
    COUNT_S_GA_UNIT *pNewPop;
    pNewPop=MemManager_Get(E_MEM_MANAGER_TYPE_2KB_BASIC);
    // 1.设定轮盘赌(计算每个个体的选择概率和累积概率)
    for(i = 0; i < COUNT_GA_POPULATION_SIZE; i++)
    {
        pop[i].rf = (double)pop[i].fitness / totalFitness;
        pop[i].cf = lastCf + pop[i].rf;
        lastCf = pop[i].cf;
    }
    // 2.获取最优个体
    Count_GA_GetBestPopulation(pop, &best);
    // 3.旋转轮盘，重新构造种群
    for(i = 0; i < COUNT_GA_POPULATION_SIZE; i++)
    {
        p = (double)Count_Rand() / (COUNT_RAND_MAX + 1);
        //随机概率小于第一个个体的积累概率，则赋予最优个体
        if(p < pop[0].cf)
        {
            pNewPop[i] = best;
        }
        //随机概率对应积累概率的位置，赋予对应的个体
        else
        {
            for(j = 0; j < COUNT_GA_POPULATION_SIZE; j++)
            {
                if((p >= pop[j].cf) && (p < pop[j + 1].cf))
                {
                    pNewPop[i] = pop[j + 1];
                }
            }
        }
    }
    // 4.把违规个体(本例即为超重)修正为最优个体
    for(i = 0; i < COUNT_GA_POPULATION_SIZE; i++)
    {
        pop[i] = (pNewPop[i].fitness == 1) ? best : pNewPop[i];
    }
    MemManager_Free(E_MEM_MANAGER_TYPE_2KB_BASIC,pNewPop);
}
//函数   ---   交叉算子 ---   交叉基因
static void Count_GA_ExchangeOver(COUNT_S_GA_UNIT *pop, int first, int second)
{
    //对随机个数的基因位进行交换
    int ecc = Count_Rand() % COUNT_GA_KNAPSACK_COUNT + 1;
    int i,idx,tg;
    for(i = 0; i < ecc; i++)
    {
        //每个位置被交换的概率是相等的
        idx = Count_Rand() % COUNT_GA_KNAPSACK_COUNT;
        tg = pop[first].gene[idx];
        pop[first].gene[idx] = pop[second].gene[idx];
        pop[second].gene[idx] = tg;
    }
}
//函数   ---   交叉算子
static void Count_GA_Crossover(COUNT_S_GA_UNIT *pop)
{
    //第一个个体已经选择的标识
    int first = -1;
    int i;
    double p;
    for(i = 0; i < COUNT_GA_POPULATION_SIZE; i++)
    {
        p = (double)Count_Rand() / (COUNT_RAND_MAX + 1);
        if(p < COUNT_GA_P_XOVER)
        {
            if(first < 0)
            {
                first = i; //选择第一个个体
            }
            else
            {
                Count_GA_ExchangeOver(pop, first, i);
                first = -1;//清除第一个个体的选择标识
            }
        }
    }
}
//函数   ---   变异算子 ---   变异基因
static void Count_GA_ReverseGene(COUNT_S_GA_UNIT *pop, int index)
{
    int i,mcc,gi;
    //对随机个数的基因位进行变异
    mcc = Count_Rand() % COUNT_GA_KNAPSACK_COUNT + 1;
    for(i = 0; i < mcc; i++)
    {
        //每个位置被交换的概率是相等的
        gi = Count_Rand() % COUNT_GA_KNAPSACK_COUNT;
        pop[index].gene[gi] = 1 - pop[index].gene[gi];
    }
}
//函数   ---   变异算子
static void Count_GA_Mutation(COUNT_S_GA_UNIT *pop)
{
    int i;
    for(i = 0; i < COUNT_GA_POPULATION_SIZE; i++)
    {
        double p = (double)Count_Rand() / (COUNT_RAND_MAX + 1);
        if(p < COUNT_GA_P_MUTATION)
        {
            Count_GA_ReverseGene(pop, i);
        }
    }
}
//测试遗传算法
static void Count_GA_Test(void)
{
    uint16_t i16 ;
    uint16_t success = 0;
    uint16_t j16;
    int totalFitness;
    //最佳单元
    COUNT_S_GA_UNIT *pbest;
    COUNT_S_GA_UNIT *pPopulation;
    //
    char *pbuf;
    pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_2KB_BASIC);
    pbest         =  (COUNT_S_GA_UNIT*)&pbuf[100];
    pPopulation   =  (COUNT_S_GA_UNIT*)&pbuf[200];
    //打印信息
    sprintf((char*)pbuf,"-----Genetic Algorithm\r\n");
    COUNT_DEBUG_OUT_STR((int8_t*)pbuf);
    //连续检验多次，验证算法效果
    for(i16=0; i16<10; i16++)
    {
        //初始化种群
        Count_GA_Initialize(pPopulation);
        //适应度评价
        totalFitness = Count_GA_EvaluateFitness(pPopulation);
        //配对次数
        j16=COUNT_GA_MAX_GENERATIONS;
        while(j16--)
        {
            //选择算子
            Count_GA_Select(totalFitness, pPopulation);
            //交叉算子
            Count_GA_Crossover(pPopulation);
            //变异算子
            Count_GA_Mutation(pPopulation);
            //适应度评价
            totalFitness = Count_GA_EvaluateFitness(pPopulation);
        }
        //获取最优
        Count_GA_GetBestPopulation(pPopulation, pbest);
        //如果最优适应度为170
        if(pbest->fitness == 170)
        {
            pbest->fitness=0;
            success++;
        }
    }
    sprintf((char*)pbuf,"SuccessRate=%d/%d\r\n",success,i16+1);
    COUNT_DEBUG_OUT_STR((int8_t*)pbuf);
    //
    MemManager_Free(E_MEM_MANAGER_TYPE_2KB_BASIC,pbuf);
}
#endif
/*******************************************************************************
*                       蚁群算法 解决  商旅问题
*  背景：给定多个地标,要求全都走到,用尽量少的路径
*******************************************************************************/
#ifdef   COUNT_ACO_ENABLE
#define COUNT_ACO_FILE_ENABLED  0
typedef struct COUNT_S_ACO_LANDMARK
{
    char *pstr;
    int x;
    int y;
} COUNT_S_ACO_LANDMARK;

const COUNT_S_ACO_LANDMARK Count_S_AcoLandmarkBuf[30]=
{
    {"1",2,99},
    {"2",4,50},
    {"3",7,64},
    {"4",13,40},
    {"5",18,54},
    {"6",18,40},
    {"7",22,60},
    {"8",24,42},
    {"9",25,62},
    {"10",25,38},
    {"11",37,84},
    {"12",41,94},
    {"13",41,26},
    {"14",44,35},
    {"15",45,21},
    {"16",54,67},
    {"17",54,62},
    {"18",58,35},
    {"19",58,69},
    {"20",62,32},
    {"21",64,60},
    {"22",68,58},
    {"23",71,44},
    {"24",71,71},
    {"25",74,78},
    {"26",82,7},
    {"27",83,46},
    {"28",83,69},
    {"29",87,76},
    {"30",91,38}
};

//蚂蚁的数量
#define COUNT_ACO_M 13
//城市的数量
#define COUNT_ACO_N (sizeof(Count_S_AcoLandmarkBuf)/sizeof(COUNT_S_ACO_LANDMARK))
//迭代次数
#define COUNT_ACO_R /*100*/50
//初始化的信息素的量
#define COUNT_ACO_PHEROMONE_INIT 1
//定义最大值
#define COUNT_ACO_DISTANCE_MAX   0x7fffffff

// 启发因子,信息素的重要程度(值越大 选择之前经过路线可能性大)
const double Count_ACO_alpha=2;
// 期望因子,城市间距离的重要程度(值越大，选择离它近的城市地标可能性越高)
const double Count_ACO_betra=/*2*/3;
// 信息素挥发参数(信息素的保留率)
const double Count_ACO_rou=/*0.7*/0.1;
// 总的信息素(xsl待定)
const double Count_ACO_Q=5000;

// 城市地标
/*
struct coordinate
{
    char city[15];  //城市名
    int x;          //城市相对横坐标
    int y;          //城市相对纵坐标
} coords[COUNT_ACO_N];
*/

// 距离矩阵：储存城市之间的距离的邻接矩阵,自己到自己记作MAX
double Count_ACO_graph[COUNT_ACO_N][COUNT_ACO_N];
// 信息素矩阵：每条路径上的信息素的量
double Count_ACO_phe[COUNT_ACO_N][COUNT_ACO_N];
// 信息素增量矩阵：代表相应路径上的信息素的增量
double Count_ACO_add[COUNT_ACO_N][COUNT_ACO_N];
// 启发函数,yita[i][j]=1/graph[i][j]
double Count_ACO_yita[COUNT_ACO_N][COUNT_ACO_N];
// 蚂蚁路线标记矩阵：标记已经走过的城市(0表示未访问,1表示已访问)
int Count_ACO_vis[COUNT_ACO_M][COUNT_ACO_N];
// 蚂蚁路线矩阵：map[K][N]记录第K只蚂蚁走的路线
int Count_ACO_map[COUNT_ACO_M][COUNT_ACO_N];
// 记录某次循环中每只蚂蚁走的路线的距离
double Count_ACO_solution[COUNT_ACO_M];
// 记录最近的那条路线
int Count_ACO_bestway[COUNT_ACO_N];
double Count_ACO_bestsolution=COUNT_ACO_DISTANCE_MAX;

// 载入地标
static void Count_ACO_Inputcoords(void)
{
#if (COUNT_ACO_FILE_ENABLED==1)
    int number;
    FILE *fp;
    fp = fopen("coords.txt","r+");
    if(fp==NULL)
    {
        printf("Sorry,the file is not exist\n");
        exit(1);
    }
    else
    {
        for(i=0; i<COUNT_ACO_N; ++i)
        {
            fscanf(fp,"%d%s",&number,Count_S_AcoLandmarkBuf[i].pstr);
            fscanf(fp,"%d,%d",&Count_S_AcoLandmarkBuf[i].x,&Count_S_AcoLandmarkBuf[i].y);
        }
    }
    // 关闭文件
    fclose(fp);
#else
    /*
    int i;
    for(i=0; i<COUNT_ACO_N; ++i)
    {
        memcpy(coords[i].city,Count_S_AcoLandmarkBuf[i].pstr,sizeof(Count_S_AcoLandmarkBuf[i].pstr));
        coords[i].x=Count_S_AcoLandmarkBuf[i].x;
        coords[i].y=Count_S_AcoLandmarkBuf[i].y;
    }
    */
#endif
}
// 计算并载入距离矩阵
static void Count_ACO_GreateGraph(void)
{
    int i,j;
    double d;
    for(i=0; i<COUNT_ACO_N-1; ++i)
    {
        //自己到自己标记为无穷大
        Count_ACO_graph[i][i]=COUNT_ACO_DISTANCE_MAX;
        for(j=i+1; j<COUNT_ACO_N; ++j)
        {
            d=(double)((Count_S_AcoLandmarkBuf[i].x-Count_S_AcoLandmarkBuf[j].x)\
                       *(Count_S_AcoLandmarkBuf[i].x-Count_S_AcoLandmarkBuf[j].x)\
                       +(Count_S_AcoLandmarkBuf[i].y-Count_S_AcoLandmarkBuf[j].y)\
                       *(Count_S_AcoLandmarkBuf[i].y-Count_S_AcoLandmarkBuf[j].y));
            Count_ACO_graph[j][i]=Count_ACO_graph[i][j]=sqrt(d);
        }
    }
    Count_ACO_graph[COUNT_ACO_N-1][COUNT_ACO_N-1]=COUNT_ACO_DISTANCE_MAX;
    return ;
}
// 路径总距离
static double Count_ACO_TotalDistance(int *p)
{
    double d=0;
    int i;
    for(i=0; i<COUNT_ACO_N-1; ++i)
    {
        d+=Count_ACO_graph[*(p+i)][*(p+i+1)];
    }
    d+=Count_ACO_graph[*(p+i)][*(p)];
    return d;
}
// 打印结果到文件
static void Count_ACO_Result(void)
{
    int i;
#if (COUNT_ACO_FILE_ENABLED==1)
    //文件打印
    FILE *fl;
    fl = fopen("out.txt","a");  //将结果保存在out.txt这个文件里面
    fprintf(fl,"%s\n","本次算法中的各参数如下:");
    fprintf(fl,"alpha=%.3lf, betra=%.3lf, rou=%.3lf, Q=%.3lf\n",Count_ACO_alpha,Count_ACO_betra,Count_ACO_rou,Count_ACO_Q);
    fprintf(fl,"%s %d\n","本次算法迭代次数为:",COUNT_ACO_R);
    fprintf(fl,"%s %.4lf\n","本算法得出的最短路径长度为:",Count_ACO_bestsolution);
    fprintf(fl,"%s\n","本算法求得的最短路径为:");
    for(i=0; i<COUNT_ACO_N; ++i)
    {
        fprintf(fl,"%s →  ",Count_S_AcoLandmarkBuf[Count_ACO_bestway[i]].pstr);
    }
    fprintf(fl,"%s",Count_S_AcoLandmarkBuf[Count_ACO_bestway[0]].pstr);
    fprintf(fl,"\n\n\n");
    fclose(fl);
    printf("Result is saved in out.txt\n");
#endif
    char *pbuf;
    pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    // 屏幕打印
    sprintf((char*)pbuf,"%s\n","本次算法中的各参数如下:");
    COUNT_DEBUG_OUT_STR((int8_t*)pbuf);
    sprintf((char*)pbuf,"alpha=%.3lf, betra=%.3lf, rou=%.3lf, Q=%.3lf\n",Count_ACO_alpha,Count_ACO_betra,Count_ACO_rou,Count_ACO_Q);
    COUNT_DEBUG_OUT_STR((int8_t*)pbuf);
    sprintf((char*)pbuf,"%s %d\n","本次算法迭代次数为:",COUNT_ACO_R);
    COUNT_DEBUG_OUT_STR((int8_t*)pbuf);
    sprintf((char*)pbuf,"%s %.4lf\n","本算法得出的最短路径长度为:",Count_ACO_bestsolution);
    COUNT_DEBUG_OUT_STR((int8_t*)pbuf);
    sprintf((char*)pbuf,"%s\n","本算法求得的最短路径为:");
    COUNT_DEBUG_OUT_STR((int8_t*)pbuf);
    for(i=0; i<COUNT_ACO_N; ++i)
    {
        sprintf((char*)pbuf,"%s →  ",Count_S_AcoLandmarkBuf[Count_ACO_bestway[i]].pstr);
        COUNT_DEBUG_OUT_STR((int8_t*)pbuf);
    }
    sprintf((char*)pbuf,"%s",Count_S_AcoLandmarkBuf[Count_ACO_bestway[0]].pstr);
    COUNT_DEBUG_OUT_STR((int8_t*)pbuf);
    sprintf((char*)pbuf,"\n\n\n");
    COUNT_DEBUG_OUT_STR((int8_t*)pbuf);
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
}

static void Count_ACO_Test(void)
{
    int NC=0;
    int i,j,k;
    int s;
    double drand,pro,psum;
    char *pbuf;
    pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    //打印信息
    sprintf((char*)pbuf,"-----Genetic ACO\r\n");
    COUNT_DEBUG_OUT_STR((int8_t*)pbuf);
    // 1    ----------  变量初始化
    // 载入城市地标
    Count_ACO_Inputcoords();
    // 计算并载入距离矩阵
    Count_ACO_GreateGraph();
    // 载入初始信息素与期望值(与距离成反比)
    for(i=0; i<COUNT_ACO_N; ++i)
    {
        for(j=0; j<COUNT_ACO_N; ++j)
        {
            Count_ACO_phe[i][j]=COUNT_ACO_PHEROMONE_INIT;
            if(i!=j)
            {
                Count_ACO_yita[i][j]=100.0/Count_ACO_graph[i][j];
            }
        }
    }
    //把蚂蚁走的路线置空
    memset(Count_ACO_map,-1,sizeof(Count_ACO_map));
    //把已访问的地标置空
    memset(Count_ACO_vis,0,sizeof(Count_ACO_vis));
    //准备随机数
#ifdef   OS_WINDOWS
    srand(time(NULL));
#endif
    // 2    ----------  进入迭代流程
    //迭代
    while(NC++<=COUNT_ACO_R)
    {
        // * 给每只蚂蚁分配一个起点,并且保证起点在N个城市里 将起点标记为已经访问
        for(k=0; k<COUNT_ACO_M; ++k)
        {
            Count_ACO_map[k][0]=(k+NC)%COUNT_ACO_N;
            Count_ACO_vis[k][Count_ACO_map[k][0]]=1;
        }
        // 每个蚂蚁按算法移动一步为一个循环，总共操作地标数量个循环
        s=1;
        while(s<COUNT_ACO_N)
        {
            // 按蚂蚁数量
            for(k=0; k<COUNT_ACO_M; ++k)
            {
                //  *   计算 当前蚂蚁 在 当前地标 与 没去过的地标 之间的信息素总和
                psum=0;
                for(j=0; j<COUNT_ACO_N; ++j)
                {
                    // 如果这个地标没有经过
                    if(Count_ACO_vis[k][j]==0)
                    {
                        // 计算该地标与当前地标间的信息素,并累加信息素得到总和
                        psum+=pow(Count_ACO_phe[Count_ACO_map[k][s-1]][j],Count_ACO_alpha)*pow(Count_ACO_yita[Count_ACO_map[k][s-1]][j],Count_ACO_betra);
                    }
                }
                //  *   进行轮盘选择
                // 生成一个小于1的随机数
                drand=(double)Count_Rand() / (COUNT_RAND_MAX + 1);
                // 按概率为蚂蚁选择下一个地标
                pro=0;
                for(j=0; j<COUNT_ACO_N; ++j)
                {
                    if(Count_ACO_vis[k][j]==0)
                        pro+=pow(Count_ACO_phe[Count_ACO_map[k][s-1]][j],Count_ACO_alpha)*pow(Count_ACO_yita[Count_ACO_map[k][s-1]][j],Count_ACO_betra)/psum;
                    if(pro>drand)
                        break;
                }
                // 标记地标走过
                Count_ACO_vis[k][j]=1;
                // 记录城市的顺序
                Count_ACO_map[k][s]=j;
            }
            s++;
        }
        // 信息素增量清零
        memset(Count_ACO_add,0,sizeof(Count_ACO_add));
        // 得到路径最短的蚂蚁，并记录最短路径与最短距离
        for(k=0; k<COUNT_ACO_M; ++k)
        {
            //蚂蚁k所走的路线的总长度
            Count_ACO_solution[k]=Count_ACO_TotalDistance(Count_ACO_map[k]);
            if(Count_ACO_solution[k]<Count_ACO_bestsolution)
            {
                Count_ACO_bestsolution=Count_ACO_solution[k];
                for(i=0; i<COUNT_ACO_N; ++i)
                    Count_ACO_bestway[i]=Count_ACO_map[k][i];
            }
        }
        //  *   更新信息素
        // 计算信息素增量矩阵
        for(k=0; k<COUNT_ACO_M; ++k)
        {
            for(j=0; j<COUNT_ACO_N-1; ++j)
            {
                Count_ACO_add[Count_ACO_map[k][j]][Count_ACO_map[k][j+1]]+=Count_ACO_Q/Count_ACO_solution[k];
            }
            Count_ACO_add[COUNT_ACO_N-1][0]+=Count_ACO_Q/Count_ACO_solution[k];
        }
        // 更新信息素
        for(i=0; i<COUNT_ACO_N; ++i)
        {
            for(j=0; j<COUNT_ACO_N; ++j)
            {
                Count_ACO_phe[i][j]=Count_ACO_phe[i][j]*Count_ACO_rou+Count_ACO_add[i][j];
                //设立一个下界
                if(Count_ACO_phe[i][j]<0.0001)
                    Count_ACO_phe[i][j]=0.0001;
                //设立一个上界,防止启发因子的作用被淹没
                else if(Count_ACO_phe[i][j]>20)
                    Count_ACO_phe[i][j]=20;
            }
        }
        memset(Count_ACO_vis,0,sizeof(Count_ACO_vis));
        memset(Count_ACO_map,-1,sizeof(Count_ACO_map));
        printf("waiting:%d\r",NC);
    }
    // 打印结果到文件
    Count_ACO_Result();
    //
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
}
#endif
//-----------------------------------------------------------------------------
#if     DEBUG_FULL_ASSERT

/*******************************************************************************
* Function Name : void ASSERT_FAILED(uint8_t* file, uint32_t line)
* Description   : 异常
* Input         :
* Output        :
* Other         :
* Date          : 2013.08.29
*******************************************************************************/
void ASSERT_FAILED(uint8_t* file, uint32_t line)
{
    uint8_t flg = 1;

    printf("wrong information 文件:%s 第%d行\r\n", file, line);
    while (flg);
}

#endif
//-------------------------------------------------------------------------------

