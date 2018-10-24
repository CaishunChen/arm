//-------------------------------------------------------------------------------//
//                           丹东华通测控有限公司                                //
//                                版权所有                                       //
//                    企业网址：http://www.htong.com                             //
//                    开发环境: MPLAB IDE V8.87                                  //
//                    编 译 器: MPLAB C30 V3.31                                  //
//                    芯片型号: dsPIC33EP128GP506                                //
//                    项目名称: HH-SPS 安防项目                                  //
//                    文件名称: RTC.c                                          //
//                    作    者: 徐松亮                                           //
//                    时    间: 2013-11-19    版本:  0.1                         //
//-------------------------------------------------------------------------------//
//
// 文件描述： 时间转换函数
//
// 注意事项：
//
// 修改记录：
//
// 修改时间:
//
//
//-------------------------------------------------------------------------------
/*******************************************************************************
*                               UNIX时间戳
*本文件实现基于RTC的日期功能，提供年月日的读写。
* RTC中保存的时间格式，是格式的。即一个32bit的time_t变量（实为u32）
*
* ANSI-C的标准库中，提供了两种表示时间的数据型：
* time_t:
*       UNIX时间戳（从1970-1-1起到某时间经过的秒数）
*   typedef INT32U time_t;
* struct tm:
*       Calendar格式（年月日形式）
*       tm结构如下：
*       struct tm {
*               int tm_sec;   // 秒 0 to 60
*               int tm_min;   // 分 0 to 59
*       int tm_hour;  // 时 0 to 23
*       int tm_mday;  // 日 1 to 31
*       int tm_mon;   // 月 0 to 11
*       int tm_year;  // 年   since 1900
*       int tm_wday;  // 星期 since Sunday, 0 to 6
*       int tm_yday;  // 从元旦起的天数 days since January 1, 0 to 365
*       int tm_isdst; // 夏令时
*       ...
*   }
*   其中wday，yday可以自动产生，软件直接读取
*   mon的取值为0-11
*   ***注意***：
*   tm_year:在time.h库中定义为1900年起的年份，即2008年应表示为2008-1900=108
*   这种表示方法对用户来说不是十分友好，与现实有较大差异。
*   所以在本文件中，屏蔽了这种差异。
*   即外部调用本文件的函数时，tm结构体类型的日期，tm_year即为2008
*   注意：若要调用系统库time.c中的函数，需要自行将tm_year-=1900
* 成员函数说明：
* struct tm Time_ConvUnixToCalendar(time_t t);
*   输入一个Unix时间戳（time_t），返回Calendar格式日期
* time_t Time_ConvCalendarToUnix(struct tm t);
*   输入一个Calendar格式日期，返回Unix时间戳（time_t）
* time_t Time_GetUnixTime(void);
*   从RTC取当前时间的Unix时间戳值
* struct tm Time_GetCalendarTime(void);
*   从RTC取当前时间的日历时间
* void Time_SetUnixTime(time_t);
*   输入UNIX时间戳格式时间，设置为当前RTC时间
* void Time_SetCalendarTime(struct tm t);
*   输入Calendar格式时间，设置为当前RTC时间
*
* 外部调用实例：
* 定义一个Calendar格式的日期变量：
* struct tm now;
* now.tm_year = 2008;
* now.tm_mon = 11;      //12月
* now.tm_mday = 20;
* now.tm_hour = 20;
* now.tm_min = 12;
* now.tm_sec = 30;
*
* 获取当前日期时间：
* tm_now = Time_GetCalendarTime();
* 然后可以直接读tm_now.tm_wday获取星期数(0表示周日)
*
* 设置时间：
* Step1. tm_now.xxx = xxxxxxxxx;
* Step2. Time_SetCalendarTime(tm_now);
*
* 计算两个时间的差
* struct tm t1,t2;
* t1_t = Time_ConvCalendarToUnix(t1);
* t2_t = Time_ConvCalendarToUnix(t2);
* dt = t1_t - t2_t;
* dt就是两个时间差的秒数
* dt_tm = mktime(dt);   //注意dt的年份匹配，ansi库中函数为相对年份，注意超限
* 另可以参考相关资料，调用ansi-c库的格式化输出等功能，ctime，strftime等
*
*******************************************************************************/
/******************************** 说明 ************************
**************************************************************/
//------------------------------- Includes --------------------
#include <includes.h>
#ifndef STM32
#include "user.h"
#endif
//------------------------------- 用户变量 --------------------
struct tm CurrentDate;//当前时间


INT8U ReadRealTimeflag = 0;         //计取实时时钟标志
INT8U RTCSelfCheckCnt  = 0;         //RTC有效计数
INT8U RTCSelfCheckErrCnt = 0;       //RTC故障计数
INT8U RTC_SEC_FLAG = 0;             //1秒中断标志

INT8U SoftTime[7] = {13,12,17,12,35,30,10};      //实时时间数据  年、月、日、时、分、秒、毫秒


const INT8U WeekTab[] =                        //润年月星期表
{
    (3 << 5) + 31,   //1月
    (6 << 5) + 29,   //2月
    (0 << 5) + 31,   //3月
    (3 << 5) + 30,   //4月
    (5 << 5) + 31,   //5月
    (1 << 5) + 30,   //6月
    (3 << 5) + 31,   //7月
    (6 << 5) + 31,   //8月
    (1 << 5) + 30,   //9月
    (4 << 5) + 31,   //10月
    (0 << 5) + 30,   //11月
    (2 << 5) + 31    //12月
};

//-------------------------------------------------------------------------------
// 函数名称: INT8U GetWeekDay(INT8U *time)
// 函数功能: 根据年月日计算出星期
// 入口参数: 时间
// 返回参数: week 星期
// 修改说明：
// 修改时间：
//-------------------------------------------------------------------------------
unsigned char GetWeekDay(INT8U y, INT8U  m, INT8U d)
{
    INT8U week, day;
    day = WeekTab[m - 1];     //月表
    week = day >> 5;          //月星期数
    day &= 0x1f;              //月天数
    if ((m < 3) && (y & 0x03))//平年
    {
        if (m == 2) day--;    //平年月天数
        week++;               //平年月表+1
    }
    y = y + (y >> 2);         //年+年/4
    week = (week +  y + d + 2) % 7;  //(星期=年+年/4+月表+2日)%7
//  return (week << 5) | day;        //返回星期和月天数
    return week;              //返回星期和月天数
}

//-------------------------------------------------------------------------------
// 函数名称: void SoftTime(void)
// 函数功能: 软件实时时间
// 入口参数: 无
// 返回参数: 无
// 修改说明：
// 修改时间：
//-------------------------------------------------------------------------------
void SoftTimeCount(void)
{
    static INT8U MScnt = 0;              // 10MS补尝计数
    if(ReadRealTimeflag == 1)            // 读标志
    {
        MScnt ++;
    }
    else
    {
        SoftTime[6] ++;                  // 10mS++
        if(SoftTime[6] > 99)             // 1秒++
        {
            SoftTime[5] ++;
            SoftTime[6] = SoftTime[6] + MScnt -100;  //10MS补尝
            MScnt = 0;

            if(SoftTime[5] > 59)         // 分++
            {
                SoftTime[5] = 0;
                SoftTime[4] ++;
                if(SoftTime[4] > 59)     // 时++
                {
                    SoftTime[4] = 0;
                    SoftTime[3] ++;
                    if(SoftTime[3] > 23) // 日++
                    {
                        SoftTime[3] = 0;
                        SoftTime[2] ++;
                        if(SoftTime[1]== 2)            //2月份 特殊
                        {
                            if((SoftTime[0]%4) == 0)   //润年  29天
                            {
                                if(SoftTime[2] > 29)   //月++
                                {
                                    SoftTime[2] = 1;
                                    SoftTime[1] ++;
                                }
                            }
                            else                             //平年  28天
                            {
                                if(SoftTime[2] > 28)   //月++
                                {
                                    SoftTime[2] = 1;
                                    SoftTime[1] ++;
                                }
                            }
                        }
                        //4月、6月、9月、11月   30天
                        else if((SoftTime[1]== 4)||(SoftTime[1]== 6)||(SoftTime[1]== 9)||(SoftTime[1]== 11))
                        {
                            if(SoftTime[2] > 30)   // 月++
                            {
                                SoftTime[2] = 1;
                                SoftTime[1] ++;
                            }
                        }
                        //1月、3月、5月、7月、8月、10月、12月   31天
                        else
                        {
                            if(SoftTime[2] > 31)   // 月++
                            {
                                SoftTime[2] = 1;
                                SoftTime[1] ++;
                            }
                        }

                        if(SoftTime[1] > 12)       // 年++
                        {
                            SoftTime[1] = 1;
                            SoftTime[0] ++;
                            if(SoftTime[0] > 99)   // 世纪
                            {
                                SoftTime[0] = 0;
                            }
                        }
                    }
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------
// 函数名称: INT8U ReadRealTime(INT8U *time,struct tm *CurrentTime_tm)
// 函数功能: 读取实时时间设置
// 入口参数: *time 短5位时间变量 标准节构2时间
// 返回参数: 调用函数后，可读取时间节构1和节构2
// 修改说明：
// 修改时间：
//-------------------------------------------------------------------------------
INT8U ReadRealTime(INT8U *time,struct tm *CurrentTime_tm)
{
    INT8U Week;
    ReadRealTimeflag = 1;   //读标志
    if(time!=NULL)
    {
        //          年 7位                  月 1位
        time[4] = ((SoftTime[0]<<1)&0xFE) + ((SoftTime[1]>>3)&0x01);

        //          月 3位                  日 5位
        time[3] = (((SoftTime[1])<<5)&0xE0) + (SoftTime[2]&0x1F);

        //          时 5位                  分 3位
        time[2] = ((SoftTime[3]<<3)&0xF8) + ((SoftTime[4]>>3)&0x07);

        //          分 3位                  秒 5位
        time[1] = ((SoftTime[4]<<5)&0xE0) + ((SoftTime[5]>>1)&0x1F);

        //          秒 1位                  MS 7位
        time[0] = ((SoftTime[5]<<7)&0x80) + (SoftTime[6]&0x7F);
    }
    if(CurrentTime_tm!=NULL)
    {
        Week = GetWeekDay(SoftTime[0],SoftTime[1],SoftTime[2]); //星期

        CurrentTime_tm->tm_sec  = SoftTime[5];   //秒
        CurrentTime_tm->tm_min  = SoftTime[4];   //分
        CurrentTime_tm->tm_hour = SoftTime[3];   //时
        CurrentTime_tm->tm_mday = SoftTime[2];   //日
        CurrentTime_tm->tm_wday = Week;          //星期
        CurrentTime_tm->tm_mon  = SoftTime[1]-1; //月  0-11
        CurrentTime_tm->tm_year = SoftTime[0]+2000;   //年
    }
    ReadRealTimeflag = 0;   //清标标志
    return OK;
}
//-------------------------------------------------------------------------------
// 函数名称: INT8U SetRtc(INT8U *time)
// 函数功能: 实时时间设置
// 入口参数: *time 时间
// 返回参数: 无
// 修改说明：
// 修改时间：
//-------------------------------------------------------------------------------
INT8U SetRealTime(INT8U *time)
{
    INT8U Week;            //星期
    INT8U temp1,temp2;     //变量

    //---------------------------------------------------------------------------
    T3CONbits.TON = 0;             //关闭TMR3
    IFS0bits.T3IF = 0;
    TMR3 = 0x0000;                 //计数补尝

    //---------------------------------------------------------------------------
    SoftTime[0] = ((time[4]>>1)&0x7F);                     //年  b39-b33
    SoftTime[1] = ((time[4]&0x01)<<3)+((time[3]>>5)&0x07); //月  b32-b29
    SoftTime[2] = ( time[3]&0x1F);                         //日  b28-b24
    SoftTime[3] = ((time[2]>>3)&0x1F);                     //时  b23-b19
    SoftTime[4] = ((time[2]&0x07)<<3)+((time[1]>>5)&0x07); //分  b18-b13
    SoftTime[5] = ((time[1]&0x1F)<<1)+((time[0]>>7)&0x01); //秒  b12-b7
    SoftTime[6] = ( time[0]&0x7F);                         //MS  b6-b0

    Week = GetWeekDay(SoftTime[0],SoftTime[1],SoftTime[2]);//星期

    T3CONbits.TON = 1;             //启动TMR3

    temp1 = (SoftTime[5]/10)<<4;        //秒 操作写
    temp2 = (SoftTime[5]%10);
    RTC_Write_1Byte(0x02,temp1|temp2);

    temp1 = (SoftTime[4]/10)<<4;        //分 操作写
    temp2 = (SoftTime[4]%10);
    RTC_Write_1Byte(0x03,temp1|temp2);

    temp1 = (SoftTime[3]/10)<<4;        //时 操作写
    temp2 = (SoftTime[3]%10);
    RTC_Write_1Byte(0x04,temp1|temp2);

    temp1 = (SoftTime[2]/10)<<4;        //日 操作写
    temp2 = (SoftTime[2]%10);
    RTC_Write_1Byte(0x05,temp1|temp2);

    RTC_Write_1Byte(0x06,Week);        //星期 操作写

    temp1 = (SoftTime[1]/10) << 4;      //月 操作写
    temp2 = (SoftTime[1]%10);
    RTC_Write_1Byte(0x07,temp1|temp2);

    temp1 = (SoftTime[0]/10)<<4;        //年 操作写
    temp2 = (SoftTime[0]%10);
    RTC_Write_1Byte(0x08,temp1|temp2);
    //---------------------------------------------------------------------------
    return OK;
}


//-------------------------------------------------------------------------------
// 函数名称: void SetSoftTime(void)
// 函数功能: 从PCF8563读取时间,修改软件时钟
// 调用函数: RTC_Read_1Byte(Addr);    //读硬件时钟秒
// 入口参数: 无
// 返回参数: 无
// 修改说明：
// 修改时间：
//-------------------------------------------------------------------------------
void SetSoftTime(void)
{
    INT8U  temp = 0;

    T3CONbits.TON = 0;             //关闭TMR3
    IFS0bits.T3IF = 0;
    TMR3 = 0x0000;                 //计数补尝

    SoftTime[6] = 0;                          // MS

    temp = RTC_Read_1Byte(0x02)&0x7f;         // 秒
    SoftTime[5] = (temp >> 4)*10 + (temp&0x0F);

    temp = RTC_Read_1Byte(0x03)&0x7f;         // 分
    SoftTime[4] = (temp >> 4)*10 + (temp&0x0F);

    temp = RTC_Read_1Byte(0x04)&0x3f;         // 时
    SoftTime[3] = (temp >> 4)*10 + (temp&0x0F);

    temp = RTC_Read_1Byte(0x05)&0x3f;         // 日
    SoftTime[2] = (temp >> 4)*10 + (temp&0x0F);

    temp = (RTC_Read_1Byte(0x07)&0x1f);       // 月
    SoftTime[1] = (temp >> 4)*10 + (temp&0x0F);

    temp = RTC_Read_1Byte(0x08);              // 年
    SoftTime[0] = (temp >> 4)*10 + (temp&0x0F);

    T3CONbits.TON = 1;             //启动TMR3
}

//-------------------------------------------------------------------------------
// 函数名称: void RTC_SelfCheck(void)
// 函数功能: PCF8563时钟运行自检程序
// 调用函数: RTC_Read_1Byte(Addr);    //读硬件时钟秒
// 入口参数: 无
// 返回参数: RTCSelfCheckCnt 有效计数   程序应保证每100毫秒调用1次
// 修改说明：
// 修改时间：
//-------------------------------------------------------------------------------
void RTC_SelfCheck(void)
{
    static INT8U Newseconds = 0;    //"秒"新值
    static INT8U Oldseconds = 0;    //"秒"旧值

    Newseconds = RTC_Read_1Byte(0x02)&0x7F;    //读秒
    if((Newseconds > Oldseconds)||((Newseconds == 0x00)&&(Oldseconds == 0x59))) //时钟有效
    {
        if(RTCSelfCheckCnt < 5)     //有效计数
        {
            RTCSelfCheckCnt ++;
        }
        else                        //故障恢复
        {
            if(RTCSelfCheckErrCnt < 5)  //出现故障后不清零
            {
                RTCSelfCheckErrCnt = 0;
            }
        }
    }
    else                            //时钟无效
    {
        if(RTCSelfCheckErrCnt > 5)  //故障计数
        {
            RTCSelfCheckCnt = 0;    //有效清零
        }
        else
        {
            RTCSelfCheckErrCnt ++;
        }
    }
    Oldseconds = Newseconds;        //更新时间
}
//-------------------------------------------------------------------------------
// 函数名称: RTCPowerONSelfCheck(void)
// 函数功能: PCF8563时钟上电自检程序
// 调用函数: RTC_Read_1Byte(Addr);    //读硬件时钟秒
// 入口参数: 无
// 返回参数: Flag.PCF8563_VL 掉电标志,为1 = RTC电池电量低,即电池故障.
// 修改说明：如果自检失败,系统时间为 00年,1月,1日,0时,0分,0秒 开始运行
// 修改时间：
//-------------------------------------------------------------------------------
void RTCPowerONSelfCheck(void)
{
    INT8U Temp;                        //自检延时

    Temp = RTC_Read_1Byte(0x02);       //读RTC 掉电位
    if(Temp&0x80)                      //芯片掉电 ?
    {
        Flag.PCF8563_VL = 1;           //掉电标志置1
    }
    else                               //电量正常
    {
        Flag.PCF8563_VL = 0;           //掉电标志清0
    }
    Temp = 0;
    while(Temp < 100)                  //10秒自检晶体
    {
        if(Time100msFlag == 1)         //1秒?
        {
            Temp ++;                   //计数
            Time100msFlag = 0;
            RTC_SelfCheck();           //自检
        }
    }
    if((RTCSelfCheckCnt >4)&&(RTCSelfCheckErrCnt == 0))     //时钟有效
    {
        SetSoftTime();        //设置时间
    }
    else                       //时钟无效
    {
        SoftTime[0] =  0;      //00年
        SoftTime[1] =  1;      //1月
        SoftTime[2] =  1;      //1日
        SoftTime[3] =  0;      //0时
        SoftTime[4] =  0;      //0分
        SoftTime[5] =  0;      //0秒
        SoftTime[6] =  0;      //0MS
        //请写下你的故障计录(RTC晶体无振荡)
    }
}
//-------------------------------------------------------------------------------
// 函数名称: void AutoSetSoftTime(void)
// 函数功能: PCF8563时钟自动校正软件时间 （10分钟/次）  (RTCSelfCheckCnt > 4)
// 调用函数: RTC_Read_1Byte(Addr);    //读硬件时钟秒
// 入口参数: 无
// 返回参数: 程序应保证每100毫秒调用1次
// 修改说明：
// 修改时间：
//-------------------------------------------------------------------------------
void AutoSetSoftTime(void)
{
    static INT16U Time10MinCnt= 0;   //1分钟计数

    Time10MinCnt ++;                 //分钟计数
    if((Time10MinCnt > 5999)&&(RTCSelfCheckErrCnt == 0)) //时钟有效,10分钟校时1次
    {
        Time10MinCnt = 0;
        if(RTCSelfCheckCnt > 4)     //时钟有效,进行较时
        {
            SetSoftTime();          //设置时间
        }
    }
}

void RTC_CLKOInit(void)
{
	INTCON2bits.INT3EP = 1;        //INT3下降沿中断
	IEC3bits.INT3IE  = 1;          //INT3中断允许
	IPC13bits.INT3IP = 3;
}

//-------------------------------------------------------------------------------
// 函数名称: INT3Interrupt
// 函数功能: INT3 中断程序
// 调用函数: 无
// 入口参数: 无
// 返回参数: 无
// 修改说明：
// 修改时间：
//-------------------------------------------------------------------------------
void __attribute__((__interrupt__, no_auto_psv)) _INT3Interrupt(void)
{
    IFS3bits.INT3IF = 0;
    RTC_SEC_FLAG = 1;
    EX_DS1 = !EX_DS1;
}
//-------------------------------------------------------------------------------
// 函数名称: INT8U TimeJudge(INT8U TimeGroupNumber)
// 函数功能: tm时间转换为5字节短时间
// 入口参数: mode = 1 : tm->time5;  mode=2:time5->tm
// 返回参数:
// 修改说明：
// 修改时间：
//-------------------------------------------------------------------------------
INT8U TimeTypeConvert(INT8U mode,struct tm* tm_time,INT8U* time)
{
    INT8U i;
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
    return OK;
}
//-------------------------------------------------------------------------------
