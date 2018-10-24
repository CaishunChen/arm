//-------------------------------------------------------------------------------//
//                           ������ͨ������޹�˾                                //
//                                ��Ȩ����                                       //
//                    ��ҵ��ַ��http://www.htong.com                             //
//                    ��������: MPLAB IDE V8.87                                  //
//                    �� �� ��: MPLAB C30 V3.31                                  //
//                    оƬ�ͺ�: dsPIC33EP128GP506                                //
//                    ��Ŀ����: HH-SPS ������Ŀ                                  //
//                    �ļ�����: RTC.c                                          //
//                    ��    ��: ������                                           //
//                    ʱ    ��: 2013-11-19    �汾:  0.1                         //
//-------------------------------------------------------------------------------//
//
// �ļ������� ʱ��ת������
//
// ע�����
//
// �޸ļ�¼��
//
// �޸�ʱ��:
//
//
//-------------------------------------------------------------------------------
/*******************************************************************************
*                               UNIXʱ���
*���ļ�ʵ�ֻ���RTC�����ڹ��ܣ��ṩ�����յĶ�д��
* RTC�б����ʱ���ʽ���Ǹ�ʽ�ġ���һ��32bit��time_t������ʵΪu32��
*
* ANSI-C�ı�׼���У��ṩ�����ֱ�ʾʱ��������ͣ�
* time_t:
*       UNIXʱ�������1970-1-1��ĳʱ�侭����������
*   typedef INT32U time_t;
* struct tm:
*       Calendar��ʽ����������ʽ��
*       tm�ṹ���£�
*       struct tm {
*               int tm_sec;   // �� 0 to 60
*               int tm_min;   // �� 0 to 59
*       int tm_hour;  // ʱ 0 to 23
*       int tm_mday;  // �� 1 to 31
*       int tm_mon;   // �� 0 to 11
*       int tm_year;  // ��   since 1900
*       int tm_wday;  // ���� since Sunday, 0 to 6
*       int tm_yday;  // ��Ԫ��������� days since January 1, 0 to 365
*       int tm_isdst; // ����ʱ
*       ...
*   }
*   ����wday��yday�����Զ����������ֱ�Ӷ�ȡ
*   mon��ȡֵΪ0-11
*   ***ע��***��
*   tm_year:��time.h���ж���Ϊ1900�������ݣ���2008��Ӧ��ʾΪ2008-1900=108
*   ���ֱ�ʾ�������û���˵����ʮ���Ѻã�����ʵ�нϴ���졣
*   �����ڱ��ļ��У����������ֲ��졣
*   ���ⲿ���ñ��ļ��ĺ���ʱ��tm�ṹ�����͵����ڣ�tm_year��Ϊ2008
*   ע�⣺��Ҫ����ϵͳ��time.c�еĺ�������Ҫ���н�tm_year-=1900
* ��Ա����˵����
* struct tm Time_ConvUnixToCalendar(time_t t);
*   ����һ��Unixʱ�����time_t��������Calendar��ʽ����
* time_t Time_ConvCalendarToUnix(struct tm t);
*   ����һ��Calendar��ʽ���ڣ�����Unixʱ�����time_t��
* time_t Time_GetUnixTime(void);
*   ��RTCȡ��ǰʱ���Unixʱ���ֵ
* struct tm Time_GetCalendarTime(void);
*   ��RTCȡ��ǰʱ�������ʱ��
* void Time_SetUnixTime(time_t);
*   ����UNIXʱ�����ʽʱ�䣬����Ϊ��ǰRTCʱ��
* void Time_SetCalendarTime(struct tm t);
*   ����Calendar��ʽʱ�䣬����Ϊ��ǰRTCʱ��
*
* �ⲿ����ʵ����
* ����һ��Calendar��ʽ�����ڱ�����
* struct tm now;
* now.tm_year = 2008;
* now.tm_mon = 11;      //12��
* now.tm_mday = 20;
* now.tm_hour = 20;
* now.tm_min = 12;
* now.tm_sec = 30;
*
* ��ȡ��ǰ����ʱ�䣺
* tm_now = Time_GetCalendarTime();
* Ȼ�����ֱ�Ӷ�tm_now.tm_wday��ȡ������(0��ʾ����)
*
* ����ʱ�䣺
* Step1. tm_now.xxx = xxxxxxxxx;
* Step2. Time_SetCalendarTime(tm_now);
*
* ��������ʱ��Ĳ�
* struct tm t1,t2;
* t1_t = Time_ConvCalendarToUnix(t1);
* t2_t = Time_ConvCalendarToUnix(t2);
* dt = t1_t - t2_t;
* dt��������ʱ��������
* dt_tm = mktime(dt);   //ע��dt�����ƥ�䣬ansi���к���Ϊ�����ݣ�ע�ⳬ��
* ����Բο�������ϣ�����ansi-c��ĸ�ʽ������ȹ��ܣ�ctime��strftime��
*
*******************************************************************************/
/******************************** ˵�� ************************
**************************************************************/
//------------------------------- Includes --------------------
#include <includes.h>
#ifndef STM32
#include "user.h"
#endif
//------------------------------- �û����� --------------------
struct tm CurrentDate;//��ǰʱ��


INT8U ReadRealTimeflag = 0;         //��ȡʵʱʱ�ӱ�־
INT8U RTCSelfCheckCnt  = 0;         //RTC��Ч����
INT8U RTCSelfCheckErrCnt = 0;       //RTC���ϼ���
INT8U RTC_SEC_FLAG = 0;             //1���жϱ�־

INT8U SoftTime[7] = {13,12,17,12,35,30,10};      //ʵʱʱ������  �ꡢ�¡��ա�ʱ���֡��롢����


const INT8U WeekTab[] =                        //���������ڱ�
{
    (3 << 5) + 31,   //1��
    (6 << 5) + 29,   //2��
    (0 << 5) + 31,   //3��
    (3 << 5) + 30,   //4��
    (5 << 5) + 31,   //5��
    (1 << 5) + 30,   //6��
    (3 << 5) + 31,   //7��
    (6 << 5) + 31,   //8��
    (1 << 5) + 30,   //9��
    (4 << 5) + 31,   //10��
    (0 << 5) + 30,   //11��
    (2 << 5) + 31    //12��
};

//-------------------------------------------------------------------------------
// ��������: INT8U GetWeekDay(INT8U *time)
// ��������: ���������ռ��������
// ��ڲ���: ʱ��
// ���ز���: week ����
// �޸�˵����
// �޸�ʱ�䣺
//-------------------------------------------------------------------------------
unsigned char GetWeekDay(INT8U y, INT8U  m, INT8U d)
{
    INT8U week, day;
    day = WeekTab[m - 1];     //�±�
    week = day >> 5;          //��������
    day &= 0x1f;              //������
    if ((m < 3) && (y & 0x03))//ƽ��
    {
        if (m == 2) day--;    //ƽ��������
        week++;               //ƽ���±�+1
    }
    y = y + (y >> 2);         //��+��/4
    week = (week +  y + d + 2) % 7;  //(����=��+��/4+�±�+2��)%7
//  return (week << 5) | day;        //�������ں�������
    return week;              //�������ں�������
}

//-------------------------------------------------------------------------------
// ��������: void SoftTime(void)
// ��������: ���ʵʱʱ��
// ��ڲ���: ��
// ���ز���: ��
// �޸�˵����
// �޸�ʱ�䣺
//-------------------------------------------------------------------------------
void SoftTimeCount(void)
{
    static INT8U MScnt = 0;              // 10MS��������
    if(ReadRealTimeflag == 1)            // ����־
    {
        MScnt ++;
    }
    else
    {
        SoftTime[6] ++;                  // 10mS++
        if(SoftTime[6] > 99)             // 1��++
        {
            SoftTime[5] ++;
            SoftTime[6] = SoftTime[6] + MScnt -100;  //10MS����
            MScnt = 0;

            if(SoftTime[5] > 59)         // ��++
            {
                SoftTime[5] = 0;
                SoftTime[4] ++;
                if(SoftTime[4] > 59)     // ʱ++
                {
                    SoftTime[4] = 0;
                    SoftTime[3] ++;
                    if(SoftTime[3] > 23) // ��++
                    {
                        SoftTime[3] = 0;
                        SoftTime[2] ++;
                        if(SoftTime[1]== 2)            //2�·� ����
                        {
                            if((SoftTime[0]%4) == 0)   //����  29��
                            {
                                if(SoftTime[2] > 29)   //��++
                                {
                                    SoftTime[2] = 1;
                                    SoftTime[1] ++;
                                }
                            }
                            else                             //ƽ��  28��
                            {
                                if(SoftTime[2] > 28)   //��++
                                {
                                    SoftTime[2] = 1;
                                    SoftTime[1] ++;
                                }
                            }
                        }
                        //4�¡�6�¡�9�¡�11��   30��
                        else if((SoftTime[1]== 4)||(SoftTime[1]== 6)||(SoftTime[1]== 9)||(SoftTime[1]== 11))
                        {
                            if(SoftTime[2] > 30)   // ��++
                            {
                                SoftTime[2] = 1;
                                SoftTime[1] ++;
                            }
                        }
                        //1�¡�3�¡�5�¡�7�¡�8�¡�10�¡�12��   31��
                        else
                        {
                            if(SoftTime[2] > 31)   // ��++
                            {
                                SoftTime[2] = 1;
                                SoftTime[1] ++;
                            }
                        }

                        if(SoftTime[1] > 12)       // ��++
                        {
                            SoftTime[1] = 1;
                            SoftTime[0] ++;
                            if(SoftTime[0] > 99)   // ����
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
// ��������: INT8U ReadRealTime(INT8U *time,struct tm *CurrentTime_tm)
// ��������: ��ȡʵʱʱ������
// ��ڲ���: *time ��5λʱ����� ��׼�ڹ�2ʱ��
// ���ز���: ���ú����󣬿ɶ�ȡʱ��ڹ�1�ͽڹ�2
// �޸�˵����
// �޸�ʱ�䣺
//-------------------------------------------------------------------------------
INT8U ReadRealTime(INT8U *time,struct tm *CurrentTime_tm)
{
    INT8U Week;
    ReadRealTimeflag = 1;   //����־
    if(time!=NULL)
    {
        //          �� 7λ                  �� 1λ
        time[4] = ((SoftTime[0]<<1)&0xFE) + ((SoftTime[1]>>3)&0x01);

        //          �� 3λ                  �� 5λ
        time[3] = (((SoftTime[1])<<5)&0xE0) + (SoftTime[2]&0x1F);

        //          ʱ 5λ                  �� 3λ
        time[2] = ((SoftTime[3]<<3)&0xF8) + ((SoftTime[4]>>3)&0x07);

        //          �� 3λ                  �� 5λ
        time[1] = ((SoftTime[4]<<5)&0xE0) + ((SoftTime[5]>>1)&0x1F);

        //          �� 1λ                  MS 7λ
        time[0] = ((SoftTime[5]<<7)&0x80) + (SoftTime[6]&0x7F);
    }
    if(CurrentTime_tm!=NULL)
    {
        Week = GetWeekDay(SoftTime[0],SoftTime[1],SoftTime[2]); //����

        CurrentTime_tm->tm_sec  = SoftTime[5];   //��
        CurrentTime_tm->tm_min  = SoftTime[4];   //��
        CurrentTime_tm->tm_hour = SoftTime[3];   //ʱ
        CurrentTime_tm->tm_mday = SoftTime[2];   //��
        CurrentTime_tm->tm_wday = Week;          //����
        CurrentTime_tm->tm_mon  = SoftTime[1]-1; //��  0-11
        CurrentTime_tm->tm_year = SoftTime[0]+2000;   //��
    }
    ReadRealTimeflag = 0;   //����־
    return OK;
}
//-------------------------------------------------------------------------------
// ��������: INT8U SetRtc(INT8U *time)
// ��������: ʵʱʱ������
// ��ڲ���: *time ʱ��
// ���ز���: ��
// �޸�˵����
// �޸�ʱ�䣺
//-------------------------------------------------------------------------------
INT8U SetRealTime(INT8U *time)
{
    INT8U Week;            //����
    INT8U temp1,temp2;     //����

    //---------------------------------------------------------------------------
    T3CONbits.TON = 0;             //�ر�TMR3
    IFS0bits.T3IF = 0;
    TMR3 = 0x0000;                 //��������

    //---------------------------------------------------------------------------
    SoftTime[0] = ((time[4]>>1)&0x7F);                     //��  b39-b33
    SoftTime[1] = ((time[4]&0x01)<<3)+((time[3]>>5)&0x07); //��  b32-b29
    SoftTime[2] = ( time[3]&0x1F);                         //��  b28-b24
    SoftTime[3] = ((time[2]>>3)&0x1F);                     //ʱ  b23-b19
    SoftTime[4] = ((time[2]&0x07)<<3)+((time[1]>>5)&0x07); //��  b18-b13
    SoftTime[5] = ((time[1]&0x1F)<<1)+((time[0]>>7)&0x01); //��  b12-b7
    SoftTime[6] = ( time[0]&0x7F);                         //MS  b6-b0

    Week = GetWeekDay(SoftTime[0],SoftTime[1],SoftTime[2]);//����

    T3CONbits.TON = 1;             //����TMR3

    temp1 = (SoftTime[5]/10)<<4;        //�� ����д
    temp2 = (SoftTime[5]%10);
    RTC_Write_1Byte(0x02,temp1|temp2);

    temp1 = (SoftTime[4]/10)<<4;        //�� ����д
    temp2 = (SoftTime[4]%10);
    RTC_Write_1Byte(0x03,temp1|temp2);

    temp1 = (SoftTime[3]/10)<<4;        //ʱ ����д
    temp2 = (SoftTime[3]%10);
    RTC_Write_1Byte(0x04,temp1|temp2);

    temp1 = (SoftTime[2]/10)<<4;        //�� ����д
    temp2 = (SoftTime[2]%10);
    RTC_Write_1Byte(0x05,temp1|temp2);

    RTC_Write_1Byte(0x06,Week);        //���� ����д

    temp1 = (SoftTime[1]/10) << 4;      //�� ����д
    temp2 = (SoftTime[1]%10);
    RTC_Write_1Byte(0x07,temp1|temp2);

    temp1 = (SoftTime[0]/10)<<4;        //�� ����д
    temp2 = (SoftTime[0]%10);
    RTC_Write_1Byte(0x08,temp1|temp2);
    //---------------------------------------------------------------------------
    return OK;
}


//-------------------------------------------------------------------------------
// ��������: void SetSoftTime(void)
// ��������: ��PCF8563��ȡʱ��,�޸����ʱ��
// ���ú���: RTC_Read_1Byte(Addr);    //��Ӳ��ʱ����
// ��ڲ���: ��
// ���ز���: ��
// �޸�˵����
// �޸�ʱ�䣺
//-------------------------------------------------------------------------------
void SetSoftTime(void)
{
    INT8U  temp = 0;

    T3CONbits.TON = 0;             //�ر�TMR3
    IFS0bits.T3IF = 0;
    TMR3 = 0x0000;                 //��������

    SoftTime[6] = 0;                          // MS

    temp = RTC_Read_1Byte(0x02)&0x7f;         // ��
    SoftTime[5] = (temp >> 4)*10 + (temp&0x0F);

    temp = RTC_Read_1Byte(0x03)&0x7f;         // ��
    SoftTime[4] = (temp >> 4)*10 + (temp&0x0F);

    temp = RTC_Read_1Byte(0x04)&0x3f;         // ʱ
    SoftTime[3] = (temp >> 4)*10 + (temp&0x0F);

    temp = RTC_Read_1Byte(0x05)&0x3f;         // ��
    SoftTime[2] = (temp >> 4)*10 + (temp&0x0F);

    temp = (RTC_Read_1Byte(0x07)&0x1f);       // ��
    SoftTime[1] = (temp >> 4)*10 + (temp&0x0F);

    temp = RTC_Read_1Byte(0x08);              // ��
    SoftTime[0] = (temp >> 4)*10 + (temp&0x0F);

    T3CONbits.TON = 1;             //����TMR3
}

//-------------------------------------------------------------------------------
// ��������: void RTC_SelfCheck(void)
// ��������: PCF8563ʱ�������Լ����
// ���ú���: RTC_Read_1Byte(Addr);    //��Ӳ��ʱ����
// ��ڲ���: ��
// ���ز���: RTCSelfCheckCnt ��Ч����   ����Ӧ��֤ÿ100�������1��
// �޸�˵����
// �޸�ʱ�䣺
//-------------------------------------------------------------------------------
void RTC_SelfCheck(void)
{
    static INT8U Newseconds = 0;    //"��"��ֵ
    static INT8U Oldseconds = 0;    //"��"��ֵ

    Newseconds = RTC_Read_1Byte(0x02)&0x7F;    //����
    if((Newseconds > Oldseconds)||((Newseconds == 0x00)&&(Oldseconds == 0x59))) //ʱ����Ч
    {
        if(RTCSelfCheckCnt < 5)     //��Ч����
        {
            RTCSelfCheckCnt ++;
        }
        else                        //���ϻָ�
        {
            if(RTCSelfCheckErrCnt < 5)  //���ֹ��Ϻ�����
            {
                RTCSelfCheckErrCnt = 0;
            }
        }
    }
    else                            //ʱ����Ч
    {
        if(RTCSelfCheckErrCnt > 5)  //���ϼ���
        {
            RTCSelfCheckCnt = 0;    //��Ч����
        }
        else
        {
            RTCSelfCheckErrCnt ++;
        }
    }
    Oldseconds = Newseconds;        //����ʱ��
}
//-------------------------------------------------------------------------------
// ��������: RTCPowerONSelfCheck(void)
// ��������: PCF8563ʱ���ϵ��Լ����
// ���ú���: RTC_Read_1Byte(Addr);    //��Ӳ��ʱ����
// ��ڲ���: ��
// ���ز���: Flag.PCF8563_VL �����־,Ϊ1 = RTC��ص�����,����ع���.
// �޸�˵��������Լ�ʧ��,ϵͳʱ��Ϊ 00��,1��,1��,0ʱ,0��,0�� ��ʼ����
// �޸�ʱ�䣺
//-------------------------------------------------------------------------------
void RTCPowerONSelfCheck(void)
{
    INT8U Temp;                        //�Լ���ʱ

    Temp = RTC_Read_1Byte(0x02);       //��RTC ����λ
    if(Temp&0x80)                      //оƬ���� ?
    {
        Flag.PCF8563_VL = 1;           //�����־��1
    }
    else                               //��������
    {
        Flag.PCF8563_VL = 0;           //�����־��0
    }
    Temp = 0;
    while(Temp < 100)                  //10���Լ쾧��
    {
        if(Time100msFlag == 1)         //1��?
        {
            Temp ++;                   //����
            Time100msFlag = 0;
            RTC_SelfCheck();           //�Լ�
        }
    }
    if((RTCSelfCheckCnt >4)&&(RTCSelfCheckErrCnt == 0))     //ʱ����Ч
    {
        SetSoftTime();        //����ʱ��
    }
    else                       //ʱ����Ч
    {
        SoftTime[0] =  0;      //00��
        SoftTime[1] =  1;      //1��
        SoftTime[2] =  1;      //1��
        SoftTime[3] =  0;      //0ʱ
        SoftTime[4] =  0;      //0��
        SoftTime[5] =  0;      //0��
        SoftTime[6] =  0;      //0MS
        //��д����Ĺ��ϼ�¼(RTC��������)
    }
}
//-------------------------------------------------------------------------------
// ��������: void AutoSetSoftTime(void)
// ��������: PCF8563ʱ���Զ�У�����ʱ�� ��10����/�Σ�  (RTCSelfCheckCnt > 4)
// ���ú���: RTC_Read_1Byte(Addr);    //��Ӳ��ʱ����
// ��ڲ���: ��
// ���ز���: ����Ӧ��֤ÿ100�������1��
// �޸�˵����
// �޸�ʱ�䣺
//-------------------------------------------------------------------------------
void AutoSetSoftTime(void)
{
    static INT16U Time10MinCnt= 0;   //1���Ӽ���

    Time10MinCnt ++;                 //���Ӽ���
    if((Time10MinCnt > 5999)&&(RTCSelfCheckErrCnt == 0)) //ʱ����Ч,10����Уʱ1��
    {
        Time10MinCnt = 0;
        if(RTCSelfCheckCnt > 4)     //ʱ����Ч,���н�ʱ
        {
            SetSoftTime();          //����ʱ��
        }
    }
}

void RTC_CLKOInit(void)
{
	INTCON2bits.INT3EP = 1;        //INT3�½����ж�
	IEC3bits.INT3IE  = 1;          //INT3�ж�����
	IPC13bits.INT3IP = 3;
}

//-------------------------------------------------------------------------------
// ��������: INT3Interrupt
// ��������: INT3 �жϳ���
// ���ú���: ��
// ��ڲ���: ��
// ���ز���: ��
// �޸�˵����
// �޸�ʱ�䣺
//-------------------------------------------------------------------------------
void __attribute__((__interrupt__, no_auto_psv)) _INT3Interrupt(void)
{
    IFS3bits.INT3IF = 0;
    RTC_SEC_FLAG = 1;
    EX_DS1 = !EX_DS1;
}
//-------------------------------------------------------------------------------
// ��������: INT8U TimeJudge(INT8U TimeGroupNumber)
// ��������: tmʱ��ת��Ϊ5�ֽڶ�ʱ��
// ��ڲ���: mode = 1 : tm->time5;  mode=2:time5->tm
// ���ز���:
// �޸�˵����
// �޸�ʱ�䣺
//-------------------------------------------------------------------------------
INT8U TimeTypeConvert(INT8U mode,struct tm* tm_time,INT8U* time)
{
    INT8U i;
    //tm->time5
    if(mode==1)
    {
        //��
        i=tm_time->tm_year-2000;
        time[4]=(i<<1);
        //��
        i=tm_time->tm_mon;
        i++;
        time[4]+=(i>>3);
        time[3]=(i<<5);
        //��
        time[3]+=tm_time->tm_mday;
        //ʱ
        time[2]=((tm_time->tm_hour)<<3);
        //��
        i=tm_time->tm_min;
        time[2]+=(i>>3);
        time[1]=(i<<5);
        //��
        i=tm_time->tm_sec;
        time[1]+=(i>>1);
        time[0]=(i<<7);
        //����
    }
    //time5->tm
    else if(mode==2)
    {
        tm_time->tm_year=  (time[4]>>1)+2000;                              // �� since 2000
        tm_time->tm_mon= (((time[4]&0x01)<<3) |(time[3]>>5))-1;   // �� 0 to 11
        tm_time->tm_mday= (time[3]&0x1f);                           // �� 1 to 31
        tm_time->tm_hour= (time[2]>>3);                             // ʱ 0 to 23
        tm_time->tm_min= ((time[2]& 0x07)<<3)|(time[1]>>5);   // �� 0 to 59
        tm_time->tm_sec= ((time[1]&0x1f)<<1) |(time[0]>>7);  // �� 0 to 60
    }
    return OK;
}
//-------------------------------------------------------------------------------
